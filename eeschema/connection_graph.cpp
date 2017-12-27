/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 CERN
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <list>
#include <unordered_map>
#include <profile.h>

#include <class_sch_screen.h>
#include <erc.h>
#include <sch_component.h>
#include <sch_pin_connection.h>
#include <sch_sheet.h>
#include <sch_sheet_path.h>
#include <sch_text.h>

#include <connection_graph.h>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::map;
using std::unordered_map;
using std::unordered_set;
using std::vector;


bool CONNECTION_SUBGRAPH::ResolveDrivers()
{
    int highest_priority = -1;
    vector<SCH_ITEM*> candidates;

    m_driver = nullptr;

    // Don't worry about drivers for graphs with no-connects
    if( m_no_connect )
        return true;

    for( auto item : m_drivers )
    {
        int item_priority = 0;

        switch( item->Type() )
        {
        case SCH_LABEL_T:               item_priority = 2; break;
        case SCH_HIERARCHICAL_LABEL_T:  item_priority = 3; break;
        case SCH_PIN_CONNECTION_T:
        {
            auto pin_connection = static_cast<SCH_PIN_CONNECTION*>( item );
            if( pin_connection->m_pin->IsPowerConnection() )
                item_priority = 5;
            else
                item_priority = 1;
            break;
        }
        case SCH_GLOBAL_LABEL_T:        item_priority = 6; break;
        default: break;
        }

        if( item_priority > highest_priority )
        {
            candidates.clear();
            candidates.push_back( item );
            highest_priority = item_priority;
        }
        else if( candidates.size() && ( item_priority == highest_priority ) )
        {
            candidates.push_back( item );
        }
    }

    if( candidates.size() )
    {
        if( candidates.size() > 1 )
        {
            if( highest_priority == 1 )
            {
                // We have multiple options and they are all component pins.
                std::sort( candidates.begin(), candidates.end(),
                           [this]( SCH_ITEM* a, SCH_ITEM* b) -> bool
                            {
                                auto pin_a = static_cast<SCH_PIN_CONNECTION*>( a );
                                auto pin_b = static_cast<SCH_PIN_CONNECTION*>( b );

                                auto name_a = pin_a->GetDefaultNetName( m_sheet );
                                auto name_b = pin_b->GetDefaultNetName( m_sheet );

                                return name_a < name_b;
                            } );
            }
            else
            {
                // TODO(JE) ERC warning about multiple drivers?
            }
        }

        m_driver = candidates[0];
    }
    else
    {
        wxLogDebug( _( "Could not resolve drivers for subgraph %li on sheet %s " ),
                    m_code, m_sheet.PathHumanReadable() );

#if DEBUG
        for( auto item : m_items )
            wxLogDebug( "    %s", item->GetSelectMenuText() );
#endif
    }

    return ( m_driver != nullptr );
}


void CONNECTION_GRAPH::Reset()
{
    m_items.clear();
    m_subgraphs.clear();
    m_net_name_to_code_map.clear();
    m_bus_name_to_code_map.clear();
    m_subgraph_code_map.clear();
    m_last_net_code = 1;
    m_last_bus_code = 1;
}


