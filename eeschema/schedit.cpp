 /*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2008-2017 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 2004-2017 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file schedit.cpp
 */

#include <fctsys.h>
#include <kiway.h>
#include <gr_basic.h>
#include <pgm_base.h>
#include <class_drawpanel.h>
#include <confirm.h>
#include <eda_doc.h>
#include <sch_edit_frame.h>
#include <kicad_device_context.h>
#include <hotkeys_basic.h>

#include <general.h>
#include <eeschema_id.h>
#include <list_operations.h>
#include <class_library.h>
#include <sch_bus_entry.h>
#include <sch_marker.h>
#include <sch_component.h>
#include <sch_junction.h>
#include <sch_line.h>
#include <sch_sheet.h>
#include <sch_sheet_path.h>


void SCH_EDIT_FRAME::Process_Special_Functions( wxCommandEvent& event )
{
    int         id = event.GetId();
    wxPoint     pos;
    SCH_SCREEN* screen = GetScreen();
    SCH_ITEM*   item = screen->GetCurItem();

    pos = wxGetMousePosition();

    pos.y += 20;

    // If needed, stop the current command and deselect current tool
    switch( id )
    {
    case wxID_CUT:
    case wxID_COPY:
    case ID_POPUP_CANCEL_CURRENT_COMMAND:
    case ID_POPUP_SCH_ENTRY_SELECT_SLASH:
    case ID_POPUP_SCH_ENTRY_SELECT_ANTISLASH:
    case ID_POPUP_SCH_BEGIN_WIRE:
    case ID_POPUP_SCH_BEGIN_BUS:
    case ID_POPUP_END_LINE:
    case ID_POPUP_SCH_SET_SHAPE_TEXT:
    case ID_POPUP_SCH_CLEANUP_SHEET:
    case ID_POPUP_SCH_END_SHEET:
    case ID_POPUP_SCH_RESIZE_SHEET:
    case ID_POPUP_IMPORT_HLABEL_TO_SHEETPIN:
    case ID_POPUP_SCH_INIT_CMP:
    case ID_POPUP_SCH_DISPLAYDOC_CMP:
    case ID_POPUP_SCH_EDIT_CONVERT_CMP:
    case ID_POPUP_DELETE_BLOCK:
    case ID_POPUP_PLACE_BLOCK:
    case ID_POPUP_ZOOM_BLOCK:
    case ID_POPUP_DRAG_BLOCK:
    case ID_POPUP_DUPLICATE_BLOCK:
    case ID_POPUP_SCH_DELETE_NODE:
    case ID_POPUP_SCH_DELETE_CONNECTION:
    case ID_POPUP_SCH_ENTER_SHEET:
    case ID_POPUP_SCH_LEAVE_SHEET:
    case ID_POPUP_SCH_ADD_JUNCTION:
    case ID_POPUP_SCH_ADD_LABEL:
    case ID_POPUP_SCH_GETINFO_MARKER:

        /* At this point: Do nothing. these commands do not need to stop the
         * current command (mainly a block command) or reset the current state
         * They will be executed later, in next switch structure.
         */
        break;

    case ID_POPUP_SCH_DELETE_CMP:
    case ID_POPUP_SCH_DELETE:

        // Stop the current command (if any) but keep the current tool
        m_canvas->EndMouseCapture();
        break;

    default:
        // Stop the current command and deselect the current tool
        SetNoToolSelected();
        break;
    }

    INSTALL_UNBUFFERED_DC( dc, m_canvas );
    item = screen->GetCurItem();    // Can be modified by previous calls.

    switch( id )
    {
    case ID_HIERARCHY:
        InstallHierarchyFrame( pos );
        SetRepeatItem( NULL );
        break;

    case wxID_CUT: // save and delete block
    case ID_POPUP_CUT_BLOCK:

        if( screen->m_BlockLocate.GetCommand() != BLOCK_MOVE )
            break;
        screen->m_BlockLocate.SetCommand( BLOCK_CUT );
        screen->m_BlockLocate.SetMessageBlock( this );
        HandleBlockEnd( &dc );
        SetRepeatItem( NULL );
        SetSheetNumberAndCount();
        break;

    case wxID_COPY:         // really this is a Save block for paste
    case ID_POPUP_COPY_BLOCK:
        screen->m_BlockLocate.SetCommand( BLOCK_COPY );
        screen->m_BlockLocate.SetMessageBlock( this );
        HandleBlockEnd( &dc );
        break;

    case wxID_PASTE:
        HandleBlockBegin( &dc, BLOCK_PASTE, GetCrossHairPosition() );
        break;

    case ID_POPUP_SCH_ENTRY_SELECT_SLASH:
        m_canvas->MoveCursorToCrossHair();
        SetBusEntryShape( &dc, dynamic_cast<SCH_BUS_ENTRY_BASE*>( item ), '/' );
        break;

    case ID_POPUP_SCH_ENTRY_SELECT_ANTISLASH:
        m_canvas->MoveCursorToCrossHair();
        SetBusEntryShape( &dc, dynamic_cast<SCH_BUS_ENTRY_BASE*>( item ), '\\' );
        break;

    case ID_POPUP_CANCEL_CURRENT_COMMAND:
        if( m_canvas->IsMouseCaptured() )
        {
            m_canvas->EndMouseCapture();
            SetToolID( GetToolId(), m_canvas->GetCurrentCursor(), wxEmptyString );
        }
        else
        {
            SetNoToolSelected();
        }

        break;

    case ID_POPUP_END_LINE:
        m_canvas->MoveCursorToCrossHair();
        EndSegment();
        break;

    case ID_POPUP_SCH_BEGIN_WIRE:
        m_canvas->MoveCursorToCrossHair();
        OnLeftClick( &dc, GetCrossHairPosition() );
        break;

    case ID_POPUP_SCH_BEGIN_BUS:
        m_canvas->MoveCursorToCrossHair();
        OnLeftClick( &dc, GetCrossHairPosition() );
        break;

    case ID_POPUP_SCH_SET_SHAPE_TEXT:
        // Not used
        break;

    case ID_POPUP_SCH_DELETE_NODE:
    case ID_POPUP_SCH_DELETE_CONNECTION:
        m_canvas->MoveCursorToCrossHair();
        DeleteConnection( id == ID_POPUP_SCH_DELETE_CONNECTION );
        SchematicCleanUp( true );
        screen->SetCurItem( NULL );
        SetRepeatItem( NULL );

        screen->TestDanglingEnds();
        m_canvas->Refresh();

        break;

    case ID_POPUP_SCH_BREAK_WIRE:
        {
            SaveWireImage();
            m_canvas->MoveCursorToCrossHair();
            BreakSegments( GetCrossHairPosition() );
            if( screen->TestDanglingEnds() )
                m_canvas->Refresh();
        }
        break;

    case ID_POPUP_SCH_DELETE_CMP:
    case ID_POPUP_SCH_DELETE:
        if( item == NULL )
            break;

        DeleteItem( item );
        SchematicCleanUp( true );
        screen->SetCurItem( NULL );
        SetRepeatItem( NULL );
        screen->TestDanglingEnds();
        SetSheetNumberAndCount();
        m_canvas->Refresh();
        OnModify();
        break;

    case ID_POPUP_SCH_END_SHEET:
        m_canvas->MoveCursorToCrossHair();
        addCurrentItemToList();
        break;

    case ID_POPUP_SCH_RESIZE_SHEET:
        ReSizeSheet( (SCH_SHEET*) item, &dc );

        if( screen->TestDanglingEnds() )
            m_canvas->Refresh();

        break;

    case ID_POPUP_IMPORT_HLABEL_TO_SHEETPIN:
        if( item != NULL && item->Type() == SCH_SHEET_T )
            screen->SetCurItem( ImportSheetPin( (SCH_SHEET*) item, &dc ) );
        break;

    case ID_POPUP_SCH_CLEANUP_SHEET:
        if( item != NULL && item->Type() == SCH_SHEET_T )
        {
            SCH_SHEET* sheet = (SCH_SHEET*) item;

            if( !sheet->HasUndefinedPins() )
            {
                DisplayInfoMessage( this,
                                    _( "There are no undefined labels in this sheet to clean up." ) );
                return;
            }

            if( !IsOK( this, _( "Do you wish to cleanup this sheet?" ) ) )
                return;

            /* Save sheet in undo list before cleaning up unreferenced hierarchical labels. */
            SaveCopyInUndoList( sheet, UR_CHANGED );
            sheet->CleanupSheet();
            OnModify();
            m_canvas->RefreshDrawingRect( sheet->GetBoundingBox() );
        }
        break;

    case ID_POPUP_SCH_INIT_CMP:
        m_canvas->MoveCursorToCrossHair();
        break;

    case ID_POPUP_SCH_EDIT_CONVERT_CMP:
        // Ensure the struct is a component (could be a struct of a component, like Field, text..)
        if( item && item->Type() == SCH_COMPONENT_T )
        {
            m_canvas->MoveCursorToCrossHair();
            ConvertPart( (SCH_COMPONENT*) item, &dc );
        }

        break;

    case ID_POPUP_SCH_DISPLAYDOC_CMP:
        // Ensure the struct is a component (could be a piece of a component, like Field, text..)
        if( item && item->Type() == SCH_COMPONENT_T )
        {
            wxString text = static_cast<SCH_COMPONENT*>( item )->GetField( DATASHEET )->GetText();
            text = ResolveUriByEnvVars( text );

            if( !text.IsEmpty() )
                GetAssociatedDocument( this, text );
        }
        break;

    case ID_POPUP_SCH_ENTER_SHEET:

        if( item && (item->Type() == SCH_SHEET_T) )
        {
            g_CurrentSheet->push_back( (SCH_SHEET*) item );
            DisplayCurrentSheet();
        }

        break;

    case ID_POPUP_SCH_LEAVE_SHEET:
        if( g_CurrentSheet->Last() != g_RootSheet )
        {
            g_CurrentSheet->pop_back();
            DisplayCurrentSheet();
        }

        break;

    case ID_POPUP_PLACE_BLOCK:
        m_canvas->SetAutoPanRequest( false );
        m_canvas->MoveCursorToCrossHair();
        HandleBlockPlace( &dc );
        break;

    case ID_POPUP_ZOOM_BLOCK:
        screen->m_BlockLocate.SetCommand( BLOCK_ZOOM );
        screen->m_BlockLocate.SetMessageBlock( this );
        HandleBlockEnd( &dc );
        break;

    case ID_POPUP_DELETE_BLOCK:
        if( screen->m_BlockLocate.GetCommand() != BLOCK_MOVE )
            break;

        m_canvas->MoveCursorToCrossHair();
        screen->m_BlockLocate.SetCommand( BLOCK_DELETE );
        screen->m_BlockLocate.SetMessageBlock( this );
        HandleBlockEnd( &dc );
        SetSheetNumberAndCount();
        break;

    case ID_POPUP_DUPLICATE_BLOCK:
        if( screen->m_BlockLocate.GetCommand() != BLOCK_MOVE )
            break;

        m_canvas->MoveCursorToCrossHair();
        screen->m_BlockLocate.SetCommand( BLOCK_DUPLICATE );
        screen->m_BlockLocate.SetMessageBlock( this );
        HandleBlockEnd( &dc );
        break;

    case ID_POPUP_DRAG_BLOCK:
        if( screen->m_BlockLocate.GetCommand() != BLOCK_MOVE )
            break;

        m_canvas->MoveCursorToCrossHair();
        screen->m_BlockLocate.SetCommand( BLOCK_DRAG );
        screen->m_BlockLocate.SetMessageBlock( this );
        HandleBlockEnd( &dc );
        break;

    case ID_POPUP_SCH_ADD_JUNCTION:
        m_canvas->MoveCursorToCrossHair();
        screen->SetCurItem( AddJunction( GetCrossHairPosition() ) );

        if( screen->TestDanglingEnds() )
            m_canvas->Refresh();

        screen->SetCurItem( NULL );
        break;

    case ID_POPUP_SCH_ADD_LABEL:
    case ID_POPUP_SCH_ADD_GLABEL:
        screen->SetCurItem( CreateNewText( &dc, id == ID_POPUP_SCH_ADD_LABEL ?
                                           LAYER_LOCLABEL : LAYER_GLOBLABEL ) );
        item = screen->GetCurItem();

        if( item )
            addCurrentItemToList();

        break;

    case ID_POPUP_SCH_GETINFO_MARKER:
        if( item && item->Type() == SCH_MARKER_T )
            ( (SCH_MARKER*) item )->DisplayMarkerInfo( this );

        break;

    default:        // Log error:
        wxFAIL_MSG( wxString::Format( wxT( "Cannot process command event ID %d" ),
                                      event.GetId() ) );
        break;
    }

    // End switch ( id )    (Command execution)

    if( GetToolId() == ID_NO_TOOL_SELECTED )
        SetRepeatItem( NULL );
}


