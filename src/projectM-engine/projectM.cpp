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
#include "pbuffer.h"

#ifdef USE_FTGL
#include <FTGL/FTGL.h>
#include <FTGL/FTGLPixmapFont.h>
#include <FTGL/FTGLPolygonFont.h>
#endif /** USE_FTGL */

#include "wipemalloc.h"
#include "BuiltinFuncs.hpp"
#include "fatal.h"
#include "common.h"
#include "compare.h"
#ifdef WIN32
#include "win32-dirent.h"
#endif

#include "timer.h"

#ifdef LINUX
#include "time.h"
#endif

//#include <xmms/plugin.h>
#include "projectM.h"
#include "BeatDetect.h"
#include "Eval.h"
#include "Param.h"
#include "Parser.h"
#include "Preset.hpp"
#include "PerPixelEqn.h"
#include "console_interface.h"
#include "menu.h"
#include "PCM.h"                    //Sound data handler (buffering, FFT, etc.)
#include "CustomWave.h"
#include "CustomShape.h"
#include "SplayTree.h"

#ifdef USE_FTGL
FTGLPixmapFont *title_font;
FTGLPixmapFont *other_font;
FTGLPolygonFont *poly_font;
#endif /** USE_FTGL */



/** Stash current engine */    
projectM *projectM::currentEngine = NULL;

/** Constructor */
DLLEXPORT projectM::projectM() {
    beatDetect = NULL;
}

/** Renders a single frame */
DLLEXPORT void projectM::renderFrame() { 

#ifdef DEBUG
char fname[1024];
FILE *f = NULL;
int index = 0;
int x, y;
#endif 
  
//     printf("Start of loop at %d\n",timestart);

      mspf=(int)(1000.0/(float)presetInputs.fps); //milliseconds per frame
      totalframes++; //total amount of frames since startup

#ifndef WIN32
      Time = getTicks( &startTime ) * 0.001;
#else
        Time = getTicks( startTime ) * 0.001;
#endif /** !WIN32 */
      
      presetInputs.frame++;  //number of frames for current preset
      presetInputs.progress= presetInputs.frame/(float)avgtime;
    DWRITE( "frame: %d\ttime: %f\tprogress: %f\tavgtime: %d\tang: %f\trot: %f\n",
             this->presetInputs.frame, Time, this->presetInputs.progress, this->avgtime, this->presetInputs.ang_per_pixel,
             this->presetOutputs.rot );
      if (presetInputs.progress>1.0) presetInputs.progress=1.0;

//       printf("start:%d at:%d min:%d stop:%d on:%d %d\n",startframe, frame frame-startframe,avgtime,  noSwitch,progress);
//      this->activePreset->evalInitConditions();
      this->activePreset->evalPerFrameEquations();

//      this->activePreset->evalCustomWaveInitConditions();
//      this->activePreset->evalCustomShapeInitConditions();
 
//     printf("%f %d\n",Time,frame);
 
      reset_per_pixel_matrices();

        beatDetect->detectFromSamples();
        DWRITE( "=== vol: %f\tbass: %f\tmid: %f\ttreb: %f ===\n",
                 beatDetect->vol,beatDetect->bass,beatDetect->mid,beatDetect->treb);
        DWRITE( "=== bass_att: %f ===\n",
                 beatDetect->bass_att );

      if (noSwitch==0) {
          nohard--;
          if((beatDetect->bass-beatDetect->bass_old>beatDetect->beat_sensitivity ||
             avgtime ) && nohard<0)
          { 
//              printf("%f %d %d\n", beatDetect->bass-beatDetect->bass_old,this->frame,this->avgtime);
//              switchPreset(RANDOM_NEXT, HARD_CUT);
              nohard=presetInputs.fps*5;
          }
      }

      count++;
      
    DWRITE( "start Pass 1 \n" );

      //BEGIN PASS 1
      //
      //This pass is used to render our texture
      //the texture is drawn to a subsection of the framebuffer
      //and then we perform our manipulations on it
      //in pass 2 we will copy the texture into texture memory

     
    lockPBuffer( renderTarget, PBUFFER_PASS1 );
      
      
      //   glPushAttrib( GL_ALL_ATTRIB_BITS ); /* Overkill, but safe */
      
      glViewport( 0, 0, renderTarget->texsize, renderTarget->texsize );
       
      glEnable( GL_TEXTURE_2D );
      if(this->renderTarget->usePbuffers)
	{
	  glBindTexture( GL_TEXTURE_2D, renderTarget->textureID[1] );
	}
      else
	{
	  glBindTexture( GL_TEXTURE_2D, renderTarget->textureID[0] );
	}
      glMatrixMode(GL_TEXTURE);  
      glLoadIdentity();
      
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_DECAL);
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
      
      glMatrixMode( GL_PROJECTION );
      glPushMatrix();
      glLoadIdentity();  
      glOrtho(0.0, 1, 0.0, 1,10,40);
      
      glMatrixMode( GL_MODELVIEW );
      glPushMatrix();
      glLoadIdentity();
      
    DWRITE( "renderFrame: renderTarget->texsize: %d x %d\n", this->renderTarget->texsize, this->renderTarget->texsize );
    
    if ( doPerPixelEffects ) {
        do_per_pixel_math();
    }


    if(this->renderTarget->usePbuffers)
      {
    	//draw_motion_vectors();        //draw motion vectors
    	//unlockPBuffer( this->renderTarget);
    	//lockPBuffer( this->renderTarget, PBUFFER_PASS1 );
      }
    do_per_frame();               //apply per-frame effects
    render_interpolation();       //apply per-pixel effects
   
    draw_title_to_texture();      //draw title to texture

//    if(!this->renderTarget->usePbuffers)
      {
	draw_motion_vectors();        //draw motion vectors
      }
    draw_shapes();
    draw_custom_waves();
    draw_waveform();
    if(this->presetOutputs.bDarkenCenter)darken_center();
    draw_borders();               //draw borders

    /** Restore original view state */
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    /** Restore all original attributes */
    //  glPopAttrib();
    glFlush();

        unlockPBuffer( this->renderTarget );


#ifdef DEBUG
    GLint msd = 0,
        psd = 0;
    glGetIntegerv( GL_MODELVIEW_STACK_DEPTH, &msd );
    glGetIntegerv( GL_PROJECTION_STACK_DEPTH, &psd );
    DWRITE( "end pass1: modelview matrix depth: %d\tprojection matrix depth: %d\n",
            msd, psd );
    DWRITE( "begin pass2\n" );
#endif

      //BEGIN PASS 2
      //
      //end of texture rendering
      //now we copy the texture from the framebuffer to
      //video texture memory and render fullscreen on a quad surface.

    /** Reset the viewport size */
    DWRITE( "viewport: %d x %d\n", this->vw, this->vh );
    glViewport( 0, 0, this->vw, this->vh );
    glClear( GL_COLOR_BUFFER_BIT );

    if ( this->renderTarget ) {
        glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID[0] );
      }

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();  
      glOrtho(-0.5, 0.5, -0.5,0.5,10,40);
     	
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

      glLineWidth( this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512.0);
      if(this->studio%2)render_texture_to_studio();
      else render_texture_to_screen();

      // glClear(GL_COLOR_BUFFER_BIT);     
      //render_Studio();

      //preset editing menu
      glMatrixMode(GL_MODELVIEW);
      glTranslated(-0.5,-0.5,-1);  

      refreshConsole();
      draw_title_to_screen();
      if(this->showhelp%2)draw_help();
      if(this->showtitle%2)draw_title();
      if(this->showfps%2)draw_fps(this->realfps);
      if(this->showpreset%2)draw_preset();
      if(this->showstats%2)draw_stats();
      glTranslatef(0.5 ,0.5,1);

    DWRITE( "end pass2\n" );

#ifndef WIN32
      /** Frame-rate limiter */
      /** Compute once per preset */
      if (this->count%100==0) {
	  this->realfps=100.0/((getTicks(&this->startTime)-this->fpsstart)/1000);
	  this->fpsstart=getTicks(&this->startTime);      	          
      	}

      int timediff = getTicks(&this->startTime)-this->timestart;

      if ( timediff < this->mspf) 
	{	 
	  // printf("%s:",this->mspf-timediff);
	  int sleepTime = (unsigned int)( this->mspf-timediff ) * 1000;
        DWRITE( "usleep: %d\n", sleepTime );
	  if ( sleepTime > 0 && sleepTime < 100000 ) {
	    if ( usleep( sleepTime ) != 0 ) 
	      {	      
	      }	  
	   }
	}
       this->timestart=getTicks(&this->startTime);
#endif /** !WIN32 */

    DWRITE( "exiting renderFrame()\n" );
}

DLLEXPORT void projectM::projectM_reset() {

    DWRITE( "projectM_reset(): in\n" );
    this->activePreset = NULL;
 
    this->presetURL = NULL;
    this->fontURL = NULL;

    /** Default variable settings */
    this->hasInit = 0;

    this->noSwitch = 0;
    this->pcmframes = 1;
    this->freqframes = 0;
    this->totalframes = 1;

    this->showfps = 0;
    this->showtitle = 0;
    this->showpreset = 0;
    this->showhelp = 0;
    this->showstats = 0;
    this->studio = 0;

    /** Allocate a new render target */
#ifdef PANTS
    if ( this->renderTarget ) {
        if ( this->renderTarget->renderTarget ) {
            /** Free existing */
            free( this->renderTarget->renderTarget );
	    this->renderTarget->renderTarget = NULL;
          }
        free( this->renderTarget );
        this->renderTarget = NULL;
      }
#endif
    this->renderTarget = (RenderTarget *)wipemalloc( sizeof( RenderTarget ) );        
    this->renderTarget->usePbuffers = 1;

#ifdef MACOS
    this->renderTarget->origContext = NULL;
    this->renderTarget->pbufferContext = NULL;
    this->renderTarget->pbuffer = NULL;
#endif

    /** Configurable engine variables */
    this->renderTarget->texsize = 1024;
    this->fvw = 800;
    this->fvh = 600;
    this->wvw = 512;
    this->wvh = 512;
    this->fullscreen = 0;

    /** Configurable mesh size */
    this->presetInputs.gx = 48;
    this->presetInputs.gy = 36;

    /** Frames per preset */
    this->avgtime = 500;

    this->title = NULL;    

    /** Other stuff... */
    this->correction = 1;
    this->aspect=1.33333333;
   

    /** Per pixel equation variables */
    this->gridx = NULL;
    this->gridy = NULL;
    this->origtheta = NULL;
    this->origrad = NULL;
    this->origx = NULL;
    this->origy = NULL;

    /** More other stuff */
    this->mspf = 0;
    this->timed = 0;
    this->timestart = 0;   
    this->nohard = 0;
    this->count = 0;
    this->realfps = 0;
    this->fpsstart = 0;

    projectM_resetengine();
  }

