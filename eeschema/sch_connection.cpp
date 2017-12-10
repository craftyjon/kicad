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

#include <sch_connection.h>
#include <class_netlist_object.h>   // IsBusVectorLabel() / IsBusGroupLabel()
#include <class_sch_screen.h>


bool SCH_CONNECTION::operator==( const SCH_CONNECTION& aOther ) const
{
    // NOTE: Not comparing m_dirty or net/bus/subgraph codes
    if( ( aOther.m_driver == m_driver ) &&
        ( aOther.m_type == m_type ) &&
        ( aOther.m_name == m_name ) )
    {
        return true;
    }

    return false;
}


bool SCH_CONNECTION::operator!=( const SCH_CONNECTION& aOther ) const
{
    return !( aOther == *this );
}


void SCH_CONNECTION::ConfigureFromLabel( wxString aLabel )
{
    if( IsBusVectorLabel( aLabel ) )
    {
        m_name = aLabel;
        m_type = CONNECTION_BUS;

        wxString prefix;
        NETLIST_OBJECT::ParseBusVector( aLabel, &prefix,
                                        &m_vector_start, &m_vector_end );

        for( long i = m_vector_start; i <= m_vector_end; ++i )
        {
            auto member = std::make_shared< SCH_CONNECTION >( m_parent );
            wxString name = prefix;
            name << i;
            member->m_name = m_prefix + name;
            member->m_vector_index = i;
            m_members.push_back( member );
        }
    }
    else if( IsBusGroupLabel( aLabel ) )
    {
        m_type = CONNECTION_BUS_GROUP;
        m_name = aLabel;

        std::vector<wxString> members;
        wxString group_name;

        if( NETLIST_OBJECT::ParseBusGroup( aLabel, &group_name, members) )
        {
            // Named bus groups generate a net prefix, unnamed ones don't
            auto prefix = ( group_name != "" ) ? ( group_name + "." ) : "";

            for( auto group_member : members )
            {
                // Handle alias inside bus group member list
                if( auto alias = SCH_SCREEN::GetBusAlias( group_member ) )
                {
                    for( auto alias_member : alias->Members() )
                    {
                        auto member = std::make_shared< SCH_CONNECTION >( m_parent );
                        member->SetPrefix( prefix );
                        member->ConfigureFromLabel( alias_member );
                        m_members.push_back( member );
                    }
                }
                else
                {
                    auto member = std::make_shared< SCH_CONNECTION >( m_parent );
                    member->SetPrefix( prefix );
                    member->ConfigureFromLabel( group_member );
                    m_members.push_back( member );
                }
            }
        }
    }
    else if( auto alias = SCH_SCREEN::GetBusAlias( aLabel ) )
    {
        m_type = CONNECTION_BUS_GROUP;
        m_name = aLabel;

        for( auto alias_member : alias->Members() )
        {
            auto member = std::make_shared< SCH_CONNECTION >( m_parent );
            member->ConfigureFromLabel( alias_member );
            m_members.push_back( member );
        }
    }
    else
    {
        m_name = m_prefix + aLabel;
        m_type = CONNECTION_NET;
    }
}


void SCH_CONNECTION::Reset()
{
    m_type = CONNECTION_NONE;
    m_name = "<NO NET>";
    m_prefix = "";
    m_driver = nullptr;
    m_members.clear();
    m_dirty = true;
    m_net_code = 0;
    m_bus_code = 0;
    m_subgraph_code = 0;
    m_vector_start = 0;
    m_vector_end = 0;
    m_vector_index = 0;
}


void SCH_CONNECTION::Clone( SCH_CONNECTION& aOther )
{
    m_type = aOther.Type();
    m_driver = aOther.Driver();
    m_name = aOther.Name();
    m_prefix = aOther.Prefix();
    m_members = aOther.Members();
    m_net_code = aOther.NetCode();
    m_bus_code = aOther.BusCode();
    m_subgraph_code = aOther.SubgraphCode();
    m_vector_start = aOther.VectorStart();
    m_vector_end = aOther.VectorEnd();
    m_vector_index = aOther.VectorIndex();
}


void SCH_CONNECTION::AppendDebugInfoToMsgPanel( MSG_PANEL_ITEMS& aList ) const
{
    wxString msg;
    msg.Printf( "%d", m_net_code );
    aList.push_back( MSG_PANEL_ITEM( _( "Net Code" ), msg, BROWN ) );

    msg.Printf( "%d", m_bus_code );
    aList.push_back( MSG_PANEL_ITEM( _( "Bus Code" ), msg, BROWN ) );

    msg.Printf( "%d", m_subgraph_code );
    aList.push_back( MSG_PANEL_ITEM( _( "Subgraph Code" ), msg, BROWN ) );

    if( Driver() )
    {
        msg.Printf( "%s at %p", Driver()->GetSelectMenuText(), Driver() );
        aList.push_back( MSG_PANEL_ITEM( _( "Connection Source" ), msg, RED ) );
    }
}
