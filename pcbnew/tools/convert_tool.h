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

#ifndef CONVERT_TOOL_H_
#define CONVERT_TOOL_H_

#include <tool/tool_interactive.h>
#include <class_board_item.h>
#include <pcb_base_frame.h>

typedef std::vector<std::pair<BOARD_ITEM*, EDA_RECT>> ALIGNMENT_RECTS;

class SELECTION_TOOL;

class CONVERT_TOOL : public TOOL_INTERACTIVE
{
public:
    CONVERT_TOOL();
    virtual ~CONVERT_TOOL();

    /// @copydoc TOOL_INTERACTIVE::Reset()
    void Reset( RESET_REASON aReason ) override {}

    /// @copydoc TOOL_INTERACTIVE::Init()
    bool Init() override;

    /**
     * Converts selected lines to a polygon, if possible
     */
    int LinesToPoly( const TOOL_EVENT& aEvent );

    ///> S@copydoc TOOL_INTERACTIVE::setTransitions()
    void setTransitions() override;

private:

    SELECTION_TOOL* m_selectionTool;

    CONTEXT_MENU* m_menu;

    PCB_BASE_FRAME* m_frame;
};

#endif
