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
 * Represents a color theme (stored in memory or as a file)
 */
class COLOR_THEME
{
public:
    COLOR_THEME() {}

    COLOR_THEME( std::initializer_list< std::pair< const int, COLOR4D > > aMapList )
        : colorMap( aMapList )
    {
    }

    /// @see COLOR_THEME_MANAGER::getLayerColor()
    COLOR4D GetLayerColor( int aLayer ) const;

    /// @see COLOR_THEME_MANAGER::setLayerColor()
    void SetLayerColor( int aLayer, COLOR4D aColor );

    /// @return true if the theme has been modified since last save
    bool IsDirty() { return dirty; }

    void Flush() { dirty = false; }

protected:

    /// Mapping of KiCad layer id to strings for settings keys
    static const std::map< int, wxString > layerMap;

private:

    /// True if the theme has been modified since last write
    bool dirty = false;

    /// Container for colors
    std::map< int, COLOR4D > colorMap;

    /// Displayed name of the theme
    wxString themeName = "";

    /// Filepath of the theme (if not the default theme)
    wxString filePath = "";
};


/**
 * Handles loading/saving color themes, and retrieving colors for applications.
 * You should probably only have one instance of this.
 */
class COLOR_THEME_MANAGER
{
public:
    COLOR_THEME_MANAGER();

    /**
     * For now, COLOR_THEME_MANAGER is a singleton.
     *
     * Singletons are not really a great idea, as they tend to indicate
     * that the software architecture has problems.  In this case, the
     * problem is that both Eeschema and PcbNew treated color settings as
     * globals, so in order to minimize the impact of the color themes feature,
     * this behavior is left in place.
     *
     * It is likely that the eventual removal of legacy drawing mode will
     * make it a lot easier to refactor the use of globals / singleton for color
     * settings.
     */
    static COLOR_THEME_MANAGER& Instance()
    {
        static COLOR_THEME_MANAGER instance;
        return instance;
    }

    COLOR4D GetLayerColor( PCB_LAYER_ID aLayer ) const
    {
        return getLayerColor( static_cast<PCB_LAYER_ID>( aLayer ) );
    }

    COLOR4D GetLayerColor( GAL_LAYER_ID aLayer ) const
    {
        return getLayerColor( static_cast<GAL_LAYER_ID>( aLayer ) );
    }

    COLOR4D GetLayerColor( SCH_LAYER_ID aLayer ) const
    {
        return getLayerColor( static_cast<SCH_LAYER_ID>( aLayer ) );
    }

    COLOR4D GetLayerColor( GERBVIEW_LAYER_ID aLayer ) const
    {
        return getLayerColor( static_cast<GERBVIEW_LAYER_ID>( aLayer ) );
    }

    void SetLayerColor( PCB_LAYER_ID aLayer, COLOR4D aColor )
    {
        setLayerColor( static_cast<PCB_LAYER_ID>( aLayer ), aColor );
    }

    void SetLayerColor( GAL_LAYER_ID aLayer, COLOR4D aColor )
    {
        setLayerColor( static_cast<GAL_LAYER_ID>( aLayer ), aColor );
    }

    void SetLayerColor( SCH_LAYER_ID aLayer, COLOR4D aColor )
    {
        setLayerColor( static_cast<SCH_LAYER_ID>( aLayer ), aColor );
    }

    void SetLayerColor( GERBVIEW_LAYER_ID aLayer, COLOR4D aColor )
    {
        setLayerColor( static_cast<GERBVIEW_LAYER_ID>( aLayer ), aColor );
    }

    void SetLegacyMode( bool aMode )
    {
        legacyMode = aMode;
    }

private:

    /**
     * Fetches the color of a layer if it's specified in the theme
     *
     * @param aLayer is valid layer identifier
     * @return the color of the layer, or COLOR4D::UNSPECIFIED
     */
    COLOR4D getLayerColor( int aLayer ) const;

    /**
     * Sets the color of a layer in a theme.
     *
     * If the layer didn't have a color entry in the theme, one is added for it.
     *
     * @param aLayer is a valid layer identifier
     * @param aColor is the color to store
     */
    void setLayerColor( int aLayer, COLOR4D aColor );

    /// The default theme is stored in code rather than as an external file
    static const COLOR_THEME defaultTheme;

    std::vector< COLOR_THEME > themes;

    /// Pointer to the active color theme
    COLOR_THEME* currentTheme = NULL;

    /// When true, colors will be converted to the nearest legacy match before return
    bool legacyMode = false;
};

#endif
