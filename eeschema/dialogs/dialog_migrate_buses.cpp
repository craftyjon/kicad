/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 CERN
 * @author Jon Evans <jon@craftyjon.com>
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

#include <dialog_migrate_buses.h>


DIALOG_MIGRATE_BUSES::DIALOG_MIGRATE_BUSES( SCH_EDIT_FRAME* aParent ) :
    DIALOG_MIGRATE_BUSES_BASE( aParent ),
    m_frame( aParent )
{
    m_migration_list->Bind( wxEVT_LIST_ITEM_SELECTED,
                            &DIALOG_MIGRATE_BUSES::onItemSelected, this );

    m_btn_accept->Bind( wxEVT_COMMAND_BUTTON_CLICKED,
                        &DIALOG_MIGRATE_BUSES::onAcceptClicked, this );

    loadGraphData();
    updateUi();

    m_frame->Zoom_Automatique( false );
}


void DIALOG_MIGRATE_BUSES::loadGraphData()
{
    m_items.clear();
    auto subgraphs = g_ConnectionGraph->GetBusesNeedingMigration();

    for( auto subgraph : subgraphs )
    {
        BUS_MIGRATION_STATUS status;

        status.subgraph = subgraph;
        status.approved = false;

        auto labels = subgraph->GetBusLabels();
        wxASSERT( labels.size() > 1 );

        for( auto label : labels )
            status.labels.push_back( static_cast<SCH_TEXT*>( label )->GetText() );

        status.possible_labels = getProposedLabels( status.labels );
        m_items.push_back( status );
    }
}


void DIALOG_MIGRATE_BUSES::updateUi()
{
    m_migration_list->DeleteAllItems();

    m_migration_list->InsertColumn( 0, _( "Sheet" ) );
    m_migration_list->InsertColumn( 1, _( "Conflicting Labels" ) );
    m_migration_list->InsertColumn( 2, _( "New Label" ) );
    m_migration_list->InsertColumn( 3, _( "Status" ) );

    for( auto item : m_items )
    {
        wxString old = item.labels[0];
        for( unsigned j = 1; j < item.labels.size(); j++ )
            old << ", " << item.labels[j];

        auto i = m_migration_list->InsertItem( m_migration_list->GetItemCount(),
                                               wxEmptyString );

        m_migration_list->SetItem( i, 0, item.subgraph->m_sheet.PathHumanReadable() );
        m_migration_list->SetItem( i, 1, old );
        m_migration_list->SetItem( i, 2, item.possible_labels[0] );
        m_migration_list->SetItem( i, 3, "" );
    }

    m_migration_list->Select( 0 );
    m_migration_list->SetColumnWidth( 1, -1 );
}


std::vector<wxString> DIALOG_MIGRATE_BUSES::getProposedLabels( std::vector<wxString> aLabelList )
{
    int lowest_start = INT_MAX;
    int highest_end = -1;
    int widest_bus = -1;

    for( auto label : aLabelList )
    {
        SCH_CONNECTION conn;
        conn.ConfigureFromLabel( label );

        int start = conn.VectorStart();
        int end = conn.VectorEnd();

        if( start < lowest_start )
            lowest_start = start;

        if( end > highest_end )
            highest_end = end;

        if( end - start + 1 > widest_bus )
            widest_bus = end - start + 1;
    }

    SCH_CONNECTION conn;
    std::vector<wxString> proposals;

    for( auto label : aLabelList )
    {
        conn.ConfigureFromLabel( label );
        wxString proposal = conn.VectorPrefix();
        proposal << "[" << highest_end << ".." << lowest_start << "]";
        proposals.push_back( proposal );
    }

    return proposals;
}


void DIALOG_MIGRATE_BUSES::onItemSelected( wxListEvent& aEvent )
{
    unsigned sel = aEvent.GetIndex();
    wxASSERT( sel < m_items.size() );

    m_selected_index = sel;

    auto subgraph = m_items[sel].subgraph;

    auto sheet = subgraph->m_sheet;
    auto driver = subgraph->m_driver;

    if( sheet != *g_CurrentSheet )
    {
        sheet.LastScreen()->SetZoom( m_frame->GetScreen()->GetZoom() );
        *g_CurrentSheet = sheet;
        g_CurrentSheet->UpdateAllScreenReferences();
        sheet.LastScreen()->TestDanglingEnds();
    }

    auto pos = driver->GetPosition();

    m_frame->SetCrossHairPosition( pos );
    m_frame->RedrawScreen( pos, false );

    m_cb_new_name->Clear();

    for( auto option : m_items[sel].possible_labels )
        m_cb_new_name->Append( option );

    m_cb_new_name->Select( 0 );
}


void DIALOG_MIGRATE_BUSES::onAcceptClicked( wxCommandEvent& aEvent )
{
    wxASSERT( m_selected_index < m_items.size() );

    auto sel = m_selected_index;

    m_items[sel].approved_label = m_cb_new_name->GetStringSelection();
    m_items[sel].approved = true;

    m_migration_list->SetItem( sel, 2, m_items[sel].approved_label );
    m_migration_list->SetItem( sel, 3, _( "Updated" ) );

    if( sel < m_items.size() - 1 )
    {
        m_migration_list->Select( sel + 1 );
    }
}
