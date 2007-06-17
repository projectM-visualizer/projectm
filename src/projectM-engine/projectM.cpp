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
#include "builtin_funcs.h"
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

      mspf=(int)(1000.0/(float)fps); //milliseconds per frame
      totalframes++; //total amount of frames since startup

#ifndef WIN32
      Time = getTicks( &startTime ) * 0.001;
#else
        Time = getTicks( startTime ) * 0.001;
#endif /** !WIN32 */
      
      frame++;  //number of frames for current preset
      progress= frame/(float)avgtime;
    DWRITE( "frame: %d\ttime: %f\tprogress: %f\tavgtime: %d\tang: %f\trot: %f\n",
             this->frame, Time, this->progress, this->avgtime, this->ang_per_pixel,
             this->rot );
      if (progress>1.0) progress=1.0;

//       printf("start:%d at:%d min:%d stop:%d on:%d %d\n",startframe, frame frame-startframe,avgtime,  noSwitch,progress);
//      Preset::active_preset->evalInitConditions();
      Preset::active_preset->evalPerFrameEquations();

//      Preset::active_preset->evalCustomWaveInitConditions();
//      Preset::active_preset->evalCustomShapeInitConditions();
 
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
              nohard=fps*5;
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
    if(this->bDarkenCenter)darken_center();
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
    Preset::active_preset = NULL;
 
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
    this->gx = 48;
    this->gy = 36;

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
    initPresetLoader();
    if ( loadPresetDir( presetURL ) == PROJECTM_ERROR ) {
        switchToIdlePreset();
      }

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
        loadPresetDir( "/Users/descarte/tmp/projectM-1.00/presets_projectM" );
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
        loadPresetDir( "c:\\tmp\\projectM\\presets_projectM" );
#else
        loadPresetDir( "/Users/descarte/tmp/projectM-1.00/presets_projectM" );
#endif
      } else {
        printf( "PresetDir: %s\n", this->presetURL );
        loadPresetDir( presetURL );
      }

#endif

printf( "pre init_display()\n" );

printf( "post init_display()\n" );

    mspf=(int)(1000.0/(float)fps);


   
  //create off-screen pbuffer (or not if unsupported)
//  CreateRenderTarget(this->renderTarget->texsize, &this->textureID, &this->renderTarget);
printf( "post CreaterenderTarget\n" );
  
    
//fps = 0;

    initMenu();  
//DWRITE( "post initMenu()\n" );

    printf("mesh: %d %d\n", this->gx,this->gy );

#ifdef PANTS
    printf( "maxsamples: %d\n", this->maxsamples );
  initPCM(this->maxsamples);
DWRITE( "post PCM init\n" );
#endif

 this->avgtime=this->fps*20;

    this->hasInit = 1;


  createPBuffers( this->renderTarget->texsize, this->renderTarget->texsize , this->renderTarget );

printf( "exiting projectM_init()\n" );
}

void projectM::free_per_pixel_matrices() {

  int x;

 for(x = 0; x < this->gx; x++)
    {
      
      free(this->gridx[x]);
      free(this->gridy[x]); 
      free(this->origtheta[x]);
      free(this->origrad[x]);
      free(this->origx[x]);
      free(this->origy[x]);
      free(this->origx2[x]);
      free(this->origy2[x]);
      free(this->x_mesh[x]);
      free(this->y_mesh[x]);
      free(this->rad_mesh[x]);
      free(this->theta_mesh[x]);
      
    }

  
  free(this->origx);
  free(this->origy);
  free(this->origx2);
  free(this->origy2);
  free(this->gridx);
  free(this->gridy);
  free(this->x_mesh);
  free(this->y_mesh);
  free(this->rad_mesh);
  free(this->theta_mesh);

  this->origx = NULL;
  this->origy = NULL;
  this->origx2 = NULL;
  this->origy2 = NULL;
  this->gridx = NULL;
  this->gridy = NULL;
  this->x_mesh = NULL;
  this->y_mesh = NULL;
  this->rad_mesh = NULL;
  this->theta_mesh = NULL;
}


