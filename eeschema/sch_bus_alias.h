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

#ifndef _SCH_BUS_ALIAS_H
#define _SCH_BUS_ALIAS_H

#include <wxstruct.h>
#include <vector>

#include <class_sch_screen.h>

class SCH_SCREEN;


class SCH_BUS_ALIAS
{
public:
    SCH_BUS_ALIAS( SCH_SCREEN* aParent );
    ~SCH_BUS_ALIAS();

    wxString GetName() { return m_name; }

    void SetName( const wxString& aName ) { m_name = aName; }

    void AddMember( const wxString& aName ) { m_members.push_back( aName ); }

    int GetMemberCount() { return m_members.size(); }

    std::vector< wxString > GetMembers() { return m_members; }

    bool Contains( const wxString& aName );

    SCH_SCREEN* GetParent() { return m_parentScreen; }

    void SetParent( SCH_SCREEN* aParent ) { m_parentScreen = aParent; }

    void Show()
    {
        std::cout << "Bus Alias: " << m_name << std::endl;
        for( auto member : m_members )
            std::cout << "    " << member << std::endl;
    }

private:

    wxString m_name;

    std::vector< wxString > m_members;

    /**
     * The bus alias editor dialog can edit aliases from all open sheets.
     * This means we have to store a reference back to our parent so that
     * the dialog can update the parent if aliases are changed or removed.
     */
    SCH_SCREEN* m_parentScreen;
};

#endif
