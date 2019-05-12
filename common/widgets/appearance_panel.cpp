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

#include <widgets/appearance_panel.h>

#include <bitmaps.h>
#include <class_board.h>
#include <pcb_base_frame.h>
#include <widgets/bitmap_toggle.h>
#include <widgets/color_swatch.h>
#include <widgets/indicator_icon.h>


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
    int indicatorSize = ConvertDialogToPixels( wxSize( 6, 6 ) ).x;
    m_IconProvider = new ROW_ICON_PROVIDER( indicatorSize );

    int pointSize = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT ).GetPointSize();
    int screenHeight = wxSystemSettings::GetMetric( wxSYS_SCREEN_Y );

    if( screenHeight <= 900 && pointSize >= indicatorSize )
        pointSize = pointSize * 8 / 10;

    // TODO(JE) Check and do something with calculated point size

    Rebuild();
}


wxSize APPEARANCE_PANEL::GetBestSize() const
{
    wxSize size( 240, 200 );
    // TODO(JE) appropriate logic
    return size;
}


void APPEARANCE_PANEL::Rebuild()
{
    rebuildLayers();
    rebuildObjects();
    rebuildNets();
    rebuildStoredSettings();

    UpdateLayers();
}


void APPEARANCE_PANEL::rebuildLayers()
{
    auto frame = static_cast<PCB_BASE_FRAME*>( GetParent() );
    BOARD* board = frame->GetBoard();
    LSET enabled = board->GetEnabledLayers();
    LSET visible = board->GetVisibleLayers();

    COLOR4D bg_color = frame->Settings().Colors().GetLayerColor( LAYER_PCB_BACKGROUND );

    m_layer_settings.clear();
    m_layers_outer_sizer->Clear( true );

    auto appendLayer = [&] ( APPEARANCE_SETTING aSetting ) {

        auto panel = new wxPanel( m_layers_window );
        auto sizer = new wxBoxSizer( wxHORIZONTAL );

        panel->SetSizer( sizer );

        // TODO(JE) consider restyling this indicator
        auto indicator = new INDICATOR_ICON( panel, *m_IconProvider,
                                             ROW_ICON_PROVIDER::STATE::OFF, aSetting.id );

        auto btn_visible = new BITMAP_TOGGLE( panel, wxID_ANY,
                                              KiBitmap( visibility_xpm ),
                                              KiBitmap( visibility_off_xpm ),
                                              aSetting.visible );

        auto swatch = new COLOR_SWATCH( panel, COLOR4D::UNSPECIFIED,
                aSetting.id, true, bg_color );

        auto label = new wxStaticText( panel, wxID_ANY, aSetting.label );
        label->Wrap( -1 );

//        auto slider = new wxSlider( panel, wxID_ANY, aSetting.color.a * 100, 0, 100,
//                                    wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
//        slider->SetMinSize( wxSize( 100, -1 ) );

        sizer->Add( indicator,   0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5 );
        sizer->Add( btn_visible, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5 );
        sizer->Add( swatch,      0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5 );
        sizer->Add( label,       1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5 );
        //sizer->Add( slider,      0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

        m_layers_outer_sizer->Add( panel, 0, wxEXPAND, 0 );

        aSetting.ctl_panel = panel;
        aSetting.ctl_indicator = indicator;
        aSetting.ctl_visibility = btn_visible;
        aSetting.ctl_color = swatch;
        aSetting.ctl_text = label;

        m_layer_settings.emplace_back( aSetting );
    };

    wxString dsc;

    // show all coppers first, with front on top, back on bottom, then technical layers
    for( LSEQ cu_stack = enabled.CuStack(); cu_stack; ++cu_stack )
    {
        PCB_LAYER_ID layer = *cu_stack;

        switch( layer )
        {
            case F_Cu:
                dsc = _( "Front copper layer" );
                break;

            case B_Cu:
                dsc = _( "Back copper layer" );
                break;

            default:
                dsc = _( "Inner copper layer" );
                break;
        }

        appendLayer( APPEARANCE_SETTING( board->GetLayerName( layer ), layer,
                                         frame->Settings().Colors().GetLayerColor( layer ),
                                         dsc, visible[layer] ) );

#ifdef NOTYET
        if( m_fp_editor_mode && LSET::ForbiddenFootprintLayers().test( layer ) )
        {
            getLayerComp( GetLayerRowCount()-1, COLUMN_COLOR_LYRNAME )->Enable( false );
            getLayerComp( GetLayerRowCount()-1, COLUMN_COLORBM )->SetToolTip( wxEmptyString );
        }
#endif
    }

    // technical layers are shown in this order:
    // Because they are static, wxGetTranslation must be explicitly
    // called for tooltips.
    static const struct {
        PCB_LAYER_ID layerId;
        wxString     tooltip;
    } non_cu_seq[] = {
        { F_Adhes,          _( "Adhesive on board's front" ) },
        { B_Adhes,          _( "Adhesive on board's back" ) },
        { F_Paste,          _( "Solder paste on board's front" ) },
        { B_Paste,          _( "Solder paste on board's back" ) },
        { F_SilkS,          _( "Silkscreen on board's front" ) },
        { B_SilkS,          _( "Silkscreen on board's back" ) },
        { F_Mask,           _( "Solder mask on board's front" ) },
        { B_Mask,           _( "Solder mask on board's back" ) },
        { Dwgs_User,        _( "Explanatory drawings" ) },
        { Cmts_User,        _( "Explanatory comments" ) },
        { Eco1_User,        _( "User defined meaning" ) },
        { Eco2_User,        _( "User defined meaning" ) },
        { Edge_Cuts,        _( "Board's perimeter definition" ) },
        { Margin,           _( "Board's edge setback outline" ) },
        { F_CrtYd,          _( "Footprint courtyards on board's front" ) },
        { B_CrtYd,          _( "Footprint courtyards on board's back" ) },
        { F_Fab,            _( "Footprint assembly on board's front" ) },
        { B_Fab,            _( "Footprint assembly on board's back" ) }
    };

    for( const auto& entry : non_cu_seq )
    {
        PCB_LAYER_ID layer = entry.layerId;

        if( !enabled[layer] )
            continue;

        appendLayer( APPEARANCE_SETTING( board->GetLayerName( layer ), layer,
                                         frame->Settings().Colors().GetLayerColor( layer ),
                                         wxGetTranslation( entry.tooltip ), visible[layer] ) );

#ifdef NOTYET
        if( m_fp_editor_mode && LSET::ForbiddenFootprintLayers().test( layer ) )
        {
            getLayerComp( GetLayerRowCount()-1, COLUMN_COLOR_LYRNAME )->Enable( false );
            getLayerComp( GetLayerRowCount()-1, COLUMN_COLORBM )->SetToolTip( wxEmptyString );
        }
#endif
    }

    m_layers_outer_sizer->Layout();
}


