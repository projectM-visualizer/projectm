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
 * wxvisFrame.cpp
 */

#include <stdio.h>
#include <wx/wx.h>

#include "wxvisApp.h"
#include "wxvisFrame.h"
#include "wxvisCanvas.h"

#ifdef DEBUG
extern FILE *debugFile;
#endif /** DEBUG */

/** Event handlers */
BEGIN_EVENT_TABLE(wxvisFrame, wxFrame)
    /** Idling */
    EVT_CLOSE(wxvisFrame::OnClose)
END_EVENT_TABLE()

/** Create the frame */
wxvisFrame::wxvisFrame( wxvisApp *parent, const wxString &title,
                        wxPoint &pos, 
                        wxSize &dims ) : wxFrame( (wxFrame *)NULL, -1, title, pos, dims ) {

    /** Ensure sub-windows/dialogs are null... */
    canvas = NULL;

    /** Store the parent */
    app = parent;

    /** Mesh/texture size menu */
    wxMenu *renderingMenu = new wxMenu();
    renderingMenu->Append( WXPROJECTMVIS_RENDERING_PER_PIXEL, "&Toggle per-pixel rendering", "Toggles per-pixel rendering", TRUE );
    renderingMenu->Check( WXPROJECTMVIS_RENDERING_PER_PIXEL, TRUE );

    /** Create a menubar */
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( renderingMenu, "Rendering" );
    SetMenuBar( menuBar );

    /** Create a status line */
    CreateStatusBar( 2 );
    SetStatusText( "Ready", 1 );
  }

/** Print out frame rate statistics */
void wxvisFrame::updaterate(const char *message ) {
//    SendMessage( progress, SB_SETTEXT, 0, (LPARAM)message );
    SetStatusText( message, 0 );
  }

/** Print out what wxvis thinks it's doing  */
void wxvisFrame::updatestatus( const char *message ) {
//    SendMessage( progress, SB_SETTEXT, 1, (LPARAM)message );
    SetStatusText( message, 1 );
  }

/** Handles closing down the application by the "X" button */
void wxvisFrame::OnClose( wxCloseEvent &event ) {
    app->shutdown();
  }
