/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id: wxvisApp.cpp,v 1.6 2004/11/12 15:47:27 cvs Exp $
 *
 */

#include <stdio.h>
#include <wx/wx.h>
#include <projectM-engine/projectM.hpp>

#include "wxvisApp.h"
#include "wxvisFrame.h"
#include "wxvisCanvas.h"

#ifdef DEBUG
FILE *debugFile = NULL;
#endif

projectM *globalPM = NULL;

IMPLEMENT_APP(wxvisApp)

BEGIN_EVENT_TABLE(wxvisApp,wxApp)
    EVT_IDLE(wxvisApp::OnIdle)
END_EVENT_TABLE()

/** Parse command-line options and create the main wxvis windows */
bool wxvisApp::OnInit() {

#ifdef DEBUG
#ifndef WIN32
    /** Open the debug file */
    debugFile = fopen( "/tmp/wxvis.txt", "w" );
#else
    /** Open the debug file */
    debugFile = fopen( "c:\\wxvis.txt", "w" );
#endif /** __WXMAC__ */
#else
//    debugFile = NULL;
#endif

#ifdef WIN32
    /** Retrieve the key from the registry for the base installation directory */
    installationRoot = (unsigned char *)malloc( sizeof( unsigned char ) * 1024 );
    installationRoot[0] = '\0';
    HKEY key;
    if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Archaeoptics\\wxvis3D",
                       0, KEY_QUERY_VALUE, &key ) != ERROR_SUCCESS ) {
//        MessageBox( NULL, "Failed to open registry key: SOFTWARE\\Archaeoptics\\wxvis\nHelp and NPR textures may be unavailable!", "projectM", MB_OK | MB_ICONEXCLAMATION );
        sprintf( (char *)installationRoot, "%s", "c:\\Program Files\\Archaeoptics\\wxvis3D" );
      } else {
        DWORD installRootType = REG_SZ;
        DWORD installRootSize = 1024;
        LONG rv = RegQueryValueEx( key, "InstallRoot", NULL, &installRootType, installationRoot, &installRootSize );
        if ( rv != ERROR_SUCCESS ) {
            /** This section causes things to crash weirdly... */
//            MessageBox( NULL, "Failed to query registry key: SOFTWARE\\Archaeoptics\\wxvis\nHelp and textures may be unavailable!", "projectM", MB_OK | MB_ICONEXCLAMATION );
/*            char msg[128];
            sprintf( msg, "Error code: %d", rv );
            wxMessageBox( msg, "projectM", wxOK | wxICON_EXCLAMATION ); */
            sprintf( (char *)installationRoot, "%s", "c:\\Program Files\\Archaeoptics\\wxvis3D" );
          }
        RegCloseKey( key );
      }

#ifdef DEBUG2
    fprintf( debugFile, "Installation Root: %s\n", installationRoot );
    fflush( debugFile );
#endif /** DEBUG */
#else
    installationRoot = NULL;
#endif /** WIN32 */

    /** Empty out various structures before initialisation */
    _visFrame = NULL;

    /** Load the icon */
    _icon = wxIcon( "ARCHAEOPTICS_ICON" );

    /** Create the rendering frame */
    wxPoint pt;
    wxSize sz;
    pt.x = 50; pt.y = 50;
    sz.x = 640; sz.y = 480;
    _visFrame = new wxvisFrame( this, wxString( "projectM" ), pt, sz );
    _visFrame->canvas = new wxvisCanvas( _visFrame );
    _visFrame->SetIcon( _icon );
    _visFrame->Show( TRUE );

    /** Check depth and stencil capabilities */
    int depthBits = _visFrame->canvas->getDepthBits();
    int stencilBits = _visFrame->canvas->getStencilBits();
    if ( depthBits < 24 || stencilBits < 1 ) {
        char msg[256];
        sprintf( msg, "Depth and stencil bit settings are sub-optimal\nBlack and White Ink Rendering and Elevation Extraction\nmay not work correctly\nDepth Bits: %d\nStencil Bits: %d\n(Should be at least 24 and 1)", depthBits, stencilBits );
//        wxMessageBox( msg, "projectM", wxOK | wxICON_EXCLAMATION );
      }

#ifdef WIN32
    /** Update file associations */
    /** Create the base wxvis key */
    int rv;
    rv = RegCreateKey( HKEY_CLASSES_ROOT, "projectM", &rootAssociationKey );
    rv = RegSetValue( rootAssociationKey, "", REG_SZ, "MilkDrop Preset", 0 );

    /** This sets the command line for "projectM" */
    char exename[256];
    sprintf( exename, "%s\\wxvis.exe %%1", installationRoot );
    rv = RegCreateKey( HKEY_CLASSES_ROOT, "projectM", &rootAssociationKey );
    rv = RegSetValue( rootAssociationKey, "shell\\open\\command", REG_SZ, exename, MAX_PATH );

    /** Set the icon */
    char iconname[256];
    sprintf( iconname, "%s\\archaeoptics_icon_64x64.ico", installationRoot );
    rv = RegCreateKey( HKEY_CLASSES_ROOT, "projectM", &rootAssociationKey );
    rv = RegSetValue( rootAssociationKey, "DefaultIcon", REG_SZ, iconname, MAX_PATH );
#endif

    /** Check for a model name on the command line */
#ifdef DEBUG2
    fprintf( debugFile, "argc: %d\n", argc );
    fflush( debugFile );
#endif
    if ( argc >= 2 ) {
        if ( ( strstr( argv[1], ".milk" ) != NULL ) ||
             ( strstr( argv[1], ".MILK" ) != NULL ) ) {
            /** Load a fixed preset */
          }
      }

    return TRUE;
  }

/** Shuts down the application */
void wxvisApp::shutdown() {
    /** Free local resources */
    if ( _visFrame ) {
        _visFrame->canvas->Destroy();
        _visFrame->Destroy();
      }

#ifdef DEBUG2
    fclose( debugFile );
#endif /** DEBUG */

    exit( 0 );
  }

void wxvisApp::OnIdle( wxIdleEvent &event ) {
    if ( _visFrame ) {
        _visFrame->canvas->Refresh( FALSE );
      }
  }