void projectM::init_per_pixel_matrices() {

  int x,y; 
  
  this->gridx=(float **)wipemalloc(this->gx * sizeof(float *));
   for(x = 0; x < this->gx; x++)
    {
      this->gridx[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->gridy=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->gridy[x] = (float *)wipemalloc(this->gy * sizeof(float)); 
    }
  this->origtheta=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->origtheta[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->origrad=(float **)wipemalloc(this->gx * sizeof(float *));
     for(x = 0; x < this->gx; x++)
    {
      this->origrad[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->origx=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->origx[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->origy=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->origy[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->origx2=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->origx2[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }  
this->origy2=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->origy2[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->x_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->x_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->y_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->y_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    
    }
  this->rad_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->rad_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->theta_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->theta_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->sx_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->sx_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->sy_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->sy_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->dx_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->dx_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->dy_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->dy_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->cx_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->cx_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->cy_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->cy_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->zoom_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->zoom_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->zoomexp_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    {
      this->zoomexp_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }
  this->rot_mesh=(float **)wipemalloc(this->gx * sizeof(float *));
 for(x = 0; x < this->gx; x++)
    { 
      this->rot_mesh[x] = (float *)wipemalloc(this->gy * sizeof(float));
    }



  //initialize reference grid values
  for (x=0;x<this->gx;x++)
    {
      for(y=0;y<this->gy;y++)
	{
	   this->origx[x][y]=x/(float)(this->gx-1);
	   this->origy[x][y]=-((y/(float)(this->gy-1))-1);
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

  Preset::active_preset->evalPerPixelEqns();

  if(!Preset::active_preset->isPerPixelEqn(CX_OP))
       { 
      for (x=0;x<this->gx;x++){
       
	for(y=0;y<this->gy;y++){
	  this->cx_mesh[x][y]=this->cx;
	}
	
      }
    }

  if(!Preset::active_preset->isPerPixelEqn(CY_OP))
        { 
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  this->cy_mesh[x][y]=this->cy;
	}}
    }
  
  if(!Preset::active_preset->isPerPixelEqn(SX_OP))
    { 
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  this->sx_mesh[x][y]=this->sx;
	}}
    }
  
  if(!Preset::active_preset->isPerPixelEqn(SY_OP))
    { 
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  this->sy_mesh[x][y]=this->sy;
	}}
    }

  if(!Preset::active_preset->isPerPixelEqn(ZOOM_OP))
    {       
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  this->zoom_mesh[x][y]=this->zoom;
	}}
    }
 
  if(!Preset::active_preset->isPerPixelEqn(ZOOMEXP_OP))
    {
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  this->zoomexp_mesh[x][y]=this->zoomexp;
	}}
    }

  if(!Preset::active_preset->isPerPixelEqn(ROT_OP))
    {       
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  this->rot_mesh[x][y]=this->rot;
	}
      }
    }

  /*
  for (x=0;x<this->gx;x++){
    for(y=0;y<this->gy;y++){	  	  
      this->x_mesh[x][y]=(this->x_mesh[x][y]-.5)*2; 
    }
  }
 
  for (x=0;x<this->gx;x++){
    for(y=0;y<this->gy;y++){	  	  
      this->y_mesh[x][y]=(this->y_mesh[x][y]-.5)*2; 
    }
  }
  */

  for (x=0;x<this->gx;x++){
    for(y=0;y<this->gy;y++){
      fZoom2 = powf( this->zoom_mesh[x][y], powf( this->zoomexp_mesh[x][y], this->rad_mesh[x][y]*2.0f - 1.0f));
      fZoom2Inv = 1.0f/fZoom2;
      this->x_mesh[x][y]= this->origx2[x][y]*0.5f*fZoom2Inv + 0.5f;
      this->y_mesh[x][y]= this->origy2[x][y]*0.5f*fZoom2Inv + 0.5f;
    }
  }
	
  for (x=0;x<this->gx;x++){
    for(y=0;y<this->gy;y++){
      this->x_mesh[x][y]  = ( this->x_mesh[x][y] - this->cx_mesh[x][y])/this->sx_mesh[x][y] + this->cx_mesh[x][y];
    }
  }
  
  for (x=0;x<this->gx;x++){
    for(y=0;y<this->gy;y++){
      this->y_mesh[x][y] = ( this->y_mesh[x][y] - this->cy_mesh[x][y])/this->sy_mesh[x][y] + this->cy_mesh[x][y];
    }
  }	   
	 

 for (x=0;x<this->gx;x++){
   for(y=0;y<this->gy;y++){
     float u2 = this->x_mesh[x][y] - this->cx_mesh[x][y];
     float v2 = this->y_mesh[x][y] - this->cy_mesh[x][y];
     
     float cos_rot = cosf(this->rot_mesh[x][y]);
     float sin_rot = sinf(this->rot_mesh[x][y]);
     
     this->x_mesh[x][y] = u2*cos_rot - v2*sin_rot + this->cx_mesh[x][y];
     this->y_mesh[x][y] = u2*sin_rot + v2*cos_rot + this->cy_mesh[x][y];

  }
 }	  

 if(Preset::active_preset->isPerPixelEqn(DX_OP))
   {
     for (x=0;x<this->gx;x++){
       for(y=0;y<this->gy;y++){	      
	 this->x_mesh[x][y] -= this->dx_mesh[x][y];
       }
     }
   }
 
 if(Preset::active_preset->isPerPixelEqn(DY_OP))
   {
     for (x=0;x<this->gx;x++){
       for(y=0;y<this->gy;y++){	      
	 this->y_mesh[x][y] -= this->dy_mesh[x][y];
       }
     }
		  	
   }

}

void projectM::reset_per_pixel_matrices() {

  int x,y;
  /*
  for (x=0;x<this->gx;x++)
    {
      memcpy(this->x_mesh[x],this->origx[x],sizeof(float)*this->gy);
    }
  for (x=0;x<this->gx;x++)
    {
      memcpy(this->y_mesh[x],this->origy[x],sizeof(float)*this->gy);
    }
  for (x=0;x<this->gx;x++)
    {
      memcpy(this->rad_mesh[x],this->origrad[x],sizeof(float)*this->gy);
    }
  for (x=0;x<this->gx;x++)
    {
      memcpy(this->theta_mesh[x],this->origtheta[x],sizeof(float)*this->gy);
    }
  */
  
    for (x=0;x<this->gx;x++)
    {
      for(y=0;y<this->gy;y++)
	{   
          this->x_mesh[x][y]=this->origx[x][y];
	  this->y_mesh[x][y]=this->origy[x][y];
	  this->rad_mesh[x][y]=this->origrad[x][y];
	  this->theta_mesh[x][y]=this->origtheta[x][y];	  
	}
    }
  
  //memcpy(this->x_mesh,this->origx,sizeof(float)*this->gy*this->gx);
  //memcpy(this->y_mesh,this->origy,sizeof(float)*this->gy*this->gx);
  //memcpy(this->rad_mesh,this->origrad,sizeof(float)*this->gy*this->gx);
  //memcpy(this->theta_mesh,this->origtheta,sizeof(float)*this->gy*this->gx);
 }