void SCH_EDIT_FRAME::OnUnfoldBus( wxCommandEvent& event )
{
    auto screen = GetScreen();
    auto item = static_cast< wxMenuItem* >( event.GetEventUserData() );
    auto net = item->GetItemLabelText();

    auto pos = GetCrossHairPosition();

    /**
     * Unfolding a bus consists of the following user inputs:
     * 1) User selects a bus to unfold (see AddMenusForBus())
     *    We land in this event handler.
     *
     * 2) User clicks to set the net label location (handled by BeginSegment())
     *    Before this first click, the posture of the bus entry  follows the
     *    mouse cursor in X and Y (handled by DrawSegment())
     *
     * 3) User is now in normal wiring mode and can exit in any normal way.
     */

    wxASSERT( !m_busUnfold.in_progress );

    m_busUnfold.entry = new SCH_BUS_WIRE_ENTRY( pos, '\\' );

    SetSchItemParent( m_busUnfold.entry, screen );
    screen->Append( m_busUnfold.entry );

    m_busUnfold.label = new SCH_LABEL( m_busUnfold.entry->m_End(), net );

    m_busUnfold.label->SetTextSize( wxSize( GetDefaultTextSize(),
                                            GetDefaultTextSize() ) );
    m_busUnfold.label->SetLabelSpinStyle( 0 );

    SetSchItemParent( m_busUnfold.label, screen );

    m_busUnfold.in_progress = true;
    m_busUnfold.origin = pos;
    m_busUnfold.net_name = net;

    SetToolID( ID_WIRE_BUTT, wxCURSOR_PENCIL, _( "Add wire" ) );

    INSTALL_UNBUFFERED_DC( dc, m_canvas );
    SetCrossHairPosition( m_busUnfold.entry->m_End() );
    BeginSegment( &dc, LAYER_WIRE );
    m_canvas->SetAutoPanRequest( true );
}


