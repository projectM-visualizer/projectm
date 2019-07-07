/*
  LICENSE
  -------
Copyright 2005-2013 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution. 

  * Neither the name of Nullsoft nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission. 
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __MILKDROP_DEFINES_H__
#define __MILKDROP_DEFINES_H__ 1

// IDENTIFIERS
/*
#define CURRENT_VERSION			220
#define CURRENT_SUBVERSION		0			// 0=first release, 1=a, 2=b, 3=c... 
#define MODULEDESC				"MilkDrop 2.2"	// used for module desc (from Winamp/Prefs) + window title for fullscreen mode
#define DLLDESC					"MilkDrop 2.2"	
#define NAME					"MilkDrop"
#define TITLE					"MilkDrop"
#define CLASSNAME               "MilkDrop"  // window class name
*/
#define TEXT_WINDOW_CLASSNAME   "MilkDrop Console [VJ Mode]"
#define DEBUGFILE				"c:\\m_debug.txt"
//#define CONFIG_INIFILE			"milkdrop_config.ini"
//#define PRESET_INIFILE			"milkdrop_presets.ini"
#define DEBUGFILEHEADER			"[milkdrop debug file]\n"

// define this to disable expression evaluation:
//   (...for some reason, evallib kills the debugger)
#ifdef _DEBUG
  #define _NO_EXPR_  //FIXME
#endif

#define MAX_GRID_X 192//128
#define MAX_GRID_Y 144//96
#define NUM_WAVES  8
#define NUM_MODES  7
#define LINEFEED_CONTROL_CHAR 1		// note: this char should be outside the ascii range from SPACE (32) to lowercase 'z' (122)
#define MAX_CUSTOM_MESSAGE_FONTS 16		// 0-15
#define MAX_CUSTOM_MESSAGES 100			// 00-99
#define MAX_CUSTOM_WAVES  4
#define MAX_CUSTOM_SHAPES 4

// aspect ratio makes the motion in the UV field [0..1] cover the screen appropriately,
//#define ASPECT_X    1.00
//#define ASPECT_Y    0.75     // ~h/w
//#define ASPECT_X  ( (m_nTexSizeY > m_nTexSizeX) ? m_nTexSizeX/(float)m_nTexSizeY : 1.0f )  //0.75f
//#define ASPECT_Y  ( (m_nTexSizeX > m_nTexSizeY) ? m_nTexSizeY/(float)m_nTexSizeX : 1.0f )  //0.75f
//  --> now stored in m_fAspectX, m_fInvAspectY, etc. <--

#define WM_MILKDROP_SYSTRAY_MSG			WM_USER + 407
#define IDC_MILKDROP_SYSTRAY_ICON			555
#define ID_MILKDROP_SYSTRAY_CLOSE			556
//#define ID_MILKDROP_SYSTRAY_RESUME			559
//#define ID_MILKDROP_SYSTRAY_SUSPEND			560
//#define ID_MILKDROP_SYSTRAY_HOTKEYS			561

#define NUMERIC_INPUT_MODE_CUST_MSG 0
#define NUMERIC_INPUT_MODE_SPRITE   1
#define NUMERIC_INPUT_MODE_SPRITE_KILL 2

#endif