void projectM::rescale_per_pixel_matrices() {

    int x, y;

    for ( x = 0 ; x < this->gx ; x++ ) {
        for ( y = 0 ; y < this->gy ; y++ ) {
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
 
  while ((wavecode = Preset::active_preset->nextCustomWave()) != NULL)
    {
     
      if(wavecode->enabled==1)
	{
	
	  if (wavecode->bAdditive==0)  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	  else    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
	  if (wavecode->bDrawThick==1)  glLineWidth(this->renderTarget->texsize < 512 ? 1 : 2*this->renderTarget->texsize/512);
	  
	   beatDetect->pcm->getPCM(wavecode->value1,wavecode->samples,0,wavecode->bSpectrum,wavecode->smoothing,0);
	   beatDetect->pcm->getPCM(wavecode->value2,wavecode->samples,1,wavecode->bSpectrum,wavecode->smoothing,0);
	  // printf("%f\n",pcmL[0]);


	  float mult=wavecode->scaling*this->fWaveScale*(wavecode->bSpectrum ? 0.015f :1.0f);

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
  
  while ((shapecode = Preset::active_preset->nextCustomShape()) != NULL)
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
	    if (this->bWaveThick==1)  glLineWidth(this->renderTarget->texsize < 512 ? 1 : 2*this->renderTarget->texsize/512);
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
	  if (this->bWaveThick==1)  glLineWidth(this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512);
	  
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

    DWRITE( "draw_waveform: %d\n", this->nWaveMode );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

  modulate_opacity_by_volume(); 
  maximize_colors();
  
  if(this->bWaveDots==1) glEnable(GL_LINE_STIPPLE);
  
  offset=this->wave_x-.5;
  scale=505.0/512.0;


  

  //Thick wave drawing
  if (this->bWaveThick==1)  glLineWidth( (this->renderTarget->texsize < 512 ) ? 2 : 2*this->renderTarget->texsize/512);

  //Additive wave drawing (vice overwrite)
  if (this->bAdditiveWaves==0)  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  else    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
 
      switch(this->nWaveMode)
	{
	  
	case 8://monitor

	  glTranslatef(0.5,0.5, 0);
	  glRotated(-this->wave_mystery*90,0,0,1);

	     glTranslatef(-0.5,-0.825, 0);
	     glTranslatef( 0, 0, -1 );
	     
	     /*
	     for (x=0;x<16;x++)
	       {
		 glBegin(GL_LINE_STRIP);
		 glColor4f(1.0-(x/15.0),.5,x/15.0,1.0);
		 glVertex3f((this->totalframes%256)*2*scale, -this->beat_val[x]*this->fWaveScale+renderTarget->texsize*wave_y,-1);
		 glColor4f(.5,.5,.5,1.0);
		 glVertex3f((this->totalframes%256)*2*scale, this->renderTarget->texsize*this->wave_y,-1);   
		 glColor4f(1.0,1.0,0,1.0);
		 //glVertex3f((this->totalframes%256)*scale*2, this->beat_val_att[x]*this->fWaveScale+this->renderTarget->texsize*this->wave_y,-1);
		 glEnd();
	       
		 glTranslatef(0,this->renderTarget->texsize*(1/36.0), 0);
	       }
	  */
	     
	    glTranslatef(0,(1/18.0), 0);

 
	     glBegin(GL_LINE_STRIP);
	     glColor4f(1.0,1.0,0.5,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, beatDetect->treb_att*5*this->fWaveScale+this->wave_y);
	     glColor4f(.2,.2,.2,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, this->wave_y);   
	     glColor4f(1.0,1.0,0,1.0);
	     glVertex2f((this->totalframes%256)*scale*2, beatDetect->treb*-5*this->fWaveScale+this->wave_y);
	     glEnd();
	       
	       glTranslatef(0,.075, 0);
	     glBegin(GL_LINE_STRIP);
	     glColor4f(0,1.0,0.0,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, beatDetect->mid_att*5*this->fWaveScale+this->wave_y);
	     glColor4f(.2,.2,.2,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, this->wave_y);   
	     glColor4f(.5,1.0,.5,1.0);
	     glVertex2f((this->totalframes%256)*scale*2, beatDetect->mid*-5*this->fWaveScale+this->wave_y);
	     glEnd(); 
	  
	   
	     glTranslatef(0,.075, 0);
	     glBegin(GL_LINE_STRIP);
	     glColor4f(1.0,0,0,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, beatDetect->bass_att*5*this->fWaveScale+this->wave_y);
	     glColor4f(.2,.2,.2,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, this->wave_y);   
	     glColor4f(1.0,.5,.5,1.0);
	     glVertex2f((this->totalframes%256)*scale*2, beatDetect->bass*-5*this->fWaveScale+this->wave_y);
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

	  this->wave_y=-1*(this->wave_y-1.0);
 
	  glBegin(GL_LINE_STRIP);

    DWRITE( "nsamples: %d\n", beatDetect->pcm->numsamples );
	 
	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    { float inv_nverts_minus_one = 1.0f/(float)(beatDetect->pcm->numsamples);
	    //co= -(fabs(x-((beatDetect->pcm->numsamples*.5)-1))/beatDetect->pcm->numsamples)+1;
	      // printf("%d %f\n",x,co);
	      //theta=x*(6.28/beatDetect->pcm->numsamples);
	      //r= ((1+2*this->wave_mystery)*(this->renderTarget->texsize/5.0)+
	      //  ( co*beatDetect->pcm->pcmdataL[x]+ (1-co)*beatDetect->pcm->pcmdataL[-(x-(beatDetect->pcm->numsamples-1))])
	      //  *25*this->fWaveScale);
	      r=(0.5 + 0.4f*.12*beatDetect->pcm->pcmdataR[x]*this->fWaveScale + this->wave_mystery)*.5;
	      theta=(x)*inv_nverts_minus_one*6.28f + this->Time*0.2f;
	      /* 
	      if (x < 51)
		{
		  float mix = x/51.0;
		  mix = 0.5f - 0.5f*cosf(mix * 3.1416f);
		  float rad_2 = 0.5f + 0.4f*.12*beatDetect->pcm->pcmdataR[x]*this->fWaveScale + this->wave_mystery;
		  r = rad_2*(1.0f-mix) + r*(mix);
		}
	      */
  glVertex2f((r*cos(theta)*(this->correction ? this->aspect : 1.0)+this->wave_x), (r*sin(theta)+this->wave_y));
	     
	    }

	  //	  r= ( (1+2*this->wave_mystery)*(this->renderTarget->texsize/5.0)+
	  //     (0.5*beatDetect->pcm->pcmdataL[0]+ 0.5*beatDetect->pcm->pcmdataL[beatDetect->pcm->numsamples-1])
	  //      *20*this->fWaveScale);
      
	  //glVertex3f(r*cos(0)+(this->wave_x*this->renderTarget->texsize),r*sin(0)+(this->wave_y*this->renderTarget->texsize),-1);

	  glEnd();
	  /*
	  glBegin(GL_LINE_LOOP);
	  
	  for ( x=0;x<(512/pcmbreak);x++)
	    {
	      theta=(blockstart+x)*((6.28*pcmbreak)/512.0);
	      r= ((1+2*this->wave_mystery)*(this->renderTarget->texsize/5.0)+fdata_buffer[fbuffer][0][blockstart+x]*.0025*this->fWaveScale);
	      
	      glVertex3f(r*cos(theta)+(this->wave_x*this->renderTarget->texsize),r*sin(theta)+(wave_y*this->renderTarget->texsize),-1);
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

	  this->wave_y=-1*(this->wave_y-1.0);

	  glBegin(GL_LINE_STRIP);
	  //theta=(frame%512)*(6.28/512.0);

	  for ( x=1;x<(512-32);x++)
	    {
	      //co= -(abs(x-255)/512.0)+1;
	      // printf("%d %f\n",x,co);
	      //theta=((this->frame%256)*(2*6.28/512.0))+beatDetect->pcm->pcmdataL[x]*.2*this->fWaveScale;
	      //r= ((1+2*this->wave_mystery)*(this->renderTarget->texsize/5.0)+
	      //   (beatDetect->pcm->pcmdataL[x]-beatDetect->pcm->pcmdataL[x-1])*80*this->fWaveScale);
	      theta=beatDetect->pcm->pcmdataL[x+32]*0.06*this->fWaveScale * 1.57 + this->Time*2.3;
	      r=(0.53 + 0.43*beatDetect->pcm->pcmdataR[x]*0.12*this->fWaveScale+ this->wave_mystery)*.5;

	     
 glVertex2f((r*cos(theta)*(this->correction ? this->aspect : 1.0)+this->wave_x),(r*sin(theta)+this->wave_y));
	    }

	  glEnd(); 
	  /*
	  this->wave_y=-1*(this->wave_y-1.0);  
	  wave_x_temp=(this->wave_x*.75)+.125;	
	  wave_x_temp=-(wave_x_temp-1); 

	  glBegin(GL_LINE_STRIP);

	    
	  
	  for (x=0; x<512-32; x++)
	    {
	      float rad = (.53 + 0.43*beatDetect->pcm->pcmdataR[x]) + this->wave_mystery;
	      float ang = beatDetect->pcm->pcmdataL[x+32] * 1.57f + this->Time*2.3f;
	      glVertex3f((rad*cosf(ang)*.2*scale*this->fWaveScale + wave_x_temp)*this->renderTarget->texsize,(rad*sinf(ang)*this->fWaveScale*.2*scale + this->wave_y)*this->renderTarget->texsize,-1);
	      
	    }
	  glEnd();
	  */

	  break;
	  
	case 2://EXPERIMENTAL

        glTranslatef( 0, 0, -1 );
	  this->wave_y=-1*(this->wave_y-1.0);  


	  glBegin(GL_LINE_STRIP);

	  for (x=0; x<512-32; x++)
	    {
	   
  glVertex2f((beatDetect->pcm->pcmdataR[x]*this->fWaveScale*0.5*(this->correction ? this->aspect : 1.0) + this->wave_x),( (beatDetect->pcm->pcmdataL[x+32]*this->fWaveScale*0.5 + this->wave_y)));
	    }
	  glEnd();	
	  
	  break;

	case 3://EXPERIMENTAL

	    glTranslatef( 0, 0, -9 );
	  this->wave_y=-1*(this->wave_y-1.0);  
	  //wave_x_temp=(this->wave_x*.75)+.125;	
	  //wave_x_temp=-(wave_x_temp-1); 
	  
	 

	  glBegin(GL_LINE_STRIP);

	  for (x=0; x<512-32; x++)
	    {
	      
	      glVertex2f((beatDetect->pcm->pcmdataR[x] * this->fWaveScale*0.5 + this->wave_x),( (beatDetect->pcm->pcmdataL[x+32]*this->fWaveScale*0.5 + this->wave_y)));
 
	    }
	  glEnd();	
	  
	  break;

	case 4://single x-axis derivative waveform
	  {
	  this->wave_y=-1*(this->wave_y-1.0);	  
	  glTranslatef(.5,.5, 0);
	  glRotated(-this->wave_mystery*90,0,0,1);
	  glTranslatef(-.5,-.5, 0);
        glTranslatef( 0, 0, -1 );
	 
	  float w1 = 0.45f + 0.5f*(this->wave_mystery*0.5f + 0.5f);	       
	  float w2 = 1.0f - w1;
	  float xx[512],yy[512];
				
	  glBegin(GL_LINE_STRIP);
	  for (int i=0; i<512; i++)
	    {
	     xx[i] = -1.0f + 2.0f*(i/512.0) + this->wave_x;
	     yy[i] =0.4* beatDetect->pcm->pcmdataL[i]*0.47f*this->fWaveScale + this->wave_y;
	     xx[i] += 0.4*beatDetect->pcm->pcmdataR[i]*0.44f*this->fWaveScale;				      
	      
	      if (i>1)
		{
		  xx[i] = xx[i]*w2 + w1*(xx[i-1]*2.0f - xx[i-2]);
		  yy[i] = yy[i]*w2 + w1*(yy[i-1]*2.0f - yy[i-2]);
		}
	      glVertex2f(xx[i],yy[i]);
	    }

	  glEnd();

	  /*
	  this->wave_x=(this->wave_x*.75)+.125;	  
	  this->wave_x=-(this->wave_x-1); 
	  glBegin(GL_LINE_STRIP);
	 
	  for ( x=1;x<512;x++)
	    {
	      dy_adj=  beatDetect->pcm->pcmdataL[x]*20*this->fWaveScale-beatDetect->pcm->pcmdataL[x-1]*20*this->fWaveScale;
	      glVertex3f((x*(this->renderTarget->texsize/512))+dy_adj, beatDetect->pcm->pcmdataL[x]*20*this->fWaveScale+this->renderTarget->texsize*this->wave_x,-1);
	    }
	  glEnd(); 
	  */
	  }
	  break;

	case 5://EXPERIMENTAL
	  	  
        glTranslatef( 0, 0, -5 );	
	  
	  this->wave_y=-1*(this->wave_y-1.0);  
	 
	  cos_rot = cosf(this->Time*0.3f);
	  sin_rot = sinf(this->Time*0.3f);

	  glBegin(GL_LINE_STRIP);

	  for (x=0; x<512; x++)
	    {	      
	      float x0 = (beatDetect->pcm->pcmdataR[x]*beatDetect->pcm->pcmdataL[x+32] + beatDetect->pcm->pcmdataL[x+32]*beatDetect->pcm->pcmdataR[x]);
	      float y0 = (beatDetect->pcm->pcmdataR[x]*beatDetect->pcm->pcmdataR[x] - beatDetect->pcm->pcmdataL[x+32]*beatDetect->pcm->pcmdataL[x+32]);
	      
          glVertex2f(((x0*cos_rot - y0*sin_rot)*this->fWaveScale*0.5*(this->correction ? this->aspect : 1.0) + this->wave_x),( (x0*sin_rot + y0*cos_rot)*this->fWaveScale*0.5 + this->wave_y));
 
	    }
	  glEnd();	
	  
	 
	  break;

	case 6://single waveform


	  //glMatrixMode(GL_MODELVIEW);
	  //	  	  glLoadIdentity();
	  
	  glTranslatef(.5,.5, 0);
	  glRotated(-this->wave_mystery*90,0,0,1);
	  glTranslatef(0,0, -1);
	  
	  wave_x_temp=-2*0.4142*(fabs(fabs(this->wave_mystery)-.5)-.5);
	  glScalef(1.0+wave_x_temp,1.0,1.0);
	  glTranslatef(-.5,-.5, 0);
	  wave_x_temp=-1*(this->wave_x-1.0);

	  glBegin(GL_LINE_STRIP);
	  //	  wave_x_temp=(wave_x*.75)+.125;	  
	  //	  wave_x_temp=-(wave_x_temp-1);
	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    {
     
	      //glVertex3f(x*scale, fdata_buffer[fbuffer][0][blockstart+x]*.0012*fWaveScale+renderTarget->texsize*wave_x_temp,-1);
	      glVertex2f(x/(float)beatDetect->pcm->numsamples, beatDetect->pcm->pcmdataR[x]*.04*this->fWaveScale+wave_x_temp);

	      //glVertex3f(x*scale, renderTarget->texsize*wave_y_temp,-1);
	    }
	  //	  printf("%f %f\n",renderTarget->texsize*wave_y_temp,wave_y_temp);
	  glEnd(); 
	  break;
	  
	case 7://dual waveforms

	  glTranslatef(.5,.5, 0);
	  glRotated(-this->wave_mystery*90,0,0,1);
	  
	  wave_x_temp=-2*0.4142*(fabs(fabs(this->wave_mystery)-.5)-.5);
	  glScalef(1.0+wave_x_temp,1.0,1.0);
	     glTranslatef(-.5,-.5, -1);
    glTranslatef( 0, 0, -1 );

         wave_y_temp=-1*(this->wave_x-1);

		  glBegin(GL_LINE_STRIP);
	 
	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    {
     
	      glVertex2f(x/(float)beatDetect->pcm->numsamples, beatDetect->pcm->pcmdataL[x]*.04*this->fWaveScale+(wave_y_temp+(this->wave_y*this->wave_y*.5)));
	    }
	  glEnd(); 

	  glBegin(GL_LINE_STRIP);
	 

	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    {
     
	      glVertex2f(x/(float)beatDetect->pcm->numsamples, beatDetect->pcm->pcmdataR[x]*.04*this->fWaveScale+(wave_y_temp-(this->wave_y*this->wave_y*.5)));
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
	      
	      glVertex2f(r*cos(theta)+this->wave_x,r*sin(theta)+this->wave_y);
	    }
	  glEnd();

glBegin(GL_LINE_STRIP);
	
	  for ( x=0;x<512;x++)
	    {
	      glVertex3f(x*scale, beatDetect->pcm->pcmdataL[x]*.04*this->fWaveScale+((this->wave_x+.1)),-1);
	    }
	  glEnd();
	  
	  glBegin(GL_LINE_STRIP);
	  
	 for ( x=0;x<512;x++)
	    {
	      glVertex3f(x*scale, beatDetect->pcm->pcmdataR[x]*.04*this->fWaveScale+((this->wave_x-.1)),-1);
	      
	    }
	  glEnd();
     break;
         if (this->bWaveThick==1)  glLineWidth( (this->renderTarget->texsize < 512) ? 1 : 2*this->renderTarget->texsize/512); 
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
 if(this->nWaveMode==2 || this->nWaveMode==5)
   {
	switch(this->renderTarget->texsize)
			{
			case 256:  this->wave_o *= 0.07f; break;
			case 512:  this->wave_o *= 0.09f; break;
			case 1024: this->wave_o *= 0.11f; break;
			case 2048: this->wave_o *= 0.13f; break;
			}
   }

 else if(this->nWaveMode==3)
   {
	switch(this->renderTarget->texsize)
			{
			case 256:  this->wave_o *= 0.075f; break;
			case 512:  this->wave_o *= 0.15f; break;
			case 1024: this->wave_o *= 0.22f; break;
			case 2048: this->wave_o *= 0.33f; break;
			}
	this->wave_o*=1.3f;
	this->wave_o*=powf(beatDetect->treb ,2.0f);
   }

      if (this->bMaximizeWaveColor==1)  
	{
	  if(this->wave_r>=this->wave_g && this->wave_r>=this->wave_b)   //red brightest
	    {
	      wave_b_switch=this->wave_b*(1/this->wave_r);
	      wave_g_switch=this->wave_g*(1/this->wave_r);
	      wave_r_switch=1.0;
	    }
	  else if   (this->wave_b>=this->wave_g && this->wave_b>=this->wave_r)         //blue brightest
	    {  
	      wave_r_switch=this->wave_r*(1/this->wave_b);
	      wave_g_switch=this->wave_g*(1/this->wave_b);
	      wave_b_switch=1.0;
	      
	    }	
	
	  else  if (this->wave_g>=this->wave_b && this->wave_g>=this->wave_r)         //green brightest
	    {
	      wave_b_switch=this->wave_b*(1/this->wave_g);
	      wave_r_switch=this->wave_r*(1/this->wave_g);
	      wave_g_switch=1.0;
	    }
 
	
	  glColor4f(wave_r_switch, wave_g_switch, wave_b_switch, this->wave_o);
	}
      else
	{ 
	  glColor4f(this->wave_r, this->wave_g, this->wave_b, this->wave_o);
	}
      
}


void projectM::modulate_opacity_by_volume() {

 //modulate volume by opacity
      //
      //set an upper and lower bound and linearly
      //calculate the opacity from 0=lower to 1=upper
      //based on current volume


      if (this->bModWaveAlphaByVolume==1)
	{if (beatDetect->vol<=this->fModWaveAlphaStart)  this->wave_o=0.0;       
	else if (beatDetect->vol>=this->fModWaveAlphaEnd) this->wave_o=this->fWaveAlpha;
	else this->wave_o=this->fWaveAlpha*((beatDetect->vol-this->fModWaveAlphaStart)/(this->fModWaveAlphaEnd-this->fModWaveAlphaStart));}
      else this->wave_o=this->fWaveAlpha;
}

void projectM::draw_motion_vectors() {

    int x,y;

    float offsetx=this->mv_dx, intervalx=1.0/(float)this->mv_x;
    float offsety=this->mv_dy, intervaly=1.0/(float)this->mv_y;
    

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    glPointSize(this->mv_l);
    glColor4f(this->mv_r, this->mv_g, this->mv_b, this->mv_a);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef( 0, 0, -1 );

    glBegin(GL_POINTS);
    for (x=0;x<this->mv_x;x++){
        for(y=0;y<this->mv_y;y++){
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
    float of=this->ob_size*.5;
    float iff=this->ib_size*.5;
    float texof=1.0-of;

    //no additive drawing for borders
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  
    glColor4d(this->ob_r,this->ob_g,this->ob_b,this->ob_a);
  
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glTranslatef( 0, 0, -1 );

    glRectd(0,0,of,1);
    glRectd(of,0,texof,of);
    glRectd(texof,0,1,1);
    glRectd(of,1,texof,texof);
    glColor4d(this->ib_r,this->ib_g,this->ib_b,this->ib_a);
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
  sprintf( buffer, "    mesh: %d x %d", this->gx,this->gy);
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
  
    glColor4f(0.0, 0.0, 0.0,this->decay);
    
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
  
  for (x=0;x<this->gx - 1;x++){
    glBegin(GL_TRIANGLE_STRIP);
    for(y=0;y<this->gy;y++){
      glTexCoord2f(this->x_mesh[x][y], this->y_mesh[x][y]); 
      glVertex2f(this->gridx[x][y], this->gridy[x][y]);
      glTexCoord2f(this->x_mesh[x+1][y], this->y_mesh[x+1][y]); 
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
  if (this->bTexWrap==0){
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
      glTranslatef(this->cx,this->cy, 0);
     if(this->correction)  glScalef(1,this->vw/(float)this->vh,1);

      if(!isPerPixelEqn(ROT_OP)) {
	//	printf("ROTATING: rot = %f\n", rot);
	glRotatef(this->rot*90, 0, 0, 1);
      }
      if(!isPerPixelEqn(SX_OP)) glScalef(1/this->sx,1,1);     
      if(!isPerPixelEqn(SY_OP)) glScalef(1,1/this->sy,1); 

      if(this->correction)glScalef(1,this->vh/(float)this->vw,1);
            glTranslatef((-this->cx) ,(-this->cy),0);  
      */

      if(!Preset::active_preset->isPerPixelEqn(DX_OP)) glTranslatef(-this->dx,0,0);   
      if(!Preset::active_preset->isPerPixelEqn(DY_OP)) glTranslatef(0 ,-this->dy,0);  
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
      glColor4f(0.0, 0.0, 0.0,this->fVideoEchoAlpha);
      glTranslatef(.5,.5,0);
      glScalef(1.0/this->fVideoEchoZoom,1.0/this->fVideoEchoZoom,1);
       glTranslatef(-.5,-.5,0);    

      switch (((int)this->nVideoEchoOrientation))
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


      if (this->bBrighten==1)
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

      if (this->bDarken==1)
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
    

      if (this->bSolarize==1)
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

      if (this->bInvert==1)
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
      glColor4f(0.0, 0.0, 0.0,this->fVideoEchoAlpha);
      glTranslated(.5,.5,0);
      glScaled(1/this->fVideoEchoZoom,1/this->fVideoEchoZoom,1);
      glTranslated(-.5,-.5,0);    

      switch (((int)this->nVideoEchoOrientation))
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


      if (this->bInvert==1)
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

       for (x=0;x<this->gx;x++){
	 glBegin(GL_LINE_STRIP);
	 for(y=0;y<this->gy;y++){
	   glVertex4f((this->x_mesh[x][y]-.5), (this->y_mesh[x][y]-.5),-1,1);
	   //glVertex4f((origx[x+1][y]-.5) * vw, (origy[x+1][y]-.5) *vh ,-1,1);
	 }
	 glEnd();	
       }    
       
       for (y=0;y<this->gy;y++){
	 glBegin(GL_LINE_STRIP);
	 for(x=0;x<this->gx;x++){
	   glVertex4f((this->x_mesh[x][y]-.5), (this->y_mesh[x][y]-.5),-1,1);
	   //glVertex4f((origx[x+1][y]-.5) * vw, (origy[x+1][y]-.5) *vh ,-1,1);
	 }
	 glEnd();	
       }    

        glEnable( GL_TEXTURE_2D );
      
       /*
       for (x=0;x<this->gx-1;x++){
	 glBegin(GL_POINTS);
	 for(y=0;y<this->gy;y++){
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
 this->zoom=1.0;
 this->zoomexp= 1.0;
 this->rot= 0.0;
 this->warp= 0.0;

 this->sx= 1.0;
 this->sy= 1.0;
 this->dx= 0.0;
 this->dy= 0.0;
 this->cx= 0.5;
 this->cy= 0.5;

 this->decay=.98;

 this->wave_r= 1.0;
 this->wave_g= 0.2;
 this->wave_b= 0.0;
 this->wave_x= 0.5;
 this->wave_y= 0.5;
 this->wave_mystery= 0.0;

 this->ob_size= 0.0;
 this->ob_r= 0.0;
 this->ob_g= 0.0;
 this->ob_b= 0.0;
 this->ob_a= 0.0;

 this->ib_size = 0.0;
 this->ib_r = 0.0;
 this->ib_g = 0.0;
 this->ib_b = 0.0;
 this->ib_a = 0.0;

 this->mv_a = 0.0;
 this->mv_r = 0.0;
 this->mv_g = 0.0;
 this->mv_b = 0.0;
 this->mv_l = 1.0;
 this->mv_x = 16.0;
 this->mv_y = 12.0;
 this->mv_dy = 0.02;
 this->mv_dx = 0.02;
  
 this->meshx = 0;
 this->meshy = 0;
 
 this->Time = 0;
 this->progress = 0;
 this->frame = 0;

    this->avgtime = 600;
//bass_thresh = 0;

/* PER_FRAME CONSTANTS END */
 this->fRating = 0;
 this->fGammaAdj = 1.0;
 this->fVideoEchoZoom = 1.0;
 this->fVideoEchoAlpha = 0;
 this->nVideoEchoOrientation = 0;
 
 this->nWaveMode = 7;
 this->bAdditiveWaves = 0;
 this->bWaveDots = 0;
 this->bWaveThick = 0;
 this->bModWaveAlphaByVolume = 0;
 this->bMaximizeWaveColor = 0;
 this->bTexWrap = 0;
 this->bDarkenCenter = 0;
 this->bRedBlueStereo = 0;
 this->bBrighten = 0;
 this->bDarken = 0;
 this->bSolarize = 0;
 this->bInvert = 0;
 this->bMotionVectorsOn = 1;
 
 this->fWaveAlpha =1.0;
 this->fWaveScale = 1.0;
 this->fWaveSmoothing = 0;
 this->fWaveParam = 0;
 this->fModWaveAlphaStart = 0;
 this->fModWaveAlphaEnd = 0;
 this->fWarpAnimSpeed = 0;
 this->fWarpScale = 0;
 this->fShader = 0;


/* PER_PIXEL CONSTANTS BEGIN */
this->x_per_pixel = 0;
this->y_per_pixel = 0;
this->rad_per_pixel = 0;
this->ang_per_pixel = 0;

/* PER_PIXEL CONSTANT END */


/* Q AND T VARIABLES START */

this->q1 = 0;
this->q2 = 0;
this->q3 = 0;
this->q4 = 0;
this->q5 = 0;
this->q6 = 0;
this->q7 = 0;
this->q8 = 0;


/* Q AND T VARIABLES END */

//per pixel meshes
 this->zoom_mesh = NULL;
 this->zoomexp_mesh = NULL;
 this->rot_mesh = NULL;
 

 this->sx_mesh = NULL;
 this->sy_mesh = NULL;
 this->dx_mesh = NULL;
 this->dy_mesh = NULL;
 this->cx_mesh = NULL;
 this->cy_mesh = NULL;

 this->x_mesh = NULL;
 this->y_mesh = NULL;
 this->rad_mesh = NULL;
 this->theta_mesh = NULL;

//custom wave per point meshes
  }

/* Reinitializes the engine variables to a default (conservative and sane) value */
DLLEXPORT void projectM::projectM_resetengine() {

  this->doPerPixelEffects = 1;
  this->doIterative = 1;

  this->zoom=1.0;
  this->zoomexp= 1.0;
  this->rot= 0.0;
  this->warp= 0.0;
  
  this->sx= 1.0;
  this->sy= 1.0;
  this->dx= 0.0;
  this->dy= 0.0;
  this->cx= 0.5;
  this->cy= 0.5;

  this->decay=.98;
  
  this->wave_r= 1.0;
  this->wave_g= 0.2;
  this->wave_b= 0.0;
  this->wave_x= 0.5;
  this->wave_y= 0.5;
  this->wave_mystery= 0.0;

  this->ob_size= 0.0;
  this->ob_r= 0.0;
  this->ob_g= 0.0;
  this->ob_b= 0.0;
  this->ob_a= 0.0;

  this->ib_size = 0.0;
  this->ib_r = 0.0;
  this->ib_g = 0.0;
  this->ib_b = 0.0;
  this->ib_a = 0.0;

  this->mv_a = 0.0;
  this->mv_r = 0.0;
  this->mv_g = 0.0;
  this->mv_b = 0.0;
  this->mv_l = 1.0;
  this->mv_x = 16.0;
  this->mv_y = 12.0;
  this->mv_dy = 0.02;
  this->mv_dx = 0.02;
  
  this->meshx = 0;
  this->meshy = 0;
 
  this->Time = 0;
    if ( beatDetect != NULL ) {
        beatDetect->reset();
      }
  this->progress = 0;
  this->frame = 0;

// bass_thresh = 0;

/* PER_FRAME CONSTANTS END */
  this->fRating = 0;
  this->fGammaAdj = 1.0;
  this->fVideoEchoZoom = 1.0;
  this->fVideoEchoAlpha = 0;
  this->nVideoEchoOrientation = 0;
 
  this->nWaveMode = 7;
  this->bAdditiveWaves = 0;
  this->bWaveDots = 0;
  this->bWaveThick = 0;
  this->bModWaveAlphaByVolume = 0;
  this->bMaximizeWaveColor = 0;
  this->bTexWrap = 0;
  this->bDarkenCenter = 0;
  this->bRedBlueStereo = 0;
  this->bBrighten = 0;
  this->bDarken = 0;
  this->bSolarize = 0;
 this->bInvert = 0;
 this->bMotionVectorsOn = 1;
 
  this->fWaveAlpha =1.0;
  this->fWaveScale = 1.0;
  this->fWaveSmoothing = 0;
  this->fWaveParam = 0;
  this->fModWaveAlphaStart = 0;
  this->fModWaveAlphaEnd = 0;
  this->fWarpAnimSpeed = 0;
  this->fWarpScale = 0;
  this->fShader = 0;


/* PER_PIXEL CONSTANTS BEGIN */
 this->x_per_pixel = 0;
 this->y_per_pixel = 0;
 this->rad_per_pixel = 0;
 this->ang_per_pixel = 0;

/* PER_PIXEL CONSTANT END */


/* Q VARIABLES START */

 this->q1 = 0;
 this->q2 = 0;
 this->q3 = 0;
 this->q4 = 0;
 this->q5 = 0;
 this->q6 = 0;
 this->q7 = 0;
 this->q8 = 0;


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

/* loadPresetDir: opens the directory buffer
   denoted by 'dir' to load presets */
int projectM::loadPresetDir(char * dir) {
	/* we no longer do anything here and instead look in PM->presetURL in switchPreset
		this allows us to find new preset files on the fly */

  /* Start the prefix index right before the first entry, so next preset
     starts at the top of the list */
//#define PRESET_KLUDGE
#ifndef PRESET_KLUDGE
  preset_index = -1;
#else
  /** KLUDGE */
  preset_index = 30;
#endif

  /* Start the first preset */
  switchPreset( RANDOM_NEXT, HARD_CUT );
  
  return PROJECTM_SUCCESS;
}

/* closePresetDir: closes the current
   preset directory buffer */
int projectM::closePresetDir() {

	/* because we don't open we don't have to close ;) */
  destroyPresetLoader();

  return PROJECTM_SUCCESS;
}

/* switchPreset: loads the next preset from the directory stream.
   loadPresetDir() must be called first. This is a
   sequential load function */

int projectM::switchPreset(switch_mode_t switch_mode, int cut_type) {

  Preset * new_preset = 0;
	
  int switch_index;
  int sindex = 0;
  int slen = 0;

    DWRITE( "switchPreset(): in\n" );
    DWRITE( "switchPreset(): %s\n", presetURL );

	switch (switch_mode) {	  
	case ALPHA_NEXT:
		preset_index = switch_index = preset_index + 1;
		break;
	case ALPHA_PREVIOUS:
		preset_index = switch_index = preset_index - 1;
		break;  
	case RANDOM_NEXT:
		switch_index = rand();
		break;
	case RESTART_ACTIVE:
		switch_index = preset_index;
		break;
	default:
		return PROJECTM_FAILURE;
	}

    DWRITE( "switch_index: %d\n", switch_index );

	// iterate through the presetURL directory looking for the next entry 
	{
		struct dirent** entries;
		int dir_size = scandir(presetURL, &entries, /* is_valid_extension */ NULL, alphasort);
        DWRITE( "dir_size: %d\n", dir_size );
		if (dir_size > 0) {
			int i;

            DWRITE( "nentries: %d\n", dir_size );
			
			switch_index %= dir_size;
			if (switch_index < 0) switch_index += dir_size;			
			for (i = 0; i < dir_size; ++i) {
				if (switch_index == i) {
					// matching entry
					const size_t len = strlen(presetURL);
					char* path = (char *) malloc(len + strlen(entries[i]->d_name) + 2);
					if (path) {					
						strcpy(path, presetURL);
						if (len && ((path[len - 1] != '/')||(path[len - 1] != '\\'))) {
#ifdef WIN32
							strcat(path + len, "\\");
#else
							strcat(path + len, "/");
#endif
						}
						strcat(path + len, entries[i]->d_name);
						
						new_preset = Preset::load_preset(path);
						free(path);
						
						// we must keep iterating to free the remaining entries
					}
				  }
				free(entries[i]);
			}
			free(entries);
		}
	}

#ifdef WIN32
    new_preset = Preset::load_preset( "c:\\tmp\\projectM-1.00\\presets_test\\C.milk" );
#else
//    new_preset = Preset::load_preset( "/Users/descarte/tmp/projectM-1.00/presets_test/B.milk" );
//    new_preset = NULL;
#endif
	
	if (!new_preset) {
		switchToIdlePreset();
		return PROJECTM_ERROR;
	}


  /* Closes a preset currently loaded, if any */
  if ((Preset::active_preset != NULL) && (Preset::active_preset != Preset::idle_preset)) {
        Preset::active_preset->close_preset();
    }

  /* Sets global Preset::active_preset pointer */
  Preset::active_preset = new_preset;

#ifndef PANTS
  /** Split out the preset name from the path */
  slen = strlen( new_preset->file_path );
  sindex = slen;
  while ( new_preset->file_path[sindex] != WIN32_PATH_SEPARATOR && 
          new_preset->file_path[sindex] != UNIX_PATH_SEPARATOR && sindex > 0 ) {
    sindex--;
  }
  sindex++;
  if ( presetName != NULL ) {
    free( presetName );
    presetName = NULL;
  }
  presetName = (char *)wipemalloc( sizeof( char ) * (slen - sindex + 1) );
  strncpy( presetName, new_preset->file_path + sindex, slen - sindex );
  presetName[slen - sindex] = '\0';
#endif

  /* Reinitialize the engine variables to sane defaults */
  projectM_resetengine();

  /* Add any missing initial conditions */
  load_init_conditions();

  /* Add any missing initial conditions for each wave */
  Preset::active_preset->load_custom_wave_init_conditions();

/* Add any missing initial conditions for each shape */
  Preset::active_preset->load_custom_shape_init_conditions();

  /* Need to evaluate the initial conditions once */
  Preset::active_preset->evalInitConditions();
  Preset::active_preset->evalCustomWaveInitConditions();
  Preset::active_preset->evalCustomShapeInitConditions();
  //  evalInitPerFrameEquations();
  return PROJECTM_SUCCESS;
}

/* Loads a specific preset by absolute path */
int projectM::loadPresetByFile(char * filename) {

  Preset * new_preset;
 
  /* Finally, load the preset using its actual path */
  if ((new_preset = Preset::load_preset(filename)) == NULL) {
#ifdef PRESET_DEBUG
	printf("loadPresetByFile: failed to load preset!\n");
#endif
	return PROJECTM_ERROR;	  
  }

  /* Closes a preset currently loaded, if any */
  if ((Preset::active_preset != NULL) && (Preset::active_preset != Preset::idle_preset))
    Preset::active_preset->close_preset(); 

  /* Sets active preset global pointer */
  Preset::active_preset = new_preset;

  /* Reinitialize engine variables */
  projectM_resetengine();

 
  /* Add any missing initial conditions for each wave */
  Preset::active_preset->load_custom_wave_init_conditions();

 /* Add any missing initial conditions for each wave */
  Preset::active_preset->load_custom_shape_init_conditions();

  /* Add any missing initial conditions */
  load_init_conditions();
  
  /* Need to do this once for menu */
  Preset::active_preset->evalInitConditions();
  //  evalPerFrameInitEquations();

  return PROJECTM_SUCCESS;
}

/* initPresetLoader: initializes the preset
   loading library. this should be done before
   any parsing */
int projectM::initPresetLoader() {

  /* Initializes the builtin parameter database */
  init_builtin_param_db();

  /* Initializes the builtin function database */
  init_builtin_func_db();
	
  /* Initializes all infix operators */
  Eval::init_infix_ops();

  /* Set the seed to the current time in seconds */
#ifdef WIN32
  srand(time(NULL));
#endif

  /* Initialize the 'idle' preset */
  Preset::init_idle_preset();

  projectM_resetengine();

//  Preset::active_preset = Preset::idle_preset;
  presetName = NULL;
  switchToIdlePreset();
  load_init_conditions();

  /* Done */
#ifdef PRESET_DEBUG
    printf("initPresetLoader: finished\n");
#endif
  return PROJECTM_SUCCESS;
}

/* Sort of experimental code here. This switches
   to a hard coded preset. Useful if preset directory
   was not properly loaded, or a preset fails to parse */

void projectM::switchToIdlePreset() {

    if ( Preset::idle_preset == NULL ) {
        return;
      }

  /* Idle Preset already activated */
  if (Preset::active_preset == Preset::idle_preset)
    return;


  /* Close active preset */
  if (Preset::active_preset != NULL)
    Preset::active_preset->close_preset();

  /* Sets global Preset::active_preset pointer */
  Preset::active_preset = Preset::idle_preset;

    /** Stash the preset name */
    if ( presetName != NULL ) {
        free( presetName );
      }
    presetName = (char *)wipemalloc( sizeof( char ) * 5 );
    strncpy( presetName, "IDLE", 4 );
    presetName[4] = '\0';

  /* Reinitialize the engine variables to sane defaults */
  projectM_resetengine();

  /* Add any missing initial conditions */
  load_init_conditions();

  /* Need to evaluate the initial conditions once */
  Preset::active_preset->evalInitConditions();

}



/* Initialize the builtin function database.
   Should only be necessary once */
int projectM::init_builtin_func_db() {
  int retval;

  builtin_func_tree = 
    SplayTree::create_splaytree((int (*)(void*,void*))compare_string, (void*(*)(void*))copy_string, (void(*)(void*))free_string);

  if (builtin_func_tree == NULL)
    return PROJECTM_OUTOFMEM_ERROR;

  retval = load_all_builtin_func();
  return PROJECTM_SUCCESS;
}


/* Destroy the builtin function database.
   Generally, do this on projectm exit */
int projectM::destroy_builtin_func_db() {

  builtin_func_tree->splay_traverse((void (*)(void*))free_func_helper);
  return PROJECTM_SUCCESS;
}

/* Insert a function into the database */
int projectM::insert_func( Func *func ) {

  builtin_func_tree->splay_insert(func, func->name);

  return PROJECTM_SUCCESS;
}

/* Remove a function from the database */
int projectM::remove_func( Func *func ) {

    builtin_func_tree->splay_delete(func->name);

  return PROJECTM_SUCCESS;
}

/* Find a function given its name */
Func *projectM::find_func(char * name) {

  Func * func = NULL;

  /* First look in the builtin database */
  func = (Func *)builtin_func_tree->splay_find(name);
	
  return func;

}

/* Loads a builtin function */
int projectM::load_builtin_func(char * name,  float (*func_ptr)(float*), int num_args) {

  Func * func; 
  int retval; 

  /* Create new function */
  func = Func::create_func(name, func_ptr, num_args);

  if (func == NULL)
    return PROJECTM_OUTOFMEM_ERROR;

  retval = insert_func( func );

  return retval;

}

/* Loads all builtin functions */
int projectM::load_all_builtin_func() {
  
  if (load_builtin_func("int", int_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("abs", abs_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("sin", sin_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("cos", cos_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("tan", tan_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("asin", asin_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("acos", acos_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("atan", atan_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("sqr", sqr_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("sqrt", sqrt_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("pow", pow_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("exp", exp_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("log", log_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("log10", log10_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("sign", sign_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("min", min_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("max", max_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("sigmoid", sigmoid_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("atan2", atan2_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("rand", rand_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("band", band_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("bor", bor_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("bnot", bnot_wrapper, 1) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("if", if_wrapper, 3) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("equal", equal_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("above", above_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("below",below_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("nchoosek", nchoosek_wrapper, 2) < 0)
    return PROJECTM_ERROR;
  if (load_builtin_func("fact", fact_wrapper, 1) < 0)
    return PROJECTM_ERROR;


  return PROJECTM_SUCCESS;
}