void SCH_EDIT_FRAME::CancelBusUnfold()
{
    auto screen = GetScreen();

    if( m_busUnfold.entry )
    {
        screen->Remove( m_busUnfold.entry );
        delete m_busUnfold.entry;
    }

    if( m_busUnfold.label )
    {
        if( m_busUnfold.label_placed )
            screen->Remove( m_busUnfold.label );

        delete m_busUnfold.label;
    }

    FinishBusUnfold();
}


void SCH_EDIT_FRAME::FinishBusUnfold()
{
    m_busUnfold = {};

    SetToolID( ID_NO_TOOL_SELECTED, m_canvas->GetDefaultCursor(), wxEmptyString );
}


void SCH_EDIT_FRAME::OnMoveItem( wxCommandEvent& aEvent )
{
    SCH_SCREEN* screen = GetScreen();
    SCH_ITEM*   item = screen->GetCurItem();

    if( screen->m_BlockLocate.GetState() != STATE_NO_BLOCK )
    {
        // trying to move an item when there is a block at the same time is not acceptable
        return;
    }

    if( item == NULL )
    {
        // If we didn't get here by a hot key, then something has gone wrong.
        if( aEvent.GetInt() == 0 )
            return;

        EDA_HOTKEY_CLIENT_DATA* data = (EDA_HOTKEY_CLIENT_DATA*) aEvent.GetClientObject();

        wxCHECK_RET( data != NULL, wxT( "Invalid hot key client object." ) );

        item = LocateAndShowItem( data->GetPosition(), SCH_COLLECTOR::MovableItems,
                                  aEvent.GetInt() );

        // Exit if no item found at the current location or the item is already being edited.
        if( (item == NULL) || (item->GetFlags() != 0) )
            return;
    }

    INSTALL_UNBUFFERED_DC( dc, m_canvas );

    switch( item->Type() )
    {
    case SCH_LINE_T:
        break;

    case SCH_JUNCTION_T:
    case SCH_NO_CONNECT_T:
    case SCH_BUS_BUS_ENTRY_T:
    case SCH_BUS_WIRE_ENTRY_T:
    case SCH_LABEL_T:
    case SCH_GLOBAL_LABEL_T:
    case SCH_HIERARCHICAL_LABEL_T:
    case SCH_TEXT_T:
    case SCH_COMPONENT_T:
    case SCH_SHEET_PIN_T:
    case SCH_FIELD_T:
    case SCH_SHEET_T:
        PrepareMoveItem( item, &dc );
        break;

    case SCH_BITMAP_T:
        // move an image is a special case:
        // we cannot undraw/redraw a bitmap just using our xor mode
        // the MoveImage function handle this undraw/redraw difficulty
        // By redrawing the full bounding box
        MoveImage( (SCH_BITMAP*) item, &dc );
        break;

    case SCH_MARKER_T:
        // Moving a marker has no sense
        break;

    default:
        // Unknown items cannot be moved
        wxFAIL_MSG( wxString::Format(
                    wxT( "Cannot move item type %d" ), item->Type() ) );
        break;
    }

    if( GetToolId() == ID_NO_TOOL_SELECTED )
        SetRepeatItem( NULL );
}


void SCH_EDIT_FRAME::OnCancelCurrentCommand( wxCommandEvent& aEvent )
{
    SCH_SCREEN* screen = GetScreen();

    if( screen->IsBlockActive() )
    {
        m_canvas->SetCursor( (wxStockCursor) m_canvas->GetDefaultCursor() );
        screen->ClearBlockCommand();

        // Stop the current command (if any) but keep the current tool
        m_canvas->EndMouseCapture();
    }
    else
    {
        if( m_canvas->IsMouseCaptured() ) // Stop the current command but keep the current tool
            m_canvas->EndMouseCapture();
        else                    // Deselect current tool
            m_canvas->EndMouseCapture( ID_NO_TOOL_SELECTED, m_canvas->GetDefaultCursor() );
     }
}


