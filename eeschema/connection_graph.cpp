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


#include <connection_graph.h>
#include <sch_pin_connection.h>

#if 0
CONNECTION_VISITOR::CONNECTION_VISITOR( SCH_CONNECTION aConnection ) :
    m_connection( aConnection )
{
}


void CONNECTION_VISITOR::tree_edge( const CONNECTION_GRAPH_T::edge_descriptor aEdge,
                                    const CONNECTION_GRAPH_T& aGraph )
{
    auto source_item = aGraph[ boost::source( aEdge, aGraph ) ].item;
    auto target_item = aGraph[ boost::target( aEdge, aGraph ) ].item;

    wxASSERT( source_item->Connection() );
    wxASSERT( target_item->Connection() );

    // Don't propagate across bus/net boundary
    if( ( m_connection.IsBus() && target_item->Connection()->IsNet() ) ||
        ( m_connection.IsNet() && target_item->Connection()->IsBus() ) )
    {
        return;
    }

    // TODO(JE) check for "drive strength" here:
    // if the target has a connection already, but my m_connection is stronger,
    // override the target.

    // std::cout << "source " << source_item->GetClass() << " " << source_item
    //           << " match_self " << ( source_item->Connection() == m_connection ) << std::endl;
    // std::cout << " target " << target_item->GetClass() << " " << target_item
    //           << " dirty " << target_item->Connection()->IsDirty() << std::endl;

    // Don't propagate when the source vertex didn't get set earlier
    if( ( source_item->Connection() == m_connection ) &&
        target_item->Connection()->IsDirty() )
    {
        target_item->Connection()->Clone( m_connection );
        target_item->Connection()->ClearDirty();
    }
}
#endif


bool CONNECTION_SUBGRAPH::ResolveDrivers()
{
    SCH_ITEM* candidate = nullptr;
    int highest_priority = -1;
    int num_items = 0;

    m_driver = nullptr;

    for( auto item : m_drivers )
    {
        int item_priority = 0;

        switch( item->Type() )
        {
        case LIB_PIN_T:                 item_priority = 1; break;
        case SCH_LABEL_T:               item_priority = 2; break;
        case SCH_HIERARCHICAL_LABEL_T:  item_priority = 3; break;
        case SCH_SHEET_PIN_T:           item_priority = 4; break;
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
            candidate = item;
            highest_priority = item_priority;
            num_items = 1;
        }
        else if( candidate && ( item_priority == highest_priority ) )
        {
            num_items++;
        }
    }

    if( num_items > 0 )
    {
        m_driver = candidate;

        if( num_items > 1 )
        {
            // TODO(JE) ERC warning about multiple drivers?
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
