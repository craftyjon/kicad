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

#ifndef _DIALOG_BUS_MANAGER_H_
#define _DIALOG_BUS_MANAGER_H_

#include "dialog_shim.h"

#include <schframe.h>
#include <wx/listctrl.h>

#include <sch_bus_alias.h>

class DIALOG_BUS_MANAGER : public DIALOG_SHIM
{
public:
    DIALOG_BUS_MANAGER( SCH_EDIT_FRAME* aParent );

    ~DIALOG_BUS_MANAGER() {}

    bool TransferDataFromWindow() override;

    bool TransferDataToWindow() override;

protected:

    void OnInitDialog( wxInitDialogEvent& aEvent );

    SCH_EDIT_FRAME* m_parent;

    wxListView* m_bus_list_view;
    wxListView* m_signal_list_view;
    wxTextCtrl* m_bus_edit;
    wxTextCtrl* m_signal_edit;

    wxButton* m_btn_add_bus;
    wxButton* m_btn_rename_bus;
    wxButton* m_btn_remove_bus;

    wxButton* m_btn_add_signal;
    wxButton* m_btn_rename_signal;
    wxButton* m_btn_remove_signal;

private:

    std::vector< SCH_BUS_ALIAS* > m_aliases;
};


#endif

// _DIALOG_BUS_MANAGER_H_
