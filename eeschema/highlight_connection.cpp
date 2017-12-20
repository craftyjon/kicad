/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 1992-2016 KiCad Developers, see AUTHORS.txt for contributors.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file highlight_connection.cpp
 * @brief This file contains basic functions related to the command to
 * highlight a connection (wires and labels) in a schematic
 * (that can be a simple or a complex hierarchy)
 */

#include <fctsys.h>
#include <class_drawpanel.h>
#include <schframe.h>
#include <erc.h>

#include <class_netlist_object.h>
#include <sch_component.h>


// TODO(JE) Probably use netcode rather than connection name here eventually
bool SCH_EDIT_FRAME::HighlightConnectionAtPosition( wxPoint aPosition )
{
    m_SelectedNetName = "";
    bool buildNetlistOk = false;

    SetStatusText( "" );

    // find which connected item is selected
    EDA_ITEMS nodeList;
    wxPoint   gridPosition = GetGridPosition( aPosition );

    if( GetScreen()->GetNode( gridPosition, nodeList ) )
    {
        if( TestDuplicateSheetNames( false ) > 0 )
            wxMessageBox( _( "Error: duplicate sub-sheet names found in current sheet. Fix it" ) );
        else
        {
            if( auto item = dynamic_cast<SCH_ITEM*>( nodeList[0] ) )
            {
                if( item->Connection( g_CurrentSheet->Last() ) )
                {
                    m_SelectedNetName = item->Connection( g_CurrentSheet->Last() )->Name();
                    SetStatusText( _( "Highlighted net: " ) + m_SelectedNetName );
                }
            }
        }
    }

    SetCurrentSheetHighlightFlags();
    m_canvas->Refresh();

    return buildNetlistOk;
}


bool SCH_EDIT_FRAME::SetCurrentSheetHighlightFlags()
{
    SCH_SCREEN* screen = g_CurrentSheet->LastScreen();

    // Update highlight flag on all items in the current screen
    for( SCH_ITEM* ptr = screen->GetDrawItems(); ptr; ptr = ptr->Next() )
    {
        auto conn = ptr->Connection( g_CurrentSheet->Last() );

        ptr->SetState( BRIGHTENED,
                       ( conn && conn->Name() == m_SelectedNetName ) );

        if( ptr->Type() == SCH_SHEET_T )
        {
            for( SCH_SHEET_PIN& pin : static_cast<SCH_SHEET*>( ptr )->GetPins() )
            {
                auto pin_conn = pin.Connection( g_CurrentSheet->Last() );

                pin.SetState( BRIGHTENED, ( pin_conn &&
                              pin_conn->Name() == m_SelectedNetName ) );
            }
        }
    }

    if( m_SelectedNetName == "" )
        return true;

    if( TestDuplicateSheetNames( false ) > 0 )
        return false;

    return true;
}
