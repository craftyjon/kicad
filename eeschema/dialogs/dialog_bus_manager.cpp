/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 Jon Evans <jon@craftyjon.com>
 * Copyright (C) 2017 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dialog_bus_manager.h"

#include <invoke_sch_dialog.h>


DIALOG_BUS_MANAGER::DIALOG_BUS_MANAGER( SCH_EDIT_FRAME* aParent )
        : DIALOG_SHIM( aParent, wxID_ANY, _( "Bus Definitions" ),
                       wxDefaultPosition, wxSize( 800, 650 ),
                       wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ),
          m_parent( aParent )
{
    auto sizer = new wxBoxSizer( wxVERTICAL );

    SetSizer( sizer );


    Layout();
}


void DIALOG_BUS_MANAGER::OnInitDialog( wxInitDialogEvent& aEvent )
{
}



void InvokeDialogBusManager( SCH_EDIT_FRAME* aCaller )
{
    DIALOG_BUS_MANAGER dlg( aCaller );
    dlg.ShowModal();
}