DLLEXPORT void projectM::projectM_init() {

#ifdef USE_FTGL
    /** Reset fonts */
    title_font = NULL;
    other_font = NULL;
    poly_font = NULL;
#endif /** USE_FTGL */

    /** Initialise engine variables */
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
    init_per_pixel_matrices();

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

printf( "pre init_display()\n" );

printf( "post init_display()\n" );

    mspf=(int)(1000.0/(float)presetInputs.fps);


   
  //create off-screen pbuffer (or not if unsupported)
//  CreateRenderTarget(this->renderTarget->texsize, &this->textureID, &this->renderTarget);
printf( "post CreaterenderTarget\n" );
  
    
//fps = 0;

    initMenu();  
//DWRITE( "post initMenu()\n" );

    printf("mesh: %d %d\n", this->presetInputs.gx,this->presetInputs.gy );

#ifdef PANTS
    printf( "maxsamples: %d\n", this->maxsamples );
  initPCM(this->maxsamples);
DWRITE( "post PCM init\n" );
#endif

 this->avgtime=this->presetInputs.fps*20;

    this->hasInit = 1;


  createPBuffers( this->renderTarget->texsize, this->renderTarget->texsize , this->renderTarget );

printf( "exiting projectM_init()\n" );
}

void projectM::free_per_pixel_matrices() {

  int x;

 for(x = 0; x < this->presetInputs.gx; x++)
    {
      
      free(this->gridx[x]);
      free(this->gridy[x]); 
      free(this->origtheta[x]);
      free(this->origrad[x]);
      free(this->origx[x]);
      free(this->origy[x]);
      free(this->origx2[x]);
      free(this->origy2[x]);
      free(this->presetInputs.x_mesh[x]);
      free(this->presetInputs.y_mesh[x]);
      free(this->presetInputs.rad_mesh[x]);
      free(this->presetInputs.theta_mesh[x]);
      
    }

  
  free(this->origx);
  free(this->origy);
  free(this->origx2);
  free(this->origy2);
  free(this->gridx);
  free(this->gridy);
  free(this->presetInputs.x_mesh);
  free(this->presetInputs.y_mesh);
  free(this->presetInputs.rad_mesh);
  free(this->presetInputs.theta_mesh);

  this->origx = NULL;
  this->origy = NULL;
  this->origx2 = NULL;
  this->origy2 = NULL;
  this->gridx = NULL;
  this->gridy = NULL;
  this->presetInputs.x_mesh = NULL;
  this->presetInputs.y_mesh = NULL;
  this->presetInputs.rad_mesh = NULL;
  this->presetInputs.theta_mesh = NULL;
}


