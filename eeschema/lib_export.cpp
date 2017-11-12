/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2008 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright (C) 2004-2017 KiCad Developers, see AUTHORS.txt for contributors.
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

/**
 * @file lib_export.cpp
 * @brief Eeschema library maintenance routines to backup modified libraries and
 *        create, edit, and delete components.
 */

#include <fctsys.h>
#include <class_drawpanel.h>
#include <confirm.h>

#include <general.h>
#include <libeditframe.h>
#include <class_library.h>
#include <wildcards_and_files_ext.h>

#include <wx/filename.h>


extern int ExportPartId;


void LIB_EDIT_FRAME::OnImportPart( wxCommandEvent& event )
{
    wxString msg;
    m_lastDrawItem = NULL;

    wxFileDialog dlg( this, _( "Import Symbol" ), m_mruPath,
                      wxEmptyString, SchematicLibraryFileWildcard(),
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST );

    if( dlg.ShowModal() == wxID_CANCEL )
        return;

    wxFileName  fn = dlg.GetPath();

    m_mruPath = fn.GetPath();

    wxArrayString symbols;
    SCH_PLUGIN::SCH_PLUGIN_RELEASER pi( SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_LEGACY ) );

    try
    {
        pi->EnumerateSymbolLib( symbols, fn.GetFullPath() );
    }
    catch( const IO_ERROR& ioe )
    {
        msg.Printf( _( "Cannot import symbol library '%s'." ), fn.GetFullPath() );
        DisplayErrorMessage( this, msg, ioe.What() );
        return;
    }

    if( symbols.empty() )
    {
        msg.Printf( _( "Symbol library file '%s' is empty." ), fn.GetFullPath() );
        DisplayError( this,  msg );
        return;
    }

    LIB_ALIAS* entry = pi->LoadSymbol( fn.GetFullPath(), symbols[0] );

    if( LoadOneLibraryPartAux( entry, fn.GetFullPath() ) )
    {
        DisplayLibInfos();
        GetScreen()->ClearUndoRedoList();
        Zoom_Automatique( false );

        // This effectively adds a new symbol to the library.  Set the modified flag so the
        // save library toolbar button and menu entry are enabled.
        OnModify();
    }
}


void LIB_EDIT_FRAME::OnExportPart( wxCommandEvent& event )
{
    wxString    msg, title;
    bool        createLib = ( event.GetId() == ExportPartId ) ? false : true;

    LIB_PART*   part = GetCurPart();

    if( !part )
    {
        DisplayError( this, _( "There is no symbol selected to save." ) );
        return;
    }

    wxFileName fn;

    fn.SetName( part->GetName().Lower() );
    fn.SetExt( SchematicLibraryFileExtension );

    title = createLib ? _( "New Symbol Library" ) : _( "Export Symbol" );

    wxFileDialog dlg( this, title, m_mruPath, fn.GetFullName(),
                      SchematicLibraryFileWildcard(), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

    if( dlg.ShowModal() == wxID_CANCEL )
        return;

    fn = dlg.GetPath();
    fn.MakeAbsolute();

    LIB_PART* old_part = NULL;

    SCH_PLUGIN::SCH_PLUGIN_RELEASER pi( SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_LEGACY ) );

    if( fn.FileExists() )
    {
        try
        {
            LIB_ALIAS* alias = pi->LoadSymbol( fn.GetFullPath(), part->GetName() );

            if( alias )
                old_part = alias->GetPart();
        }
        catch( const IO_ERROR& ioe )
        {
            msg.Printf( _( "Error occurred attempting to load symbol library file '%s'" ),
                        fn.GetFullPath() );
            DisplayErrorMessage( this, msg, ioe.What() );
            return;
        }

        if( old_part )
        {
            msg.Printf( _( "Symbol '%s' already exists. Overwrite it?" ), part->GetName() );

            if( !IsOK( this, msg ) )
                return;
        }
    }

    if( fn.Exists() && !fn.IsDirWritable() )
    {
        msg.Printf( _( "Write permissions are required to save library '%s'." ), fn.GetFullPath() );
        DisplayError( this, msg );
        return;
    }

    try
    {
        if( !fn.FileExists() )
            pi->CreateSymbolLib( fn.GetFullPath() );

        pi->SaveSymbol( fn.GetFullPath(), new LIB_PART( *part ) );
    }
    catch( const IO_ERROR& ioe )
    {
        msg = _( "Failed to create symbol library file " ) + fn.GetFullPath();
        DisplayErrorMessage( this, msg, ioe.What() );
        msg.Printf( _( "Error creating symbol library '%s'" ), fn.GetFullName() );
        SetStatusText( msg );
        return;
    }

    m_mruPath = fn.GetPath();

    /// @todo Give the user a choice to add the new library to the symbol library table.
    DisplayInfoMessage( this, _( "This library will not be available until it is added to the "
                                 "symbol library table." ) );

    GetScreen()->ClrModify();
    m_drawItem = m_lastDrawItem = NULL;

    msg.Printf( _( "Symbol '%s' saved in library '%s'" ), part->GetName(), fn.GetFullPath() );
    SetStatusText( msg );
}
