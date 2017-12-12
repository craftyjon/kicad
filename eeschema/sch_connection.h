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

#ifndef _SCH_CONNECTION_H
#define _SCH_CONNECTION_H

#include <memory>
#include <unordered_set>

#include <boost/optional.hpp>

#include <bus_alias.h>
#include <msgpanel.h>


class CONNECTABLE_ITEM;
class EDA_ITEM;


enum CONNECTION_TYPE
{
    CONNECTION_NONE,    ///< No connection to this item
    CONNECTION_NET,     ///< This item represents a net
    CONNECTION_BUS,     ///< This item represents a bus vector
    CONNECTION_BUS_GROUP,   ///< This item represents a bus group
};

/**
 * Each graphical item can have a SCH_CONNECTION describing its logical
 * connection (to a bus or net).  These are generated when netlisting, or when
 * editing operations that can change the netlist are performed.
 *
 * These objects are used to implement the bus unfolding and net highlighting
 * features, not the final netlist (at least for now).
 */
class SCH_CONNECTION
{
public:
    SCH_CONNECTION( CONNECTABLE_ITEM* aParent ) :
        m_parent( aParent )
    {
        Reset();
    }

    ~SCH_CONNECTION()
    {}

    bool operator==( const SCH_CONNECTION& aOther ) const;

    bool operator!=( const SCH_CONNECTION& aOther ) const;

    /**
     * Configures the connection given a label.
     * For CONNECTION_NET, this just sets the name.
     * For CONNECTION_BUS, this will deduce the correct BUS_TYPE and also
     * generate a correct list of members.
     */
    void ConfigureFromLabel( wxString aLabel );

    void Reset();

    void Clone( SCH_CONNECTION& aOther );

    CONNECTABLE_ITEM* Parent() const
    {
        return m_parent;
    }

    CONNECTABLE_ITEM* Driver() const
    {
        return m_driver;
    }

    void SetDriver( CONNECTABLE_ITEM* aItem )
    {
        m_driver = aItem;
    }

    bool IsDriver() const;

    bool IsBus() const
    {
        return ( m_type == CONNECTION_BUS || m_type == CONNECTION_BUS_GROUP );
    }

    bool IsNet() const
    {
        return ( m_type == CONNECTION_NET );
    }

    bool IsDirty() const
    {
        return m_dirty;
    }

    void SetDirty()
    {
        m_dirty = true;
    }

    void ClearDirty()
    {
        m_dirty = false;
    }

    wxString Name() const
    {
        return m_name;
    }

    wxString Prefix() const
    {
        return m_prefix;
    }

    void SetPrefix( wxString aPrefix )
    {
        m_prefix = aPrefix;
    }

    CONNECTION_TYPE Type() const
    {
        return m_type;
    }

    void SetType( CONNECTION_TYPE aType )
    {
        m_type = aType;
    }

    int NetCode() const
    {
        return m_net_code;
    }

    void SetNetCode( int aCode )
    {
        m_net_code = aCode;
    }

    int BusCode() const
    {
        return m_bus_code;
    }

    void SetBusCode( int aCode )
    {
        m_bus_code = aCode;
    }

    int SubgraphCode() const
    {
        return m_subgraph_code;
    }

    void SetSubgraphCode( int aCode )
    {
        m_subgraph_code = aCode;
    }

    long VectorStart() const
    {
        return m_vector_start;
    }

    long VectorEnd() const
    {
        return m_vector_end;
    }

    long VectorIndex() const
    {
        return m_vector_index;
    }

    std::vector< std::shared_ptr< SCH_CONNECTION > >& Members()
    {
        return m_members;
    }

    const std::vector< std::shared_ptr< SCH_CONNECTION > >& Members() const
    {
        return m_members;
    }

    /**
     * Parses a bus vector (e.g. A[7..0]) into name, begin, and end.
     * Ensures that begin and end are positive and that end > begin.
     *
     * @param vector is a bus vector label string
     * @param name output of the name portion of the label
     * @param begin is the first entry in the vector
     * @param end is the last entry in the vector
     */
    static void ParseBusVector( wxString vector, wxString* name, long* begin, long* end );

    /**
     * Parses a bus group label into the name and a list of components
     *
     * @param aGroup is the input label, e.g. "USB{DP DM}"
     * @param name is the output group name, e.g. "USB"
     * @param aMemberList is a list of member strings, e.g. "DP", "DM"
     * @return true if aGroup was successfully parsed
     */
    static bool ParseBusGroup( wxString aGroup, wxString* name, std::vector<wxString>& aMemberList );

