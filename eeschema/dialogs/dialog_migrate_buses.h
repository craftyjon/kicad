/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 CERN
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

#ifndef _DIALOG_MIGRATE_BUSES_H
#define _DIALOG_MIGRATE_BUSES_H

#include <vector>

#include <connection_graph.h>
#include <sch_edit_frame.h>

#include <dialog_migrate_buses_base.h>


struct BUS_MIGRATION_STATUS
{
    CONNECTION_SUBGRAPH* subgraph;

    std::vector<wxString> labels;

    wxString proposed_label;

    bool approved;
};

class DIALOG_MIGRATE_BUSES : public DIALOG_MIGRATE_BUSES_BASE
{
public:

    DIALOG_MIGRATE_BUSES( SCH_EDIT_FRAME* aParent );

private:

    SCH_EDIT_FRAME* m_frame;

    void loadGraphData();

    void updateUi();

    wxString getProposedLabel( std::vector<wxString> aLabelList );

    void onItemSelected( wxListEvent& aEvent );

    std::vector<BUS_MIGRATION_STATUS> m_items;
};

#endif
