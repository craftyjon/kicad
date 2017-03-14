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


#ifndef COLOR_THEME_H
#define COLOR_THEME_H

#include <map>

#include <gal/color4d.h>
#include <layers_id_colors_and_visibility.h>

using KIGFX::COLOR4D;

/**
 * Class COLOR_THEME
 * Represents a color theme (stored in memory or as a file)
 * Used only by COLOR_THEME_MANAGER
 */
class COLOR_THEME
{
public:
    COLOR_THEME() {}

    COLOR_THEME( std::initializer_list< std::pair< const LAYER_ID, COLOR4D > > aMapList )
        : m_colorMap( aMapList )
    {
    }

    /// @see COLOR_THEME_MANAGER::GetLayerColor()
    COLOR4D GetLayerColor( LAYER_ID aLayer ) const;

protected:

    /// Mapping of KiCad layer id to strings for settings keys
    static const std::map< int, wxString > layerMap;

private:

    /// True if the theme has been modified since last write
    bool m_dirty = false;

    /// Container for colors
    std::map< LAYER_ID, COLOR4D > m_colorMap;

    /// Displayed name of the theme
    wxString m_themeName = "";

    /// Filepath of the theme (if not the default theme)
    wxString m_filePath = "";
};


/**
 * Class COLOR_THEME_MANAGER
 * Handles loading/saving color themes, and retrieving colors for applications.
 */
class COLOR_THEME_MANAGER
{
public:
    COLOR_THEME_MANAGER();

    /**
     * @brief Returns the color of a named layer
     *
     * @param aLayer The layer to retrieve color for (see layers_id_colors_and_visibility.h)
     */
    COLOR4D GetLayerColor( LAYER_ID aLayer ) const;

private:
    /// The default theme is stored in code rather than as an external file
    static const COLOR_THEME defaultTheme;

    /// Pointer to the active color theme
    COLOR_THEME* m_currentTheme = NULL;

    /// When true, colors will be converted to the nearest legacy match before return
    bool m_legacyMode = false;
};

#endif
