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
#include <sch_sheet_path.h>


DIALOG_BUS_MANAGER::DIALOG_BUS_MANAGER( SCH_EDIT_FRAME* aParent )
        : DIALOG_SHIM( aParent, wxID_ANY, _( "Bus Definitions" ),
                       wxDefaultPosition, wxSize( 640, 480 ),
                       wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ),
          m_parent( aParent )
{
    auto sizer = new wxBoxSizer( wxVERTICAL );
    auto buttons = new wxStdDialogButtonSizer();

    buttons->AddButton( new wxButton( this, wxID_OK ) );
    buttons->AddButton( new wxButton( this, wxID_CANCEL ) );
    buttons->Realize();

    auto top_container = new wxBoxSizer( wxHORIZONTAL );
    auto left_pane = new wxBoxSizer( wxVERTICAL );
    auto right_pane = new wxBoxSizer( wxVERTICAL );

    // Left pane: alias list
    m_bus_list_view = new wxListView( this, wxID_ANY, wxDefaultPosition,
                                      wxSize( 300, 300 ), wxLC_ALIGN_LEFT |
                                      wxLC_NO_HEADER | wxLC_REPORT |
                                      wxLC_SINGLE_SEL );
    m_bus_list_view->InsertColumn( 0, "" );
    m_bus_edit = new wxTextCtrl( this, wxID_ANY );
    
    auto left_button_sizer = new wxBoxSizer( wxHORIZONTAL );
    
    m_btn_add_bus = new wxButton( this, wxID_ANY, _( "Add" ) );
    m_btn_rename_bus = new wxButton( this, wxID_ANY, _( "Rename" ) );
    m_btn_remove_bus = new wxButton( this, wxID_ANY, _( "Remove" ) );

    left_button_sizer->Add( m_btn_add_bus );
    left_button_sizer->Add( m_btn_rename_bus );
    left_button_sizer->Add( m_btn_remove_bus );

    left_pane->Add( m_bus_list_view, 1, wxEXPAND | wxALL, 5 );
    left_pane->Add( m_bus_edit, 0, wxEXPAND | wxALL, 5 );
    left_pane->Add( left_button_sizer, 0, wxEXPAND | wxALL, 5 );

    // Right pane: signal list
    m_signal_list_view = new wxListView( this, wxID_ANY, wxDefaultPosition,
                                         wxSize( 300, 300 ), wxLC_ALIGN_LEFT |
                                         wxLC_NO_HEADER | wxLC_REPORT |
                                         wxLC_SINGLE_SEL );
    m_signal_list_view->InsertColumn( 0, "" );
    m_signal_edit = new wxTextCtrl( this, wxID_ANY );

    auto right_button_sizer = new wxBoxSizer( wxHORIZONTAL );

    m_btn_add_signal = new wxButton( this, wxID_ANY, _( "Add" ) );
    m_btn_rename_signal = new wxButton( this, wxID_ANY, _( "Rename" ) );
    m_btn_remove_signal = new wxButton( this, wxID_ANY, _( "Remove" ) );

    right_button_sizer->Add( m_btn_add_signal );
    right_button_sizer->Add( m_btn_rename_signal );
    right_button_sizer->Add( m_btn_remove_signal );

    right_pane->Add( m_signal_list_view, 1, wxEXPAND | wxALL, 5 );
    right_pane->Add( m_signal_edit, 0, wxEXPAND | wxALL, 5 );
    right_pane->Add( right_button_sizer, 0, wxEXPAND | wxALL, 5 );

    top_container->Add( left_pane, 1, wxEXPAND );
    top_container->Add( right_pane, 1, wxEXPAND );

    sizer->Add( top_container, 1, wxEXPAND | wxALL, 5 );
    sizer->Add( buttons, 0, wxEXPAND | wxBOTTOM, 10 );
    SetSizer( sizer );

    Bind( wxEVT_INIT_DIALOG, &DIALOG_BUS_MANAGER::OnInitDialog, this );
    m_bus_list_view->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED,
            wxListEventHandler( DIALOG_BUS_MANAGER::OnSelectBus ), NULL, this );
    m_bus_list_view->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED,
            wxListEventHandler( DIALOG_BUS_MANAGER::OnSelectBus ), NULL, this );
    m_signal_list_view->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED,
            wxListEventHandler( DIALOG_BUS_MANAGER::OnSelectSignal ), NULL, this );
    m_signal_list_view->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED,
            wxListEventHandler( DIALOG_BUS_MANAGER::OnSelectSignal ), NULL, this );

    Layout();
}


void DIALOG_BUS_MANAGER::OnInitDialog( wxInitDialogEvent& aEvent )
{
    TransferDataToWindow();
}


bool DIALOG_BUS_MANAGER::TransferDataToWindow()
{
    // TODO(JE) Unclear whether we should reach straight into g_RootSheet here.
    m_aliases.clear();

    SCH_SHEET_LIST aSheets( g_RootSheet );

    for( unsigned i = 0; i < aSheets.size(); i++ )
    {
        auto sheet_aliases = aSheets[i].LastScreen()->GetBusAliases();

        m_aliases.insert( m_aliases.end(),
                          sheet_aliases.begin(), sheet_aliases.end() );
    }

    for( unsigned i = 0; i < m_aliases.size(); i++ )
    {
        m_bus_list_view->InsertItem( i, m_aliases[i]->GetName() );
    }

    return true;
}


bool DIALOG_BUS_MANAGER::TransferDataFromWindow()
{
    std::cout << "TransferDataFromWindow" << std::endl;
    return true;
}


void DIALOG_BUS_MANAGER::OnSelectBus( wxListEvent& event )
{
    m_bus_edit->Clear();
    m_signal_edit->Clear();
    m_signal_list_view->DeleteAllItems();

    if( event.GetEventType() == wxEVT_COMMAND_LIST_ITEM_SELECTED )
    {
        auto alias = m_aliases[ ( unsigned )event.GetIndex() ];
        m_activeAlias = alias;

        m_bus_edit->AppendText( alias->GetName() );

        auto members = alias->GetMembers();

        for( unsigned i = 0; i < members.size(); i++ )
        {
            m_signal_list_view->InsertItem( i, members[i] );
        }
    }
}


void DIALOG_BUS_MANAGER::OnSelectSignal( wxListEvent& event )
{
    m_signal_edit->Clear();

    if( event.GetEventType() == wxEVT_COMMAND_LIST_ITEM_SELECTED )
    {
        m_signal_edit->AppendText( event.GetText() );
    }
}



// see invoke_sch_dialog.h
void InvokeDialogBusManager( SCH_EDIT_FRAME* aCaller )
{
    DIALOG_BUS_MANAGER dlg( aCaller );
    dlg.ShowModal();
}
