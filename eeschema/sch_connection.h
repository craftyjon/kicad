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

#include <sch_bus_alias.h>


class SCH_ITEM;


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
    SCH_CONNECTION( SCH_ITEM* aParent = NULL ) :
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

    wxString Name()
    {
        return m_name;
    }

    CONNECTION_TYPE Type()
    {
        return m_type;
    }

    void SetType( CONNECTION_TYPE aType )
    {
        m_type = aType;
    }

    long VectorStart()
    {
        return m_vector_start;
    }

    long VectorEnd()
    {
        return m_vector_end;
    }

    long VectorIndex()
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

private:

    bool m_dirty;

    SCH_ITEM* m_parent;

    CONNECTION_TYPE m_type; ///< @see enum CONNECTION_TYPE

    wxString m_name;        ///< Name of the bus.

    int m_net_code;         // TODO(JE) remove if unused

    int m_bus_code;         // TODO(JE) remove if unused

    long m_vector_index;    ///< Index of bus vector member nets

    long m_vector_start;    ///< Highest member of a vector bus

    long m_vector_end;      ///< Lowest member of a vector bus

    // For bus connections, store a list of member connections
    std::vector< std::shared_ptr< SCH_CONNECTION > > m_members;

};

#endif

