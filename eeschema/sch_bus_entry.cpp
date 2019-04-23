/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2004-2019 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file sch_bus_entry.cpp
 *
 */

#include <fctsys.h>
#include <gr_basic.h>
#include <macros.h>
#include <sch_draw_panel.h>
#include <trigo.h>
#include <common.h>
#include <richio.h>
#include <plotter.h>
#include <bitmaps.h>

#include <eeschema_config.h>
#include <general.h>
#include <sch_bus_entry.h>
#include <sch_line.h>
#include <sch_text.h>


SCH_BUS_ENTRY_BASE::SCH_BUS_ENTRY_BASE( KICAD_T aType, const wxPoint& pos, char shape ) :
    SCH_ITEM( NULL, aType )
{
    m_pos    = pos;
    m_size.x = 100;
    m_size.y = 100;

    if( shape == '/' )
        m_size.y = -100;

    m_isDanglingStart = m_isDanglingEnd = true;
}

SCH_BUS_WIRE_ENTRY::SCH_BUS_WIRE_ENTRY( const wxPoint& pos, char shape ) :
    SCH_BUS_ENTRY_BASE( SCH_BUS_WIRE_ENTRY_T, pos, shape )
{
    m_Layer  = LAYER_WIRE;
    m_connected_bus_item = nullptr;
}

SCH_BUS_BUS_ENTRY::SCH_BUS_BUS_ENTRY( const wxPoint& pos, char shape ) :
    SCH_BUS_ENTRY_BASE( SCH_BUS_BUS_ENTRY_T, pos, shape )
{
    m_Layer = LAYER_BUS;
    m_connected_bus_items[0] = nullptr;
    m_connected_bus_items[1] = nullptr;
}

EDA_ITEM* SCH_BUS_WIRE_ENTRY::Clone() const
{
    return new SCH_BUS_WIRE_ENTRY( *this );
}

EDA_ITEM* SCH_BUS_BUS_ENTRY::Clone() const
{
    return new SCH_BUS_BUS_ENTRY( *this );
}


bool SCH_BUS_ENTRY_BASE::doIsConnected( const wxPoint& aPosition ) const
{
    return ( m_pos == aPosition || m_End() == aPosition );
}


wxPoint SCH_BUS_ENTRY_BASE::m_End() const
{
    return wxPoint( m_pos.x + m_size.x, m_pos.y + m_size.y );
}


void SCH_BUS_ENTRY_BASE::SwapData( SCH_ITEM* aItem )
{
    SCH_BUS_ENTRY_BASE* item = dynamic_cast<SCH_BUS_ENTRY_BASE*>( aItem );
    wxCHECK_RET( item, wxT( "Cannot swap bus entry data with invalid item." ) );

    std::swap( m_pos, item->m_pos );
    std::swap( m_size, item->m_size );
}


void SCH_BUS_ENTRY_BASE::ViewGetLayers( int aLayers[], int& aCount ) const
{
    aCount      = 1;

    aLayers[0]  = Type() == SCH_BUS_BUS_ENTRY_T ? LAYER_BUS : LAYER_WIRE;
}


const EDA_RECT SCH_BUS_ENTRY_BASE::GetBoundingBox() const
{
    EDA_RECT box;

    box.SetOrigin( m_pos );
    box.SetEnd( m_End() );

    box.Normalize();
    box.Inflate( GetPenSize() / 2 );

    return box;
}


int SCH_BUS_WIRE_ENTRY::GetPenSize() const
{
    return GetDefaultLineThickness();
}


int SCH_BUS_BUS_ENTRY::GetPenSize() const
{
    return GetDefaultBusThickness();
}


void SCH_BUS_ENTRY_BASE::Draw( EDA_DRAW_PANEL* aPanel, wxDC* aDC, const wxPoint& aOffset )
{
    COLOR4D   color = GetLayerColor( m_Layer );
    EDA_RECT* clipbox = aPanel->GetClipBox();

    GRLine( clipbox, aDC, m_pos.x + aOffset.x, m_pos.y + aOffset.y,
            m_End().x + aOffset.x, m_End().y + aOffset.y, GetPenSize(), color );
}


void SCH_BUS_ENTRY_BASE::MirrorX( int aXaxis_position )
{
    MIRROR( m_pos.y, aXaxis_position );
    m_size.y = -m_size.y;
}


void SCH_BUS_ENTRY_BASE::MirrorY( int aYaxis_position )
{
    MIRROR( m_pos.x, aYaxis_position );
    m_size.x = -m_size.x;
}