void projectM::init_per_pixel_matrices() {

  int x,y; 
  
  this->gridx=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
   for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->gridx[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->gridy=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->gridy[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float)); 
    }
  this->origtheta=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->origtheta[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->origrad=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
     for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->origrad[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->origx=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->origx[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->origy=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->origy[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->origx2=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->origx2[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }  
this->origy2=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->origy2[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetInputs.x_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetInputs.x_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetInputs.y_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetInputs.y_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    
    }
  this->presetInputs.rad_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetInputs.rad_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetInputs.theta_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetInputs.theta_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetOutputs.sx_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetOutputs.sx_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetOutputs.sy_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetOutputs.sy_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetOutputs.dx_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetOutputs.dx_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetOutputs.dy_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetOutputs.dy_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetOutputs.cx_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetOutputs.cx_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetOutputs.cy_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetOutputs.cy_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetOutputs.zoom_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetOutputs.zoom_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetOutputs.zoomexp_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    {
      this->presetOutputs.zoomexp_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }
  this->presetOutputs.rot_mesh=(float **)wipemalloc(this->presetInputs.gx * sizeof(float *));
 for(x = 0; x < this->presetInputs.gx; x++)
    { 
      this->presetOutputs.rot_mesh[x] = (float *)wipemalloc(this->presetInputs.gy * sizeof(float));
    }



  //initialize reference grid values
  for (x=0;x<this->presetInputs.gx;x++)
    {
      for(y=0;y<this->presetInputs.gy;y++)
	{
	   this->origx[x][y]=x/(float)(this->presetInputs.gx-1);
	   this->origy[x][y]=-((y/(float)(this->presetInputs.gy-1))-1);
	   this->origrad[x][y]=hypot((this->origx[x][y]-.5)*2,(this->origy[x][y]-.5)*2) * .7071067;
  	   this->origtheta[x][y]=atan2(((this->origy[x][y]-.5)*2),((this->origx[x][y]-.5)*2));
	   this->gridx[x][y]=this->origx[x][y]*this->renderTarget->texsize;
	   this->gridy[x][y]=this->origy[x][y]*this->renderTarget->texsize;
	   this->origx2[x][y]=( this->origx[x][y]-.5)*2;
	   this->origy2[x][y]=( this->origy[x][y]-.5)*2;
	}}
}



//calculate matrices for per_pixel
void projectM::do_per_pixel_math() {

  int x,y;
  float fZoom2,fZoom2Inv;

  this->activePreset->evalPerPixelEqns();

  if(!this->activePreset->isPerPixelEqn(CX_OP))
       { 
      for (x=0;x<this->presetInputs.gx;x++){
       
	for(y=0;y<this->presetInputs.gy;y++){
	  this->presetOutputs.cx_mesh[x][y]=this->presetOutputs.cx;
	}
	
      }
    }

  if(!this->activePreset->isPerPixelEqn(CY_OP))
        { 
      for (x=0;x<this->presetInputs.gx;x++){
	for(y=0;y<this->presetInputs.gy;y++){
	  this->presetOutputs.cy_mesh[x][y]=this->presetOutputs.cy;
	}}
    }
  
  if(!this->activePreset->isPerPixelEqn(SX_OP))
    { 
      for (x=0;x<this->presetInputs.gx;x++){
	for(y=0;y<this->presetInputs.gy;y++){
	  this->presetOutputs.sx_mesh[x][y]=this->presetOutputs.sx;
	}}
    }
  
  if(!this->activePreset->isPerPixelEqn(SY_OP))
    { 
      for (x=0;x<this->presetInputs.gx;x++){
	for(y=0;y<this->presetInputs.gy;y++){
	  this->presetOutputs.sy_mesh[x][y]=this->presetOutputs.sy;
	}}
    }

  if(!this->activePreset->isPerPixelEqn(ZOOM_OP))
    {       
      for (x=0;x<this->presetInputs.gx;x++){
	for(y=0;y<this->presetInputs.gy;y++){
	  this->presetOutputs.zoom_mesh[x][y]=this->presetOutputs.zoom;
	}}
    }
 
  if(!this->activePreset->isPerPixelEqn(ZOOMEXP_OP))
    {
      for (x=0;x<this->presetInputs.gx;x++){
	for(y=0;y<this->presetInputs.gy;y++){
	  this->presetOutputs.zoomexp_mesh[x][y]=this->presetOutputs.zoomexp;
	}}
    }

  if(!this->activePreset->isPerPixelEqn(ROT_OP))
    {       
      for (x=0;x<this->presetInputs.gx;x++){
	for(y=0;y<this->presetInputs.gy;y++){
	  this->presetOutputs.rot_mesh[x][y]=this->presetOutputs.rot;
	}
      }
    }

  /*
  for (x=0;x<this->presetInputs.gx;x++){
    for(y=0;y<this->presetInputs.gy;y++){	  	  
      this->presetInputs.x_mesh[x][y]=(this->presetInputs.x_mesh[x][y]-.5)*2; 
    }
  }
 
  for (x=0;x<this->presetInputs.gx;x++){
    for(y=0;y<this->presetInputs.gy;y++){	  	  
      this->presetInputs.y_mesh[x][y]=(this->presetInputs.y_mesh[x][y]-.5)*2; 
    }
  }
  */

  for (x=0;x<this->presetInputs.gx;x++){
    for(y=0;y<this->presetInputs.gy;y++){
      fZoom2 = powf( this->presetOutputs.zoom_mesh[x][y], powf( this->presetOutputs.zoomexp_mesh[x][y], this->presetInputs.rad_mesh[x][y]*2.0f - 1.0f));
      fZoom2Inv = 1.0f/fZoom2;
      this->presetInputs.x_mesh[x][y]= this->origx2[x][y]*0.5f*fZoom2Inv + 0.5f;
      this->presetInputs.y_mesh[x][y]= this->origy2[x][y]*0.5f*fZoom2Inv + 0.5f;
    }
  }
	
  for (x=0;x<this->presetInputs.gx;x++){
    for(y=0;y<this->presetInputs.gy;y++){
      this->presetInputs.x_mesh[x][y]  = ( this->presetInputs.x_mesh[x][y] - this->presetOutputs.cx_mesh[x][y])/this->presetOutputs.sx_mesh[x][y] + this->presetOutputs.cx_mesh[x][y];
    }
  }
  
  for (x=0;x<this->presetInputs.gx;x++){
    for(y=0;y<this->presetInputs.gy;y++){
      this->presetInputs.y_mesh[x][y] = ( this->presetInputs.y_mesh[x][y] - this->presetOutputs.cy_mesh[x][y])/this->presetOutputs.sy_mesh[x][y] + this->presetOutputs.cy_mesh[x][y];
    }
  }	   
	 

 for (x=0;x<this->presetInputs.gx;x++){
   for(y=0;y<this->presetInputs.gy;y++){
     float u2 = this->presetInputs.x_mesh[x][y] - this->presetOutputs.cx_mesh[x][y];
     float v2 = this->presetInputs.y_mesh[x][y] - this->presetOutputs.cy_mesh[x][y];
     
     float cos_rot = cosf(this->presetOutputs.rot_mesh[x][y]);
     float sin_rot = sinf(this->presetOutputs.rot_mesh[x][y]);
     
     this->presetInputs.x_mesh[x][y] = u2*cos_rot - v2*sin_rot + this->presetOutputs.cx_mesh[x][y];
     this->presetInputs.y_mesh[x][y] = u2*sin_rot + v2*cos_rot + this->presetOutputs.cy_mesh[x][y];

  }
 }	  

 if(this->activePreset->isPerPixelEqn(DX_OP))
   {
     for (x=0;x<this->presetInputs.gx;x++){
       for(y=0;y<this->presetInputs.gy;y++){	      
	 this->presetInputs.x_mesh[x][y] -= this->presetOutputs.dx_mesh[x][y];
       }
     }
   }
 
 if(this->activePreset->isPerPixelEqn(DY_OP))
   {
     for (x=0;x<this->presetInputs.gx;x++){
       for(y=0;y<this->presetInputs.gy;y++){	      
	 this->presetInputs.y_mesh[x][y] -= this->presetOutputs.dy_mesh[x][y];
       }
     }
		  	
   }

}

void projectM::reset_per_pixel_matrices() {

  int x,y;
  /*
  for (x=0;x<this->presetInputs.gx;x++)
    {
      memcpy(this->presetInputs.x_mesh[x],this->origx[x],sizeof(float)*this->presetInputs.gy);
    }
  for (x=0;x<this->presetInputs.gx;x++)
    {
      memcpy(this->presetInputs.y_mesh[x],this->origy[x],sizeof(float)*this->presetInputs.gy);
    }
  for (x=0;x<this->presetInputs.gx;x++)
    {
      memcpy(this->presetInputs.rad_mesh[x],this->origrad[x],sizeof(float)*this->presetInputs.gy);
    }
  for (x=0;x<this->presetInputs.gx;x++)
    {
      memcpy(this->presetInputs.theta_mesh[x],this->origtheta[x],sizeof(float)*this->presetInputs.gy);
    }
  */
  
    for (x=0;x<this->presetInputs.gx;x++)
    {
      for(y=0;y<this->presetInputs.gy;y++)
	{   
          this->presetInputs.x_mesh[x][y]=this->origx[x][y];
	  this->presetInputs.y_mesh[x][y]=this->origy[x][y];
	  this->presetInputs.rad_mesh[x][y]=this->origrad[x][y];
	  this->presetInputs.theta_mesh[x][y]=this->origtheta[x][y];	  
	}
    }
  
  //memcpy(this->presetInputs.x_mesh,this->origx,sizeof(float)*this->presetInputs.gy*this->presetInputs.gx);
  //memcpy(this->presetInputs.y_mesh,this->origy,sizeof(float)*this->presetInputs.gy*this->presetInputs.gx);
  //memcpy(this->presetInputs.rad_mesh,this->origrad,sizeof(float)*this->presetInputs.gy*this->presetInputs.gx);
  //memcpy(this->presetInputs.theta_mesh,this->origtheta,sizeof(float)*this->presetInputs.gy*this->presetInputs.gx);
 }

void projectM::rescale_per_pixel_matrices() {

    int x, y;

    for ( x = 0 ; x < this->presetInputs.gx ; x++ ) {
        for ( y = 0 ; y < this->presetInputs.gy ; y++ ) {
            this->gridx[x][y]=this->origx[x][y];
            this->gridy[x][y]=this->origy[x][y];

          }
      }
  }

void projectM::draw_custom_waves() {

  int x;
  CustomWave *wavecode;

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glTranslatef( 0, 0, -1 );

  glPointSize(this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512); 
 
  while ((wavecode = this->activePreset->nextCustomWave()) != NULL)
    {
     
      if(wavecode->enabled==1)
	{
	
	  if (wavecode->bAdditive==0)  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	  else    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
	  if (wavecode->bDrawThick==1)  glLineWidth(this->renderTarget->texsize < 512 ? 1 : 2*this->renderTarget->texsize/512);
	  
	   beatDetect->pcm->getPCM(wavecode->value1,wavecode->samples,0,wavecode->bSpectrum,wavecode->smoothing,0);
	   beatDetect->pcm->getPCM(wavecode->value2,wavecode->samples,1,wavecode->bSpectrum,wavecode->smoothing,0);
	  // printf("%f\n",pcmL[0]);


	  float mult=wavecode->scaling*this->presetOutputs.fWaveScale*(wavecode->bSpectrum ? 0.015f :1.0f);

	  for(x=0;x<wavecode->samples;x++)
	    {wavecode->value1[x]*=mult;}
	  
	  for(x=0;x<wavecode->samples;x++)
	    {wavecode->value2[x]*=mult;}

	   for(x=0;x<wavecode->samples;x++)
	     {wavecode->sample_mesh[x]=((float)x)/((float)(wavecode->samples-1));}
	  
	  // printf("mid inner loop\n");  
	  wavecode->evalPerPointEqns();
	
	  //put drawing code here
	  if (wavecode->bUseDots==1)   glBegin(GL_POINTS);
	  else   glBegin(GL_LINE_STRIP);
	  
	  for(x=0;x<wavecode->samples;x++)
	    {
	     
	      glColor4f(wavecode->r_mesh[x],wavecode->g_mesh[x],wavecode->b_mesh[x],wavecode->a_mesh[x]);
	      glVertex3f(wavecode->x_mesh[x],-(wavecode->y_mesh[x]-1),-1);
	    }
	  glEnd();
	  glPointSize(this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512); 
	  glLineWidth(this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512); 
	  glDisable(GL_LINE_STIPPLE); 
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	  //  glPopMatrix();
	  
	}
      
    }

    glPopMatrix();
}

void projectM::darken_center() {

  float unit=0.05f;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix(); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  glTranslatef(0.5,0.5, 0);

  glBegin(GL_TRIANGLE_FAN);
  glColor4f(0,0,0,3.0f/32.0f);
  glVertex3f(0,0,-1);
  glColor4f(0,0,0,-1);
  glVertex3f(-unit,0,-1);
  glVertex3f(0,-unit,-1);
  glVertex3f(unit,0,-1);
  glVertex3f(0,unit,-1);
  glVertex3f(-unit,0,-1);
  glEnd();

  glPopMatrix();
}

void projectM::draw_shapes() { 

  int i;

  float theta;
  float radius;

  CustomShape *shapecode;
 
  float pi = 3.14159265;
  float start,inc,xval,yval;
  
  float t;

  //  more=isMoreCustomWave();
  // printf("not inner loop\n");

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glTranslatef( 0, 0, -1 );
  
  while ((shapecode = this->activePreset->nextCustomShape()) != NULL)
    {

      if(shapecode->enabled==1)
	{
	  // printf("drawing shape %f\n",shapecode->ang);
	  shapecode->y=-((shapecode->y)-1);
	  radius=.5;
	  shapecode->radius=shapecode->radius*(.707*.707*.707*1.04);
	  //Additive Drawing or Overwrite
	  if (shapecode->additive==0)  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	  else    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
	  
	  glMatrixMode(GL_MODELVIEW);
	  	   glPushMatrix(); 
		   /* DEPRECATED
		   if(this->correction)
		     {		     
		       glTranslatef(0.5,0.5, 0);
		       glScalef(1.0,this->vw/(float)this->vh,1.0);  
		       glTranslatef(-0.5 ,-0.5,0);   
		     }
		   */
	
	   xval=shapecode->x;
	   yval=shapecode->y;
	  
	  if (shapecode->textured)
	    {
	      glMatrixMode(GL_TEXTURE);
	       glPushMatrix();
	      glLoadIdentity();
	      //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	      //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	      //glTranslatef(.5,.5, 0);
	      //if (this->correction) glScalef(1,this->vw/(float)this->vh,1);
   
	      //glRotatef((shapecode->tex_ang*360/6.280), 0, 0, 1);
	      
	      //glScalef(1/(shapecode->tex_zoom),1/(shapecode->tex_zoom),1); 
	      
	      // glScalef(1,vh/(float)vw,1);
	      //glTranslatef((-.5) ,(-.5),0);  
	      // glScalef(1,this->vw/(float)this->vh,1);
	      glEnable(GL_TEXTURE_2D);
	      	     

	      glBegin(GL_TRIANGLE_FAN);
	      glColor4f(0.0,0.0,0.0,shapecode->a);
	      //glColor4f(shapecode->r,shapecode->g,shapecode->b,shapecode->a);
	   
	      glTexCoord2f(.5,.5);
	      glVertex3f(xval,yval,-1);	 
	      //glColor4f(shapecode->r2,shapecode->g2,shapecode->b2,shapecode->a2);  
	      glColor4f(0.0,0.0,0.0,shapecode->a2);

	      for ( i=1;i<shapecode->sides+2;i++)
		{
		 
		  //		  theta+=inc;
		  //  glColor4f(shapecode->r2,shapecode->g2,shapecode->b2,shapecode->a2);
		  //glTexCoord2f(radius*cos(theta)+.5 ,radius*sin(theta)+.5 );
		  //glVertex3f(shapecode->radius*cos(theta)+xval,shapecode->radius*sin(theta)+yval,-1);  
		  t = (i-1)/(float)shapecode->sides;
		  
		  glTexCoord2f(  0.5f + 0.5f*cosf(t*3.1415927f*2 + shapecode->tex_ang + 3.1415927f*0.25f)*(this->correction ? this->aspect : 1.0)/shapecode->tex_zoom, 0.5f + 0.5f*sinf(t*3.1415927f*2 + shapecode->tex_ang + 3.1415927f*0.25f)/shapecode->tex_zoom);
		   glVertex3f(shapecode->radius*cosf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)*(this->correction ? this->aspect : 1.0)+xval, shapecode->radius*sinf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)+yval,-1);   
		}	
	      glEnd();

	    
	      
	    
	      glDisable(GL_TEXTURE_2D);
	       glPopMatrix();
	      glMatrixMode(GL_MODELVIEW);          
	    }
	  else{//Untextured (use color values)
	    //printf("untextured %f %f %f @:%f,%f %f %f\n",shapecode->a2,shapecode->a,shapecode->border_a, shapecode->x,shapecode->y,shapecode->radius,shapecode->ang);
	    //draw first n-1 triangular pieces
	      glBegin(GL_TRIANGLE_FAN);
	      
	      glColor4f(shapecode->r,shapecode->g,shapecode->b,shapecode->a);
	    
	      // glTexCoord2f(.5,.5);
	      glVertex3f(xval,yval,-1);	 
	     glColor4f(shapecode->r2,shapecode->g2,shapecode->b2,shapecode->a2);

	      for ( i=1;i<shapecode->sides+2;i++)
		{
		  
		  //theta+=inc;
		  //  glColor4f(shapecode->r2,shapecode->g2,shapecode->b2,shapecode->a2);
		  //  glTexCoord2f(radius*cos(theta)+.5 ,radius*sin(theta)+.5 );
		  //glVertex3f(shapecode->radius*cos(theta)+xval,shapecode->radius*sin(theta)+yval,-1);	  

		  t = (i-1)/(float)shapecode->sides;
		   glVertex3f(shapecode->radius*cosf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)*(this->correction ? this->aspect : 1.0)+xval, shapecode->radius*sinf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)+yval,-1);   	  
		       
		}	
	      glEnd();

	   	 
	  }
	    if (this->presetOutputs.bWaveThick==1)  glLineWidth(this->renderTarget->texsize < 512 ? 1 : 2*this->renderTarget->texsize/512);
	      glBegin(GL_LINE_LOOP);
	      glColor4f(shapecode->border_r,shapecode->border_g,shapecode->border_b,shapecode->border_a);
	      for ( i=1;i<shapecode->sides+1;i++)
		{

		  t = (i-1)/(float)shapecode->sides;
		   glVertex3f(shapecode->radius*cosf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)*(this->correction ? this->aspect : 1.0)+xval, shapecode->radius*sinf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)+yval,-1); 	  
		 
		  //theta+=inc;
		  //glVertex3f(shapecode->radius*cos(theta)+xval,shapecode->radius*sin(theta)+yval,-1);
		}
	      glEnd();
	  if (this->presetOutputs.bWaveThick==1)  glLineWidth(this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512);
	  
	  glPopMatrix();
	}
    }

    glPopMatrix();
}