    /**
     * Adds information about the connection object to aList
     */
    void AppendDebugInfoToMsgPanel( MSG_PANEL_ITEMS& aList ) const;

private:

    bool m_dirty;

    CONNECTABLE_ITEM* m_parent;     ///< The CONNECTABLE_ITEM this connection is owned by

    CONNECTABLE_ITEM* m_driver;     ///< The CONNECTABLE_ITEM that drives this connection's net

    CONNECTION_TYPE m_type; ///< @see enum CONNECTION_TYPE

    wxString m_name;        ///< Name of the bus.

    ///< Prefix if connection is member of a labeled bus group (or "" if not)
    wxString m_prefix;

    int m_net_code;         // TODO(JE) remove if unused

    int m_bus_code;         // TODO(JE) remove if unused

    int m_subgraph_code;    ///< Groups directly-connected items

    long m_vector_index;    ///< Index of bus vector member nets

    long m_vector_start;    ///< Highest member of a vector bus

    long m_vector_end;      ///< Lowest member of a vector bus

    // For bus connections, store a list of member connections
    std::vector< std::shared_ptr< SCH_CONNECTION > > m_members;

};

/**
 * Mix-in class to add SCH_CONNECTION to SCH_ITEM and LIB_ITEM
 * @details [long description]
 *
 */
class CONNECTABLE_ITEM
{
protected:
    /// Stores net/bus information for items that have it (schematic only)
    boost::optional<SCH_CONNECTION> m_connection;

    /// Stores pointers to other items that are connected to this one (schematic only)
    std::unordered_set<CONNECTABLE_ITEM*> m_connected_items;

public:
    /**
     * Retrieves the connection associated with this object, if it exists
     *
     * Used for schematic only.  Lives in EDA_ITEM because it's needed by both
     * SCH_ITEM and LIB_ITEM.  If those classes are ever unified, this can be
     * hoisted out.
     */
    virtual boost::optional<SCH_CONNECTION>& Connection()
    {
        return m_connection;
    }

    /**
     * Retrieves the set of items connected to this item (schematic only)
     *
     * Used for schematic only.  Lives in EDA_ITEM because it's needed by both
     * SCH_ITEM and LIB_ITEM.  If those classes are ever unified, this can be
     * hoisted out.
     */
    virtual std::unordered_set<CONNECTABLE_ITEM*>& ConnectedItems()
    {
        return m_connected_items;
    }

    /**
     * Adds a connection link between this item and another
     *
     * Used for schematic only.  Lives in EDA_ITEM because it's needed by both
     * SCH_ITEM and LIB_ITEM.  If those classes are ever unified, this can be
     * hoisted out.
     */
    virtual void AddConnectionTo( CONNECTABLE_ITEM* aItem )
    {
        m_connected_items.insert( aItem );
    }

    /**
     * Creates a new connection object associated with this object
     *
     * Used for schematic only.  Lives in EDA_ITEM because it's needed by both
     * SCH_ITEM and LIB_ITEM.  If those classes are ever unified, this can be
     * hoisted out.
     */
    virtual void InitializeConnection( CONNECTABLE_ITEM* aParent = nullptr )
    {
        if( aParent == nullptr )
            aParent = this;

        m_connection = SCH_CONNECTION( aParent );
    }

    /**
     * Returns true if this item should propagate connection info to aItem
     */
    virtual bool ConnectionPropagatesTo( const EDA_ITEM* aItem ) const { return true; }
};

/**
 * Test if \a aLabel has a bus notation.
 *
 * @param aLabel A wxString object containing the label to test.
 * @return true if text is a bus notation format otherwise false is returned.
 */
extern bool IsBusLabel( const wxString& aLabel );

/**
 * Test if \a aLabel has a bus vector notation (simple bus, e.g. A[7..0])
 *
 * @param aLabel A wxString object containing the label to test.
 * @return true if text is a bus notation format otherwise false is returned.
 */
extern bool IsBusVectorLabel( const wxString& aLabel );

/**
 * Test if \a aLabel has a bus group notation.
 *
 * @param aLabel A wxString object containing the label to test.
 * @return true if text is a bus group notation format
 */
extern bool IsBusGroupLabel( const wxString& aLabel );

#endif

