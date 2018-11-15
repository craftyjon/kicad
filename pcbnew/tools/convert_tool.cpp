/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 1992-2018 KiCad Developers, see AUTHORS.txt for contributors.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <bitmaps.h>
#include <board_commit.h>
#include <class_board.h>
#include <class_drawsegment.h>
#include <class_zone.h>
#include <collectors.h>
#include <confirm.h>
#include <menus_helpers.h>
#include <tool/tool_manager.h>
#include <pcb_edit_frame.h>

#include "edit_tool.h"
#include "pcb_actions.h"
#include "selection_tool.h"
#include "tool/selection.h"

#include "convert_tool.h"

/**
 * Conversions to support:
 *
 * Lines, tracks to (graphic) polygon, zone, keepout
 *
 * Polygon/zone/keepout to lines, tracks
 *
 * Graphic arc, graphic circle to tracks (approximation)
 *
 * Graphic circle to zone/keepout (approximation) maybe?
 * Or just add support for round zones/keepouts
 */

// TODO: Icons
TOOL_ACTION PCB_ACTIONS::convertLinesToPoly( "pcbnew.Convert.convertLinesToPoly",
        AS_GLOBAL, 0,
        _( "Lines to Polygon" ),
        _( "Converts selcted lines to a polygon" ), add_graphical_polygon_xpm );

CONVERT_TOOL::CONVERT_TOOL() :
    TOOL_INTERACTIVE( "pcbnew.Convert" ), m_selectionTool( NULL ),
    m_menu( NULL ), m_frame( NULL )
{
}

CONVERT_TOOL::~CONVERT_TOOL()
{
    delete m_menu;
}


bool CONVERT_TOOL::Init()
{
    m_selectionTool = static_cast<SELECTION_TOOL*>( m_toolMgr->FindTool( "pcbnew.InteractiveSelection" ) );

    if( !m_selectionTool )
    {
        DisplayError( NULL, wxT( "pcbnew.InteractiveSelection tool is not available" ) );
        return false;
    }

    m_frame = getEditFrame<PCB_BASE_FRAME>();

    // Create a context menu and make it available through selection tool
    m_menu = new CONTEXT_MENU;
    m_menu->SetIcon( align_items_xpm );
    m_menu->SetTitle( _( "Convert" ) );

    m_menu->Add( PCB_ACTIONS::convertLinesToPoly );

    // TODO: Actual selection conditions
    m_selectionTool->GetToolMenu().GetMenu().AddMenu( m_menu, false );

    return true;
}


int CONVERT_TOOL::LinesToPoly( const TOOL_EVENT& aEvent )
{
    auto& selection = m_selectionTool->RequestSelection(
        []( const VECTOR2I& aPt, GENERAL_COLLECTOR& aCollector )
        {
            EditToolSelectionFilter( aCollector,
                                     EXCLUDE_LOCKED | EXCLUDE_TRANSIENTS );


            for( int i = aCollector.GetCount() - 1; i >= 0; --i )
            {
                auto item = aCollector[i];

                if( item->Type() != PCB_LINE_T )
                    aCollector.Remove( item );
            }
        } );

    BOARD_COMMIT commit( m_frame );

    SHAPE_POLY_SET polySet;

    polySet.NewOutline();

    auto items = selection.GetItems();

    while( !items.empty() )
    {
        if( polySet.VertexCount() == 0 )
        {
            auto line = static_cast<DRAWSEGMENT*>( items.back() );
            items.pop_back();

            polySet.Append( VECTOR2I( line->GetStart() ) );
            polySet.Append( VECTOR2I( line->GetEnd() ) );

            commit.Remove( line );
        }
        else
        {
            auto testPoint = polySet.Vertex( polySet.VertexCount() - 1 );
            bool found = false;
            BOARD_ITEM* toRemove = nullptr;

            for( auto next : items )
            {
                auto nextLine = static_cast<DRAWSEGMENT*>( next );

                if( testPoint == nextLine->GetStart() )
                    polySet.Append( VECTOR2I( nextLine->GetEnd() ) );
                else if( testPoint == nextLine->GetEnd() )
                    polySet.Append( VECTOR2I( nextLine->GetStart() ) );
                else
                    continue;

                found = true;
                commit.Remove( nextLine );
                toRemove = nextLine;
            }

            if( toRemove )
                items.erase( std::remove( items.begin(), items.end(), toRemove ), items.end() );

            if( !found )
                break;
        }
    }

    if( polySet.Vertex( polySet.VertexCount() - 1 ) == polySet.Vertex( 0 ) )
        polySet.RemoveVertex( polySet.VertexCount() - 1 );

    if( polySet.VertexCount() < 3 )
    {
        commit.Revert();

        // TODO(JE) some kind of error feedback to the user

        return 0;
    }

    // TODO: smarter layer selection?
    PCB_LAYER_ID layer = m_frame->GetActiveLayer();

    auto poly = new DRAWSEGMENT;

    poly->SetShape( S_POLYGON );
    poly->SetLayer( layer );
    poly->SetPolyShape( polySet );

    commit.Add( poly );

    commit.Push( _( "Convert lines to polygon" ) );

    return 0;
}


void CONVERT_TOOL::setTransitions()
{
    Go( &CONVERT_TOOL::LinesToPoly,    PCB_ACTIONS::convertLinesToPoly.MakeEvent() );
}
