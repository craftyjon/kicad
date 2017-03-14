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

#include <color_theme.h>


const std::map< int, wxString > COLOR_THEME::layerMap = {
    /// Pcbnew board layers
    { F_Cu,         "F.Cu" },
    { In1_Cu,       "In1.Cu" },
    { In2_Cu,       "In2.Cu" },
    { In3_Cu,       "In3.Cu" },
    { In4_Cu,       "In4.Cu" },
    { In5_Cu,       "In5.Cu" },
    { In6_Cu,       "In6.Cu" },
    { In7_Cu,       "In7.Cu" },
    { In8_Cu,       "In8.Cu" },
    { In9_Cu,       "In9.Cu" },
    { In10_Cu,      "In10.Cu" },
    { In11_Cu,      "In11.Cu" },
    { In12_Cu,      "In12.Cu" },
    { In13_Cu,      "In13.Cu" },
    { In14_Cu,      "In14.Cu" },
    { In15_Cu,      "In15.Cu" },
    { In16_Cu,      "In16.Cu" },
    { In17_Cu,      "In17.Cu" },
    { In18_Cu,      "In18.Cu" },
    { In19_Cu,      "In19.Cu" },
    { In20_Cu,      "In20.Cu" },
    { In21_Cu,      "In21.Cu" },
    { In22_Cu,      "In22.Cu" },
    { In23_Cu,      "In23.Cu" },
    { In24_Cu,      "In24.Cu" },
    { In25_Cu,      "In25.Cu" },
    { In26_Cu,      "In26.Cu" },
    { In27_Cu,      "In27.Cu" },
    { In28_Cu,      "In28.Cu" },
    { In29_Cu,      "In29.Cu" },
    { In30_Cu,      "In30.Cu" },
    { B_Cu,         "B.Cu" },
    { B_Adhes,      "B.Adhes" },
    { F_Adhes,      "F.Adhes" },
    { B_Paste,      "B.Paste" },
    { F_Paste,      "F.Paste" },
    { B_SilkS,      "B.SilkS" },
    { F_SilkS,      "F.SilkS" },
    { B_Mask,       "B.Mask" },
    { F_Mask,       "F.Mask" },
    { Dwgs_User,    "Dwgs.User" },
    { Cmts_User,    "Cmts.User" },
    { Eco1_User,    "Eco1.User" },
    { Eco2_User,    "Eco2.User" },
    { Edge_Cuts,    "Edge.Cuts" },
    { Margin,       "Margin" },
    { B_CrtYd,      "B.CrtYd" },
    { F_CrtYd,      "F.CrtYd" },
    { B_Fab,        "B.Fab" },
    { F_Fab,        "F.Fab" },

    /// Pcbnew GAL layers
    { LAYER_VIAS,            "Via" },
    { LAYER_VIA_MICROVIA,    "MicroVia" },
    { LAYER_VIA_BBLIND,      "BuriedBlindVia" },
    { LAYER_VIA_THROUGH,     "ThroughVia" },
    { LAYER_NON_PLATED,      "NonPlatedHole" },
    { LAYER_MOD_TEXT_FR,     "ModTextFront" },
    { LAYER_MOD_TEXT_BK,     "ModTextBack" },
    { LAYER_ANCHOR,          "Anchor" },
    { LAYER_PAD_FR,          "PadFront" },
    { LAYER_PAD_BK,          "PadBack" },
    { LAYER_RATSNEST,        "Ratsnest" },
    { LAYER_GRID,            "Grid" },

    /// Eeschema layers


    /// GerbView layers
};


