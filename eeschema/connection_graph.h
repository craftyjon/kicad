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

#ifndef _CONNECTION_GRAPH_H
#define _CONNECTION_GRAPH_H


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <utility>
#include <algorithm>

#include <sch_item_struct.h>


/**
 * Define a hash operator for wxPoint so it can be used as a std::map key
 */
namespace std {

    template <>
    struct hash<wxPoint>
    {
        std::size_t operator() ( const wxPoint& k ) const
        {
            return ( ( std::hash<int>()( k.x )
                     ^ ( std::hash<int>()( k.y ) << 1 ) ) >> 1 );
        }
    };
}


/**
 * Properties to attach to each connection graph vertex
 * Right now, we just hold a pointer back to the item the vertex refers to
 */
struct CONNECTION_VERTEX_PROPS
{
    SCH_ITEM* item;
};


/**
 * Properties to attach to each connection graph edge
 * NOTE: this data is not currently used
 */
struct CONNECTION_EDGE_PROPS
{
    wxString name;
};


typedef boost::adjacency_list< boost::listS, boost::vecS, boost::undirectedS,
                               CONNECTION_VERTEX_PROPS,
                               CONNECTION_EDGE_PROPS > CONNECTION_GRAPH_T;


typedef boost::graph_traits< CONNECTION_GRAPH_T >::vertex_descriptor CONNECTION_VERTEX;


typedef boost::graph_traits< CONNECTION_GRAPH_T >::vertex_iterator CONNECTION_VERTEX_ITERATOR;


typedef boost::graph_traits< CONNECTION_GRAPH_T >::edge_iterator CONNECTION_EDGE_ITERATOR;


/**
 * A map to look up graph vertices by item
 */
typedef std::unordered_map< SCH_ITEM*, CONNECTION_VERTEX > VERTEX_MAP_T;


/**
 * An index map for BFS to keep track of visited vertices
 */
typedef std::unordered_map< CONNECTION_VERTEX, size_t > VERTEX_INDEX_MAP_T;


/**
 * This visitor is responsible for propagating connection updates through the
 * graph when information changes.
 */
class CONNECTION_VISITOR : public boost::default_bfs_visitor
{
public:

    CONNECTION_VISITOR( SCH_CONNECTION aConnection );

    /**
     * Visitor function called for each edge during the search.
     * @param aEdge is the edge (source->target) to visit
     * @param aGraph is the graph being searched
     */
    void tree_edge( const CONNECTION_GRAPH_T::edge_descriptor aEdge,
                    const CONNECTION_GRAPH_T& aGraph );

private:

    /// This connection is the "template" to propagate through the graph
    SCH_CONNECTION m_connection;

    /// This ID will represent a directly-connected portion of the graph
    int m_subgraph_code;
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