void CONNECTION_GRAPH::UpdateItemConnectivity( const SCH_SHEET_PATH aSheet,
                                               vector<SCH_ITEM*> aItemList )
{
    PROF_COUNTER phase1;

    unordered_map< wxPoint, vector<SCH_ITEM*> > connection_map;

    for( auto item : aItemList )
    {
        vector< wxPoint > points;
        item->GetConnectionPoints( points );
        item->ConnectedItems().clear();

        if( item->Type() == SCH_SHEET_T )
        {
            for( auto& pin : static_cast<SCH_SHEET*>( item )->GetPins() )
            {
                if( !pin.Connection( aSheet ) )
                {
                    pin.InitializeConnection( aSheet );
                }

                pin.ConnectedItems().clear();
                pin.Connection( aSheet )->Reset();

                connection_map[ pin.GetTextPos() ].push_back( &pin );
                m_items.push_back( &pin );
            }
        }
        else if( item->Type() == SCH_COMPONENT_T )
        {
            auto component = static_cast<SCH_COMPONENT*>( item );

            component->UpdatePinConnections( aSheet );

            for( auto pin_connection : component->m_pin_connections )
            {
                // TODO(JE) use cached location from m_Pins
                auto pin_pos = pin_connection->m_pin->GetPosition();
                auto pos = component->GetTransform().TransformCoordinate( pin_pos ) +
                           component->GetPosition();

                // because calling the first time is not thread-safe
                pin_connection->GetDefaultNetName( aSheet );

                connection_map[ pos ].push_back( pin_connection );
                m_items.push_back( pin_connection );
            }
        }
        else
        {
            m_items.push_back( item );

            if( !item->Connection( aSheet ) )
            {
                item->InitializeConnection( aSheet );
            }

            auto conn = item->Connection( aSheet );

            conn->Reset();

            // Set bus/net property here so that the propagation code uses it
            switch( item->Type() )
            {
            case SCH_LINE_T:
                conn->SetType( ( item->GetLayer() == LAYER_BUS ) ?
                               CONNECTION_BUS : CONNECTION_NET );
                break;

            case SCH_BUS_BUS_ENTRY_T:
                conn->SetType( CONNECTION_BUS );
                break;

            case SCH_PIN_CONNECTION_T:
            case SCH_BUS_WIRE_ENTRY_T:
                conn->SetType( CONNECTION_NET );
                break;

            default:
                break;
            }

            for( auto point : points )
            {
                connection_map[ point ].push_back( item );
            }
        }
    }

    for( auto& it : connection_map )
    {
        auto connection_vec = it.second;
        SCH_ITEM* junction = nullptr;

        for( auto connected_item : connection_vec )
        {
            // Look for junctions.  For points that have a junction, we want all
            // items to connect to the junction but not to each other.

            if( connected_item->Type() == SCH_JUNCTION_T )
            {
                junction = connected_item;
            }

            for( auto test_item : connection_vec )
            {
                if( !junction && test_item->Type() == SCH_JUNCTION_T )
                {
                    junction = test_item;
                }

                if( connected_item != test_item &&
                    connected_item != junction &&
                    connected_item->ConnectionPropagatesTo( test_item ) &&
                    test_item->ConnectionPropagatesTo( connected_item ) )
                {
                    connected_item->ConnectedItems().insert( test_item );
                    test_item->ConnectedItems().insert( connected_item );
                }
            }
        }
    }

    phase1.Stop();
    std::cout << "UpdateItemConnectivity() " << phase1.msecs() << " ms" << std::endl;
}


// TODO(JE) This won't give the same subgraph IDs (and eventually net/graph codes)
// to the same subgraph necessarily if it runs over and over again on the same
// sheet.  We need:
//
//  a) a cache of net/bus codes, like used before
//  b) to persist the CONNECTION_GRAPH globally so the cache is persistent,
//  c) some way of trying to avoid changing net names.  so we should keep track
//     of the previous driver of a net, and if it comes down to choosing between
//     equally-prioritized drivers, choose the one that already exists as a driver
//     on some portion of the items.


