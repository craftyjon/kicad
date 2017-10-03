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

#include <limits>

#include <functional>
using namespace std::placeholders;

#include <base_struct.h>

//#include <confirm.h>

#include <class_draw_panel_gal.h>
#include <view/view.h>
#include <view/view_controls.h>
#include <view/view_group.h>
#include <painter.h>
#include <bitmaps.h>
#include <hotkeys.h>

#include <tool/tool_event.h>
#include <tool/tool_manager.h>
#include <preview_items/ruler_item.h>
#include <preview_items/selection_area.h>

#include <gerbview_id.h>
#include <gerbview_painter.h>

#include "gerbview_selection_tool.h"
#include "gerbview_actions.h"

// Selection tool actions
TOOL_ACTION GERBVIEW_ACTIONS::selectionActivate( "gerbview.InteractiveSelection",
        AS_GLOBAL, 0,
        "", "", NULL, AF_ACTIVATE ); // No description, it is not supposed to be shown anywhere

TOOL_ACTION GERBVIEW_ACTIONS::selectionCursor( "gerbview.InteractiveSelection.Cursor",
        AS_GLOBAL, 0,
        "", "" );    // No description, it is not supposed to be shown anywhere

TOOL_ACTION GERBVIEW_ACTIONS::selectItem( "gerbview.InteractiveSelection.SelectItem",
        AS_GLOBAL, 0,
        "", "" );    // No description, it is not supposed to be shown anywhere

TOOL_ACTION GERBVIEW_ACTIONS::unselectItem( "gerbview.InteractiveSelection.UnselectItem",
        AS_GLOBAL, 0,
        "", "" );    // No description, it is not supposed to be shown anywhere

TOOL_ACTION GERBVIEW_ACTIONS::selectionClear( "gerbview.InteractiveSelection.Clear",
        AS_GLOBAL, 0,
        "", "" );    // No description, it is not supposed to be shown anywhere

TOOL_ACTION GERBVIEW_ACTIONS::measureTool( "gerbview.InteractiveSelection.measureTool",
        AS_GLOBAL, MD_CTRL + MD_SHIFT + 'M',
        _( "Measure tool" ), _( "Interactively measure distance between points" ),
        nullptr, AF_ACTIVATE );


class HIGHLIGHT_MENU: public CONTEXT_MENU
{
public:
    HIGHLIGHT_MENU()
    {
        SetTitle( _( "Highlight..." ) );
    }

private:

    void update() override
    {
        const auto& selection = getToolManager()->GetTool<GERBVIEW_SELECTION_TOOL>()->GetSelection();

        if( selection.Size() == 1 )
        {
            auto item = static_cast<GERBER_DRAW_ITEM*>( selection[0] );
            const auto& net_attr = item->GetNetAttributes();

            if( ( net_attr.m_NetAttribType & GBR_NETLIST_METADATA::GBR_NETINFO_PAD ) ||
                ( net_attr.m_NetAttribType & GBR_NETLIST_METADATA::GBR_NETINFO_CMP ) )
            {
                auto menuEntry = Add( GERBVIEW_ACTIONS::highlightComponent );
                menuEntry->SetItemLabel( wxString::Format( _( "Highlight items of component '%s'" ),
                                         GetChars( net_attr.m_Cmpref ) ) );
            }

            if( ( net_attr.m_NetAttribType & GBR_NETLIST_METADATA::GBR_NETINFO_NET ) )
            {
                auto menuEntry = Add( GERBVIEW_ACTIONS::highlightNet );
                menuEntry->SetItemLabel( wxString::Format( _( "Highlight items of net '%s'" ),
                                         GetChars( net_attr.m_Netname ) ) );
            }

            D_CODE* apertDescr = item->GetDcodeDescr();

            if( apertDescr && !apertDescr->m_AperFunction.IsEmpty() )
            {
                auto menuEntry = Add( GERBVIEW_ACTIONS::highlightAttribute );
                menuEntry->SetItemLabel( wxString::Format( _( "Highlight aperture type '%s'" ),
                                         GetChars( apertDescr->m_AperFunction ) ) );
            }
        }

        Add( GERBVIEW_ACTIONS::highlightClear );
    }

    CONTEXT_MENU* create() const override
    {
        return new HIGHLIGHT_MENU();
    }
};


GERBVIEW_SELECTION_TOOL::GERBVIEW_SELECTION_TOOL() :
        SELECTION_TOOL()
{
}