void SCH_EDIT_FRAME::OnSelectTool( wxCommandEvent& aEvent )
{
    int id = aEvent.GetId();
    int lastToolID = GetToolId();

    // Stop the current command and deselect the current tool.
    m_canvas->EndMouseCapture( ID_NO_TOOL_SELECTED, m_canvas->GetDefaultCursor() );

    switch( id )
    {
    case ID_NO_TOOL_SELECTED:
        SetNoToolSelected();
        break;

    case ID_HIGHLIGHT:
        SetToolID( ID_HIGHLIGHT, wxCURSOR_HAND, _("Highlight specific net") );
        break;

    case ID_MENU_ZOOM_SELECTION:
    case ID_ZOOM_SELECTION:
        // This tool is located on the main toolbar: switch it on or off on click
        if( lastToolID != ID_ZOOM_SELECTION )
            SetToolID( ID_ZOOM_SELECTION, wxCURSOR_MAGNIFIER, _( "Zoom to selection" ) );
        else
            SetNoToolSelected();
        break;

    case ID_MENU_NOCONN_BUTT:
    case ID_NOCONN_BUTT:
        SetToolID( ID_NOCONN_BUTT, wxCURSOR_PENCIL, _( "Add no connect" ) );
        break;

    case ID_MENU_WIRE_BUTT:
    case ID_WIRE_BUTT:
        SetToolID( ID_WIRE_BUTT, wxCURSOR_PENCIL, _( "Add wire" ) );
        break;

    case ID_MENU_BUS_BUTT:
    case ID_BUS_BUTT:
        SetToolID( ID_BUS_BUTT, wxCURSOR_PENCIL, _( "Add bus" ) );
        break;

    case ID_MENU_LINE_COMMENT_BUTT:
    case ID_LINE_COMMENT_BUTT:
        SetToolID( ID_LINE_COMMENT_BUTT, wxCURSOR_PENCIL, _( "Add lines" ) );
        break;

    case ID_MENU_JUNCTION_BUTT:
    case ID_JUNCTION_BUTT:
        SetToolID( ID_JUNCTION_BUTT, wxCURSOR_PENCIL, _( "Add junction" ) );
        break;

    case ID_MENU_LABEL_BUTT:
    case ID_LABEL_BUTT:
        SetToolID( ID_LABEL_BUTT, wxCURSOR_PENCIL, _( "Add label" ) );
        break;

    case ID_MENU_GLABEL_BUTT:
    case ID_GLABEL_BUTT:
        SetToolID( ID_GLABEL_BUTT, wxCURSOR_PENCIL, _( "Add global label" ) );
        break;

    case ID_MENU_HIERLABEL_BUTT:
    case ID_HIERLABEL_BUTT:
        SetToolID( ID_HIERLABEL_BUTT, wxCURSOR_PENCIL, _( "Add hierarchical label" ) );
        break;

    case ID_MENU_TEXT_COMMENT_BUTT:
    case ID_TEXT_COMMENT_BUTT:
        SetToolID( ID_TEXT_COMMENT_BUTT, wxCURSOR_PENCIL, _( "Add text" ) );
        break;

    case ID_MENU_ADD_IMAGE_BUTT:
    case ID_ADD_IMAGE_BUTT:
        SetToolID( ID_ADD_IMAGE_BUTT, wxCURSOR_PENCIL, _( "Add image" ) );
        break;

    case ID_MENU_WIRETOBUS_ENTRY_BUTT:
    case ID_WIRETOBUS_ENTRY_BUTT:
        SetToolID( ID_WIRETOBUS_ENTRY_BUTT, wxCURSOR_PENCIL, _( "Add wire to bus entry" ) );
        break;

    case ID_MENU_BUSTOBUS_ENTRY_BUTT:
    case ID_BUSTOBUS_ENTRY_BUTT:
        SetToolID( ID_BUSTOBUS_ENTRY_BUTT, wxCURSOR_PENCIL, _( "Add bus to bus entry" ) );
        break;

    case ID_MENU_SHEET_SYMBOL_BUTT:
    case ID_SHEET_SYMBOL_BUTT:
        SetToolID( ID_SHEET_SYMBOL_BUTT, wxCURSOR_PENCIL, _( "Add sheet" ) );
        break;

    case ID_MENU_SHEET_PIN_BUTT:
    case ID_SHEET_PIN_BUTT:
        SetToolID( ID_SHEET_PIN_BUTT, wxCURSOR_PENCIL, _( "Add sheet pins" ) );
        break;

    case ID_MENU_IMPORT_HLABEL_BUTT:
    case ID_IMPORT_HLABEL_BUTT:
        SetToolID( ID_IMPORT_HLABEL_BUTT, wxCURSOR_PENCIL, _( "Import sheet pins" ) );
        break;

    case ID_MENU_PLACE_COMPONENT:
    case ID_SCH_PLACE_COMPONENT:
        SetToolID( ID_SCH_PLACE_COMPONENT, wxCURSOR_PENCIL, _( "Add component" ) );
        break;

    case ID_MENU_PLACE_POWER_BUTT:
    case ID_PLACE_POWER_BUTT:
        SetToolID( ID_PLACE_POWER_BUTT, wxCURSOR_PENCIL, _( "Add power" ) );
        break;

    case ID_MENU_DELETE_ITEM_BUTT:
    case ID_SCHEMATIC_DELETE_ITEM_BUTT:
        SetToolID( ID_SCHEMATIC_DELETE_ITEM_BUTT, wxCURSOR_BULLSEYE, _( "Delete item" ) );
        break;

#ifdef KICAD_SPICE
    case ID_SIM_PROBE:
        SetToolID( id, -1, _( "Add a simulator probe" ) );
        m_canvas->SetCursor( CURSOR_PROBE );
        break;

    case ID_SIM_TUNE:
        SetToolID( id, -1, _( "Select a value to be tuned" ) );
        m_canvas->SetCursor( CURSOR_TUNE );
        break;
#endif /* KICAD_SPICE */

    default:
        SetRepeatItem( NULL );
    }

    // Simulate left click event if we got here from a hot key.
    if( aEvent.GetClientObject() != NULL )
    {
        EDA_HOTKEY_CLIENT_DATA* data = (EDA_HOTKEY_CLIENT_DATA*) aEvent.GetClientObject();

        wxPoint pos = data->GetPosition();

        INSTALL_UNBUFFERED_DC( dc, m_canvas );
        OnLeftClick( &dc, pos );
    }
}