void projectM::draw_waveform() {

  int x;
  
  float r,theta;
 
  float offset,scale,dy2_adj;

  float co;  
  
  float wave_x_temp=0;
  float wave_y_temp=0;
  float dy_adj;
  float xx,yy; 

  float cos_rot;
  float sin_rot;    

    DWRITE( "draw_waveform: %d\n", this->presetOutputs.nWaveMode );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

  modulate_opacity_by_volume(); 
  maximize_colors();
  
  if(this->presetOutputs.bWaveDots==1) glEnable(GL_LINE_STIPPLE);
  
  offset=this->presetOutputs.wave_x-.5;
  scale=505.0/512.0;


  

  //Thick wave drawing
  if (this->presetOutputs.bWaveThick==1)  glLineWidth( (this->renderTarget->texsize < 512 ) ? 2 : 2*this->renderTarget->texsize/512);

  //Additive wave drawing (vice overwrite)
  if (this->presetOutputs.bAdditiveWaves==0)  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  else    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
 
      switch(this->presetOutputs.nWaveMode)
	{
	  
	case 8://monitor

	  glTranslatef(0.5,0.5, 0);
	  glRotated(-this->presetOutputs.wave_mystery*90,0,0,1);

	     glTranslatef(-0.5,-0.825, 0);
	     glTranslatef( 0, 0, -1 );
	     
	     /*
	     for (x=0;x<16;x++)
	       {
		 glBegin(GL_LINE_STRIP);
		 glColor4f(1.0-(x/15.0),.5,x/15.0,1.0);
		 glVertex3f((this->totalframes%256)*2*scale, -this->beat_val[x]*this->presetOutputs.fWaveScale+renderTarget->texsize*wave_y,-1);
		 glColor4f(.5,.5,.5,1.0);
		 glVertex3f((this->totalframes%256)*2*scale, this->renderTarget->texsize*this->presetOutputs.wave_y,-1);   
		 glColor4f(1.0,1.0,0,1.0);
		 //glVertex3f((this->totalframes%256)*scale*2, this->beat_val_att[x]*this->presetOutputs.fWaveScale+this->renderTarget->texsize*this->presetOutputs.wave_y,-1);
		 glEnd();
	       
		 glTranslatef(0,this->renderTarget->texsize*(1/36.0), 0);
	       }
	  */
	     
	    glTranslatef(0,(1/18.0), 0);

 
	     glBegin(GL_LINE_STRIP);
	     glColor4f(1.0,1.0,0.5,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, beatDetect->treb_att*5*this->presetOutputs.fWaveScale+this->presetOutputs.wave_y);
	     glColor4f(.2,.2,.2,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, this->presetOutputs.wave_y);   
	     glColor4f(1.0,1.0,0,1.0);
	     glVertex2f((this->totalframes%256)*scale*2, beatDetect->treb*-5*this->presetOutputs.fWaveScale+this->presetOutputs.wave_y);
	     glEnd();
	       
	       glTranslatef(0,.075, 0);
	     glBegin(GL_LINE_STRIP);
	     glColor4f(0,1.0,0.0,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, beatDetect->mid_att*5*this->presetOutputs.fWaveScale+this->presetOutputs.wave_y);
	     glColor4f(.2,.2,.2,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, this->presetOutputs.wave_y);   
	     glColor4f(.5,1.0,.5,1.0);
	     glVertex2f((this->totalframes%256)*scale*2, beatDetect->mid*-5*this->presetOutputs.fWaveScale+this->presetOutputs.wave_y);
	     glEnd(); 
	  
	   
	     glTranslatef(0,.075, 0);
	     glBegin(GL_LINE_STRIP);
	     glColor4f(1.0,0,0,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, beatDetect->bass_att*5*this->presetOutputs.fWaveScale+this->presetOutputs.wave_y);
	     glColor4f(.2,.2,.2,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, this->presetOutputs.wave_y);   
	     glColor4f(1.0,.5,.5,1.0);
	     glVertex2f((this->totalframes%256)*scale*2, beatDetect->bass*-5*this->presetOutputs.fWaveScale+this->presetOutputs.wave_y);
	     glEnd(); 
	     
	  break;
	  
	case 0://circular waveforms 
	  //  float co;
	  //	  glPushMatrix(); 
	  /*
	  if(this->correction)
	    {
	      glTranslatef(this->renderTarget->texsize*.5,this->renderTarget->texsize*.5, 0);
	      glScalef(1.0,this->vw/(float)this->vh,1.0);
	      glTranslatef((-this->renderTarget->texsize*.5) ,(-this->renderTarget->texsize*.5),0);   
	    }
	  */

    glTranslatef( 0, 0, -1 );

	  this->presetOutputs.wave_y=-1*(this->presetOutputs.wave_y-1.0);
 
	  glBegin(GL_LINE_STRIP);

    DWRITE( "nsamples: %d\n", beatDetect->pcm->numsamples );
	 
	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    { float inv_nverts_minus_one = 1.0f/(float)(beatDetect->pcm->numsamples);
	    //co= -(fabs(x-((beatDetect->pcm->numsamples*.5)-1))/beatDetect->pcm->numsamples)+1;
	      // printf("%d %f\n",x,co);
	      //theta=x*(6.28/beatDetect->pcm->numsamples);
	      //r= ((1+2*this->presetOutputs.wave_mystery)*(this->renderTarget->texsize/5.0)+
	      //  ( co*beatDetect->pcm->pcmdataL[x]+ (1-co)*beatDetect->pcm->pcmdataL[-(x-(beatDetect->pcm->numsamples-1))])
	      //  *25*this->presetOutputs.fWaveScale);
	      r=(0.5 + 0.4f*.12*beatDetect->pcm->pcmdataR[x]*this->presetOutputs.fWaveScale + this->presetOutputs.wave_mystery)*.5;
	      theta=(x)*inv_nverts_minus_one*6.28f + this->Time*0.2f;
	      /* 
	      if (x < 51)
		{
		  float mix = x/51.0;
		  mix = 0.5f - 0.5f*cosf(mix * 3.1416f);
		  float rad_2 = 0.5f + 0.4f*.12*beatDetect->pcm->pcmdataR[x]*this->presetOutputs.fWaveScale + this->presetOutputs.wave_mystery;
		  r = rad_2*(1.0f-mix) + r*(mix);
		}
	      */
  glVertex2f((r*cos(theta)*(this->correction ? this->aspect : 1.0)+this->presetOutputs.wave_x), (r*sin(theta)+this->presetOutputs.wave_y));
	     
	    }

	  //	  r= ( (1+2*this->presetOutputs.wave_mystery)*(this->renderTarget->texsize/5.0)+
	  //     (0.5*beatDetect->pcm->pcmdataL[0]+ 0.5*beatDetect->pcm->pcmdataL[beatDetect->pcm->numsamples-1])
	  //      *20*this->presetOutputs.fWaveScale);
      
	  //glVertex3f(r*cos(0)+(this->presetOutputs.wave_x*this->renderTarget->texsize),r*sin(0)+(this->presetOutputs.wave_y*this->renderTarget->texsize),-1);

	  glEnd();
	  /*
	  glBegin(GL_LINE_LOOP);
	  
	  for ( x=0;x<(512/pcmbreak);x++)
	    {
	      theta=(blockstart+x)*((6.28*pcmbreak)/512.0);
	      r= ((1+2*this->presetOutputs.wave_mystery)*(this->renderTarget->texsize/5.0)+fdata_buffer[fbuffer][0][blockstart+x]*.0025*this->presetOutputs.fWaveScale);
	      
	      glVertex3f(r*cos(theta)+(this->presetOutputs.wave_x*this->renderTarget->texsize),r*sin(theta)+(wave_y*this->renderTarget->texsize),-1);
	    }
	  glEnd();
	  */
	  //glPopMatrix();

	  break;	
	
	case 1://circularly moving waveform
	  //  float co;
	  
	  glTranslatef(.5,.5, 0);
	  glScalef(1.0,this->vw/(float)this->vh,1.0);
	  glTranslatef((-.5) ,(-.5),0);   
	  glTranslatef( 0, 0, -1 );

	  this->presetOutputs.wave_y=-1*(this->presetOutputs.wave_y-1.0);

	  glBegin(GL_LINE_STRIP);
	  //theta=(frame%512)*(6.28/512.0);

	  for ( x=1;x<(512-32);x++)
	    {
	      //co= -(abs(x-255)/512.0)+1;
	      // printf("%d %f\n",x,co);
	      //theta=((this->frame%256)*(2*6.28/512.0))+beatDetect->pcm->pcmdataL[x]*.2*this->presetOutputs.fWaveScale;
	      //r= ((1+2*this->presetOutputs.wave_mystery)*(this->renderTarget->texsize/5.0)+
	      //   (beatDetect->pcm->pcmdataL[x]-beatDetect->pcm->pcmdataL[x-1])*80*this->presetOutputs.fWaveScale);
	      theta=beatDetect->pcm->pcmdataL[x+32]*0.06*this->presetOutputs.fWaveScale * 1.57 + this->Time*2.3;
	      r=(0.53 + 0.43*beatDetect->pcm->pcmdataR[x]*0.12*this->presetOutputs.fWaveScale+ this->presetOutputs.wave_mystery)*.5;

	     
 glVertex2f((r*cos(theta)*(this->correction ? this->aspect : 1.0)+this->presetOutputs.wave_x),(r*sin(theta)+this->presetOutputs.wave_y));
	    }

	  glEnd(); 
	  /*
	  this->presetOutputs.wave_y=-1*(this->presetOutputs.wave_y-1.0);  
	  wave_x_temp=(this->presetOutputs.wave_x*.75)+.125;	
	  wave_x_temp=-(wave_x_temp-1); 

	  glBegin(GL_LINE_STRIP);

	    
	  
	  for (x=0; x<512-32; x++)
	    {
	      float rad = (.53 + 0.43*beatDetect->pcm->pcmdataR[x]) + this->presetOutputs.wave_mystery;
	      float ang = beatDetect->pcm->pcmdataL[x+32] * 1.57f + this->Time*2.3f;
	      glVertex3f((rad*cosf(ang)*.2*scale*this->presetOutputs.fWaveScale + wave_x_temp)*this->renderTarget->texsize,(rad*sinf(ang)*this->presetOutputs.fWaveScale*.2*scale + this->presetOutputs.wave_y)*this->renderTarget->texsize,-1);
	      
	    }
	  glEnd();
	  */

	  break;
	  
	case 2://EXPERIMENTAL

        glTranslatef( 0, 0, -1 );
	  this->presetOutputs.wave_y=-1*(this->presetOutputs.wave_y-1.0);  


	  glBegin(GL_LINE_STRIP);

	  for (x=0; x<512-32; x++)
	    {
	   
  glVertex2f((beatDetect->pcm->pcmdataR[x]*this->presetOutputs.fWaveScale*0.5*(this->correction ? this->aspect : 1.0) + this->presetOutputs.wave_x),( (beatDetect->pcm->pcmdataL[x+32]*this->presetOutputs.fWaveScale*0.5 + this->presetOutputs.wave_y)));
	    }
	  glEnd();	
	  
	  break;

	case 3://EXPERIMENTAL

	    glTranslatef( 0, 0, -9 );
	  this->presetOutputs.wave_y=-1*(this->presetOutputs.wave_y-1.0);  
	  //wave_x_temp=(this->presetOutputs.wave_x*.75)+.125;	
	  //wave_x_temp=-(wave_x_temp-1); 
	  
	 

	  glBegin(GL_LINE_STRIP);

	  for (x=0; x<512-32; x++)
	    {
	      
	      glVertex2f((beatDetect->pcm->pcmdataR[x] * this->presetOutputs.fWaveScale*0.5 + this->presetOutputs.wave_x),( (beatDetect->pcm->pcmdataL[x+32]*this->presetOutputs.fWaveScale*0.5 + this->presetOutputs.wave_y)));
 
	    }
	  glEnd();	
	  
	  break;

	case 4://single x-axis derivative waveform
	  {
	  this->presetOutputs.wave_y=-1*(this->presetOutputs.wave_y-1.0);	  
	  glTranslatef(.5,.5, 0);
	  glRotated(-this->presetOutputs.wave_mystery*90,0,0,1);
	  glTranslatef(-.5,-.5, 0);
        glTranslatef( 0, 0, -1 );
	 
	  float w1 = 0.45f + 0.5f*(this->presetOutputs.wave_mystery*0.5f + 0.5f);	       
	  float w2 = 1.0f - w1;
	  float xx[512],yy[512];
				
	  glBegin(GL_LINE_STRIP);
	  for (int i=0; i<512; i++)
	    {
	     xx[i] = -1.0f + 2.0f*(i/512.0) + this->presetOutputs.wave_x;
	     yy[i] =0.4* beatDetect->pcm->pcmdataL[i]*0.47f*this->presetOutputs.fWaveScale + this->presetOutputs.wave_y;
	     xx[i] += 0.4*beatDetect->pcm->pcmdataR[i]*0.44f*this->presetOutputs.fWaveScale;				      
	      
	      if (i>1)
		{
		  xx[i] = xx[i]*w2 + w1*(xx[i-1]*2.0f - xx[i-2]);
		  yy[i] = yy[i]*w2 + w1*(yy[i-1]*2.0f - yy[i-2]);
		}
	      glVertex2f(xx[i],yy[i]);
	    }

	  glEnd();

	  /*
	  this->presetOutputs.wave_x=(this->presetOutputs.wave_x*.75)+.125;	  
	  this->presetOutputs.wave_x=-(this->presetOutputs.wave_x-1); 
	  glBegin(GL_LINE_STRIP);
	 
	  for ( x=1;x<512;x++)
	    {
	      dy_adj=  beatDetect->pcm->pcmdataL[x]*20*this->presetOutputs.fWaveScale-beatDetect->pcm->pcmdataL[x-1]*20*this->presetOutputs.fWaveScale;
	      glVertex3f((x*(this->renderTarget->texsize/512))+dy_adj, beatDetect->pcm->pcmdataL[x]*20*this->presetOutputs.fWaveScale+this->renderTarget->texsize*this->presetOutputs.wave_x,-1);
	    }
	  glEnd(); 
	  */
	  }
	  break;

	case 5://EXPERIMENTAL
	  	  
        glTranslatef( 0, 0, -5 );	
	  
	  this->presetOutputs.wave_y=-1*(this->presetOutputs.wave_y-1.0);  
	 
	  cos_rot = cosf(this->Time*0.3f);
	  sin_rot = sinf(this->Time*0.3f);

	  glBegin(GL_LINE_STRIP);

	  for (x=0; x<512; x++)
	    {	      
	      float x0 = (beatDetect->pcm->pcmdataR[x]*beatDetect->pcm->pcmdataL[x+32] + beatDetect->pcm->pcmdataL[x+32]*beatDetect->pcm->pcmdataR[x]);
	      float y0 = (beatDetect->pcm->pcmdataR[x]*beatDetect->pcm->pcmdataR[x] - beatDetect->pcm->pcmdataL[x+32]*beatDetect->pcm->pcmdataL[x+32]);
	      
          glVertex2f(((x0*cos_rot - y0*sin_rot)*this->presetOutputs.fWaveScale*0.5*(this->correction ? this->aspect : 1.0) + this->presetOutputs.wave_x),( (x0*sin_rot + y0*cos_rot)*this->presetOutputs.fWaveScale*0.5 + this->presetOutputs.wave_y));
 
	    }
	  glEnd();	
	  
	 
	  break;

	case 6://single waveform


	  //glMatrixMode(GL_MODELVIEW);
	  //	  	  glLoadIdentity();
	  
	  glTranslatef(.5,.5, 0);
	  glRotated(-this->presetOutputs.wave_mystery*90,0,0,1);
	  glTranslatef(0,0, -1);
	  
	  wave_x_temp=-2*0.4142*(fabs(fabs(this->presetOutputs.wave_mystery)-.5)-.5);
	  glScalef(1.0+wave_x_temp,1.0,1.0);
	  glTranslatef(-.5,-.5, 0);
	  wave_x_temp=-1*(this->presetOutputs.wave_x-1.0);

	  glBegin(GL_LINE_STRIP);
	  //	  wave_x_temp=(wave_x*.75)+.125;	  
	  //	  wave_x_temp=-(wave_x_temp-1);
	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    {
     
	      //glVertex3f(x*scale, fdata_buffer[fbuffer][0][blockstart+x]*.0012*fWaveScale+renderTarget->texsize*wave_x_temp,-1);
	      glVertex2f(x/(float)beatDetect->pcm->numsamples, beatDetect->pcm->pcmdataR[x]*.04*this->presetOutputs.fWaveScale+wave_x_temp);

	      //glVertex3f(x*scale, renderTarget->texsize*wave_y_temp,-1);
	    }
	  //	  printf("%f %f\n",renderTarget->texsize*wave_y_temp,wave_y_temp);
	  glEnd(); 
	  break;
	  
	case 7://dual waveforms

	  glTranslatef(.5,.5, 0);
	  glRotated(-this->presetOutputs.wave_mystery*90,0,0,1);
	  
	  wave_x_temp=-2*0.4142*(fabs(fabs(this->presetOutputs.wave_mystery)-.5)-.5);
	  glScalef(1.0+wave_x_temp,1.0,1.0);
	     glTranslatef(-.5,-.5, -1);
    glTranslatef( 0, 0, -1 );

         wave_y_temp=-1*(this->presetOutputs.wave_x-1);

		  glBegin(GL_LINE_STRIP);
	 
	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    {
     
	      glVertex2f(x/(float)beatDetect->pcm->numsamples, beatDetect->pcm->pcmdataL[x]*.04*this->presetOutputs.fWaveScale+(wave_y_temp+(this->presetOutputs.wave_y*this->presetOutputs.wave_y*.5)));
	    }
	  glEnd(); 

	  glBegin(GL_LINE_STRIP);
	 

	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    {
     
	      glVertex2f(x/(float)beatDetect->pcm->numsamples, beatDetect->pcm->pcmdataR[x]*.04*this->presetOutputs.fWaveScale+(wave_y_temp-(this->presetOutputs.wave_y*this->presetOutputs.wave_y*.5)));
	    }
	  glEnd(); 
	  glPopMatrix();
     break;
     
	default:  
 glTranslatef( 0, 0, -1 );
 glBegin(GL_LINE_LOOP);
	  
	  for ( x=0;x<512;x++)
	    {
	      theta=(x)*(6.28/512.0);
	      r= (0.2+beatDetect->pcm->pcmdataL[x]*.002);
	      
	      glVertex2f(r*cos(theta)+this->presetOutputs.wave_x,r*sin(theta)+this->presetOutputs.wave_y);
	    }
	  glEnd();

glBegin(GL_LINE_STRIP);
	
	  for ( x=0;x<512;x++)
	    {
	      glVertex3f(x*scale, beatDetect->pcm->pcmdataL[x]*.04*this->presetOutputs.fWaveScale+((this->presetOutputs.wave_x+.1)),-1);
	    }
	  glEnd();
	  
	  glBegin(GL_LINE_STRIP);
	  
	 for ( x=0;x<512;x++)
	    {
	      glVertex3f(x*scale, beatDetect->pcm->pcmdataR[x]*.04*this->presetOutputs.fWaveScale+((this->presetOutputs.wave_x-.1)),-1);
	      
	    }
	  glEnd();
     break;
         if (this->presetOutputs.bWaveThick==1)  glLineWidth( (this->renderTarget->texsize < 512) ? 1 : 2*this->renderTarget->texsize/512); 
}	
      glLineWidth( this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512);
      glDisable(GL_LINE_STIPPLE);

    glPopMatrix();
}

