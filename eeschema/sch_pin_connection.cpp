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

#include <lib_pin.h>
#include <sch_component.h>
#include <sch_pin_connection.h>


SCH_PIN_CONNECTION::SCH_PIN_CONNECTION( EDA_ITEM* aParent ) :
    SCH_ITEM( aParent, SCH_PIN_CONNECTION_T )
{
}


wxString SCH_PIN_CONNECTION::GetSelectMenuText() const
{
    wxString tmp;

#ifdef DEBUG
    tmp.Printf( _( "SCH_PIN_CONNECTION for %s %s" ),
                GetChars( m_comp->GetSelectMenuText() ),
                GetChars( m_pin->GetSelectMenuText() ) );
#else
    tmp.Printf( _( "%s %s" ),
                GetChars( m_comp->GetSelectMenuText() ),
                GetChars( m_pin->GetSelectMenuText() ) );
#endif

    return tmp;
}


wxString SCH_PIN_CONNECTION::GetDefaultNetName()
{
    if( m_pin->IsPowerConnection() )
        return m_pin->GetName();

    wxString name = wxT( "Net-(" );

    // TODO(JE) Uncomment once we have the sheet path
    //name << m_comp->GetRef( &m_netNameCandidate->m_SheetPath );

    // if( adoptTimestamp && netName.Last() == '?' )
    //     netName << link->GetTimeStamp();

    name << _( "-Pad" ) << m_pin->GetNumber() << _( ")" );

    return name;
}
