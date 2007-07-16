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
#include "wipemalloc.h"
#include "BuiltinFuncs.hpp"
#include "fatal.h"
#include "Common.hpp"
#include "compare.h"
#ifdef WIN32
#include "win32-dirent.h"
#endif

#include "timer.h"

#ifdef LINUX
#include "time.h"
#endif

//#include <xmms/plugin.h>
#include "projectM.hpp"
#include "BeatDetect.hpp"
#include "Eval.hpp"
#include "Param.hpp"
#include "Parser.hpp"
#include "Preset.hpp"
#include "PerPixelEqn.hpp"
//#include "menu.h"
#include "PCM.hpp"                    //Sound data handler (buffering, FFT, etc.)
#include "CustomWave.hpp"
#include "CustomShape.hpp"
#include <map>
#include "Renderer.hpp"
#include "PresetChooser.hpp"

#ifdef LINUX
const std::string projectM::PROJECTM_PRESET_PATH("/usr/share/projectM/presets/");
#endif

#ifdef MACOS
const std::string projectM::PROJECTM_PRESET_PATH("/usr/share/projectM/presets/");
#endif


#ifdef WIN32
const std::string projectM::PROJECTM_PRESET_PATH("C:\\Program Files\\ProjectM\\presets");
#endif

/** Stash current engine */
projectM *projectM::currentEngine = NULL;

/** Static variable initialization involving the render. */ 
/// @bug These probably shouldn't be static!
RenderTarget * projectM::renderTarget = NULL;
Renderer * projectM::renderer = NULL;


DLLEXPORT projectM::projectM() {
    beatDetect = NULL;
}


DLLEXPORT void projectM::renderFrame() {

#ifdef DEBUG
    char fname[1024];
    FILE *f = NULL;
    int index = 0;
    int x, y;
#endif

//     printf("Start of loop at %d\n",timestart);

    mspf=(int)(1000.0/(float)presetInputs.fps); //milliseconds per frame


#ifndef WIN32
    presetInputs.time = getTicks( &startTime ) * 0.001;
#else
    presetInputs.time = getTicks( startTime ) * 0.001;
#endif /** !WIN32 */

    presetInputs.frame++;  //number of frames for current preset
    presetInputs.progress= presetInputs.frame/(float)avgtime;
    DWRITE( "frame: %d\ttime: %f\tprogress: %f\tavgtime: %d\tang: %f\trot: %f\n",
            this->presetInputs.frame, presetInputs.time, this->presetInputs.progress, this->avgtime, this->presetInputs.ang_per_pixel,
            this->presetOutputs.rot );
    if (presetInputs.progress>1.0) presetInputs.progress=1.0;

//       printf("start:%d at:%d min:%d stop:%d on:%d %d\n",startframe, frame frame-startframe,avgtime,  noSwitch,progress);
    presetInputs.ResetMesh();
    m_activePreset->evaluateFrame();


//     printf("%f %d\n",Time,frame);


    beatDetect->detectFromSamples();
    DWRITE( "=== vol: %f\tbass: %f\tmid: %f\ttreb: %f ===\n",
            beatDetect->vol,beatDetect->bass,beatDetect->mid,beatDetect->treb);
    DWRITE( "=== bass_att: %f ===\n",
            beatDetect->bass_att );

    if (renderer->noSwitch==0) {
        nohard--;
        if ((beatDetect->bass-beatDetect->bass_old>beatDetect->beat_sensitivity ||
                avgtime ) && nohard<0) {
//              printf("%f %d %d\n", beatDetect->bass-beatDetect->bass_old,this->frame,this->avgtime);
//              switchPreset(RANDOM_NEXT, HARD_CUT);
            nohard=presetInputs.fps*5;
        }
    }

    count++;

    renderer->RenderFrame(&presetOutputs, &presetInputs);

#ifndef WIN32
    /** Frame-rate limiter */
    /** Compute once per preset */
    if (this->count%100==0) {
        this->renderer->realfps=100.0/((getTicks(&this->startTime)-this->fpsstart)/1000);
        this->fpsstart=getTicks(&this->startTime);
    }

    int timediff = getTicks(&this->startTime)-this->timestart;

    if ( timediff < this->mspf) {
        // printf("%s:",this->mspf-timediff);
        int sleepTime = (unsigned int)( this->mspf-timediff ) * 1000;
        DWRITE( "usleep: %d\n", sleepTime );
        if ( sleepTime > 0 && sleepTime < 100000 ) {
            if ( usleep( sleepTime ) != 0 ) {}
        }
    }
    this->timestart=getTicks(&this->startTime);
#endif /** !WIN32 */

    DWRITE( "exiting renderFrame()\n" );
}

