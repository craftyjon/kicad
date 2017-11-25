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


CONNECTION_VISITOR::CONNECTION_VISITOR( SCH_CONNECTION aConnection ) :
    m_connection( aConnection )
{
}


void CONNECTION_VISITOR::tree_edge( const CONNECTION_GRAPH_T::edge_descriptor aEdge,
                                    const CONNECTION_GRAPH_T& aGraph )
{
    auto source_item = aGraph[ boost::source( aEdge, aGraph ) ].item;
    auto target_item = aGraph[ boost::target( aEdge, aGraph ) ].item;

    // Don't propagate across bus/net boundary
    if( ( m_connection.IsBus() && target_item->m_connection->IsNet() ) ||
        ( m_connection.IsNet() && target_item->m_connection->IsBus() ) )
    {
        return;
    }

    // Don't propagate when the source vertex didn't get set earlier
    if( ( source_item->m_connection == m_connection ) &&
        target_item->m_connection->IsDirty() )
    {
        target_item->m_connection = m_connection;
        target_item->m_connection->ClearDirty();
    }

    // std::cout << "source " << source_item->GetClass() << " " << source_item
    //           << " target " << target_item->GetClass() << " " << target_item
    //           << std::endl;
}