void APPEARANCE_PANEL::UpdateLayers()
{
    auto frame = static_cast<PCB_BASE_FRAME*>( GetParent() );
    BOARD* board = frame->GetBoard();
    LSET visible = board->GetVisibleLayers();
    PCB_LAYER_ID current_layer = board->GetLayer();

    for( APPEARANCE_SETTING& setting : m_layer_settings )
    {
        LAYER_NUM layer = setting.id;

        setting.color = frame->Settings().Colors().GetLayerColor( layer );

        if( setting.ctl_panel )
        {
            setting.ctl_panel->SetBackgroundColour( current_layer == layer ?
                    wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) :
                    wxSystemSettings::GetColour( wxSYS_COLOUR_FRAMEBK ) );
        }

        if( setting.ctl_indicator )
        {
            setting.ctl_indicator->SetIndicatorState( current_layer == layer ?
                                                      ROW_ICON_PROVIDER::STATE::ON :
                                                      ROW_ICON_PROVIDER::STATE::OFF );
        }

        if( setting.ctl_visibility )
            setting.ctl_visibility->SetValue( visible[setting.id] );

        if( setting.ctl_color )
        {
            // Remove alpha component from swatch
            COLOR4D color = setting.color;
            color.a = 1.0;
            setting.ctl_color->SetSwatchColor( color, false );
        }
    }
}


void APPEARANCE_PANEL::rebuildObjects()
{

}


void APPEARANCE_PANEL::rebuildNets()
{

}


void APPEARANCE_PANEL::rebuildStoredSettings()
{

}