DLLEXPORT void projectM::projectM_reset() {

    DWRITE( "projectM_reset(): in\n" );

    /// @bug it's very possible this is a hack
    m_activePreset = std::auto_ptr<Preset>(0);

    this->presetURL = NULL;
    this->fontURL = NULL;

    /** Default variable settings */
    this->hasInit = 0;

    this->pcmframes = 1;
    this->freqframes = 0;



    this->fvw = 800;
    this->fvh = 600;
    this->wvw = 512;
    this->wvh = 512;
    this->fullscreen = 0;

    /** Frames per preset */
    this->avgtime = 500;



    /** More other stuff */
    this->mspf = 0;
    this->timed = 0;
    this->timestart = 0;
    this->nohard = 0;
    this->count = 0;

    this->fpsstart = 0;

    projectM_resetengine();
}

DLLEXPORT void projectM::projectM_init(int gx, int gy, int fps, int texsize, int width, int height) {



    /** Initialise engine variables */

    presetInputs.Initialize(gx, gy);
    presetOutputs.Initialize(gx, gy);
    projectM_initengine();

    DWRITE("projectM plugin: Initializing\n");

    /** Initialise start time */
#ifndef WIN32
    gettimeofday(&this->startTime, NULL);
#else
    startTime = GetTickCount();
#endif /** !WIN32 */

    /** Nullify frame stash */
    fbuffer = NULL;

    /** Initialise per-pixel matrix calculations */


    presetInputs.fps = fps;

    /** We need to initialise this before the builtin param db otherwise bass/mid etc won't bind correctly */
    beatDetect = new BeatDetect();

    /* Preset loading function */
    initPresetTools();

    /* Load default preset directory */
#ifdef MACOS2
    /** Probe the bundle for info */
    CFBundleRef bundle = CFBundleGetMainBundle();
    char msg[1024];
    sprintf( msg, "bundle: %X\n", bundle );
    DWRITE( msg );
    if ( bundle != NULL ) {
        CFPlugInRef pluginRef = CFBundleGetPlugIn( bundle );
        if ( pluginRef != NULL ) {
            DWRITE( "located plugin ref\n" );
        } else {
            DWRITE( "failed to find plugin ref\n" );
        }

        CFURLRef bundleURL = CFBundleCopyBundleURL( bundle );
        if ( bundleURL == NULL ) {
            DWRITE( "bundleURL failed\n" );
        } else {
            DWRITE( "bundleURL OK\n" );
        }
        char *bundleName =
            (char *)CFStringGetCStringPtr( CFURLGetString( bundleURL ), kCFStringEncodingMacRoman );
        DWRITE( "bundleURL: %s\n", bundleName );

        presetURL = CFBundleCopyResourceURL( bundle, purl, NULL, NULL );
        if ( presetURL != NULL ) {
            this->presetURL = (char *)CFStringGetCStringPtr( CFURLCopyPath( presetURL ), kCFStringEncodingMacRoman);
            sprintf( msg, "Preset: %s\n", presetURL );
            DWRITE( msg );
            printf( msg );

            /** Stash the short preset name */

        } else {
            DWRITE( "Failed to probe 'presets' bundle ref\n" );
            this->presetURL = NULL;
        }

        fontURL = CFBundleCopyResourceURL( bundle, furl, NULL, NULL );
        if ( fontURL != NULL ) {
            fontURL = (char *)CFStringGetCStringPtr( CFURLCopyPath( fontURL ), kCFStringEncodingMacRoman);
            sprintf( msg, "Font: %s\n", fontURL );
            DWRITE( msg );
            printf( msg );
        } else {
            DWRITE( "Failed to probe 'fonts' bundle ref\n" );
            fontURL = NULL;
        }
    }

    /** Sanity check */
    if ( bundle == NULL || presetURL == NULL || fontURL == NULL ) {
        sprintf( msg, "defaulting presets\n" );
        DWRITE( msg );
        this->fontURL = (char *)wipemalloc( sizeof( char ) * 512 );
//        strcpy( this->fontURL, "../../fonts/" );
        strcpy( fontURL, "/Users/descarte/tmp/projectM/fonts" );
        this->fontURL[34] = '\0';
//        loadPresetDir( "../../presets/" );
//        loadPresetDir( "/Users/descarte/tmp/projectM-1.00/presets_projectM" );
    } else {
        printf( "PresetDir: %s\n", this->presetURL );
        loadPresetDir( presetURL );
    }
#else
    if ( presetURL == NULL || fontURL == NULL ) {
        char msg[1024];
        sprintf( msg, "defaulting presets\n" );
        DWRITE( msg );
        fontURL = (char *)wipemalloc( sizeof( char ) * 512 );
#ifdef WIN32
        strcpy( this->fontURL, "c:\\tmp\\projectM\\fonts" );
        fontURL[24] = '\0';
#else
    strcpy( this->fontURL, "/Users/descarte/tmp/projectM/fonts" );
    fontURL[34] = '\0';
#endif
        DWRITE( "loading font URL directly: %s\n", this->fontURL );
#ifdef WIN32
        //      loadPresetDir( "c:\\tmp\\projectM\\presets_projectM" );
#else
    //    loadPresetDir( "/Users/descarte/tmp/projectM-1.00/presets_projectM" );
#endif
    } else {
        printf( "PresetDir: %s\n", this->presetURL );
        //loadPresetDir( presetURL );
    }

#endif


    mspf=(int)(1000.0/(float)presetInputs.fps);


//    initMenu();
//DWRITE( "post initMenu()\n" );

    printf("mesh: %d %d\n", gx,gy );

#ifdef PANTS
    printf( "maxsamples: %d\n", this->maxsamples );
    initPCM(this->maxsamples);
    DWRITE( "post PCM init\n" );
#endif

    this->avgtime=this->presetInputs.fps*20;

    this->hasInit = 1;

    this->renderTarget = new RenderTarget(texsize, width, height);
    this->presetInputs.gx = gx;
    this->presetInputs.gy = gy;

    this->renderer = new Renderer(width, height, gx, gy, renderTarget, beatDetect, fontURL);

    printf( "exiting projectM_init()\n" );
}