void SCH_EDIT_FRAME::OnUpdateSelectTool( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( GetToolId() == aEvent.GetId() );
}


void SCH_EDIT_FRAME::DeleteConnection( bool aFullConnection )
{
    PICKED_ITEMS_LIST   pickList;
    SCH_SCREEN*         screen = GetScreen();
    wxPoint             pos = GetCrossHairPosition();

    if( screen->GetConnection( pos, pickList, aFullConnection ) != 0 )
    {
        DeleteItemsInList( pickList );
        SchematicCleanUp( true );
        OnModify();
    }
}


bool SCH_EDIT_FRAME::DeleteItemAtCrossHair( wxDC* DC )
{
    SCH_ITEM*   item;
    SCH_SCREEN* screen = GetScreen();

    item = LocateItem( GetCrossHairPosition(), SCH_COLLECTOR::ParentItems );

    if( item )
    {
        bool itemHasConnections = item->IsConnectable();

        screen->SetCurItem( NULL );
        SetRepeatItem( NULL );
        DeleteItem( item );

        if( itemHasConnections && screen->TestDanglingEnds() )
            m_canvas->Refresh();

        OnModify();
        return true;
    }

    return false;
}

// This function is a callback function, called by the mouse cursor movin event
// when an item is currently moved
static void moveItemWithMouseCursor( EDA_DRAW_PANEL* aPanel, wxDC* aDC,
                                     const wxPoint& aPosition, bool aErase )
{
    SCH_SCREEN* screen = (SCH_SCREEN*) aPanel->GetScreen();
    SCH_ITEM*   item   = screen->GetCurItem();

    wxCHECK_RET( (item != NULL), wxT( "Cannot move invalid schematic item." ) );

    SCH_COMPONENT* cmp = NULL;

    if( item->Type() == SCH_COMPONENT_T )
        cmp = static_cast< SCH_COMPONENT* >( item );

#ifndef USE_WX_OVERLAY
    // Erase the current item at its current position.
    if( aErase )
    {
        if( cmp )   // Use fast mode (do not draw pin texts)
            cmp->Draw( aPanel, aDC, wxPoint( 0, 0 ), g_XorMode, COLOR4D::UNSPECIFIED, false );
        else
            item->Draw( aPanel, aDC, wxPoint( 0, 0 ), g_XorMode );
    }
#endif

    wxPoint cpos = aPanel->GetParent()->GetCrossHairPosition();
    cpos -= item->GetStoredPos();

    item->SetPosition( cpos );

    // Draw the item item at it's new position.
    item->SetWireImage();  // While moving, the item may choose to render differently

    if( cmp )   // Use fast mode (do not draw pin texts)
        cmp->Draw( aPanel, aDC, wxPoint( 0, 0 ), g_XorMode, COLOR4D::UNSPECIFIED, false );
    else
        item->Draw( aPanel, aDC, wxPoint( 0, 0 ), g_XorMode );
}


static void abortMoveItem( EDA_DRAW_PANEL* aPanel, wxDC* aDC )
{
    SCH_SCREEN*     screen = (SCH_SCREEN*) aPanel->GetScreen();
    SCH_ITEM*       item = screen->GetCurItem();
    SCH_EDIT_FRAME* parent = (SCH_EDIT_FRAME*) aPanel->GetParent();

    parent->SetRepeatItem( NULL );
    screen->SetCurItem( NULL );

    if( item == NULL )  /* no current item */
        return;

    if( item->IsNew() )
    {
        delete item;
        item = NULL;
    }
    else
    {
        SCH_ITEM* oldItem = parent->GetUndoItem();

        SCH_ITEM* currentItem;

        // Items that are children of other objects are undone by swapping the contents
        // of the parent items.
        if( (item->Type() == SCH_SHEET_PIN_T) || (item->Type() == SCH_FIELD_T) )
        {
            currentItem = (SCH_ITEM*) item->GetParent();
        }
        else
        {
            currentItem = item;
        }

        wxCHECK_RET( oldItem != NULL && currentItem->Type() == oldItem->Type(),
                     wxT( "Cannot restore undefined or bad last schematic item." ) );

        // Never delete existing item, because it can be referenced by an undo/redo command
        // Just restore its data
        currentItem->SwapData( oldItem );

        // Erase the wire representation before the 'normal' view is drawn.
        if ( item->IsWireImage() )
            item->Draw( aPanel, aDC, wxPoint( 0, 0 ), g_XorMode );

        item->ClearFlags();
    }

    aPanel->Refresh();
}


void SCH_EDIT_FRAME::PrepareMoveItem( SCH_ITEM* aItem, wxDC* aDC )
{
    wxCHECK_RET( aItem != NULL, wxT( "Cannot move invalid schematic item" ) );

    SetRepeatItem( NULL );

    if( !aItem->IsNew() )
    {
        if( (aItem->Type() == SCH_SHEET_PIN_T) || (aItem->Type() == SCH_FIELD_T) )
            SetUndoItem( (SCH_ITEM*) aItem->GetParent() );
        else
            SetUndoItem( aItem );
    }

    if( aItem->Type() == SCH_FIELD_T && aItem->GetParent()->Type() == SCH_COMPONENT_T )
    {
        // Now that we're moving a field, they're no longer autoplaced.
        SCH_COMPONENT *parent = static_cast<SCH_COMPONENT*>( aItem->GetParent() );
        parent->ClearFieldsAutoplaced();
    }

    aItem->SetFlags( IS_MOVED );

    // For some items, moving the cursor to anchor is not good
    // (for instance large hierarchical sheets od componants can have
    // the anchor position outside the canvas)
    // these items return IsMovableFromAnchorPoint() == false
    // For these items, do not wrap the cursor
    if( aItem->IsMovableFromAnchorPoint() )
    {
        SetCrossHairPosition( aItem->GetPosition() );
        m_canvas->MoveCursorToCrossHair();
        aItem->SetStoredPos( wxPoint( 0,0 ) );
    }
    else
    {
        // Round the point under the cursor to a multiple of the grid
        wxPoint cursorpos = GetCrossHairPosition() - aItem->GetPosition();
        wxPoint gridsize = GetScreen()->GetGridSize();
        cursorpos.x = ( cursorpos.x / gridsize.x ) * gridsize.x;
        cursorpos.y = ( cursorpos.y / gridsize.y ) * gridsize.y;

        aItem->SetStoredPos( cursorpos );
    }

    OnModify();

    GetScreen()->SetCurItem( aItem );
    m_canvas->SetMouseCapture( moveItemWithMouseCursor, abortMoveItem );

    m_canvas->Refresh();
}