void SCH_BUS_ENTRY_BASE::Rotate( wxPoint aPosition )
{
    RotatePoint( &m_pos, aPosition, 900 );
    RotatePoint( &m_size.x, &m_size.y, 900 );
}


bool SCH_BUS_ENTRY_BASE::IsDangling() const
{
    return m_isDanglingStart || m_isDanglingEnd;
}


bool SCH_BUS_ENTRY_BASE::IsSelectStateChanged( const wxRect& aRect )
{
    bool previousState = IsSelected();

    // If either end of the bus entry is inside the selection rectangle, the entire
    // bus entry is selected.  Bus entries have a fixed length and angle.
    if( aRect.Contains( m_pos ) || aRect.Contains( m_End() ) )
        SetFlags( SELECTED );
    else
        ClearFlags( SELECTED );

    return previousState != IsSelected();
}


void SCH_BUS_ENTRY_BASE::GetConnectionPoints( std::vector< wxPoint >& aPoints ) const
{
    aPoints.push_back( m_pos );
    aPoints.push_back( m_End() );
}


wxString SCH_BUS_WIRE_ENTRY::GetSelectMenuText( EDA_UNITS_T aUnits ) const
{
    return wxString( _( "Bus to Wire Entry" ) );
}


wxString SCH_BUS_BUS_ENTRY::GetSelectMenuText( EDA_UNITS_T aUnits ) const
{
    return wxString( _( "Bus to Bus Entry" ) );
}


BITMAP_DEF SCH_BUS_WIRE_ENTRY::GetMenuImage() const
{
    return add_line2bus_xpm;
}


BITMAP_DEF SCH_BUS_BUS_ENTRY::GetMenuImage() const
{
    return add_bus2bus_xpm;
}


bool SCH_BUS_ENTRY_BASE::HitTest( const wxPoint& aPosition, int aAccuracy ) const
{
    return TestSegmentHit( aPosition, m_pos, m_End(), aAccuracy );
}


bool SCH_BUS_ENTRY_BASE::HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy ) const
{
    EDA_RECT rect = aRect;

    rect.Inflate( aAccuracy );

    if( aContained )
        return rect.Contains( GetBoundingBox() );

    return rect.Intersects( GetBoundingBox() );
}


void SCH_BUS_ENTRY_BASE::Plot( PLOTTER* aPlotter )
{
    aPlotter->SetCurrentLineWidth( GetPenSize() );
    aPlotter->SetColor( GetLayerColor( GetLayer() ) );
    aPlotter->MoveTo( m_pos );
    aPlotter->FinishTo( m_End() );
}


void SCH_BUS_ENTRY_BASE::SetBusEntryShape( char aShape )
{
    switch( aShape )
    {
    case '\\':
        if( m_size.y < 0 )
            m_size.y = -m_size.y;
        break;

    case '/':
        if( m_size.y > 0 )
            m_size.y = -m_size.y;
        break;
    }
}


char SCH_BUS_ENTRY_BASE::GetBusEntryShape() const
{
    if( GetSize().y < 0 )
        return '/';
    else
        return '\\';
}


void SCH_BUS_ENTRY_BASE::GetMsgPanelInfo( EDA_UNITS_T aUnits, MSG_PANEL_ITEMS& aList )
{
    if( auto conn = Connection( *g_CurrentSheet ) )
    {
#if defined(DEBUG)
        conn->AppendDebugInfoToMsgPanel( aList );
#else
        conn->AppendInfoToMsgPanel( aList );
#endif
    }
}


bool SCH_BUS_WIRE_ENTRY::ConnectionPropagatesTo( const EDA_ITEM* aItem ) const
{
    // Don't generate connections between bus entries and buses, since there is
    // a connectivity change at that point (e.g. A[7..0] to A7)
    if( ( aItem->Type() == SCH_LINE_T ) &&
        ( static_cast<const SCH_LINE*>( aItem )->GetLayer() == LAYER_BUS ) )
    {
        return false;
    }

    // Don't generate connections between bus entries and bus labels that happen
    // to land at the same point on the bus wire as this bus entry
    if( ( aItem->Type() == SCH_LABEL_T ) &&
        SCH_CONNECTION::IsBusLabel( static_cast<const SCH_LABEL*>( aItem )->GetText() ) )
    {
        return false;
    }

    // Don't generate connections between two bus-wire entries
    if( aItem->Type() == SCH_BUS_WIRE_ENTRY_T )
        return false;

    return true;
}