//calculate matrices for per_pixel




DLLEXPORT void projectM::projectM_initengine() {

    /* PER FRAME CONSTANTS BEGIN */
    this->presetOutputs.zoom=1.0;
    this->presetOutputs.zoomexp= 1.0;
    this->presetOutputs.rot= 0.0;
    this->presetOutputs.warp= 0.0;

    this->presetOutputs.sx= 1.0;
    this->presetOutputs.sy= 1.0;
    this->presetOutputs.dx= 0.0;
    this->presetOutputs.dy= 0.0;
    this->presetOutputs.cx= 0.5;
    this->presetOutputs.cy= 0.5;

    this->presetOutputs.decay=.98;

    this->presetOutputs.wave_r= 1.0;
    this->presetOutputs.wave_g= 0.2;
    this->presetOutputs.wave_b= 0.0;
    this->presetOutputs.wave_x= 0.5;
    this->presetOutputs.wave_y= 0.5;
    this->presetOutputs.wave_mystery= 0.0;

    this->presetOutputs.ob_size= 0.0;
    this->presetOutputs.ob_r= 0.0;
    this->presetOutputs.ob_g= 0.0;
    this->presetOutputs.ob_b= 0.0;
    this->presetOutputs.ob_a= 0.0;

    this->presetOutputs.ib_size = 0.0;
    this->presetOutputs.ib_r = 0.0;
    this->presetOutputs.ib_g = 0.0;
    this->presetOutputs.ib_b = 0.0;
    this->presetOutputs.ib_a = 0.0;

    this->presetOutputs.mv_a = 0.0;
    this->presetOutputs.mv_r = 0.0;
    this->presetOutputs.mv_g = 0.0;
    this->presetOutputs.mv_b = 0.0;
    this->presetOutputs.mv_l = 1.0;
    this->presetOutputs.mv_x = 16.0;
    this->presetOutputs.mv_y = 12.0;
    this->presetOutputs.mv_dy = 0.02;
    this->presetOutputs.mv_dx = 0.02;

//this->presetInputs.meshx = 0;
//this->presetInputs.meshy = 0;


    this->presetInputs.progress = 0;
    this->presetInputs.frame = 0;

    this->avgtime = 600;
//bass_thresh = 0;

    /* PER_FRAME CONSTANTS END */
    this->presetOutputs.fRating = 0;
    this->presetOutputs.fGammaAdj = 1.0;
    this->presetOutputs.fVideoEchoZoom = 1.0;
    this->presetOutputs.fVideoEchoAlpha = 0;
    this->presetOutputs.nVideoEchoOrientation = 0;

    this->presetOutputs.nWaveMode = 7;
    this->presetOutputs.bAdditiveWaves = 0;
    this->presetOutputs.bWaveDots = 0;
    this->presetOutputs.bWaveThick = 0;
    this->presetOutputs.bModWaveAlphaByVolume = 0;
    this->presetOutputs.bMaximizeWaveColor = 0;
    this->presetOutputs.bTexWrap = 0;
    this->presetOutputs.bDarkenCenter = 0;
    this->presetOutputs.bRedBlueStereo = 0;
    this->presetOutputs.bBrighten = 0;
    this->presetOutputs.bDarken = 0;
    this->presetOutputs.bSolarize = 0;
    this->presetOutputs.bInvert = 0;
    this->presetOutputs.bMotionVectorsOn = 1;

    this->presetOutputs.fWaveAlpha =1.0;
    this->presetOutputs.fWaveScale = 1.0;
    this->presetOutputs.fWaveSmoothing = 0;
    this->presetOutputs.fWaveParam = 0;
    this->presetOutputs.fModWaveAlphaStart = 0;
    this->presetOutputs.fModWaveAlphaEnd = 0;
    this->presetOutputs.fWarpAnimSpeed = 0;
    this->presetOutputs.fWarpScale = 0;
    this->presetOutputs.fShader = 0;


    /* PER_PIXEL CONSTANTS BEGIN */
    this->presetInputs.x_per_pixel = 0;
    this->presetInputs.y_per_pixel = 0;
    this->presetInputs.rad_per_pixel = 0;
    this->presetInputs.ang_per_pixel = 0;

    /* PER_PIXEL CONSTANT END */


    /* Q AND T VARIABLES START */

    this->presetOutputs.q1 = 0;
    this->presetOutputs.q2 = 0;
    this->presetOutputs.q3 = 0;
    this->presetOutputs.q4 = 0;
    this->presetOutputs.q5 = 0;
    this->presetOutputs.q6 = 0;
    this->presetOutputs.q7 = 0;
    this->presetOutputs.q8 = 0;


    /* Q AND T VARIABLES END */

//per pixel meshes
    this->presetOutputs.zoom_mesh = NULL;
    this->presetOutputs.zoomexp_mesh = NULL;
    this->presetOutputs.rot_mesh = NULL;


    this->presetOutputs.sx_mesh = NULL;
    this->presetOutputs.sy_mesh = NULL;
    this->presetOutputs.dx_mesh = NULL;
    this->presetOutputs.dy_mesh = NULL;
    this->presetOutputs.cx_mesh = NULL;
    this->presetOutputs.cy_mesh = NULL;

    this->presetInputs.x_mesh = NULL;
    this->presetInputs.y_mesh = NULL;
    this->presetInputs.rad_mesh = NULL;
    this->presetInputs.theta_mesh = NULL;

//custom wave per point meshes
}

