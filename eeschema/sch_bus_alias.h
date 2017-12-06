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

#ifndef _SCH_BUS_ALIAS_H
#define _SCH_BUS_ALIAS_H

#include <algorithm>

#include <wx/string.h>
#include <wxstruct.h>


class SCH_SCREEN;


class SCH_BUS_ALIAS
{
public:
    SCH_BUS_ALIAS( SCH_SCREEN* aParent = NULL );

    ~SCH_BUS_ALIAS();

    std::shared_ptr< SCH_BUS_ALIAS > Clone() const
    {
        return std::make_shared< SCH_BUS_ALIAS >( *this );
    }

    wxString GetName()
    {
        return m_name;
    }

    void SetName( const wxString& aName )
    {
        m_name = aName;
    }

    void ClearMembers()
    {
        m_members.clear();
    }

    void AddMember( const wxString& aName )
    {
        m_members.push_back( aName );
    }

    int GetMemberCount()
    {
        return m_members.size();
    }

    std::vector< wxString >& Members()
    {
        return m_members;
    }

    bool Contains( const wxString& aName )
    {
        return ( std::find( m_members.begin(), m_members.end(), aName )
                 != m_members.end() );
    }

    SCH_SCREEN* GetParent()
    {
        return m_parent;
    }

    void SetParent( SCH_SCREEN* aParent )
    {
        m_parent = aParent;
    }

    void Show()
    {
#ifdef DEBUG
        std::cout << "Bus Alias: " << m_name << std::endl;
        for( const auto& member : m_members )
            std::cout << "    " << member << std::endl;
#endif
    }

protected:

    wxString m_name;

    std::vector< wxString > m_members;

    /**
     * The bus alias editor dialog can edit aliases from all open sheets.
     * This means we have to store a reference back to our parent so that
     * the dialog can update the parent if aliases are changed or removed.
     */
    SCH_SCREEN* m_parent;
};

#endif
