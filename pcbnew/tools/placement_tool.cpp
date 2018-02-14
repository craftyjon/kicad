/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2014-2016 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
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
#include "tool/selection.h"
#include "placement_tool.h"
#include "pcb_actions.h"
#include "selection_tool.h"
#include <tool/tool_manager.h>

#include <pcb_edit_frame.h>
#include <class_board.h>
#include <ratsnest_data.h>
#include <board_commit.h>
#include <bitmaps.h>

#include <confirm.h>
#include <menus_helpers.h>

// Placement tool
TOOL_ACTION PCB_ACTIONS::alignTop( "pcbnew.AlignAndDistribute.alignTop",
        AS_GLOBAL, 0,
        _( "Align to Top" ),
        _( "Aligns selected items to the top edge" ), up_xpm );

TOOL_ACTION PCB_ACTIONS::alignBottom( "pcbnew.AlignAndDistribute.alignBottom",
        AS_GLOBAL, 0,
        _( "Align to Bottom" ),
        _( "Aligns selected items to the bottom edge" ), down_xpm );

TOOL_ACTION PCB_ACTIONS::alignLeft( "pcbnew.AlignAndDistribute.alignLeft",
        AS_GLOBAL, 0,
        _( "Align to Left" ),
        _( "Aligns selected items to the left edge" ), left_xpm );

TOOL_ACTION PCB_ACTIONS::alignRight( "pcbnew.AlignAndDistribute.alignRight",
        AS_GLOBAL, 0,
        _( "Align to Right" ),
        _( "Aligns selected items to the right edge" ), right_xpm );

TOOL_ACTION PCB_ACTIONS::distributeHorizontally( "pcbnew.AlignAndDistribute.distributeHorizontally",
        AS_GLOBAL, 0,
        _( "Distribute Horizontally" ),
        _( "Distributes selected items along the horizontal axis" ), distribute_horizontal_xpm );

TOOL_ACTION PCB_ACTIONS::distributeVertically( "pcbnew.AlignAndDistribute.distributeVertically",
        AS_GLOBAL, 0,
        _( "Distribute Vertically" ),
        _( "Distributes selected items along the vertical axis" ), distribute_vertical_xpm );


ALIGN_DISTRIBUTE_TOOL::ALIGN_DISTRIBUTE_TOOL() :
    TOOL_INTERACTIVE( "pcbnew.Placement" ), m_selectionTool( NULL ), m_placementMenu( NULL )
{
}

ALIGN_DISTRIBUTE_TOOL::~ALIGN_DISTRIBUTE_TOOL()
{
    delete m_placementMenu;
}


bool ALIGN_DISTRIBUTE_TOOL::Init()
{
    // Find the selection tool, so they can cooperate
    m_selectionTool = static_cast<SELECTION_TOOL*>( m_toolMgr->FindTool( "pcbnew.InteractiveSelection" ) );

    if( !m_selectionTool )
    {
        DisplayError( NULL, wxT( "pcbnew.InteractiveSelection tool is not available" ) );
        return false;
    }

    // Create a context menu and make it available through selection tool
    m_placementMenu = new CONTEXT_MENU;
    m_placementMenu->SetIcon( align_items_xpm );
    m_placementMenu->SetTitle( _( "Align/Distribute" ) );

    // Add all align/distribute commands
    m_placementMenu->Add( PCB_ACTIONS::alignTop );
    m_placementMenu->Add( PCB_ACTIONS::alignBottom );
    m_placementMenu->Add( PCB_ACTIONS::alignLeft );
    m_placementMenu->Add( PCB_ACTIONS::alignRight );
    m_placementMenu->AppendSeparator();
    m_placementMenu->Add( PCB_ACTIONS::distributeHorizontally );
    m_placementMenu->Add( PCB_ACTIONS::distributeVertically );

    m_selectionTool->GetToolMenu().GetMenu().AddMenu( m_placementMenu, false,
            SELECTION_CONDITIONS::MoreThan( 1 ) );

    return true;
}


bool SortLeftmostX( const std::pair<BOARD_ITEM*, EDA_RECT> left, const std::pair<BOARD_ITEM*, EDA_RECT> right)
{
    return ( left.second.GetX() < right.second.GetX() );
}

bool SortRightmostX( const std::pair<BOARD_ITEM*, EDA_RECT> left, const std::pair<BOARD_ITEM*, EDA_RECT> right)
{
    return ( left.second.GetRight() > right.second.GetRight() );
}

bool SortTopmostY( const std::pair<BOARD_ITEM*, EDA_RECT> left, const std::pair<BOARD_ITEM*, EDA_RECT> right)
{
    return ( left.second.GetY() < right.second.GetY() );
}

