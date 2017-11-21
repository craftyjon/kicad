/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef _CONNECTION_GRAPH_H
#define _CONNECTION_GRAPH_H


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <utility>
#include <algorithm>


struct CONNECTION_VERTEX_PROPS
{
    SCH_ITEM* item;
};

struct CONNECTION_EDGE_PROPS
{
    wxString name;
};

typedef boost::adjacency_list< boost::listS, boost::listS, boost::undirectedS,
                               CONNECTION_VERTEX_PROPS,
                               CONNECTION_EDGE_PROPS > CONNECTION_GRAPH_T;

typedef boost::graph_traits< CONNECTION_GRAPH_T >::vertex_descriptor CONNECTION_VERTEX;

typedef boost::graph_traits< CONNECTION_GRAPH_T >::vertex_iterator CONNECTION_VERTEX_ITERATOR;

typedef boost::graph_traits< CONNECTION_GRAPH_T >::edge_iterator CONNECTION_EDGE_ITERATOR;

typedef std::map< SCH_ITEM*, CONNECTION_VERTEX > VERTEX_MAP_T;

typedef std::map< CONNECTION_VERTEX, size_t > VERTEX_INDEX_MAP_T;

class CONNECTION_VISITOR : public boost::default_bfs_visitor
{
public:
    CONNECTION_VISITOR( SCH_CONNECTION aConnection ) : m_connection( aConnection ) {}

    void tree_edge( const CONNECTION_GRAPH_T::edge_descriptor aEdge,
                    const CONNECTION_GRAPH_T& aGraph ) const
    {
        auto source_item = aGraph[ boost::source( aEdge, aGraph ) ].item;
        auto target_item = aGraph[ boost::target( aEdge, aGraph ) ].item;

        //if( source_item->m_connection_dirty )
        {
            //std::cout << "Updating " << source_item->GetClass() << std::endl;
            source_item->m_connection = m_connection;
            source_item->m_connection_dirty = false;
        }

        //if( target_item->m_connection_dirty )
        {
            //std::cout << "Updating " << target_item->GetClass() << std::endl;
            target_item->m_connection = m_connection;
            target_item->m_connection_dirty = false;
        }
    }

private:
    SCH_CONNECTION m_connection;
};


class CONNECTION_GRAPH
{
public:
    CONNECTION_GRAPH()
    {
        m_vertex_index_property_map = boost::associative_property_map< VERTEX_INDEX_MAP_T >( m_vertex_index_map );
    }

    ~CONNECTION_GRAPH()
    {

    }

    /**
     * Adds an item to the graph (with no connections)
     */
    void Add( SCH_ITEM* aItem );

    /**
     * Connects two members of the graph together
     * Make sure to call Add() first for both of the items
     */
    void Connect( SCH_ITEM* aFirstItem, SCH_ITEM* aSecondItem );

    /**
     * Sets the net name for all members of the graph
     */
    void PropagateNetName( const wxString aName );

    bool m_dirty;

    CONNECTION_GRAPH_T m_graph;

    VERTEX_MAP_T m_vertex_map;

    VERTEX_INDEX_MAP_T m_vertex_index_map;

    boost::associative_property_map< VERTEX_INDEX_MAP_T > m_vertex_index_property_map;

};

#endif