void CONNECTION_GRAPH::BuildConnectionGraph()
{
    PROF_COUNTER phase2;

    SCH_SHEET_LIST all_sheets( g_RootSheet );

    for( unsigned i = 0; i < all_sheets.size(); i++ )
    {
        for( auto alias : all_sheets[i].LastScreen()->GetBusAliases() )
        {
            m_bus_alias_cache[ alias->GetName() ] = alias;
        }
    }

    long subgraph_code = 1;

    for( auto item : m_items )
    {
        for( auto it : item->m_connection_map )
        {
            const auto sheet = it.first;
            auto connection = it.second;

            if( connection->SubgraphCode() == 0 )
            {
                auto subgraph = new CONNECTION_SUBGRAPH( {} );

                subgraph->m_code = subgraph_code++;
                subgraph->m_sheet = sheet;

                subgraph->m_items.push_back( item );

                // std::cout << "SG " << subgraph.m_code << " started with "
                //           << item->GetSelectMenuText() << std::endl;

                if( connection->IsDriver() )
                    subgraph->m_drivers.push_back( item );

                if( item->Type() == SCH_NO_CONNECT_T )
                    subgraph->m_no_connect = item;

                connection->SetSubgraphCode( subgraph->m_code );

                std::list<SCH_ITEM*> members( item->ConnectedItems().begin(),
                                              item->ConnectedItems().end() );

                for( auto connected_item : members )
                {
                    if( !connected_item->Connection( sheet ) )
                    {
                        // std::cout << "Warning: uninitialized " << connected_item->GetSelectMenuText() << std::endl;
                        connected_item->InitializeConnection( sheet );
                    }

                    if( connected_item->Type() == SCH_NO_CONNECT_T )
                        subgraph->m_no_connect = connected_item;

                    auto connected_conn = connected_item->Connection( sheet );

                    wxASSERT( connected_conn );

                    if( connected_conn->SubgraphCode() == 0 )
                    {
                        connected_conn->SetSubgraphCode( subgraph->m_code );
                        subgraph->m_items.push_back( connected_item );

                        // std::cout << "   +" << connected_item->GetSelectMenuText() << std::endl;

                        if( connected_conn->IsDriver() )
                            subgraph->m_drivers.push_back( connected_item );

                        members.insert( members.end(),
                                        connected_item->ConnectedItems().begin(),
                                        connected_item->ConnectedItems().end() );
                    }
                }

                subgraph->m_dirty = true;
                m_subgraphs.push_back( subgraph );
            }
        }
    }

    m_last_net_code = 0;
    m_last_bus_code = 0;

    /**
     * TODO(JE)
     *
     * It would be good if net codes were preserved as much as possible when
     * generating netlists, so that unnamed nets don't keep shifting around when
     * you regenerate.
     *
     * Right now, we are clearing out the old connections up in
     * UpdateItemConnectivity(), but that is useful information, so maybe we
     * need to just set the dirty flag or something.
     *
     * That way, ResolveDrivers() can check what the driver of the subgraph was
     * previously, and if it is in the situation of choosing between equal
     * candidates for an auto-generated net name, pick the previous one.
     *
     * N.B. the old algorithm solves this by sorting the possible net names
     * alphabetically, so as long as the same refdes components are involved,
     * the net will be the same.
     */

#ifdef USE_OPENMP
    #pragma omp parallel for schedule(dynamic)
#endif
    for( auto it = m_subgraphs.begin(); it < m_subgraphs.end(); it++ )
    {
        auto subgraph = *it;

        if( !subgraph->m_dirty )
            continue;

        if( !subgraph->ResolveDrivers() )
        {
            // TODO(JE) ERC Error: multiple equivalent drivers
        }
        else
        {
            if( subgraph->m_no_connect )
            {
                subgraph->m_dirty = false;
                continue;
            }

            // Now the subgraph has only one driver
            auto driver = subgraph->m_driver;
            auto sheet = subgraph->m_sheet;
            auto connection = driver->Connection( sheet );

            // TODO(JE) This should live in SCH_CONNECTION probably
            switch( driver->Type() )
            {
            case SCH_LABEL_T:
            case SCH_GLOBAL_LABEL_T:
            case SCH_HIERARCHICAL_LABEL_T:
            case SCH_PIN_CONNECTION_T:
            case SCH_SHEET_PIN_T:
            case SCH_SHEET_T:
            {
                if( driver->Type() == SCH_PIN_CONNECTION_T )
                {
                    auto pin = static_cast<SCH_PIN_CONNECTION*>( driver );

                    // NOTE(JE) GetDefaultNetName is not thread-safe.
                    connection->ConfigureFromLabel( pin->GetDefaultNetName( sheet ) );
                }
                else
                {
                    auto text = static_cast<SCH_TEXT*>( driver );
                    connection->ConfigureFromLabel( text->GetText() );
                }

                connection->SetDriver( driver );
                connection->ClearDirty();
                break;
            }
            default:
                break;
            }

            // std::cout << "Propagating SG " << subgraph.m_code << " driven by "
            //           << subgraph.m_driver->GetSelectMenuText() << " net "
            //           << subgraph.m_driver->Connection()->Name() << std::endl;

            for( auto item : subgraph->m_items )
            {
                auto item_conn = item->Connection( sheet );

                if( ( connection->IsBus() && item_conn->IsNet() ) ||
                    ( connection->IsNet() && item_conn->IsBus() ) )
                {
                    continue;
                }

                if( item != driver )
                {
                    // std::cout << "   +" << item->GetSelectMenuText() << std::endl;
                    item_conn->Clone( *connection );
                    item_conn->ClearDirty();
                }
            }

            subgraph->m_dirty = false;
        }
    }

    phase2.Stop();
    std::cout << "BuildConnectionGraph() " <<  phase2.msecs() << " ms" << std::endl;
}


