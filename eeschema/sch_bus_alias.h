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


class SCH_BUS_ALIAS
{
public:
    SCH_BUS_ALIAS();
    ~SCH_BUS_ALIAS();

    wxString GetName() { return m_name; }

    void SetName( const wxString& aName ) { m_name = aName; }

    int GetMemberCount() { return m_members.size(); }

    bool Contains( const wxString& aName );

private:

    wxString m_name;
    std::vector< wxString > m_members;
};

#endif