void projectM::maximize_colors() {

 float wave_r_switch=0,wave_g_switch=0,wave_b_switch=0;
 //wave color brightening
      //
      //forces max color value to 1.0 and scales
      // the rest accordingly
 if(this->presetOutputs.nWaveMode==2 || this->presetOutputs.nWaveMode==5)
   {
	switch(this->renderTarget->texsize)
			{
			case 256:  this->presetOutputs.wave_o *= 0.07f; break;
			case 512:  this->presetOutputs.wave_o *= 0.09f; break;
			case 1024: this->presetOutputs.wave_o *= 0.11f; break;
			case 2048: this->presetOutputs.wave_o *= 0.13f; break;
			}
   }

 else if(this->presetOutputs.nWaveMode==3)
   {
	switch(this->renderTarget->texsize)
			{
			case 256:  this->presetOutputs.wave_o *= 0.075f; break;
			case 512:  this->presetOutputs.wave_o *= 0.15f; break;
			case 1024: this->presetOutputs.wave_o *= 0.22f; break;
			case 2048: this->presetOutputs.wave_o *= 0.33f; break;
			}
	this->presetOutputs.wave_o*=1.3f;
	this->presetOutputs.wave_o*=powf(beatDetect->treb ,2.0f);
   }

      if (this->presetOutputs.bMaximizeWaveColor==1)  
	{
	  if(this->presetOutputs.wave_r>=this->presetOutputs.wave_g && this->presetOutputs.wave_r>=this->presetOutputs.wave_b)   //red brightest
	    {
	      wave_b_switch=this->presetOutputs.wave_b*(1/this->presetOutputs.wave_r);
	      wave_g_switch=this->presetOutputs.wave_g*(1/this->presetOutputs.wave_r);
	      wave_r_switch=1.0;
	    }
	  else if   (this->presetOutputs.wave_b>=this->presetOutputs.wave_g && this->presetOutputs.wave_b>=this->presetOutputs.wave_r)         //blue brightest
	    {  
	      wave_r_switch=this->presetOutputs.wave_r*(1/this->presetOutputs.wave_b);
	      wave_g_switch=this->presetOutputs.wave_g*(1/this->presetOutputs.wave_b);
	      wave_b_switch=1.0;
	      
	    }	
	
	  else  if (this->presetOutputs.wave_g>=this->presetOutputs.wave_b && this->presetOutputs.wave_g>=this->presetOutputs.wave_r)         //green brightest
	    {
	      wave_b_switch=this->presetOutputs.wave_b*(1/this->presetOutputs.wave_g);
	      wave_r_switch=this->presetOutputs.wave_r*(1/this->presetOutputs.wave_g);
	      wave_g_switch=1.0;
	    }
 
	
	  glColor4f(wave_r_switch, wave_g_switch, wave_b_switch, this->presetOutputs.wave_o);
	}
      else
	{ 
	  glColor4f(this->presetOutputs.wave_r, this->presetOutputs.wave_g, this->presetOutputs.wave_b, this->presetOutputs.wave_o);
	}
      
}