/* Reinitializes the engine variables to a default (conservative and sane) value */
DLLEXPORT void projectM::projectM_resetengine() {

    this->presetOutputs.zoom=1.0;
    this->presetOutputs.zoomexp= 1.0;
    this->presetOutputs.rot= 0.0;
    this->presetOutputs.warp= 0.0;

    this->presetOutputs.sx= 1.0;
    this->presetOutputs.sy= 1.0;
    this->presetOutputs.dx= 0.0;
    this->presetOutputs.dy= 0.0;
    this->presetOutputs.cx= 0.5;
    this->presetOutputs.cy= 0.5;

    this->presetOutputs.decay=.98;

    this->presetOutputs.wave_r= 1.0;
    this->presetOutputs.wave_g= 0.2;
    this->presetOutputs.wave_b= 0.0;
    this->presetOutputs.wave_x= 0.5;
    this->presetOutputs.wave_y= 0.5;
    this->presetOutputs.wave_mystery= 0.0;

    this->presetOutputs.ob_size= 0.0;
    this->presetOutputs.ob_r= 0.0;
    this->presetOutputs.ob_g= 0.0;
    this->presetOutputs.ob_b= 0.0;
    this->presetOutputs.ob_a= 0.0;

    this->presetOutputs.ib_size = 0.0;
    this->presetOutputs.ib_r = 0.0;
    this->presetOutputs.ib_g = 0.0;
    this->presetOutputs.ib_b = 0.0;
    this->presetOutputs.ib_a = 0.0;

    this->presetOutputs.mv_a = 0.0;
    this->presetOutputs.mv_r = 0.0;
    this->presetOutputs.mv_g = 0.0;
    this->presetOutputs.mv_b = 0.0;
    this->presetOutputs.mv_l = 1.0;
    this->presetOutputs.mv_x = 16.0;
    this->presetOutputs.mv_y = 12.0;
    this->presetOutputs.mv_dy = 0.02;
    this->presetOutputs.mv_dx = 0.02;

    /// @bug think these are just gx/gy
    //this->meshx = 0;
    //this->meshy = 0;

    if ( beatDetect != NULL ) {
        beatDetect->reset();
    }
    this->presetInputs.progress = 0;
    this->presetInputs.frame = 0;

// bass_thresh = 0;

    /* PER_FRAME CONSTANTS END */
    this->presetOutputs.fRating = 0;
    this->presetOutputs.fGammaAdj = 1.0;
    this->presetOutputs.fVideoEchoZoom = 1.0;
    this->presetOutputs.fVideoEchoAlpha = 0;
    this->presetOutputs.nVideoEchoOrientation = 0;

    this->presetOutputs.nWaveMode = 7;
    this->presetOutputs.bAdditiveWaves = 0;
    this->presetOutputs.bWaveDots = 0;
    this->presetOutputs.bWaveThick = 0;
    this->presetOutputs.bModWaveAlphaByVolume = 0;
    this->presetOutputs.bMaximizeWaveColor = 0;
    this->presetOutputs.bTexWrap = 0;
    this->presetOutputs.bDarkenCenter = 0;
    this->presetOutputs.bRedBlueStereo = 0;
    this->presetOutputs.bBrighten = 0;
    this->presetOutputs.bDarken = 0;
    this->presetOutputs.bSolarize = 0;
    this->presetOutputs.bInvert = 0;
    this->presetOutputs.bMotionVectorsOn = 1;

    this->presetOutputs.fWaveAlpha =1.0;
    this->presetOutputs.fWaveScale = 1.0;
    this->presetOutputs.fWaveSmoothing = 0;
    this->presetOutputs.fWaveParam = 0;
    this->presetOutputs.fModWaveAlphaStart = 0;
    this->presetOutputs.fModWaveAlphaEnd = 0;
    this->presetOutputs.fWarpAnimSpeed = 0;
    this->presetOutputs.fWarpScale = 0;
    this->presetOutputs.fShader = 0;


    /* PER_PIXEL CONSTANTS BEGIN */
    this->presetInputs.x_per_pixel = 0;
    this->presetInputs.y_per_pixel = 0;
    this->presetInputs.rad_per_pixel = 0;
    this->presetInputs.ang_per_pixel = 0;

    /* PER_PIXEL CONSTANT END */


    /* Q VARIABLES START */

    this->presetOutputs.q1 = 0;
    this->presetOutputs.q2 = 0;
    this->presetOutputs.q3 = 0;
    this->presetOutputs.q4 = 0;
    this->presetOutputs.q5 = 0;
    this->presetOutputs.q6 = 0;
    this->presetOutputs.q7 = 0;
    this->presetOutputs.q8 = 0;


    /* Q VARIABLES END */

    /** Stash the current engine */
    currentEngine = this;
}

