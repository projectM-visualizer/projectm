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
 * wxvisCanvas.h
 *
 * Encapsulation of an OpenGL drawing canvas...
 *
 */

#ifndef _WXPROJECTMVISCANVAS_H
#define _WXPROJECTMVISCANVAS_H

#include <wx/wx.h>
#include <wx/glcanvas.h>

class projectM;
class wxvisFrame;

class wxvisCanvas : public wxGLCanvas {
private:
    /** Parent frame in the hierarchy */
    wxvisFrame *_parent;

    /** Has this viewport been initialised */
    int _hasInitialised;

#ifdef WIN32
    /** Refresh mutex */
    CRITICAL_SECTION refreshMutex;
    HANDLE refreshSemaphore;
#endif /** WIN32 */

    /** projectM */
    projectM *pm;

public:
    /** Constructor */
    wxvisCanvas( wxFrame *parent, const wxWindowID id = -1,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &dims = wxDefaultSize,
                 long style = 0, const wxString &name = "wxvisCanvas" );

    /** Event handlers */
    void OnPaint( wxPaintEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnEraseBackground( wxEraseEvent &event );
    void OnKeyPress( wxKeyEvent &event );

    DECLARE_EVENT_TABLE()

public:
    /** Miscellaneous methods */
    void initialise();

    /** Returns the number of depth bits */
    int getDepthBits();

    /** Returns the number of stencil bits */
    int getStencilBits();

    /** Returns the parent frame */
    wxvisFrame *getParent() { return _parent; }
  };

#endif /** !_WXPROJECTMVISCANVAS_H */
