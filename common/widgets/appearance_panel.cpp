/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2014-2019 KiCad Developers, see AUTHORS.txt for contributors.
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

#include <common/widgets/appearance_panel.h>
#include <pcb_base_frame.h>


/// This is a read only template that is copied and modified before adding to LAYER_WIDGET
const APPEARANCE_PANEL::APPEARANCE_SETTING APPEARANCE_PANEL::s_render_rows[] = {

#define RR  APPEARANCE_PANEL::APPEARANCE_SETTING   // Render Row abbreviation to reduce source width
#define NOCOLOR COLOR4D::UNSPECIFIED    // specify rows that do not have a color selector icon

        // text                     id                    color     tooltip
    RR( _( "Footprints Front" ),    LAYER_MOD_FR,         NOCOLOR,  _( "Show footprints that are on board's front") ),
    RR( _( "Footprints Back" ),     LAYER_MOD_BK,         NOCOLOR,  _( "Show footprints that are on board's back") ),
    RR( _( "Values" ),              LAYER_MOD_VALUES,     NOCOLOR,  _( "Show footprint values") ),
    RR( _( "References" ),          LAYER_MOD_REFERENCES, NOCOLOR,  _( "Show footprint references") ),
    RR( _( "Footprint Text Front" ),LAYER_MOD_TEXT_FR,    NOCOLOR,  _( "Show footprint text on board's front" ) ),
    RR( _( "Footprint Text Back" ), LAYER_MOD_TEXT_BK,    NOCOLOR,  _( "Show footprint text on board's back" ) ),
    RR( _( "Hidden Text" ),         LAYER_MOD_TEXT_INVISIBLE, WHITE, _( "Show footprint text marked as invisible" ) ),
    RR( _( "Pads Front" ),          LAYER_PAD_FR,         WHITE,    _( "Show footprint pads on board's front" ) ),
    RR( _( "Pads Back" ),           LAYER_PAD_BK,         WHITE,    _( "Show footprint pads on board's back" ) ),
    RR( _( "Through Hole Pads" ),   LAYER_PADS_TH,        YELLOW,   _( "Show through hole pads in specific color") ),
    RR(),
    RR( _( "Tracks" ),              LAYER_TRACKS,         NOCOLOR,  _( "Show tracks" ) ),
    RR( _( "Through Via" ),         LAYER_VIA_THROUGH,    WHITE,    _( "Show through vias" ) ),
    RR( _( "Bl/Buried Via" ),       LAYER_VIA_BBLIND,     WHITE,    _( "Show blind or buried vias" )  ),
    RR( _( "Micro Via" ),           LAYER_VIA_MICROVIA,   WHITE,    _( "Show micro vias") ),
    RR( _( "Non Plated Holes" ),    LAYER_NON_PLATEDHOLES,WHITE,    _( "Show non plated holes in specific color") ),
    RR(),
    RR( _( "Ratsnest" ),            LAYER_RATSNEST,       WHITE,    _( "Show unconnected nets as a ratsnest") ),
    RR( _( "No-Connects" ),         LAYER_NO_CONNECTS,    BLUE,     _( "Show a marker on pads which have no net connected" ) ),
    RR( _( "Anchors" ),             LAYER_ANCHOR,         WHITE,    _( "Show footprint and text origins as a cross" ) ),
    RR( _( "Worksheet" ),           LAYER_WORKSHEET,      DARKRED,  _( "Show worksheet") ),
    RR( _( "Cursor" ),              LAYER_CURSOR,         WHITE,    _( "PCB Cursor" ), true, false ),
    RR( _( "Aux items" ),           LAYER_AUX_ITEMS,      WHITE,    _( "Auxiliary items (rulers, assistants, axes, etc.)" ), true, false ),
    RR( _( "Grid" ),                LAYER_GRID,           WHITE,    _( "Show the (x,y) grid dots" ) ),
    RR( _( "Background" ),          LAYER_PCB_BACKGROUND, BLACK,    _( "PCB Background" ), true, false )
};


APPEARANCE_PANEL::APPEARANCE_PANEL( PCB_BASE_FRAME* aParent, wxWindow* aFocusOwner,
                                    bool aFpEditorMode ) :
        APPEARANCE_PANEL_BASE( aParent ),
        m_focus_owner( aFocusOwner )
{
}


wxSize APPEARANCE_PANEL::GetBestSize() const
{
    wxSize size( 240, 200 );


    return size;
}


void APPEARANCE_PANEL::rebuild()
{

}