void projectM::modulate_opacity_by_volume() {

 //modulate volume by opacity
      //
      //set an upper and lower bound and linearly
      //calculate the opacity from 0=lower to 1=upper
      //based on current volume


      if (this->presetOutputs.bModWaveAlphaByVolume==1)
	{if (beatDetect->vol<=this->presetOutputs.fModWaveAlphaStart)  this->presetOutputs.wave_o=0.0;       
	else if (beatDetect->vol>=this->presetOutputs.fModWaveAlphaEnd) this->presetOutputs.wave_o=this->presetOutputs.fWaveAlpha;
	else this->presetOutputs.wave_o=this->presetOutputs.fWaveAlpha*((beatDetect->vol-this->presetOutputs.fModWaveAlphaStart)/(this->presetOutputs.fModWaveAlphaEnd-this->presetOutputs.fModWaveAlphaStart));}
      else this->presetOutputs.wave_o=this->presetOutputs.fWaveAlpha;
}

void projectM::draw_motion_vectors() {

    int x,y;

    float offsetx=this->presetOutputs.mv_dx, intervalx=1.0/(float)this->presetOutputs.mv_x;
    float offsety=this->presetOutputs.mv_dy, intervaly=1.0/(float)this->presetOutputs.mv_y;
    

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    glPointSize(this->presetOutputs.mv_l);
    glColor4f(this->presetOutputs.mv_r, this->presetOutputs.mv_g, this->presetOutputs.mv_b, this->presetOutputs.mv_a);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef( 0, 0, -1 );

    glBegin(GL_POINTS);
    for (x=0;x<this->presetOutputs.mv_x;x++){
        for(y=0;y<this->presetOutputs.mv_y;y++){
            float lx, ly, lz;
            lx = offsetx+x*intervalx;
            ly = offsety+y*intervaly;
            lz = -1;
            glVertex2f(lx,ly);	  
          }
      }
  
    glEnd();

    glPopMatrix();    
}


void projectM::draw_borders() {

    //Draw Borders
    float of=this->presetOutputs.ob_size*.5;
    float iff=this->presetOutputs.ib_size*.5;
    float texof=1.0-of;

    //no additive drawing for borders
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  
    glColor4d(this->presetOutputs.ob_r,this->presetOutputs.ob_g,this->presetOutputs.ob_b,this->presetOutputs.ob_a);
  
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glTranslatef( 0, 0, -1 );

    glRectd(0,0,of,1);
    glRectd(of,0,texof,of);
    glRectd(texof,0,1,1);
    glRectd(of,1,texof,texof);
    glColor4d(this->presetOutputs.ib_r,this->presetOutputs.ib_g,this->presetOutputs.ib_b,this->presetOutputs.ib_a);
    glRectd(of,of,of+iff,texof);
    glRectd(of+iff,of,texof-iff,of+iff);
    glRectd(texof-iff,of,texof,texof);
    glRectd(of+iff,texof,texof-iff,texof-iff);

    glPopMatrix();
}

void projectM::draw_title_to_texture() {
  
#ifdef USE_FTGL 
    if (this->drawtitle>80) 
      //    if(1)
      {
      glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
      glColor4f(1.0,1.0,1.0,1.0);
      glPushMatrix();
     
      glTranslatef(0,0.5, -1);
    
      glScalef(0.0025,-0.0025,30*.0025);
      //glTranslatef(0,0, 1.0);
      poly_font->FaceSize( 22);
    
      glRasterPos2f(0.0, 0.0);

   if ( this->title != NULL ) {
      poly_font->Render(this->title );
      } else {
	poly_font->Render("Unknown" );
      }
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glPopMatrix();
      this->drawtitle=0;
    }
#endif /** USE_FTGL */
}

void projectM::draw_title_to_screen() {

#ifdef USE_FTGL
  if(this->drawtitle>0)
    { 
      float easein = ((80-this->drawtitle)*.0125);
      float easein2 = easein * easein;
      float easein3 = .0025/((-easein2)+1.0);

      glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
      glColor4f(1.0,1.0,1.0,1.0);
      glPushMatrix();


      //glTranslatef(this->vw*.5,this->vh*.5 , -1.0);
      glTranslatef(0,0.5 , -1.0);

      glScalef(easein3,easein3,30*.0025);

      glRotatef(easein2*360,1,0,0);


      //glTranslatef(-.5*this->vw,0, 0.0);
      
      //poly_font->Depth(1.0);  
      poly_font->FaceSize(22);

      glRasterPos2f(0.0, 0.0);
      if ( this->title != NULL ) {
	poly_font->Render(this->title );
      } else {
	poly_font->Render("Unknown" );
      }
      // poly_font->Depth(0.0);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glPopMatrix();	
      
      this->drawtitle++;

    }
#endif /** USE_FTGL */
}

void projectM::draw_title() {
#ifdef USE_FTGL
  //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

    glColor4f(1.0,1.0,1.0,1.0);
  //  glPushMatrix();
  //  glTranslatef(this->vw*.001,this->vh*.03, -1);
  //  glScalef(this->vw*.015,this->vh*.025,0);

      glRasterPos2f(0.01, 0.05);
      title_font->FaceSize( (unsigned)(20*(this->vh/512.0)));
       
      if ( this->title != NULL ) {
       	 title_font->Render(this->title );
      } else {
       	 title_font->Render("Unknown" );
      }
      //  glPopMatrix();
      //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
     
#endif /** USE_FTGL */
}
void projectM::draw_preset() { 
#ifdef USE_FTGL
  //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
    
  glColor4f(1.0,1.0,1.0,1.0);
      //      glPushMatrix();
      //glTranslatef(this->vw*.001,this->vh*-.01, -1);
      //glScalef(this->vw*.003,this->vh*.004,0);

   
        glRasterPos2f(0.01, 0.01);

	title_font->FaceSize((unsigned)(12*(this->vh/512.0)));
	if(this->noSwitch) title_font->Render("[LOCKED]  " );
	title_font->FaceSize((unsigned)(20*(this->vh/512.0)));
        title_font->Render(this->presetName );

                 
        
	//glPopMatrix();
	// glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
#endif /** USE_FTGL */    
}

