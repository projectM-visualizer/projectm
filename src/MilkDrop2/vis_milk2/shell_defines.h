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

#ifndef __NULLSOFT_DX9_PLUGIN_SHELL_SHELL_DEFINES_H__
#define __NULLSOFT_DX9_PLUGIN_SHELL_SHELL_DEFINES_H__ 1

#include <windows.h>

#define DEFAULT_FULLSCREEN_WIDTH  640
#define DEFAULT_FULLSCREEN_HEIGHT 480
#define MAX_ICON_TEXTURES 8
#define ICON_TEXTURE_SIZE 256
#define DEFAULT_WINDOW_SIZE 0.625f      // as a portion of the width or height of the screen (whichever is smaller)
#define DESKTOP_MODE_KEYBOARD_INPUT_WINDOW_CLASSNAME "DESKTOP MODE KEYBOARD INPUT WINDOW"
#define BGR2RGB(x) (((x>>16)&0xFF) | (x & 0xFF00) | ((x<<16)&0xFF0000))

#define NUM_BASIC_FONTS 4 
#define  SYS_FONT 0
#define  DEC_FONT 1
#define  HELP_FONT 2
#define  DESK_FONT 3
#define MAX_EXTRA_FONTS 5
typedef enum
{
    SIMPLE_FONT = 0,  // aka 'system' font; should be legible
    DECORATIVE_FONT = 1,
    HELPSCREEN_FONT = 2,
    PLAYLIST_FONT = 3,
    EXTRA_1 = 4, 
    EXTRA_2 = 5,
    EXTRA_3 = 6,
    EXTRA_4 = 7,
    EXTRA_5 = 8
} 
eFontIndex;

// for m_screenmode:
typedef enum
{
    NOT_YET_KNOWN   = -1,
    FULLSCREEN      =  0,
    WINDOWED        =  1,
    FAKE_FULLSCREEN =  2,
    DESKTOP         =  3       // doesn't use overlays! =)
} 
eScrMode;

#include "../Winamp/wa_ipc.h"

#endif