void SCH_EDIT_FRAME::SelectAllFromSheet( wxCommandEvent& aEvent )
{
    SCH_SCREEN* screen = GetScreen();
    SCH_ITEM*   item = screen->GetCurItem();

    if( item != NULL )
    {
        item = LocateAndShowItem( item->GetPosition() );
        SendMessageToPCBNEW( item, NULL );
    }
    else
    {
        // If we didn't get here by a hot key, then something has gone wrong.
        if( aEvent.GetInt() == 0 )
            return;

        EDA_HOTKEY_CLIENT_DATA* data = (EDA_HOTKEY_CLIENT_DATA*) aEvent.GetClientObject();

        wxCHECK_RET( data != NULL, wxT( "Invalid hot key client object." ) );

        item = LocateAndShowItem( data->GetPosition() );
        SendMessageToPCBNEW( item, NULL );
    }
}


void SCH_EDIT_FRAME::OnRotate( wxCommandEvent& aEvent )
{
    SCH_SCREEN* screen = GetScreen();
    SCH_ITEM*   item = screen->GetCurItem();

    INSTALL_UNBUFFERED_DC( dc, m_canvas );

    // Allows block rotate operation on hot key.
    if( screen->m_BlockLocate.GetState() != STATE_NO_BLOCK )
    {
        screen->m_BlockLocate.SetCommand( BLOCK_ROTATE );
        HandleBlockEnd( &dc );
        return;
    }

    if( item == NULL )
    {
        // If we didn't get here by a hot key, then something has gone wrong.
        if( aEvent.GetInt() == 0 )
            return;

        EDA_HOTKEY_CLIENT_DATA* data = (EDA_HOTKEY_CLIENT_DATA*) aEvent.GetClientObject();

        wxCHECK_RET( data != NULL, wxT( "Invalid hot key client object." ) );

        item = LocateAndShowItem( data->GetPosition(), SCH_COLLECTOR::RotatableItems,
                                  aEvent.GetInt() );

        // Exit if no item found at the current location or the item is already being edited.
        if( (item == NULL) || (item->GetFlags() != 0) )
            return;
    }

    switch( item->Type() )
    {
    case SCH_COMPONENT_T:
        {
            SCH_COMPONENT* component = static_cast<SCH_COMPONENT*>( item );
            if( aEvent.GetId() == ID_SCH_ROTATE_CLOCKWISE )
                OrientComponent( CMP_ROTATE_CLOCKWISE );
            else if( aEvent.GetId() == ID_SCH_ROTATE_COUNTERCLOCKWISE )
                OrientComponent( CMP_ROTATE_COUNTERCLOCKWISE );
            else
                wxFAIL_MSG( wxT( "Unknown rotate item command ID." ) );

            if( m_autoplaceFields )
                component->AutoAutoplaceFields( GetScreen() );

            m_canvas->Refresh();

            break;
        }

    case SCH_TEXT_T:
    case SCH_LABEL_T:
    case SCH_GLOBAL_LABEL_T:
    case SCH_HIERARCHICAL_LABEL_T:
        m_canvas->MoveCursorToCrossHair();
        ChangeTextOrient( (SCH_TEXT*) item );
        m_canvas->Refresh();
        break;

    case SCH_FIELD_T:
        m_canvas->MoveCursorToCrossHair();
        RotateField( (SCH_FIELD*) item );
        if( item->GetParent()->Type() == SCH_COMPONENT_T )
        {
            // Now that we're moving a field, they're no longer autoplaced.
            SCH_COMPONENT *parent = static_cast<SCH_COMPONENT*>( item->GetParent() );
            parent->ClearFieldsAutoplaced();
        }
        m_canvas->Refresh();
        break;

    case SCH_BITMAP_T:
        RotateImage( (SCH_BITMAP*) item );
        break;

    case SCH_SHEET_T:
        if( !item->IsNew() )    // rotate a sheet during its creation has no sense
        {
            bool retCCW = ( aEvent.GetId() == ID_SCH_ROTATE_COUNTERCLOCKWISE );
            RotateHierarchicalSheet( static_cast<SCH_SHEET*>( item ), retCCW );
        }

        break;

    case SCH_JUNCTION_T:
    case SCH_NO_CONNECT_T:
        // these items are not rotated, because rotation does not change them.
        break;

    default:
        // Other items (wires...) cannot be rotated, at least during creation
        if( item->IsNew() )
            break;

        wxFAIL_MSG( wxString::Format( wxT( "Cannot rotate schematic item type %s." ),
                                      GetChars( item->GetClass() ) ) );
    }

    if( item->GetFlags() == 0 )
        screen->SetCurItem( NULL );
}