void projectM::draw_help( ) { 

#ifdef USE_FTGL
//glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
  DWRITE("pre-help");
      glColor4f(1.0,1.0,1.0,1.0);
      glPushMatrix();
       glTranslatef(0,1, 0);
      //glScalef(this->vw*.02,this->vh*.02 ,0);

     
       title_font->FaceSize((unsigned)( 18*(this->vh/512.0)));

      glRasterPos2f(0.01, -0.05);
       title_font->Render("Help");  
      
      glRasterPos2f(0.01, -0.09);     
       title_font->Render("----------------------------");  
      
      glRasterPos2f(0.01, -0.13); 
       title_font->Render("F1: This help menu");
  
      glRasterPos2f(0.01, -0.17);
       title_font->Render("F2: Show song title");
      
      glRasterPos2f(0.01, -0.21);
       title_font->Render("F3: Show preset name");
 
       glRasterPos2f(0.01, -0.25);
       title_font->Render("F4: Show Rendering Settings");
 
      glRasterPos2f(0.01, -0.29);
       title_font->Render("F5: Show FPS");

      glRasterPos2f(0.01, -0.35);
       title_font->Render("F: Fullscreen");

      glRasterPos2f(0.01, -0.39);
       title_font->Render("L: Lock/Unlock Preset");

      glRasterPos2f(0.01, -0.43);
       title_font->Render("M: Show Menu");
      
      glRasterPos2f(0.01, -0.49);
       title_font->Render("R: Random preset");
      glRasterPos2f(0.01, -0.53);
       title_font->Render("N: Next preset");
 
      glRasterPos2f(0.01, -0.57);
       title_font->Render("P: Previous preset");

       glPopMatrix();
      //         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
       DWRITE("post-help");
#endif /** USE_FTGL */
}

void projectM::draw_stats() {

#ifdef USE_FTGL
 char buffer[128];  
  float offset= (this->showfps%2 ? -0.05 : 0.0);
  // glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
 
  glColor4f(1.0,1.0,1.0,1.0);
  glPushMatrix();
  glTranslatef(0.01,1, 0);
 glRasterPos2f(0, -.05+offset);  
  other_font->Render(this->correction ? "  aspect: corrected" : "  aspect: stretched");  
sprintf( buffer, " (%f)", this->aspect);
    other_font->Render(buffer);



  glRasterPos2f(0, -.09+offset);
  other_font->FaceSize((unsigned)(18*(this->vh/512.0)));

  sprintf( buffer, " texsize: %d", this->renderTarget->texsize);
  other_font->Render(buffer);

  glRasterPos2f(0, -.13+offset);
  sprintf( buffer, "viewport: %d x %d", this->vw, this->vh);
  other_font->Render(buffer);

  glRasterPos2f(0, -.17+offset);  
  other_font->Render((this->renderTarget->usePbuffers ? "     FBO: on" : "     FBO: off"));

  glRasterPos2f(0, -.21+offset); 
  sprintf( buffer, "    mesh: %d x %d", this->presetInputs.gx,this->presetInputs.gy);
  other_font->Render(buffer);


  glPopMatrix();
  // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
 
    
#endif /** USE_FTGL */    
}
void projectM::draw_fps( float realfps ) {
#ifdef USE_FTGL
  char bufferfps[20];  
  sprintf( bufferfps, "%.1f fps", realfps);
  // glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
 
  glColor4f(1.0,1.0,1.0,1.0);
  glPushMatrix();
  glTranslatef(0.01,1, 0);
  glRasterPos2f(0, -0.05);
  title_font->FaceSize((unsigned)(20*(this->vh/512.0)));
   title_font->Render(bufferfps);
  
  glPopMatrix();
  // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
#endif /** USE_FTGL */    
}
//Here we render the interpolated mesh, and then apply the texture to it.  
//Well, we actually do the inverse, but its all the same.
void projectM::render_interpolation() {
  
    int x,y;  
  
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
//    glLoadIdentity();
    glTranslated( 0, 0, -1 );
  
    glColor4f(0.0, 0.0, 0.0,this->presetOutputs.decay);
    
    glEnable(GL_TEXTURE_2D);
    //glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID[0] );
//    glLoadIdentity();
//  glTranslated(0, 0, -9);
#ifdef MACOS2
    /** Bind the stashed texture */
    if ( this->renderTarget->pbuffer != NULL ) {
       glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID[0] );
#ifdef DEBUG
        if ( glGetError() ) {
            DWRITE( "failed to bind texture\n" );
          }
#endif
      }
#endif
  
  for (x=0;x<this->presetInputs.gx - 1;x++){
    glBegin(GL_TRIANGLE_STRIP);
    for(y=0;y<this->presetInputs.gy;y++){
      glTexCoord2f(this->presetInputs.x_mesh[x][y], this->presetInputs.y_mesh[x][y]); 
      glVertex2f(this->gridx[x][y], this->gridy[x][y]);
      glTexCoord2f(this->presetInputs.x_mesh[x+1][y], this->presetInputs.y_mesh[x+1][y]); 
      glVertex2f(this->gridx[x+1][y], this->gridy[x+1][y]);
    }
    glEnd();	
  }

#ifdef MACOS2
    /** Re-bind the pbuffer */
    if ( this->renderTarget->pbuffer != NULL ) {
      glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID[0] );
      }
#endif

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
  }

void projectM::do_per_frame() {

    glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(0, 0, -9);

  //Texture wrapping( clamp vs. wrap)
  if (this->presetOutputs.bTexWrap==0){
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);}
  else{ glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);}
      

  //      glRasterPos2i(0,0);
      //      glClear(GL_COLOR_BUFFER_BIT);
      //      glColor4d(0.0, 0.0, 0.0,1.0);     
       
  //      glMatrixMode(GL_TEXTURE);
    //  glLoadIdentity();

      glRasterPos2i(0,0);
      glClear(GL_COLOR_BUFFER_BIT);
      glColor4d(0.0, 0.0, 0.0,1.0);

 glMatrixMode(GL_TEXTURE);
      glLoadIdentity();

      /*
      glTranslatef(this->presetOutputs.cx,this->presetOutputs.cy, 0);
     if(this->correction)  glScalef(1,this->vw/(float)this->vh,1);

      if(!isPerPixelEqn(ROT_OP)) {
	//	printf("ROTATING: rot = %f\n", rot);
	glRotatef(this->presetOutputs.rot*90, 0, 0, 1);
      }
      if(!isPerPixelEqn(SX_OP)) glScalef(1/this->presetOutputs.sx,1,1);     
      if(!isPerPixelEqn(SY_OP)) glScalef(1,1/this->presetOutputs.sy,1); 

      if(this->correction)glScalef(1,this->vh/(float)this->vw,1);
            glTranslatef((-this->presetOutputs.cx) ,(-this->presetOutputs.cy),0);  
      */

      if(!this->activePreset->isPerPixelEqn(DX_OP)) glTranslatef(-this->presetOutputs.dx,0,0);   
      if(!this->activePreset->isPerPixelEqn(DY_OP)) glTranslatef(0 ,-this->presetOutputs.dy,0);  
}