bool SortBottommostY( const std::pair<BOARD_ITEM*, EDA_RECT> left, const std::pair<BOARD_ITEM*, EDA_RECT> right)
{
    return ( left.second.GetBottom() > right.second.GetBottom() );
}

ALIGNMENT_SET GetBoundingBoxesV( const SELECTION& sel )
{
    const SELECTION& selection = sel;

    ALIGNMENT_SET aSet;

    for( auto item : selection )
    {
        if( item->Type() == PCB_MODULE_T )
        {
            aSet.push_back( std::make_pair( static_cast<BOARD_ITEM*>( item ), static_cast<MODULE*>( item )->GetFootprintRect() ) );
        }
        else
        {
            aSet.push_back( std::make_pair( static_cast<BOARD_ITEM*>( item ), static_cast<MODULE*>( item )->GetBoundingBox() ) );
        }
    }
    return aSet;
}


int ALIGN_DISTRIBUTE_TOOL::AlignTop( const TOOL_EVENT& aEvent )
{
    const SELECTION& selection = m_selectionTool->GetSelection();

    if( selection.Size() <= 1 )
        return 0;

    BOARD_COMMIT commit( getEditFrame<PCB_BASE_FRAME>() );
    commit.StageItems( selection, CHT_MODIFY );

    // Compute the topmost point of selection - it will be the edge of alignment
    auto alignMap = GetBoundingBoxesV( selection );
    std::sort( alignMap.begin(), alignMap.end(), SortTopmostY );

    int top = alignMap.begin()->second.GetY();

    // Move the selected items
    for( auto& i : alignMap )
    {
        int difference = top - i.second.GetY();
        i.first->Move( wxPoint( 0, difference ) );
    }

    commit.Push( _( "Align to top" ) );

    return 0;
}


int ALIGN_DISTRIBUTE_TOOL::AlignBottom( const TOOL_EVENT& aEvent )
{
    const SELECTION& selection = m_selectionTool->GetSelection();

    if( selection.Size() <= 1 )
        return 0;

    BOARD_COMMIT commit( getEditFrame<PCB_BASE_FRAME>() );
    commit.StageItems( selection, CHT_MODIFY );

    // Compute the lowest point of selection - it will be the edge of alignment
    auto alignMap = GetBoundingBoxesV( selection );
    std::sort( alignMap.begin(), alignMap.end(), SortBottommostY );

   int bottom = alignMap.begin()->second.GetBottom();

    // Move the selected items
    for( auto& i : alignMap )
    {
        int difference = bottom - i.second.GetBottom();
        i.first->Move( wxPoint( 0, difference ) );
    }

    commit.Push( _( "Align to bottom" ) );

    return 0;
}


int ALIGN_DISTRIBUTE_TOOL::AlignLeft( const TOOL_EVENT& aEvent )
{
    // Because this tool uses bounding boxes and they aren't mirrored even when
    // the view is mirrored, we need to call the other one if mirrored.
    if( getView()->IsMirroredX() )
    {
        return doAlignRight();
    }
    else
    {
        return doAlignLeft();
    }
}


int ALIGN_DISTRIBUTE_TOOL::doAlignLeft()
{
    const SELECTION& selection = m_selectionTool->GetSelection();

    if( selection.Size() <= 1 )
        return 0;

    BOARD_COMMIT commit( getEditFrame<PCB_BASE_FRAME>() );
    commit.StageItems( selection, CHT_MODIFY );

    // Compute the leftmost point of selection - it will be the edge of alignment
   auto alignMap = GetBoundingBoxesV( selection );

   std::sort( alignMap.begin(), alignMap.end(), SortLeftmostX );

   int left = alignMap.begin()->second.GetX();

    // Move the selected items
    for( auto& i : alignMap )
    {
        int difference = left - i.second.GetX();
        i.first->Move( wxPoint( difference, 0 ) );
    }

    commit.Push( _( "Align to left" ) );

    return 0;
}


int ALIGN_DISTRIBUTE_TOOL::AlignRight( const TOOL_EVENT& aEvent )
{
    // Because this tool uses bounding boxes and they aren't mirrored even when
    // the view is mirrored, we need to call the other one if mirrored.
    if( getView()->IsMirroredX() )
    {
        return doAlignLeft();
    }
    else
    {
        return doAlignRight();
    }
}


