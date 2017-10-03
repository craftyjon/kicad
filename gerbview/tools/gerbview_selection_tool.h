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

#ifndef __GERBVIEW_SELECTION_TOOL_H
#define __GERBVIEW_SELECTION_TOOL_H

#include <gerber_collectors.h>
#include <gerbview_frame.h>

#include <tool/selection_tool.h>


class GERBER_COLLECTOR;


class GERBVIEW_SELECTION_TOOL : public SELECTION_TOOL
{
public:
    GERBVIEW_SELECTION_TOOL();
    ~GERBVIEW_SELECTION_TOOL();

    /// @copydoc TOOL_BASE::Init()
    bool Init() override;

    /// @copydoc TOOL_BASE::Reset()
    void Reset( RESET_REASON aReason ) override;

    /**
     * Function RequestSelection()
     *
     * Returns the current selection set, filtered according to aFlags.
     * If the set is empty, performs the legacy-style hover selection.
     */
    virtual SELECTION& RequestSelection( int aFlags = SELECTION_DEFAULT );

    ///> Sets up handlers for various events.
    void setTransitions() override;

private:

    /**
     * Function selectable()
     * Checks conditions for an item to be selected.
     *
     * @return True if the item fulfills conditions to be selected.
     */
    bool selectable( const EDA_ITEM* aItem ) const override;

    /**
     * Function selectVisually()
     * Marks item as selected, but does not add it to the ITEMS_PICKED_LIST.
     * @param aItem is an item to be be marked.
     */
    void selectVisually( EDA_ITEM* aItem ) override;

    /**
     * Function unselectVisually()
     * Marks item as selected, but does not add it to the ITEMS_PICKED_LIST.
     * @param aItem is an item to be be marked.
     */
    void unselectVisually( EDA_ITEM* aItem ) override;

    ///> Returns the selection collector (creating if needed)
    COLLECTOR* getCollector() override
    {
        if( m_collector == NULL )
        {
            m_collector = new GERBER_COLLECTOR;
        }

        return m_collector;
    }

    ///> Returns the collection filter for this tool
    const KICAD_T* getCollectionFilter() override { return GERBER_COLLECTOR::AllItems; }

    GERBVIEW_FRAME* getFrame() { return static_cast<GERBVIEW_FRAME*>( m_frame ); }

};

#endif