std::shared_ptr<BUS_ALIAS> CONNECTION_GRAPH::GetBusAlias( wxString aName )
{
    try
    {
        return m_bus_alias_cache.at( aName );
    }
    catch( const std::out_of_range& oor )
    {
        return nullptr;
    }
}


int CONNECTION_GRAPH::RunERC( bool aCreateMarkers )
{
    int error_count = 0;

    for( auto it = m_subgraphs.begin(); it < m_subgraphs.end(); it++ )
    {
        auto subgraph = *it;

        // Graph is supposed to be up-to-date before calling RunERC()
        wxASSERT( !subgraph->m_dirty );

        /**
         * Check that labels attached to bus subgraphs follow proper format
         *
         * NOTE: this check doesn't need to be here right now because labels
         * won't actually be connected to bus wires if they aren't in the right
         * format.
         */

        /**
         * Check for conflict between type (bus vs net) of wires and ports/pins
         */
        if( !ercCheckBusToNetConflicts( subgraph, aCreateMarkers ) )
            error_count++;

        /**
         * Check that subgraphs of nets that connect to buses via a bus entry
         * actually exist in the bus they are attached to
         */

        /**
         * Check that the nets broken out of a named bus group are properly
         * prefixed with the bus group name
         */

        /**
         * Check that bus subgraphs that contain both a label and a pin/port
         * have some bus members in common between the two
         */
        if( !ercCheckBusToBusConflicts( subgraph, aCreateMarkers ) )
            error_count++;

        /**
         * Check that subgraphs don't have multiple conflicting drivers
         */

        /**
         * Check that no-connect subgraphs don't have anything other than pins
         *
         * NOTE: this is already implemented in the "classic" ERC code, but I
         * have re-implemented it here to demonstrate how we could move all the
         * ERC code to the CONNECTION_GRAPH if desired.
         */
#if 0
        if( subgraph->m_no_connect != nullptr )
        {
            bool has_invalid_items = false;
            SCH_PIN_CONNECTION* pin = nullptr;
            std::vector<SCH_ITEM*> invalid_items;

            for( auto item : subgraph->m_items )
            {
                switch( item->Type() )
                {
                case SCH_PIN_CONNECTION_T:
                    pin = static_cast<SCH_PIN_CONNECTION*>( item );
                    break;

                case SCH_NO_CONNECT_T:
                    break;

                default:
                    has_invalid_items = true;
                    invalid_items.push_back( item );
                }
            }

            // TODO: Should it be an error to have a NC item but no pin?
            if( pin && has_invalid_items )
            {
                error_count++;

                auto pos = pin->GetPosition();
                msg.Printf( _( "Pin on %s has a no-connect marker but is connected" ),
                            GetChars( pin->m_comp->GetRef( &subgraph->m_sheet ) ) );

                auto marker = new SCH_MARKER();
                marker->SetTimeStamp( GetNewTimeStamp() );
                marker->SetMarkerType( MARKER_BASE::MARKER_ERC );
                marker->SetErrorLevel( MARKER_BASE::MARKER_SEVERITY_WARNING );
                marker->SetData( ERCE_NOCONNECT_CONNECTED, pos, msg, pos );

                screen->Append( marker );
            }
        }
#endif
    }

    return error_count;
}


