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

#include <vector>

#include <common.h>
#include <sch_connection.h>
#include <sch_item_struct.h>


/**
 * A subgraph is a set of items that are "physically" connected in the schematic.
 *
 * For example, a label connected to a wire and so on.
 * A net is composed of one or more subgraphs.
 *
 * A set of items that appears to be physically connected may actually be more
 * than one subgraph, because some items don't connect electrically.
 *
 * For example, multiple bus wires can come together at a junction but have
 * different labels on each branch.  Each label+wire branch is its own subgraph.
 *
 */
class CONNECTION_SUBGRAPH
{
public:
    /**
     * Determines which potential driver should drive the subgraph.
     *
     * If multiple possible drivers exist, picks one according to the priority.
     * If multiple "winners" exist, returns false and sets m_driver to nullptr.
     *
     * @return true if m_driver was set, or false if a conflict occurred
     */
    bool ResolveDrivers();

    long m_code;

    /// No-connect item in graph, if any
    SCH_ITEM* m_no_connect;

    std::vector<SCH_ITEM*> m_items;

    std::vector<SCH_ITEM*> m_drivers;

    SCH_ITEM* m_driver;

    SCH_SHEET_PATH m_sheet;
};


/**
 * Calculates the connectivity of a schematic and generates netlists
 */
class CONNECTION_GRAPH
{
public:
    CONNECTION_GRAPH() {}

    void Reset();

    /**
     * Updates the physical connectivity between items (i.e. where they touch)
     * The items passed in must be on the same sheet.
     *
     * As a side effect, loads the items into m_items for BuildConnectionGraph()
     *
     * @param aSheet is the path to the sheet of all items in the list
     * @param aItemList is a list of items to consider
     */
    void UpdateItemConnectivity( const SCH_SHEET_PATH aSheet,
                                 std::vector<SCH_ITEM*> aItemList );

    /**
     * Generates connectivity (using CONNECTION_SUBGRAPH) for all items
     */
    void BuildConnectionGraph();

    /**
     * Updates the connectivity graph based on a single item
     */
    void RebuildGraphForItem( SCH_ITEM* aItem );

private:

    std::vector<SCH_ITEM*> m_items;

    std::unordered_set<CONNECTION_SUBGRAPH*> m_subgraphs;

    std::map<wxString, int> m_net_name_to_code_map;

    std::map<wxString, int> m_bus_name_to_code_map;

    std::unordered_map<int, CONNECTION_SUBGRAPH*> m_subgraph_code_map;

    int m_last_net_code;

    int m_last_bus_code;
};

#endif
