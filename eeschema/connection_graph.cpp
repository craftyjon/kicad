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


void CONNECTION_GRAPH::UpdateItemConnectivity( const SCH_SHEET* aSheet,
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
                // TODO(JE) casting-away constness because SCH_SHEET_LIST
                // can't deal with const but other things require it...
                SCH_SHEET_LIST path( ( SCH_SHEET* )aSheet );
                pin_connection->GetDefaultNetName( &path.at( 0 ) );

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

    long subgraph_code = 1;
    vector<CONNECTION_SUBGRAPH> subgraphs;

    for( auto item : m_items )
    {
        for( auto it : item->m_connection_map )
        {
            auto sheet = it.first;
            auto connection = it.second;

            if( connection->SubgraphCode() == 0 )
            {
                CONNECTION_SUBGRAPH subgraph = {};

                subgraph.m_code = subgraph_code++;
                subgraph.m_sheet = sheet;

                subgraph.m_items.push_back( item );

                // std::cout << "SG " << subgraph.m_code << " started with "
                //           << item->GetSelectMenuText() << std::endl;

                if( connection->IsDriver() )
                    subgraph.m_drivers.push_back( item );

                if( item->Type() == SCH_NO_CONNECT_T )
                    subgraph.m_no_connect = item;

                connection->SetSubgraphCode( subgraph.m_code );

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
                        subgraph.m_no_connect = connected_item;

                    auto connected_conn = connected_item->Connection( sheet );

                    wxASSERT( connected_conn );

                    if( connected_conn->SubgraphCode() == 0 )
                    {
                        connected_conn->SetSubgraphCode( subgraph.m_code );
                        subgraph.m_items.push_back( connected_item );

                        // std::cout << "   +" << connected_item->GetSelectMenuText() << std::endl;

                        if( connected_conn->IsDriver() )
                            subgraph.m_drivers.push_back( connected_item );

                        members.insert( members.end(),
                                        connected_item->ConnectedItems().begin(),
                                        connected_item->ConnectedItems().end() );
                    }
                }

                subgraphs.push_back( subgraph );
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
    for( auto it = subgraphs.begin(); it < subgraphs.end(); it++ )
    {
        auto subgraph = *it;

        if( !subgraph.ResolveDrivers() )
        {
            // TODO(JE) ERC Error: multiple equivalent drivers
        }
        else
        {
            if( subgraph.m_no_connect )
            {
                // TODO(JE) ERC check that the only other thing in the graph
                // is a component pin
                continue;
            }

            // Now the subgraph has only one driver
            auto driver = subgraph.m_driver;
            auto sheet = subgraph.m_sheet;
            auto connection = driver->Connection( subgraph.m_sheet );

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
                    auto path = SCH_SHEET_LIST( ( SCH_SHEET* )sheet );

                    // NOTE(JE) GetDefaultNetName is not thread-safe.

                    connection->ConfigureFromLabel( pin->GetDefaultNetName( &path.at( 0 ) ) );
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

            for( auto item : subgraph.m_items )
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
        }
    }

    phase2.Stop();
    std::cout << "BuildConnectionGraph() " <<  phase2.msecs() << " ms" << std::endl;
}


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
        m_driver = candidates[0];

        if( candidates.size() > 1 )
        {
            if( highest_priority == 1 )
            {
                // We have multiple options and they are all component pins.
                SCH_SHEET_LIST list( ( SCH_SHEET* )m_sheet );
                auto path = list.at( 0 );

                std::sort( candidates.begin(), candidates.end(),
                           [path]( SCH_ITEM* a, SCH_ITEM* b) -> bool
                            {
                                auto pin_a = static_cast<SCH_PIN_CONNECTION*>( a );
                                auto pin_b = static_cast<SCH_PIN_CONNECTION*>( b );

                                auto name_a = pin_a->GetDefaultNetName( &path );
                                auto name_b = pin_b->GetDefaultNetName( &path );

                                return name_a > name_b;
                            } );
            }
            else
            {
                // TODO(JE) ERC warning about multiple drivers?
            }
        }
    }
    else
    {
        std::cout << "Warning: could not resolve drivers for SG " << m_code << std::endl;
        for( auto item : m_items )
        {
            std::cout << "    " << item->GetSelectMenuText() << std::endl;
        }
    }

    return ( m_driver != nullptr );
}