void SCH_EDIT_FRAME::OnEditItem( wxCommandEvent& aEvent )
{
    SCH_SCREEN* screen = GetScreen();
    SCH_ITEM* item = screen->GetCurItem();

    if( item == NULL )
    {
        // If we didn't get here by a hot key, then something has gone wrong.
        if( aEvent.GetInt() == 0 )
            return;

        EDA_HOTKEY_CLIENT_DATA* data = (EDA_HOTKEY_CLIENT_DATA*) aEvent.GetClientObject();

        wxCHECK_RET( data != NULL, wxT( "Invalid hot key client object." ) );

        // Set the locat filter, according to the edit command
        const KICAD_T* filterList = SCH_COLLECTOR::EditableItems;
        const KICAD_T* filterListAux = NULL;

        switch( aEvent.GetId() )
        {
        case ID_SCH_EDIT_COMPONENT_REFERENCE:
            filterList = SCH_COLLECTOR::CmpFieldReferenceOnly;
            filterListAux = SCH_COLLECTOR::ComponentsOnly;
            break;

        case ID_SCH_EDIT_COMPONENT_VALUE:
            filterList = SCH_COLLECTOR::CmpFieldValueOnly;
            filterListAux = SCH_COLLECTOR::ComponentsOnly;
            break;

        case ID_SCH_EDIT_COMPONENT_FOOTPRINT:
            filterList = SCH_COLLECTOR::CmpFieldFootprintOnly;
            filterListAux = SCH_COLLECTOR::ComponentsOnly;
            break;

        default:
            break;
        }

        item = LocateAndShowItem( data->GetPosition(), filterList, aEvent.GetInt() );

        // If no item found, and if an auxiliary filter exists, try to use it
        if( !item && filterListAux )
            item = LocateAndShowItem( data->GetPosition(), filterListAux, aEvent.GetInt() );

        // Exit if no item found at the current location or the item is already being edited.
        if( (item == NULL) || (item->GetFlags() != 0) )
            return;
    }

    switch( item->Type() )
    {
    case SCH_COMPONENT_T:
    {
        switch( aEvent.GetId() )
        {
        case ID_SCH_EDIT_COMPONENT_REFERENCE:
            EditComponentFieldText( ( (SCH_COMPONENT*) item )->GetField( REFERENCE ) );
            break;

        case ID_SCH_EDIT_COMPONENT_VALUE:
            EditComponentFieldText( ( (SCH_COMPONENT*) item )->GetField( VALUE ) );
            break;

        case ID_SCH_EDIT_COMPONENT_FOOTPRINT:
            EditComponentFieldText( ( (SCH_COMPONENT*) item )->GetField( FOOTPRINT ) );
            break;

        case ID_SCH_EDIT_ITEM:
            EditComponent( (SCH_COMPONENT*) item );
            break;

        default:
            wxFAIL_MSG( wxString::Format( wxT( "Invalid schematic component edit command ID %d" ),
                                          aEvent.GetId() ) );
        }

        break;
    }

    case SCH_SHEET_T:
        if( EditSheet( (SCH_SHEET*) item, g_CurrentSheet ) )
            m_canvas->Refresh();
        break;

    case SCH_SHEET_PIN_T:
        EditSheetPin( (SCH_SHEET_PIN*) item, true );
        break;

    case SCH_TEXT_T:
    case SCH_LABEL_T:
    case SCH_GLOBAL_LABEL_T:
    case SCH_HIERARCHICAL_LABEL_T:
        EditSchematicText( (SCH_TEXT*) item );
        break;

    case SCH_FIELD_T:
        EditComponentFieldText( (SCH_FIELD*) item );
        break;

    case SCH_BITMAP_T:
        EditImage( (SCH_BITMAP*) item );
        break;

    case SCH_LINE_T:        // These items have no param to edit
        EditLine( (SCH_LINE*) item, true );
        break;
    case SCH_MARKER_T:
    case SCH_JUNCTION_T:
    case SCH_NO_CONNECT_T:
        break;

    default:                // Unexpected item
        wxFAIL_MSG( wxString::Format( wxT( "Cannot edit schematic item type %s." ),
                                      GetChars( item->GetClass() ) ) );
    }

    if( item->GetFlags() == 0 )
        screen->SetCurItem( NULL );
}


void SCH_EDIT_FRAME::OnDragItem( wxCommandEvent& aEvent )
{
    SCH_SCREEN* screen = GetScreen();
    SCH_ITEM* item = screen->GetCurItem();

    // The easiest way to handle a menu or a hot key drag command
    // is to simulate a block drag command
    //
    // When a drag item is requested, some items use a BLOCK_DRAG_ITEM drag type
    // an some items use a BLOCK_DRAG drag type  (mainly a junction)
    // a BLOCK_DRAG collects all items in a block (here a 2x2 rect centered on the cursor)
    // and BLOCK_DRAG_ITEM drag only the selected item
    BLOCK_COMMAND_T dragType = BLOCK_DRAG_ITEM;

    if( item == NULL )
    {
        // If we didn't get here by a hot key, then something has gone wrong.
        if( aEvent.GetInt() == 0 )
            return;

        EDA_HOTKEY_CLIENT_DATA* data = (EDA_HOTKEY_CLIENT_DATA*) aEvent.GetClientObject();

        wxCHECK_RET( data != NULL, wxT( "Invalid hot key client object." ) );

        item = LocateAndShowItem( data->GetPosition(), SCH_COLLECTOR::DraggableItems,
                                  aEvent.GetInt() );

        // Exit if no item found at the current location or the item is already being edited.
        if( (item == NULL) || (item->GetFlags() != 0) )
            return;

        // When a junction or a node is found, a BLOCK_DRAG is better
        if( m_collectedItems.IsCorner() || m_collectedItems.IsNode( false )
            || m_collectedItems.IsDraggableJunction() )
            dragType = BLOCK_DRAG;
    }

    switch( item->Type() )
    {
    case SCH_BUS_BUS_ENTRY_T:
    case SCH_BUS_WIRE_ENTRY_T:
    case SCH_LINE_T:
    case SCH_JUNCTION_T:
    case SCH_COMPONENT_T:
    case SCH_LABEL_T:
    case SCH_GLOBAL_LABEL_T:
    case SCH_HIERARCHICAL_LABEL_T:
    case SCH_SHEET_T:
        m_canvas->MoveCursorToCrossHair();

        if( screen->m_BlockLocate.GetState() == STATE_NO_BLOCK )
        {
            INSTALL_UNBUFFERED_DC( dc, m_canvas );

            if( !HandleBlockBegin( &dc, dragType, GetCrossHairPosition() ) )
                break;

            // Give a non null size to the search block:
            screen->m_BlockLocate.Inflate( 1 );
            HandleBlockEnd( &dc );
        }

        break;

    default:
        wxFAIL_MSG( wxString::Format( wxT( "Cannot drag schematic item type %s." ),
                                      GetChars( item->GetClass() ) ) );
    }
}