//Actually draws the texture to the screen
//
//The Video Echo effect is also applied here
void projectM::render_texture_to_screen() { 

      int flipx=1,flipy=1;
      //glBindTexture( GL_TEXTURE_2D,this->renderTarget->textureID[0] );
     glMatrixMode(GL_TEXTURE);  
     glLoadIdentity();

    glClear( GL_DEPTH_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(0, 0, -15);  
     
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_DECAL);
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
      
      //       glClear(GL_ACCUM_BUFFER_BIT);
      glColor4d(0.0, 0.0, 0.0,1.0f);

    DWRITE( "rendering texture to screen\n" );

   glBegin(GL_QUADS);
    glVertex3d( 0, 0, -1 );
     glVertex4d(-0.5,-0.5,-1,1);
     glVertex4d(-0.5,  0.5,-1,1);
     glVertex4d(0.5,  0.5,-1,1);
     glVertex4d(0.5, -0.5,-1,1);
      glEnd();
     
      
      glEnable(GL_TEXTURE_2D); 
      //glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID[0] );
//      glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID );

      // glAccum(GL_LOAD,0);
      // if (bDarken==1)  glBlendFunc(GL_SRC_COLOR,GL_ZERO); 
	
      //Draw giant rectangle and texture it with our texture!
      glBegin(GL_QUADS);
      glTexCoord4d(0, 1,0,1); glVertex4d(-0.5,-0.5,-1,1);
      glTexCoord4d(0, 0,0,1); glVertex4d(-0.5,  0.5,-1,1);
      glTexCoord4d(1, 0,0,1); glVertex4d(0.5,  0.5,-1,1);
      glTexCoord4d(1, 1,0,1); glVertex4d(0.5, -0.5,-1,1);
      glEnd();
       
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //  if (bDarken==1)  glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_ALPHA); 

  // if (bDarken==1) { glAccum(GL_ACCUM,1-fVideoEchoAlpha); glBlendFunc(GL_SRC_COLOR,GL_ZERO); }

       glMatrixMode(GL_TEXTURE);

      //draw video echo
      glColor4f(0.0, 0.0, 0.0,this->presetOutputs.fVideoEchoAlpha);
      glTranslatef(.5,.5,0);
      glScalef(1.0/this->presetOutputs.fVideoEchoZoom,1.0/this->presetOutputs.fVideoEchoZoom,1);
       glTranslatef(-.5,-.5,0);    

      switch (((int)this->presetOutputs.nVideoEchoOrientation))
	{
	case 0: flipx=1;flipy=1;break;
	case 1: flipx=-1;flipy=1;break;
  	case 2: flipx=1;flipy=-1;break;
	case 3: flipx=-1;flipy=-1;break;
	default: flipx=1;flipy=1; break;
	}
      glBegin(GL_QUADS);
      glTexCoord4d(0, 1,0,1); glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
      glTexCoord4d(0, 0,0,1); glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
      glTexCoord4d(1, 0,0,1); glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
      glTexCoord4d(1, 1,0,1); glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
      glEnd();

    
      glDisable(GL_TEXTURE_2D);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


      if (this->presetOutputs.bBrighten==1)
	{ 
	  glColor4f(1.0, 1.0, 1.0,1.0);
	  glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  glBlendFunc(GL_ZERO, GL_DST_COLOR);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();

	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	} 

      if (this->presetOutputs.bDarken==1)
	{ 
	  
	  glColor4f(1.0, 1.0, 1.0,1.0);
	  glBlendFunc(GL_ZERO,GL_DST_COLOR);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  


	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	} 
    

      if (this->presetOutputs.bSolarize)
	{ 
       
	  glColor4f(1.0, 1.0, 1.0,1.0);
	  glBlendFunc(GL_ZERO,GL_ONE_MINUS_DST_COLOR);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  glBlendFunc(GL_DST_COLOR,GL_ONE);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();


	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	} 

      if (this->presetOutputs.bInvert)
	{ 
	  glColor4f(1.0, 1.0, 1.0,1.0);
	  glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	} 
}
void projectM::render_texture_to_studio() { 

      int x,y;
      int flipx=1,flipy=1;
 
     glMatrixMode(GL_TEXTURE);  
     glLoadIdentity();

    glClear( GL_DEPTH_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(0, 0, -15);  
     
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_DECAL);
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
      
      //       glClear(GL_ACCUM_BUFFER_BIT);
      glColor4f(0.0, 0.0, 0.0,0.04);
      

   glBegin(GL_QUADS);
     glVertex4d(-0.5,-0.5,-1,1);
     glVertex4d(-0.5,  0.5,-1,1);
     glVertex4d(0.5,  0.5,-1,1);
     glVertex4d(0.5, -0.5,-1,1);
      glEnd();


      glColor4f(0.0, 0.0, 0.0,1.0);
      
      glBegin(GL_QUADS);
      glVertex4d(-0.5,0,-1,1);
      glVertex4d(-0.5,  0.5,-1,1);
      glVertex4d(0.5,  0.5,-1,1);
      glVertex4d(0.5, 0,-1,1);
      glEnd();
     
     glBegin(GL_QUADS);
     glVertex4d(0,-0.5,-1,1);
     glVertex4d(0,  0.5,-1,1);
     glVertex4d(0.5,  0.5,-1,1);
     glVertex4d(0.5, -0.5,-1,1);
     glEnd();

     glPushMatrix();
     glTranslatef(.25, .25, 0);
     glScalef(.5,.5,1);
     
     glEnable(GL_TEXTURE_2D);
    

      //Draw giant rectangle and texture it with our texture!
      glBegin(GL_QUADS);
      glTexCoord4d(0, 1,0,1); glVertex4d(-0.5,-0.5,-1,1);
      glTexCoord4d(0, 0,0,1); glVertex4d(-0.5,  0.5,-1,1);
      glTexCoord4d(1, 0,0,1); glVertex4d(0.5,  0.5,-1,1);
      glTexCoord4d(1, 1,0,1); glVertex4d(0.5, -0.5,-1,1);
      glEnd();
       
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
 

      glMatrixMode(GL_TEXTURE);

      //draw video echo
      glColor4f(0.0, 0.0, 0.0,this->presetOutputs.fVideoEchoAlpha);
      glTranslated(.5,.5,0);
      glScaled(1/this->presetOutputs.fVideoEchoZoom,1/this->presetOutputs.fVideoEchoZoom,1);
      glTranslated(-.5,-.5,0);    

      switch (((int)this->presetOutputs.nVideoEchoOrientation))
	{
	case 0: flipx=1;flipy=1;break;
	case 1: flipx=-1;flipy=1;break;
  	case 2: flipx=1;flipy=-1;break;
	case 3: flipx=-1;flipy=-1;break;
	default: flipx=1;flipy=1; break;
	}
      glBegin(GL_QUADS);
      glTexCoord4d(0, 1,0,1); glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
      glTexCoord4d(0, 0,0,1); glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
      glTexCoord4d(1, 0,0,1); glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
      glTexCoord4d(1, 1,0,1); glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
      glEnd();

    
      //glDisable(GL_TEXTURE_2D);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

 // if (bDarken==1) { glAccum(GL_ACCUM,fVideoEchoAlpha); glAccum(GL_RETURN,1);}


      if (this->presetOutputs.bInvert)
	{ 
	  glColor4f(1.0, 1.0, 1.0,1.0);
	  glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	} 

      //  glTranslated(.5,.5,0);
  //  glScaled(1/fVideoEchoZoom,1/fVideoEchoZoom,1);
      //   glTranslated(-.5,-.5,0);    
      //glTranslatef(0,.5*vh,0);

      /** Per-pixel mesh display -- bottom-right corner */
      //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
       
      glDisable(GL_TEXTURE_2D);
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glPushMatrix();
      glTranslatef(.25, -.25, 0);
      glScalef(.5,.5,1);
       glColor4f(1.0,1.0,1.0,1.0);

       for (x=0;x<this->presetInputs.gx;x++){
	 glBegin(GL_LINE_STRIP);
	 for(y=0;y<this->presetInputs.gy;y++){
	   glVertex4f((this->presetInputs.x_mesh[x][y]-.5), (this->presetInputs.y_mesh[x][y]-.5),-1,1);
	   //glVertex4f((origx[x+1][y]-.5) * vw, (origy[x+1][y]-.5) *vh ,-1,1);
	 }
	 glEnd();	
       }    
       
       for (y=0;y<this->presetInputs.gy;y++){
	 glBegin(GL_LINE_STRIP);
	 for(x=0;x<this->presetInputs.gx;x++){
	   glVertex4f((this->presetInputs.x_mesh[x][y]-.5), (this->presetInputs.y_mesh[x][y]-.5),-1,1);
	   //glVertex4f((origx[x+1][y]-.5) * vw, (origy[x+1][y]-.5) *vh ,-1,1);
	 }
	 glEnd();	
       }    

        glEnable( GL_TEXTURE_2D );
      
       /*
       for (x=0;x<this->presetInputs.gx-1;x++){
	 glBegin(GL_POINTS);
	 for(y=0;y<this->presetInputs.gy;y++){
	   glVertex4f((this->origx[x][y]-.5)* this->vw, (this->origy[x][y]-.5)*this->vh,-1,1);
	   glVertex4f((this->origx[x+1][y]-.5) * this->vw, (this->origy[x+1][y]-.5) *this->vh ,-1,1);
	 }
	 glEnd();	
       }    
       */
 // glTranslated(-.5,-.5,0);     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 

    /** Waveform display -- bottom-left */
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
       glMatrixMode(GL_MODELVIEW);
       glPushMatrix();
   glTranslatef(-.5,0, 0);

    glTranslatef(0,-0.10, 0);
   glBegin(GL_LINE_STRIP);
	     glColor4f(0,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->treb_att*-7,-1);
	     glColor4f(1.0,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)),0 ,-1);   
	     glColor4f(.5,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->treb*7,-1);
	     glEnd(); 	
	       
	       glTranslatef(0,-0.13, 0);
 glBegin(GL_LINE_STRIP);
	      glColor4f(0,1.0,0.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->mid_att*-7,-1);
	     glColor4f(1.0,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)),0 ,-1);   
	     glColor4f(.5,1.0,0.0,0.5);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->mid*7,-1);
	     glEnd();
	  
	   
	     glTranslatef(0,-0.13, 0);
 glBegin(GL_LINE_STRIP);
	     glColor4f(1.0,0.0,0.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->bass_att*-7,-1);
	     glColor4f(1.0,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)),0 ,-1);   
	     glColor4f(.7,0.2,0.2,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->bass*7,-1);
	     glEnd();

 glTranslatef(0,-0.13, 0);
 glBegin(GL_LINES);
	     
	     glColor4f(1.0,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)),0 ,-1);   
	     glColor4f(1.0,0.6,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->vol*7,-1);
	     glEnd();

	     glPopMatrix();

   glDisable(GL_TEXTURE_2D);
}


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
 
 this->Time = 0;
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

  this->doPerPixelEffects = 1;
  this->doIterative = 1;

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
 
  this->Time = 0;
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
   
    char path[1024];
    int mindim, origtexsize;

    DWRITE( "projectM_resetGL(): in: %d x %d\n", w, h );

    /** Stash the new dimensions */
    this->vw = w;
    this->vh = h;
    this->aspect=(float)h / (float)w;

    if (!this->renderTarget->usePbuffers) {
      createPBuffers(w,h,this->renderTarget);
      }

    if ( this->fbuffer != NULL ) {
        free( this->fbuffer );
      }
    this->fbuffer = 
        (GLubyte *)malloc( sizeof( GLubyte ) * this->renderTarget->texsize * this->renderTarget->texsize * 3 );

    /* Our shading model--Gouraud (smooth). */
    glShadeModel( GL_SMOOTH);
    /* Culling. */
    //    glCullFace( GL_BACK );
    //    glFrontFace( GL_CCW );
    //    glEnable( GL_CULL_FACE );
    /* Set the clear color. */
    glClearColor( 0, 0, 0, 0 );
    /* Setup our viewport. */
    glViewport( 0, 0, w, h );
    /*
    * Change to the projection matrix and set
    * our viewing volume.
    */
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    //    gluOrtho2D(0.0, (GLfloat) width, 0.0, (GLfloat) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();  

    //    glFrustum(0.0, height, 0.0,width,10,40);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDrawBuffer(GL_BACK); 
    glReadBuffer(GL_BACK); 
    glEnable(GL_BLEND); 

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    glEnable( GL_LINE_SMOOTH );
      glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);		
    
    glEnable(GL_POINT_SMOOTH);
  
    // glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,renderTarget->texsize,renderTarget->texsize,0);
    //glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,renderTarget->texsize,renderTarget->texsize);
    glLineStipple(2, 0xAAAA);

    /** (Re)create the offscreen for pass 1 */
   

    rescale_per_pixel_matrices();

    /** Load TTF font **/
   


#ifdef USE_FTGL
    /**f Load the standard fonts */
    if ( title_font == NULL && other_font == NULL ) {
       

        sprintf( path, "%s%cVera.ttf", this->fontURL, PATH_SEPARATOR );
        title_font = new FTGLPixmapFont(path);
	poly_font = new FTGLPolygonFont(path);
	sprintf( path, "%s%cVeraMono.ttf", this->fontURL, PATH_SEPARATOR );
        other_font = new FTGLPixmapFont(path);
   
      }
#endif /** USE_FTGL */      
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



/* initPresetLoader: initializes the preset
   loading library. this should be done before
   any parsing */
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

  //switchToIdlePreset();
  //load_init_conditions();

  /* Done */
#ifdef PRESET_DEBUG
    printf("initPresetLoader: finished\n");
#endif
  return PROJECTM_SUCCESS;
}


