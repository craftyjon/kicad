/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 CERN
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

#ifndef _SCH_PIN_CONNECTION_H
#define _SCH_PIN_CONNECTION_H

#include <base_struct.h>
#include <sch_connection.h>
#include <lib_pin.h>

class SCH_COMPONENT;

/**
 * Container to describe the net connectivity of a specific pin on a component.
 */
class SCH_PIN_CONNECTION : public EDA_ITEM, public CONNECTABLE_ITEM
{
public:
    SCH_PIN_CONNECTION( EDA_ITEM* aParent = nullptr );

    wxString GetClass() const override
    {
        return wxT( "SCH_PIN_CONNECTION" );
    }

    wxString GetSelectMenuText() const override;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override {}
#endif

    LIB_PIN* m_pin;

    SCH_COMPONENT* m_comp;
};

#endif
