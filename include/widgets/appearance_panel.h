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

#ifndef _APPEARANCE_PANEL_H
#define _APPEARANCE_PANEL_H

#include <vector>

#include <widgets/appearance_panel_base.h>
#include <gal/color4d.h>

class BITMAP_TOGGLE;
class COLOR_SWATCH;
class INDICATOR_ICON;
class PCB_BASE_FRAME;
class ROW_ICON_PROVIDER;

using KIGFX::COLOR4D;


class APPEARANCE_PANEL : public APPEARANCE_PANEL_BASE
{
public:
    struct APPEARANCE_SETTING
    {
        int         id;
        wxString    label;
        wxString    tooltip;
        COLOR4D     color;
        bool        visible;
        bool        can_control_visibility;
        bool        can_control_opacity;

        wxPanel*        ctl_panel;
        INDICATOR_ICON* ctl_indicator;
        BITMAP_TOGGLE*  ctl_visibility;
        COLOR_SWATCH*   ctl_color;
        wxStaticText*   ctl_text;
        wxSlider*       ctl_opacity;

        APPEARANCE_SETTING( const wxString& aLabel, int aId, COLOR4D aColor = COLOR4D::UNSPECIFIED,
                            const wxString& aTooltip = wxEmptyString, bool aVisible = true,
                            bool aCanControlVisibility = true, bool aCanControlOpacity = true )
        {
            label   = aLabel;
            id      = aId;
            color   = aColor;
            visible = aVisible;
            tooltip = aTooltip;
            can_control_visibility = aCanControlVisibility;
            can_control_opacity = aCanControlOpacity;

            ctl_panel = nullptr;
            ctl_indicator = nullptr;
            ctl_visibility = nullptr;
            ctl_color = nullptr;
            ctl_text = nullptr;
            ctl_opacity = nullptr;
        }

        APPEARANCE_SETTING() : id( -1 ), label( "" ), tooltip( "" ), color( COLOR4D::UNSPECIFIED ),
                               visible( false ), can_control_visibility( false ),
                               can_control_opacity( false )
        {}
    };

    APPEARANCE_PANEL( PCB_BASE_FRAME* aParent, wxWindow* aFocusOwner, bool aFpEditorMode = false );

    wxSize GetBestSize() const;

    ///> Updates the panel contents from the application and board models
    void Rebuild();

    void UpdateLayers();

private:
    wxWindow* m_focus_owner;

    static const APPEARANCE_SETTING s_render_rows[];

    ROW_ICON_PROVIDER* m_IconProvider;

    // TODO(JE) does it make sense for all four of these to be separate?

    std::vector<APPEARANCE_SETTING> m_layer_settings;

    std::vector<APPEARANCE_SETTING> m_object_settings;

    std::vector<APPEARANCE_SETTING> m_net_settings;

    std::vector<APPEARANCE_SETTING> m_netclass_settings;

    void rebuildLayers();

    void rebuildObjects();

    void rebuildNets();

    void rebuildStoredSettings();

    void onLayerClick( wxMouseEvent& aEvent );
};


#endif
