/*
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
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
 * $Id: projectM.h,v 1.1.1.1 2005/12/23 18:05:11 psperl Exp $
 *
 * Encapsulation of ProjectM engine
 *
 * $Log$
 */

#ifndef _PROJECTM_H
#define _PROJECTM_H

#include "pbuffer.h"

#ifdef WIN32
#include "win32-dirent.h"
#else
#include <dirent.h>
#endif /** WIN32 */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/types.h>

#ifdef MACOS
#include <MacWindows.h>
#include <gl.h>
#include <glu.h>
#else
#ifdef WIN32
#include <windows.h>
#endif /** WIN32 */
#include <GL/gl.h>
#include <GL/glu.h>
#endif /** MACOS */
#ifdef WIN322
#define inline
#endif /** WIN32 */
#ifndef WIN32
#include <sys/time.h>
#else
#endif /** !WIN32 */

#include "dlldefs.h"
#include "event.h"
#include "fatal.h"
#include "PresetFrameIO.hpp"
#include "Preset.hpp"
#include "Renderer.hpp"

//#include <dmalloc.h>

class BeatDetect;
class Func;

class Preset;
class SplayTree;

#ifdef WIN32
#pragma warning (disable:4244)
#pragma warning (disable:4305)
#endif /** WIN32 */

#ifdef MACOS2
#define inline
#endif

/** KEEP THIS UP TO DATE! */
#define PROJECTM_VERSION "0.99.10"
#define PROJECTM_TITLE "projectM 0.99.10"

/** Per-platform path separators */
#define WIN32_PATH_SEPARATOR '\\'
#define UNIX_PATH_SEPARATOR '/'
#ifdef WIN32
#define PATH_SEPARATOR WIN32_PATH_SEPARATOR
#else
#define PATH_SEPARATOR UNIX_PATH_SEPARATOR
#endif /** WIN32 */

/** Thread state */
typedef enum { GO, STOP } PMThreadState;

/** Interface types */
typedef enum {
    MENU_INTERFACE,
    SHELL_INTERFACE,
    EDITOR_INTERFACE,
    DEFAULT_INTERFACE,
    BROWSER_INTERFACE
  } interface_t;


class projectM {
public:
    static projectM *currentEngine;
    static Preset *activePreset;
    static Renderer *renderer; 
    static RenderTarget *renderTarget;

    char *presetURL;
    char *presetName;
    char *fontURL;

    int hasInit;

    int noSwitch;
    int pcmframes;
    int freqframes;
    int totalframes;


    GLubyte *fbuffer;

    /** Preset information */
   /// bug: move
    int preset_index;               /** Index into the preset dir */

#ifndef WIN32
    /* The first ticks value of the application */
    struct timeval startTime;
#else
    long startTime;
#endif /** !WIN32 */
    float Time;

    /** Render target texture ID */
   

    char disp[80];

    float wave_o;

    //int texsize=1024;   //size of texture to do actual graphics
    int fvw;     //fullscreen dimensions
    int fvh;
    int wvw;      //windowed dimensions
    int wvh;
    
    int fullscreen;
    
    int avgtime;  //# frames per preset
    
    char *title;
    int drawtitle;
  
 

    /** Timing information */
    int mspf;
    int timed;
    int timestart;
    int nohard;
    int count;
    float realfps,
           fpsstart;

    /** Various toggles */
        /* PER_FRAME CONSTANTS END */

    /** Beat detection engine */
    BeatDetect *beatDetect;

    /** All readonly variables 
     *  which are passed as inputs 
     * to presets. See struct defintition above */
    PresetInputs presetInputs;

    /** The presets modify these values. For now this is declared on stack
     * but might be better on heap for sake of smooth preset switching */
    PresetOutputs presetOutputs;

    /** Functions */
    DLLEXPORT projectM();

    DLLEXPORT void projectM_init(int gx, int gy, int texsize, int width, int height);
    DLLEXPORT void projectM_reset();
    DLLEXPORT void projectM_resetGL( int width, int height );
    DLLEXPORT void projectM_setTitle( char *title );
    DLLEXPORT void renderFrame();

    DLLEXPORT void projectM_initengine();
    DLLEXPORT void projectM_resetengine();
    void draw_help();
    void draw_fps(float fps);
    void draw_preset();
    void draw_title();
    void draw_stats();

    void modulate_opacity_by_volume();
    void maximize_colors();
    void darken_center();

    void do_per_pixel_math();
    void do_per_frame();

    void render_interpolation();
    void render_texture_to_screen();
    void render_texture_to_studio();

    void draw_motion_vectors();
    void draw_borders();
    void draw_shapes();
    void draw_waveform();
    void draw_custom_waves();

    void draw_title_to_screen();
    void draw_title_to_texture();
    void get_title();

    void reset_per_pixel_matrices();
    void init_per_pixel_matrices();
    void rescale_per_pixel_matrices();
    void free_per_pixel_matrices();

    void key_handler( projectMEvent event,
                      projectMKeycode keycode, projectMModifier modifier );
    void default_key_handler( projectMEvent event, projectMKeycode keycode );

    int initPresetTools();
  };

#endif /** !_PROJECTM_H */
