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

#include <sch_connection.h>
#include <class_netlist_object.h>   // IsBusLabel() etc.


void SCH_CONNECTION::ConfigureFromLabel( wxString aLabel )
{
    if( IsBusVectorLabel( aLabel ) )
    {
        // TODO(JE) Is storing the whole name right? (vs. the prefix)
        m_name = aLabel;
        m_type = CONNECTION_BUS;
        m_bus_type = BUS_TYPE_VECTOR;

        wxString prefix;
        NETLIST_OBJECT::ParseBusVector( aLabel, &prefix,
                                        &m_vector_start, &m_vector_end );

        for( long i = m_vector_start; i <= m_vector_end; ++i )
        {
            auto member = std::make_shared< SCH_CONNECTION >( m_parent );
            wxString name = prefix;
            name << i;
            member->m_name = name;
            member->m_vector_index = i;
            m_members.push_back( member );
        }
    }
    else if( IsHeteroBusLabel( aLabel ) )
    {
        std::cout << "Hetero bus label " << aLabel << " not handled yet." << std::endl;
        m_type = CONNECTION_BUS;
        m_bus_type = BUS_TYPE_GROUP;
    }
    // else if( IsBusAlias( aLabel ) )
    // {

    // }
    else
    {
        m_name = aLabel;
        m_type = CONNECTION_NET;
    }
}