void SCH_EDIT_FRAME::OnOrient( wxCommandEvent& aEvent )
{
    SCH_SCREEN* screen = GetScreen();
    SCH_ITEM*   item   = screen->GetCurItem();

    INSTALL_UNBUFFERED_DC( dc, m_canvas );

    // Allows block rotate operation on hot key.
    if( screen->m_BlockLocate.GetState() != STATE_NO_BLOCK )
    {
        if( aEvent.GetId() == ID_SCH_MIRROR_X )
        {
            m_canvas->MoveCursorToCrossHair();
            screen->m_BlockLocate.SetMessageBlock( this );
            screen->m_BlockLocate.SetCommand( BLOCK_MIRROR_X );
            HandleBlockEnd( &dc );
        }
        else if( aEvent.GetId() == ID_SCH_MIRROR_Y )
        {
            m_canvas->MoveCursorToCrossHair();
            screen->m_BlockLocate.SetMessageBlock( this );
            screen->m_BlockLocate.SetCommand( BLOCK_MIRROR_Y );
            HandleBlockEnd( &dc );
        }
        else
        {
            wxFAIL_MSG( wxT( "Unknown block oriention command ID." ) );
        }

        return;
    }

    if( item == NULL )
    {
        // If we didn't get here by a hot key, then something has gone wrong.
        if( aEvent.GetInt() == 0 )
            return;

        EDA_HOTKEY_CLIENT_DATA* data = (EDA_HOTKEY_CLIENT_DATA*) aEvent.GetClientObject();

        wxCHECK_RET( data != NULL, wxT( "Invalid hot key client object." ) );

        item = LocateAndShowItem( data->GetPosition(), SCH_COLLECTOR::OrientableItems,
                                  aEvent.GetInt() );

        // Exit if no item found at the current location or the item is already being edited.
        if( (item == NULL) || (item->GetFlags() != 0) )
            return;
    }


    switch( item->Type() )
    {
    case SCH_COMPONENT_T:
        {
            SCH_COMPONENT *component = static_cast<SCH_COMPONENT*>( item );
            if( aEvent.GetId() == ID_SCH_MIRROR_X )
                OrientComponent( CMP_MIRROR_X );
            else if( aEvent.GetId() == ID_SCH_MIRROR_Y )
                OrientComponent( CMP_MIRROR_Y );
            else if( aEvent.GetId() == ID_SCH_ORIENT_NORMAL )
                OrientComponent( CMP_NORMAL );
            else
                wxFAIL_MSG( wxT( "Invalid orient schematic component command ID." ) );

            if( m_autoplaceFields )
                component->AutoAutoplaceFields( GetScreen() );

            m_canvas->Refresh();

            break;
        }

    case SCH_BITMAP_T:
        if( aEvent.GetId() == ID_SCH_MIRROR_X )
            MirrorImage( (SCH_BITMAP*) item, true );
        else if( aEvent.GetId() == ID_SCH_MIRROR_Y )
            MirrorImage( (SCH_BITMAP*) item, false );

        break;

    case SCH_SHEET_T:
        if( aEvent.GetId() == ID_SCH_MIRROR_X )
            MirrorSheet( (SCH_SHEET*) item, true );
        else if( aEvent.GetId() == ID_SCH_MIRROR_Y )
            MirrorSheet( (SCH_SHEET*) item, false );

        break;

    default:
        // This object cannot be oriented.
        ;
    }

    if( item->GetFlags() == 0 )
        screen->SetCurItem( NULL );
}


void SCH_EDIT_FRAME::OnUnfoldBusHotkey( wxCommandEvent& aEvent )
{
    auto data = (EDA_HOTKEY_CLIENT_DATA*) aEvent.GetClientObject();
    auto item = GetScreen()->GetCurItem();

    wxCHECK_RET( data != NULL, wxT( "Invalid hot key client object." ) );

    if( item == NULL )
    {
        // If we didn't get here by a hot key, then something has gone wrong.
        if( aEvent.GetInt() == 0 )
            return;

        item = LocateAndShowItem( data->GetPosition(), SCH_COLLECTOR::EditableItems,
                                  aEvent.GetInt() );

        // Exit if no item found at the current location or the item is already being edited.
        if( (item == NULL) || (item->GetFlags() != 0) )
            return;
    }

    auto connection = item->Connection( *g_CurrentSheet );

    if( connection && connection->IsBus() )
    {
        int idx = 0;
        wxMenu* bus_unfolding_menu = new wxMenu;

        for( const auto& member : connection->Members() )
        {
            int id = ID_POPUP_SCH_UNFOLD_BUS + ( idx++ );

            if( member->Type() == CONNECTION_BUS )
            {
                wxMenu* submenu = new wxMenu;
                bus_unfolding_menu->AppendSubMenu( submenu, _( member->Name() ) );

                for( const auto& sub_member : member->Members() )
                {
                    id = ID_POPUP_SCH_UNFOLD_BUS + ( idx++ );

                    submenu->Append( id, sub_member->Name(), wxEmptyString );

                    // See comment in else clause below
                    auto sub_item_clone = new wxMenuItem();
                    sub_item_clone->SetItemLabel( sub_member->Name() );

                    Bind( wxEVT_COMMAND_MENU_SELECTED, &SCH_EDIT_FRAME::OnUnfoldBus,
                                 this, id, id, sub_item_clone );
                }
            }
            else
            {
                bus_unfolding_menu->Append( id, member->Name(),
                                            wxEmptyString );

                // Because Bind() takes ownership of the user data item, we
                // make a new menu item here and set its label.  Why create a
                // menu item instead of just a wxString or something? Because
                // Bind() requires a pointer to wxObject rather than a void
                // pointer.  Maybe at some point I'll think of a better way...
                auto item_clone = new wxMenuItem();
                item_clone->SetItemLabel( member->Name() );

                Bind( wxEVT_COMMAND_MENU_SELECTED, &SCH_EDIT_FRAME::OnUnfoldBus,
                             this, id, id, item_clone );
            }
        }

        m_canvas->PopupMenu( bus_unfolding_menu, GetCrossHairScreenPosition() );
    }
}
