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
#include <class_sch_screen.h>

#include <wx/regex.h>
#include <wx/tokenzr.h>


/**
 *
 * Buses can be defined in multiple ways. A bus vector consists of a prefix and
 * a numeric range of suffixes:
 *
 *     BUS_NAME[M..N]
 *
 * For example, the bus A[3..0] will contain nets A3, A2, A1, and A0.
 * The BUS_NAME is required.  M and N must be integers but do not need to be in
 * any particular order -- A[0..3] produces the same result.
 *
 * Like net names, bus names cannot contain whitespace.
 *
 * A bus group is just a grouping of signals, separated by spaces, some
 * of which may be bus vectors.  Bus groups can have names, but do not need to.
 *
 *     MEMORY{A[15..0] D[7..0] RW CE OE}
 *
 * In named bus groups, the net names are expanded as <BUS_NAME>.<NET_NAME>
 * In the above example, the nets would be named like MEMORY.A15, MEMORY.D0, etc.
 *
 *     {USB_DP USB_DN}
 *
 * In the above example, the bus is unnamed and so the underlying net names are
 * just USB_DP and USB_DN.
 *
 */
static wxRegEx busLabelRe( wxT( "^([^[:space:]]+)(\\[[\\d]+\\.+[\\d]+\\])$" ), wxRE_ADVANCED );
static wxRegEx busGroupLabelRe( wxT( "^([^[:space:]]+)?\\{((?:[^[:space:]]+(?:\\[[\\d]+\\.+[\\d]+\\])? ?)+)\\}$" ), wxRE_ADVANCED );


bool IsBusLabel( const wxString& aLabel )
{
    return IsBusVectorLabel( aLabel ) || IsBusGroupLabel( aLabel );
}


bool IsBusVectorLabel( const wxString& aLabel )
{
    wxCHECK_MSG( busLabelRe.IsValid(), false,
                 wxT( "Invalid regular expression in IsBusLabel()." ) );

    return busLabelRe.Matches( aLabel );
}


bool IsBusGroupLabel( const wxString& aLabel )
{
    wxCHECK_MSG( busGroupLabelRe.IsValid(), false,
                 wxT( "Invalid regular expression in IsBusGroupLabel()." ) );

    return busGroupLabelRe.Matches( aLabel );
}


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
        ParseBusVector( aLabel, &prefix, &m_vector_start, &m_vector_end );

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

        if( ParseBusGroup( aLabel, &group_name, members) )
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


bool SCH_CONNECTION::IsDriver() const
{
    wxASSERT( Parent() );

    switch( Parent()->Type() )
    {
    case SCH_LABEL_T:
    case SCH_GLOBAL_LABEL_T:
    case SCH_HIERARCHICAL_LABEL_T:
    case SCH_PIN_CONNECTION_T:
    case SCH_SHEET_PIN_T:
    case SCH_SHEET_T:
    case LIB_PIN_T:
        return true;

    default:
        return false;
    }
}


void SCH_CONNECTION::AppendDebugInfoToMsgPanel( MSG_PANEL_ITEMS& aList ) const
{
    wxString msg;

    aList.push_back( MSG_PANEL_ITEM( _( "Connection Name" ), m_name, BROWN ) );

    msg.Printf( "%d", m_net_code );
    aList.push_back( MSG_PANEL_ITEM( _( "Net Code" ), msg, BROWN ) );

    msg.Printf( "%d", m_bus_code );
    aList.push_back( MSG_PANEL_ITEM( _( "Bus Code" ), msg, BROWN ) );

    msg.Printf( "%d", m_subgraph_code );
    aList.push_back( MSG_PANEL_ITEM( _( "Subgraph Code" ), msg, BROWN ) );

    if( auto driver = Driver() )
    {
        msg.Printf( "%s at %p", driver->GetSelectMenuText(), driver );
        aList.push_back( MSG_PANEL_ITEM( _( "Connection Source" ), msg, RED ) );
    }
}


void SCH_CONNECTION::ParseBusVector( wxString vector, wxString* name,
                                     long* begin, long* end )
{
    wxCHECK_RET( IsBusLabel( vector ),
                 wxT( "<" ) + vector + wxT( "> is not a valid bus vector." ) );

    *name = busLabelRe.GetMatch( vector, 1 );
    wxString numberString = busLabelRe.GetMatch( vector, 2 );

    // numberString will include the brackets, e.g. [5..0] so skip the first one
    size_t i = 1, len = numberString.Len();
    wxString tmp;

    while( i < len && numberString[i] != '.' )
    {
        tmp.Append( numberString[i] );
        i++;
    }

    tmp.ToLong( begin );

    while( i < len && numberString[i] == '.' )
        i++;

    tmp.Empty();

    while( i < len && numberString[i] != ']' )
    {
        tmp.Append( numberString[i] );
        i++;
    }

    tmp.ToLong( end );

    if( *begin < 0 )
        *begin = 0;

    if( *end < 0 )
        *end = 0;

    if( *begin > *end )
        std::swap( *begin, *end );
}


bool SCH_CONNECTION::ParseBusGroup( wxString aGroup, wxString* name,
                                    std::vector<wxString>& aMemberList )
{
    if( !IsBusGroupLabel( aGroup ) )
    {
        return false;
    }

    *name = busGroupLabelRe.GetMatch( aGroup, 1 );
    auto contents = busGroupLabelRe.GetMatch( aGroup, 2 );

    wxStringTokenizer tokenizer( contents, " " );
    while( tokenizer.HasMoreTokens() )
    {
        aMemberList.push_back( tokenizer.GetNextToken() );
    }

    return true;
}