int ALIGN_DISTRIBUTE_TOOL::doAlignRight()
{
    const SELECTION& selection = m_selectionTool->GetSelection();

    if( selection.Size() <= 1 )
        return 0;

    BOARD_COMMIT commit( getEditFrame<PCB_BASE_FRAME>() );
    commit.StageItems( selection, CHT_MODIFY );

    // Compute the rightmost point of selection - it will be the edge of alignment
    auto alignMap = GetBoundingBoxesV( selection );
    std::sort( alignMap.begin(), alignMap.end(), SortRightmostX );

    int right = alignMap.begin()->second.GetRight();

    // Move the selected items
    for( auto& i : alignMap )
    {
        int difference = right - i.second.GetRight();
        i.first->Move( wxPoint( difference, 0 ) );
    }

    commit.Push( _( "Align to right" ) );

    return 0;
}


int ALIGN_DISTRIBUTE_TOOL::DistributeHorizontally( const TOOL_EVENT& aEvent )
{
    const SELECTION& selection = m_selectionTool->GetSelection();

    if( selection.Size() <= 1 )
        return 0;

    BOARD_COMMIT commit( getEditFrame<PCB_BASE_FRAME>() );
    commit.StageItems( selection, CHT_MODIFY );

    auto alignMap = GetBoundingBoxesV( selection );

    // find the last item by reverse sorting
    std::sort( alignMap.begin(), alignMap.end(), SortRightmostX );
    const auto maxRight = alignMap.begin()->second.GetRight();
    const auto lastItem = alignMap.begin()->first;

    // sort to get starting order
    std::sort( alignMap.begin(), alignMap.end(), SortLeftmostX );

    auto totalGap = maxRight - alignMap.begin()->second.GetX();

    for( auto& i : alignMap )
    {
        totalGap -= i.second.GetWidth();
    }

    const auto itemGap = totalGap / ( alignMap.size() - 1 );

    auto targetX = alignMap.begin()->second.GetX();

    for( auto& i : alignMap )
    {
        // cover the corner case where the last item is wider than the previous item and gap
        if( lastItem == i.first )
            continue;

        int difference = targetX - i.second.GetX();
        i.first->Move( wxPoint( difference, 0 ) );
        targetX += ( i.second.GetWidth() + itemGap );
    }

    commit.Push( _( "Distribute horizontally" ) );

    return 0;
}


int ALIGN_DISTRIBUTE_TOOL::DistributeVertically( const TOOL_EVENT& aEvent )
{
    const SELECTION& selection = m_selectionTool->GetSelection();

    if( selection.Size() <= 1 )
        return 0;

    BOARD_COMMIT commit( getEditFrame<PCB_BASE_FRAME>() );
    commit.StageItems( selection, CHT_MODIFY );

    auto alignMap = GetBoundingBoxesV( selection );

    // find the last item by reverse sorting
    std::sort( alignMap.begin(), alignMap.end(), SortBottommostY );
    const auto maxBottom = alignMap.begin()->second.GetBottom();
    const auto lastItem = alignMap.begin()->first;

    // sort to get starting order
    std::sort( alignMap.begin(), alignMap.end(), SortTopmostY );

    auto totalGap = maxBottom - alignMap.begin()->second.GetY();

    for( auto& i : alignMap )
    {
        totalGap -= i.second.GetHeight();
    }

    const auto itemGap = totalGap / ( alignMap.size() - 1 );

    auto targetY = alignMap.begin()->second.GetY();

    for( auto& i : alignMap )
    {
        // cover the corner case where the last item is wider than the previous item and gap
        if( lastItem == i.first )
            continue;

        int difference = targetY - i.second.GetY();
        i.first->Move( wxPoint( 0, difference ) );
        targetY += ( i.second.GetHeight() + itemGap );
    }

    commit.Push( _( "Distribute vertically" ) );

    return 0;
}


void ALIGN_DISTRIBUTE_TOOL::setTransitions()
{
    Go( &ALIGN_DISTRIBUTE_TOOL::AlignTop,    PCB_ACTIONS::alignTop.MakeEvent() );
    Go( &ALIGN_DISTRIBUTE_TOOL::AlignBottom, PCB_ACTIONS::alignBottom.MakeEvent() );
    Go( &ALIGN_DISTRIBUTE_TOOL::AlignLeft,   PCB_ACTIONS::alignLeft.MakeEvent() );
    Go( &ALIGN_DISTRIBUTE_TOOL::AlignRight,  PCB_ACTIONS::alignRight.MakeEvent() );

    Go( &ALIGN_DISTRIBUTE_TOOL::DistributeHorizontally,  PCB_ACTIONS::distributeHorizontally.MakeEvent() );
    Go( &ALIGN_DISTRIBUTE_TOOL::DistributeVertically,    PCB_ACTIONS::distributeVertically.MakeEvent() );
}