bool CONNECTION_GRAPH::ercCheckBusToNetConflicts( CONNECTION_SUBGRAPH* aSubgraph,
                                                  bool aCreateMarkers )
{
    wxString msg;
    auto sheet = aSubgraph->m_sheet;
    auto screen = sheet.LastScreen();
    auto driver = aSubgraph->m_driver;

    SCH_ITEM* net_item = nullptr;
    SCH_ITEM* bus_item = nullptr;

    for( auto item : aSubgraph->m_items )
    {
        switch( item->Type() )
        {
        case SCH_LINE_T:
        {
            if( item->GetLayer() == LAYER_BUS )
                bus_item = ( !bus_item ) ? item : bus_item;
            else
                net_item = ( !net_item ) ? item : net_item;
            break;
        }

        case SCH_GLOBAL_LABEL_T:
        case SCH_SHEET_PIN_T:
        case SCH_HIERARCHICAL_LABEL_T:
        {
            auto text = static_cast<SCH_TEXT*>( driver );
            SCH_CONNECTION conn;
            conn.ConfigureFromLabel( text->GetText() );

            if( conn.IsBus() )
                bus_item = ( !bus_item ) ? item : bus_item;
            else
                net_item = ( !net_item ) ? item : net_item;
            break;
        }

        default:
            break;
        }
    }

    if( net_item && bus_item )
    {
        msg.Printf( _( "%s and %s are graphically connected but cannot"
                       " electrically connect because one is a bus and"
                       " the other is a net." ),
                    bus_item->GetSelectMenuText(),
                    net_item->GetSelectMenuText() );

        auto marker = new SCH_MARKER();
        marker->SetTimeStamp( GetNewTimeStamp() );
        marker->SetMarkerType( MARKER_BASE::MARKER_ERC );
        marker->SetErrorLevel( MARKER_BASE::MARKER_SEVERITY_ERROR );
        marker->SetData( ERCE_BUS_TO_NET_CONFLICT,
                         net_item->GetPosition(), msg,
                         bus_item->GetPosition() );

        screen->Append( marker );

        return false;
    }

    return true;
}


bool CONNECTION_GRAPH::ercCheckBusToBusConflicts( CONNECTION_SUBGRAPH* aSubgraph,
                                                  bool aCreateMarkers )
{
    wxString msg;
    auto sheet = aSubgraph->m_sheet;
    auto screen = sheet.LastScreen();

    SCH_ITEM* label = nullptr;
    SCH_ITEM* port = nullptr;

    for( auto item : aSubgraph->m_items )
    {
        switch( item->Type() )
        {
        case SCH_TEXT_T:
        case SCH_GLOBAL_LABEL_T:
        {
            if( item->Connection( sheet )->IsBus() )
                label = ( !label ) ? item : label;
            break;
        }

        case SCH_SHEET_PIN_T:
        case SCH_HIERARCHICAL_LABEL_T:
        {
            if( item->Connection( sheet )->IsBus() )
                port = ( !port ) ? item : port;
            break;
        }

        default:
            break;
        }
    }

    if( label && port )
    {
        bool match = false;

        for( auto member : label->Connection( sheet )->Members() )
        {
            for( auto test : port->Connection( sheet )->Members() )
            {
                if( test != member && member->Name() == test->Name() )
                {
                    match = true;
                    break;
                }
            }

            if( match )
                break;
        }

        if( !match )
        {
            msg.Printf( _( "%s and %s are graphically connected but do "
                           "not share any bus members" ),
                        label->GetSelectMenuText(),
                        port->GetSelectMenuText() );

            auto marker = new SCH_MARKER();
            marker->SetTimeStamp( GetNewTimeStamp() );
            marker->SetMarkerType( MARKER_BASE::MARKER_ERC );
            marker->SetErrorLevel( MARKER_BASE::MARKER_SEVERITY_ERROR );
            marker->SetData( ERCE_BUS_TO_BUS_CONFLICT,
                             label->GetPosition(), msg,
                             port->GetPosition() );

            screen->Append( marker );

            return false;
        }
    }

    return true;
}
