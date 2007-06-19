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
#include "Renderer.hpp"

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
      this->activePreset->evalPerPixelEqns();
//      this->activePreset->evalCustomWaveInitConditions();
//      this->activePreset->evalCustomShapeInitConditions();
 
//     printf("%f %d\n",Time,frame);
 
    
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
      
      renderer->RenderFrame(&presetOutputs, &presetInputs);

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
   
    this->fvw = 800;
    this->fvh = 600;
    this->wvw = 512;
    this->wvh = 512;
    this->fullscreen = 0;

    /** Configurable mesh size */
    this->gx = 48;
    this->gy = 36;
    this->texsize=512;

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


    renderer = new Renderer(gx,gy,texsize);

printf( "exiting projectM_init()\n" );
}




//calculate matrices for per_pixel



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

  sprintf( buffer, " texsize: %d", this->texsize);
  other_font->Render(buffer);

  glRasterPos2f(0, -.13+offset);
  sprintf( buffer, "viewport: %d x %d", this->vw, this->vh);
  other_font->Render(buffer);
  /* REME: FIX
  glRasterPos2f(0, -.17+offset);  
  other_font->Render((this->renderer->renderTarget->usePbuffers ? "     FBO: on" : "     FBO: off"));
  */
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


