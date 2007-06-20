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


//#include <dmalloc.h>

class BeatDetect;
class Func;
class Param;
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

typedef enum {
    ALPHA_NEXT,
    ALPHA_PREVIOUS,
    RANDOM_NEXT,
    RESTART_ACTIVE,
  } switch_mode_t;

#define IDLE_PRESET_STRING "[idlepreset]\n"

class projectM {
public:
    static projectM *currentEngine;

    char *presetURL;
    char *presetName;
    char *fontURL;

    int hasInit;

    int noSwitch;
    int pcmframes;
    int freqframes;
    int totalframes;

    int showfps;
    int showtitle;
    int showpreset;
    int showhelp;
    int showstats;

    int studio;

    GLubyte *fbuffer;

    /** Preset information */
    int preset_index;               /** Index into the preset dir */

#ifndef WIN32
    /* The first ticks value of the application */
    struct timeval startTime;
#else
    long startTime;
#endif /** !WIN32 */
    float Time;

    /** Render target texture ID */
    RenderTarget *renderTarget;

    char disp[80];

    float wave_o;

    //int texsize=1024;   //size of texture to do actual graphics
    int fvw;     //fullscreen dimensions
    int fvh;
    int wvw;      //windowed dimensions
    int wvh;
    int vw;           //runtime dimensions
    int vh;
    int fullscreen;
    
    int avgtime;  //# frames per preset
    
    char *title;
    int drawtitle;
  
    int correction;
    float aspect;
    
    //per pixel equation variables
    float **gridx;  //grid containing interpolated mesh 
    float **gridy;  
    float **origtheta;  //grid containing interpolated mesh reference values
    float **origrad;  
    float **origx;  //original mesh 
    float **origy;
    float **origx2;  //original mesh 
    float **origy2;

    int mesh_i, mesh_j;

    /** Timing information */
    int mspf;      
    int timed;
    int timestart;
    int nohard;    
    int count;
    float realfps,
           fpsstart;

    /** Various toggles */
    int doPerPixelEffects;
    int doIterative;

    /** ENGINE VARIABLES */
    /** From engine_vars.h */
    char preset_name[256];

    /* PER FRAME CONSTANTS BEGIN */
    float zoom;
    float zoomexp;
    float rot;
    float warp;

    float sx;
    float sy;
    float dx;
    float dy;
    float cx;
    float cy;

    int gy;
    int gx;

    float decay;

    float wave_r;
    float wave_g;
    float wave_b;
    float wave_x;
    float wave_y;
    float wave_mystery;

    float ob_size;
    float ob_r;
    float ob_g;
    float ob_b;
    float ob_a;

    float ib_size;
    float ib_r;
    float ib_g;
    float ib_b;
    float ib_a;

    int meshx;
    int meshy;

    float mv_a ;
    float mv_r ;
    float mv_g ;
    float mv_b ;
    float mv_l;
    float mv_x;
    float mv_y;
    float mv_dy;
    float mv_dx;

    float progress ;
    int frame ;

        /* PER_FRAME CONSTANTS END */

    /* PER_PIXEL CONSTANTS BEGIN */

    float x_per_pixel;
    float y_per_pixel;
    float rad_per_pixel;
    float ang_per_pixel;

    /* PER_PIXEL CONSTANT END */


    float fRating;
    float fGammaAdj;
    float fVideoEchoZoom;
    float fVideoEchoAlpha;
    
    int nVideoEchoOrientation;
    int nWaveMode;
    int bAdditiveWaves;
    int bWaveDots;
    int bWaveThick;
    int bModWaveAlphaByVolume;
    int bMaximizeWaveColor;
    int bTexWrap;
    int bDarkenCenter;
    int bRedBlueStereo;
    int bBrighten;
    int bDarken;
    int bSolarize;
    int bInvert;
    int bMotionVectorsOn;
    int fps; 
    
    float fWaveAlpha ;
    float fWaveScale;
    float fWaveSmoothing;
    float fWaveParam;
    float fModWaveAlphaStart;
    float fModWaveAlphaEnd;
    float fWarpAnimSpeed;
    float fWarpScale;
    float fShader;
    
    
    /* Q VARIABLES START */

    float q1;
    float q2;
    float q3;
    float q4;
    float q5;
    float q6;
    float q7;
    float q8;


    /* Q VARIABLES END */

    float **zoom_mesh;
    float **zoomexp_mesh;
    float **rot_mesh;

    float **sx_mesh;
    float **sy_mesh;
    float **dx_mesh;
    float **dy_mesh;
    float **cx_mesh;
    float **cy_mesh;

    float **x_mesh;
    float **y_mesh;
    float **rad_mesh;
    float **theta_mesh;
	
#ifdef USE_GLF
	int title_font,
		other_font;
    float title_font_xsize,
          title_font_ysize;
    float other_font_xsize,
          other_font_ysize;
#endif /** USE_GLF */

    /** Beat detection engine */
    BeatDetect *beatDetect;

    /** Builtin databases */
    SplayTree *builtin_param_tree;
    SplayTree *builtin_func_tree;

    /** Functions */
    DLLEXPORT projectM();

    DLLEXPORT void projectM_init();
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

    /** Preset switching */
    int loadPresetDir( char *dir );
    int closePresetDir();
    int switchPreset( switch_mode_t switch_mode, int cut_type );
    void switchToIdlePreset();
    int loadPresetByFile( char *filename );
    int initPresetLoader();
    int destroyPresetLoader();
    int load_preset_file( const char *filename, Preset *preset );

    /** Idle preset */
    int initIdlePreset();
    int destroyIdlePreset();

    /** Param database */
    int load_all_builtin_param();
    void load_init_conditions();
    int init_builtin_param_db();
    int destroy_builtin_param_db();

    int insert_param_alt_name( Param *param, char *alt_name );
    Param *find_builtin_param( char *name );
    int load_builtin_param_float( char *name, void *engine_val, void *matrix,
                                  short int flags,
                                  float init_val, float upper_bound,
                                  float lower_bound, char *alt_name );
    int load_builtin_param_int( char *name, void *engine_val, short int flags,
                                int init_val, int upper_bound,
                                int lower_bound, char *alt_name );
    int load_builtin_param_bool( char *name, void *engine_val, short int flags,
                                int init_val, char *alt_name );
    int insert_builtin_param( Param *param );

    /** Func database */
    int init_builtin_func_db();
    int destroy_builtin_func_db();
    int load_all_builtin_func();
    int load_builtin_func( char * name, float (*func_ptr)(float*), int num_args );

    int insert_func( Func *func );
    int remove_func( Func *func );
    Func *find_func( char *name );
  };

extern int is_valid_extension( const struct dirent *ent );

#endif /** !_PROJECTM_H */