/** Resets OpenGL state */
DLLEXPORT void projectM::projectM_resetGL( int w, int h ) {


    int mindim, origtexsize;

    DWRITE( "projectM_resetGL(): in: %d x %d\n", w, h );

    /** Stash the new dimensions */



    renderer->reset(w,h);
}

/** Sets the title to display */
DLLEXPORT void projectM::projectM_setTitle( char *title ) {
    /*
    if (strcmp(this->title, title)!=0)
     {printf("new title\n");
        this->drawtitle=1; 
      
        if ( this->title != NULL ) {
    free( this->title );
    this->title = NULL;
        }
      
        this->title = (char *)wipemalloc( sizeof( char ) * ( strlen( title ) + 1 ) );
        strcpy( this->title, title );
      
      }
    */
}


int projectM::initPresetTools() {

    /* Initializes the builtin function database */
    BuiltinFuncs::init_builtin_func_db();

    /* Initializes all infix operators */
    Eval::init_infix_ops();

    /* Set the seed to the current time in seconds */
#ifdef WIN32
    srand(time(NULL));
#endif

    /* Initialize the 'idle' preset */
    //Preset::init_idle_preset();

    projectM_resetengine();

    if (m_presetLoader = new PresetLoader(PROJECTM_PRESET_PATH)) {
        m_presetLoader = 0;
        return PROJECTM_FAILURE;
    }

    if (m_presetChooser = new PresetChooser(*m_presetLoader)) {
        delete(m_presetLoader);
        m_presetChooser = 0;
        return PROJECTM_FAILURE;
    }

// Start the iterator
    m_presetPos = new PresetIterator();
    *m_presetPos = m_presetChooser->begin();

    /* Done */
#ifdef PRESET_DEBUG
    printf("initPresetLoader: finished\n");
#endif
    return PROJECTM_SUCCESS;
}

void projectM::destroyPresetTools() {

    if (m_presetChooser)
        delete(m_presetChooser);

    if (m_presetLoader)
        delete(m_presetLoader);

    Eval::destroy_infix_ops();
    BuiltinFuncs::destroy_builtin_func_db();

}