GERBVIEW_SELECTION_TOOL::~GERBVIEW_SELECTION_TOOL()
{
    getView()->Remove( &m_selection );
}


bool GERBVIEW_SELECTION_TOOL::Init()
{
    auto selectMenu = std::make_shared<HIGHLIGHT_MENU>();
    selectMenu->SetTool( this );
    m_menu.AddSubMenu( selectMenu );

    auto& menu = m_menu.GetMenu();

    menu.AddMenu( selectMenu.get(), false );
    menu.AddSeparator( SELECTION_CONDITIONS::ShowAlways, 1000 );

    m_menu.AddStandardSubMenus( *getEditFrame<GERBVIEW_FRAME>() );

    return true;
}


void GERBVIEW_SELECTION_TOOL::Reset( RESET_REASON aReason )
{
    m_frame = getEditFrame<GERBVIEW_FRAME>();
    m_preliminary = true;

    if( aReason == TOOL_BASE::MODEL_RELOAD )
    {
        // Remove pointers to the selected items from containers
        // without changing their properties (as they are already deleted
        // while a new file is loaded)
        m_selection.Clear();
        getView()->GetPainter()->GetSettings()->SetHighlight( false );
    }
    else
        // Restore previous properties of selected items and remove them from containers
        clearSelection();

    // Reinsert the VIEW_GROUP, in case it was removed from the VIEW
    getView()->Remove( &m_selection );
    getView()->Add( &m_selection );
}


SELECTION& GERBVIEW_SELECTION_TOOL::RequestSelection( int aFlags )
{
    if( m_selection.Empty() )
    {
        m_toolMgr->RunAction( GERBVIEW_ACTIONS::selectionCursor, true, 0 );
        m_selection.SetIsHover( true );
    }

    // Be careful with iterators: items can be removed from list
    // that invalidate iterators.
    for( unsigned ii = 0; ii < m_selection.GetSize(); ii++ )
    {
        EDA_ITEM* item = m_selection[ii];

        if( ( aFlags & SELECTION_EDITABLE ) && item->Type() == PCB_MARKER_T )
        {
            unselect( static_cast<EDA_ITEM *>( item ) );
        }
    }

    return m_selection;
}


bool GERBVIEW_SELECTION_TOOL::selectable( const EDA_ITEM* aItem ) const
{
    auto item = static_cast<const GERBER_DRAW_ITEM*>( aItem );

    if( item->GetLayerPolarity() )
    {
        // Don't allow selection of invisible negative items
        auto rs = static_cast<KIGFX::GERBVIEW_RENDER_SETTINGS*>( getView()->GetPainter()->GetSettings() );
        if( !rs->IsShowNegativeItems() )
            return false;
    }

    return getEditFrame<GERBVIEW_FRAME>()->IsLayerVisible( item->GetLayer() );
}


void GERBVIEW_SELECTION_TOOL::selectVisually( EDA_ITEM* aItem )
{
    // Move the item's layer to the front
    int layer = static_cast<GERBER_DRAW_ITEM*>( aItem )->GetLayer();
    getFrame()->SetActiveLayer( layer, true );

    // Hide the original item, so it is shown only on overlay
    aItem->SetSelected();
    getView()->Hide( aItem, true );

    getView()->Update( &m_selection );
}


void GERBVIEW_SELECTION_TOOL::unselectVisually( EDA_ITEM* aItem )
{
    // Restore original item visibility
    aItem->ClearSelected();
    getView()->Hide( aItem, false );
    getView()->Update( aItem, KIGFX::ALL );

    getView()->Update( &m_selection );
}


void GERBVIEW_SELECTION_TOOL::setTransitions()
{
    Go( &GERBVIEW_SELECTION_TOOL::Main,             GERBVIEW_ACTIONS::selectionActivate.MakeEvent() );
    Go( &GERBVIEW_SELECTION_TOOL::CursorSelection,  GERBVIEW_ACTIONS::selectionCursor.MakeEvent() );
    Go( &GERBVIEW_SELECTION_TOOL::ClearSelection,   GERBVIEW_ACTIONS::selectionClear.MakeEvent() );
    Go( &GERBVIEW_SELECTION_TOOL::SelectItem,       GERBVIEW_ACTIONS::selectItem.MakeEvent() );
    Go( &GERBVIEW_SELECTION_TOOL::UnselectItem,     GERBVIEW_ACTIONS::unselectItem.MakeEvent() );
    Go( &GERBVIEW_SELECTION_TOOL::MeasureTool,      GERBVIEW_ACTIONS::measureTool.MakeEvent() );
}