const COLOR_THEME COLOR_THEME_MANAGER::defaultTheme = {

    /// Pcbnew board layers
    { F_Cu,         COLOR4D( RED ) },
    { In1_Cu,       COLOR4D( YELLOW ) },
    { In2_Cu,       COLOR4D( LIGHTMAGENTA ) },
    { In3_Cu,       COLOR4D( LIGHTRED ) },
    { In4_Cu,       COLOR4D( CYAN ) },
    { In5_Cu,       COLOR4D( GREEN ) },
    { In6_Cu,       COLOR4D( BLUE ) },
    { In7_Cu,       COLOR4D( DARKGRAY ) },
    { In8_Cu,       COLOR4D( MAGENTA ) },
    { In9_Cu,       COLOR4D( LIGHTGRAY ) },
    { In10_Cu,      COLOR4D( MAGENTA ) },
    { In11_Cu,      COLOR4D( RED ) },
    { In12_Cu,      COLOR4D( BROWN ) },
    { In13_Cu,      COLOR4D( LIGHTGRAY ) },
    { In14_Cu,      COLOR4D( BLUE ) },
    { In15_Cu,      COLOR4D( GREEN ) },
    { In16_Cu,      COLOR4D( RED ) },
    { In17_Cu,      COLOR4D( YELLOW ) },
    { In18_Cu,      COLOR4D( LIGHTMAGENTA ) },
    { In19_Cu,      COLOR4D( LIGHTRED ) },
    { In20_Cu,      COLOR4D( CYAN ) },
    { In21_Cu,      COLOR4D( GREEN ) },
    { In22_Cu,      COLOR4D( BLUE ) },
    { In23_Cu,      COLOR4D( DARKGRAY ) },
    { In24_Cu,      COLOR4D( MAGENTA ) },
    { In25_Cu,      COLOR4D( LIGHTGRAY ) },
    { In26_Cu,      COLOR4D( MAGENTA ) },
    { In27_Cu,      COLOR4D( RED ) },
    { In28_Cu,      COLOR4D( BROWN ) },
    { In29_Cu,      COLOR4D( LIGHTGRAY ) },
    { In30_Cu,      COLOR4D( BLUE ) },
    { B_Cu,         COLOR4D( GREEN ) },

    { B_Adhes,      COLOR4D( BLUE ) },
    { F_Adhes,      COLOR4D( MAGENTA ) },
    { B_Paste,      COLOR4D( LIGHTCYAN ) },
    { F_Paste,      COLOR4D( RED ) },
    { B_SilkS,      COLOR4D( MAGENTA ) },
    { F_SilkS,      COLOR4D( CYAN ) },
    { B_Mask,       COLOR4D( BROWN ) },
    { F_Mask,       COLOR4D( MAGENTA ) },
    { Dwgs_User,    COLOR4D( LIGHTGRAY ) },
    { Cmts_User,    COLOR4D( BLUE ) },
    { Eco1_User,    COLOR4D( GREEN ) },
    { Eco2_User,    COLOR4D( YELLOW ) },
    { Edge_Cuts,    COLOR4D( YELLOW ) },
    { Margin,       COLOR4D( LIGHTMAGENTA ) },
    { B_CrtYd,      COLOR4D( YELLOW ) },
    { F_CrtYd,      COLOR4D( DARKGRAY ) },
    { B_Fab,        COLOR4D( RED ) },
    { F_Fab,        COLOR4D( YELLOW ) },

    /// Pcbnew GAL Layers
    { LAYER_VIAS,                  COLOR4D( LIGHTGRAY ) },
    { LAYER_VIA_MICROVIA,          COLOR4D( CYAN ) },
    { LAYER_VIA_BBLIND,            COLOR4D( BROWN ) },
    { LAYER_VIA_THROUGH,           COLOR4D( LIGHTGRAY ) },
    { LAYER_NON_PLATED,            COLOR4D( YELLOW ) },
    { LAYER_MOD_TEXT_FR,           COLOR4D( LIGHTGRAY ) },
    { LAYER_MOD_TEXT_BK,           COLOR4D( BLUE ) },
    { LAYER_MOD_TEXT_INVISIBLE,    COLOR4D( DARKGRAY ) },
    { LAYER_ANCHOR,                COLOR4D( BLUE ) },
    { LAYER_PAD_FR,                COLOR4D( RED ) },
    { LAYER_PAD_BK,                COLOR4D( GREEN ) },
    { LAYER_RATSNEST,              COLOR4D( LIGHTGRAY ) },
    { LAYER_GRID,                  COLOR4D( DARKGRAY ) },
    { LAYER_GRID_AXES,             COLOR4D( BLUE ) },
    { LAYER_NO_CONNECTS,           COLOR4D( LIGHTGRAY ) },
    { LAYER_MOD_FR,                COLOR4D( LIGHTGRAY ) },
    { LAYER_MOD_BK,                COLOR4D( LIGHTGRAY ) },
    { LAYER_MOD_VALUES,            COLOR4D( LIGHTGRAY ) },
    { LAYER_MOD_REFERENCES,        COLOR4D( LIGHTGRAY ) },
    { LAYER_TRACKS,                COLOR4D( LIGHTGRAY ) },
    { LAYER_PADS,                  COLOR4D( LIGHTGRAY ) },
    { LAYER_PADS_HOLES,            COLOR4D( LIGHTGRAY ) },
    { LAYER_VIAS_HOLES,            COLOR4D( LIGHTGRAY ) },
    { LAYER_DRC,                   COLOR4D( LIGHTGRAY ) },
    { LAYER_WORKSHEET,             COLOR4D( LIGHTGRAY ) },

    /// Eeschema layers
    { LAYER_WIRE,                  COLOR4D( GREEN ) },
    { LAYER_BUS,                   COLOR4D( BLUE ) },
    { LAYER_JUNCTION,              COLOR4D( GREEN ) },
    { LAYER_LOCLABEL,              COLOR4D( BLACK ) },
    { LAYER_HIERLABEL,             COLOR4D( BROWN ) },
    { LAYER_GLOBLABEL,             COLOR4D( RED ) },
    { LAYER_PINNUM,                COLOR4D( RED ) },
    { LAYER_PINNAM,                COLOR4D( CYAN ) },
    { LAYER_FIELDS,                COLOR4D( MAGENTA ) },
    { LAYER_REFERENCEPART,         COLOR4D( CYAN ) },
    { LAYER_VALUEPART,             COLOR4D( CYAN ) },
    { LAYER_NOTES,                 COLOR4D( LIGHTBLUE ) },
    { LAYER_DEVICE,                COLOR4D( RED ) },
    { LAYER_DEVICE_BACKGROUND,     COLOR4D( LIGHTYELLOW ) },
    { LAYER_NETNAM,                COLOR4D( DARKGRAY ) },
    { LAYER_PIN,                   COLOR4D( RED ) },
    { LAYER_SHEET,                 COLOR4D( MAGENTA ) },
    { LAYER_SHEETFILENAME,         COLOR4D( BROWN ) },
    { LAYER_SHEETNAME,             COLOR4D( CYAN ) },
    { LAYER_SHEETLABEL,            COLOR4D( BROWN ) },
    { LAYER_NOCONNECT,             COLOR4D( BLUE ) },
    { LAYER_ERC_WARN,              COLOR4D( GREEN ) },
    { LAYER_ERC_ERR,               COLOR4D( RED ) },
    { LAYER_SCHEMATIC_GRID,        COLOR4D( DARKGRAY ) },
    { LAYER_SCHEMATIC_BACKGROUND,  COLOR4D( WHITE ) },
    { LAYER_BRIGHTENED,            COLOR4D( PUREMAGENTA ) },

    /// GerbView layers
    { GERBVIEW_DRAW_LAYER( 0 ),    COLOR4D( GREEN ) },
    { GERBVIEW_DRAW_LAYER( 1 ),    COLOR4D( BLUE ) },
    { GERBVIEW_DRAW_LAYER( 2 ),    COLOR4D( LIGHTGRAY ) },
    { GERBVIEW_DRAW_LAYER( 3 ),    COLOR4D( MAGENTA ) },
    { GERBVIEW_DRAW_LAYER( 4 ),    COLOR4D( RED ) },
    { GERBVIEW_DRAW_LAYER( 5 ),    COLOR4D( DARKGREEN ) },
    { GERBVIEW_DRAW_LAYER( 6 ),    COLOR4D( BROWN ) },
    { GERBVIEW_DRAW_LAYER( 7 ),    COLOR4D( MAGENTA ) },
    { GERBVIEW_DRAW_LAYER( 8 ),    COLOR4D( LIGHTGRAY ) },
    { GERBVIEW_DRAW_LAYER( 9 ),    COLOR4D( BLUE ) },
    { GERBVIEW_DRAW_LAYER( 10 ),   COLOR4D( GREEN ) },
    { GERBVIEW_DRAW_LAYER( 11 ),   COLOR4D( CYAN ) },
    { GERBVIEW_DRAW_LAYER( 12 ),   COLOR4D( LIGHTRED ) },
    { GERBVIEW_DRAW_LAYER( 13 ),   COLOR4D( LIGHTMAGENTA ) },
    { GERBVIEW_DRAW_LAYER( 14 ),   COLOR4D( YELLOW ) },
    { GERBVIEW_DRAW_LAYER( 15 ),   COLOR4D( RED ) },
    { GERBVIEW_DRAW_LAYER( 16 ),   COLOR4D( BLUE ) },
    { GERBVIEW_DRAW_LAYER( 17 ),   COLOR4D( BROWN ) },
    { GERBVIEW_DRAW_LAYER( 18 ),   COLOR4D( LIGHTCYAN ) },
    { GERBVIEW_DRAW_LAYER( 19 ),   COLOR4D( RED ) },
    { GERBVIEW_DRAW_LAYER( 20 ),   COLOR4D( MAGENTA ) },
    { GERBVIEW_DRAW_LAYER( 21 ),   COLOR4D( CYAN ) },
    { GERBVIEW_DRAW_LAYER( 22 ),   COLOR4D( BROWN ) },
    { GERBVIEW_DRAW_LAYER( 23 ),   COLOR4D( MAGENTA ) },
    { GERBVIEW_DRAW_LAYER( 24 ),   COLOR4D( LIGHTGRAY ) },
    { GERBVIEW_DRAW_LAYER( 25 ),   COLOR4D( BLUE ) },
    { GERBVIEW_DRAW_LAYER( 26 ),   COLOR4D( GREEN ) },
    { GERBVIEW_DRAW_LAYER( 27 ),   COLOR4D( DARKCYAN ) },
    { GERBVIEW_DRAW_LAYER( 28 ),   COLOR4D( YELLOW ) },
    { GERBVIEW_DRAW_LAYER( 29 ),   COLOR4D( LIGHTMAGENTA ) },
    { GERBVIEW_DRAW_LAYER( 30 ),   COLOR4D( YELLOW ) },
    { GERBVIEW_DRAW_LAYER( 31 ),   COLOR4D( LIGHTGRAY ) },

    { LAYER_DCODES,                COLOR4D( WHITE ) },
    { LAYER_NEGATIVE_OBJECTS,      COLOR4D( LIGHTGRAY ) },
};


COLOR4D COLOR_THEME::GetLayerColor( LAYER_ID aLayer ) const
{
    auto it = m_colorMap.find( aLayer );

    if( it != m_colorMap.end() )
    {
        return it->second;
    }

    wxASSERT_MSG( false, wxString::Format( "No layer map entry for layer %d!", aLayer ) );

    return COLOR4D::UNSPECIFIED;
}


COLOR_THEME_MANAGER::COLOR_THEME_MANAGER()
{
}


COLOR4D COLOR_THEME_MANAGER::GetLayerColor( LAYER_ID aLayer ) const
{
    return m_currentTheme ? m_currentTheme->GetLayerColor( aLayer )
                          : defaultTheme.GetLayerColor( aLayer );
}
