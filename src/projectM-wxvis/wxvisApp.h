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
 * $Id: wxvisApp.h,v 1.2 2004/11/12 15:47:27 cvs Exp $
 *
 * wxvis Application
 *
 */

#ifndef _WXPROJECTMVISAPP_H
#define _WXPROJECTMVISAPP_H

#undef __WXDEBUG__

#include <wx/wx.h>
#ifdef WIN32
#include <windows.h>
#endif /** WIN32 */

class wxvisFrame;

/** The main GUI class */
class wxvisApp : public wxApp {
private:
    /** Icon */
    wxIcon _icon;

    /** Main rendering frame */
    wxvisFrame *_visFrame;

public:
    /** Installation root queried from the registry */
    unsigned char *installationRoot;

#ifdef WIN32
    /** wxvis root association key from the registry */
    HKEY rootAssociationKey;
#endif

    bool OnInit();
    void OnIdle( wxIdleEvent &event );

    /** Returns various sub-frames */
    wxvisFrame *getRenderFrame() { return _visFrame; }

    /** Shuts down the application */
    void shutdown();

    DECLARE_EVENT_TABLE()
  };

#endif /** !_WXPROJECTMVISAPP_H */
