/* 
projectM v0.95 - xmms-projectm.sourceforge.net
--------------------------------------------------

Lead Developers:  Carmelo Piccione (cep@andrew.cmu.edu) &
                  Peter Sperl (peter@sperl.com)

We have also been advised by some professors at CMU, namely Roger B. Dannenberg.
http://www-2.cs.cmu.edu/~rbd/    
  
The inspiration for this program was Milkdrop by Ryan Geiss. Obviously. 

This code is distributed under the GPL.


THANKS FOR THE CODE!!!
-------------------------------------------------
The base for this program was andy@nobugs.org's XMMS plugin tutorial
http://www.xmms.org/docs/vis-plugin.html

We used some FFT code by Takuya OOURA instead of XMMS' built-in fft code
fftsg.c - http://momonga.t.u-tokyo.ac.jp/~ooura/fft.html

For font rendering we used GLF by Roman Podobedov
glf.c - http://astronomy.swin.edu.au/~pbourke/opengl/glf/

and some beat detection code was inspired by Frederic Patin @
www.gamedev.net/reference/programming/features/beatdetection/

*/

#include <stdio.h>
#include <xmms/plugin.h>
#include <string.h>
#include <stdlib.h>
#include <xmms/util.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <xmms/xmmsctrl.h>
#include <math.h>

#include "../projectM/projectM.h"
#include "../event/sdltoprojectM.h"
#include "video_init.h"



// Forward declarations 
static void projectM_xmms_init(void); 
static void projectM_cleanup(void);
static void projectM_about(void);
static void projectM_configure(void);
static void projectM_playback_start(void);
static void projectM_playback_stop(void);
static void projectM_render_pcm(gint16 pcm_data[2][512]);
static void projectM_render_freq(gint16 pcm_data[2][256]);
void read_config();


//extern preset_t * active_preset;

// Callback functions
VisPlugin projectM_vtable = {
  NULL, // Handle, filled in by xmms
  NULL, // Filename, filled in by xmms
  0,                     // Session ID
  "projectM v0.96",       // description
  2, // # of PCM channels for render_pcm()
  0, // # of freq channels wanted for render_freq()
  projectM_xmms_init,     // Called when plugin is enabled
  projectM_cleanup,        // Called when plugin is disabled
  projectM_about,          // Show the about box
  projectM_configure,      // Show the configure box
  NULL,                     // Called to disable plugin, filled in by xmms
  projectM_playback_start, // Called when playback starts
  projectM_playback_stop,  // Called when playback stops
  projectM_render_pcm,      // Render the PCM data, must return quickly
  projectM_render_freq     // Render the freq data, must return quickly
};

// XMMS entry point
VisPlugin *get_vplugin_info(void)
{
  return &projectM_vtable;
}

// Our worker thread
SDL_Thread *worker_thread;

SDL_mutex *mutex;

SDL_Event event;

SDL_Surface *screen;
//SDL_RenderTarget *RenderTarget = NULL;
//GLuint RenderTargetTextureID;

projectM_t *globalPM = NULL;

int maxsamples=512;

int texsize=512;
int gx=32,gy=24;
int wvw=400,wvh=400;
int fvw=1024,fvh=768;
int fps=60, fullscreen=0;
char *disp;
// char *title;

void worker_func()
{ 
  printf("1\n");
 
  read_config();   printf("2\n");
  init_display(wvw,wvh,fullscreen);   printf("3\n");
  SDL_WM_SetCaption("projectM v0.96", "projectM v0.96");

   printf("4\n");

  /** Initialise projectM */
    
  globalPM = (projectM_t *)malloc( sizeof( projectM_t ) );
  projectM_reset( globalPM );
  
  globalPM->fullscreen = fullscreen;
  globalPM->renderTarget->texsize = texsize;
  globalPM->gx=gx;
  globalPM->gy=gy;
  
   printf("3\n");
  
  
  globalPM->fontURL = (char *)malloc( sizeof( char ) * 512 );
  strcpy( globalPM->fontURL, "/etc/projectM/fonts" );
    
    globalPM->presetURL = (char *)malloc( sizeof( char ) * 512 );
    strcpy( globalPM->presetURL, "/etc/projectM/presets" );
   

    projectM_init( globalPM );
  
    projectM_resetGL( globalPM, wvw, wvh );
   
    /** Initialise the thread */
 
 while ( 1 ) {
        projectMEvent evt;
        projectMKeycode key;
        projectMModifier mod;

        /** Process SDL events */
        SDL_Event event;
        while ( SDL_PollEvent( &event ) ) {
            /** Translate into projectM codes and process */
            evt = sdl2pmEvent( event );
            key = sdl2pmKeycode( event.key.keysym.sym );
            mod = sdl2pmModifier( event.key.keysym.mod );

            if ( evt == PROJECTM_KEYDOWN ) {                 
     

	      if(key == SDLK_f)
		{
		  if (fullscreen==1)
		    {
		      fullscreen=0;
		      init_display(wvw,wvh,fullscreen); 
		      globalPM->fullscreen=0;
		      projectM_resetGL( globalPM, wvw, wvh );

		    }
		  else 
		    {
		      fullscreen=1;
		      init_display(fvw,fvh,fullscreen);
		      globalPM->fullscreen=1;
		      projectM_resetGL( globalPM, fvw, fvh );
		    }
		}
	      else  key_handler(evt,key,mod);

              }
	    else if ( evt == PROJECTM_VIDEORESIZE )
	      {
		wvw=event.resize.w;
		wvh=event.resize.h;
	       
		
		init_display(wvw,wvh,fullscreen);
		projectM_resetGL( globalPM, wvw, wvh ); 
 
              } 

          }

        /** Add the waveform data */
      

        /** Render the new frame */
	//	 strcpy(title,xmms_remote_get_playlist_title(projectM_vtable.xmms_session, xmms_remote_get_playlist_pos(projectM_vtable.xmms_session))); 

	 //printf("%s\n",title);
	// strcpy(globalPM->title,title);
	  renderFrame( globalPM );

      

        SDL_GL_SwapBuffers();
      }

 
		
  printf("Worker thread: Exiting\n");
}

static void projectM_xmms_init(void) 
{
  

  // read_cfg();
  printf("projectM plugin: Initializing\n");
  /* 
 if (SDL_Init (SDL_INIT_VIDEO) < 0) {
    printf ("Failed to initialize SDL\n"); 
    //    projectM_vtable.disable_plugin (&projectM_vtable);
    exit(-1);  
    return;
  }
  */
  SDL_EnableUNICODE(1);
  //title=malloc(sizeof(char)*512);
  /*
  pcmdataL=(double *)malloc(maxsamples*sizeof(double));
 pcmdataR=(double *)malloc(maxsamples*sizeof(double));
  */
  /* Preset loading function */
 // initPresetLoader();
  
  /* Load default preset directory */
  //  loadPresetDir("/etc/projectM/presets/"); 
  //loadPresetDir("/home/pete/103");
  //loadPresetDir("/home/carm/carm_presets");
  // loadPresetDir("/home/pete/good");
  //loadPresetDir("/home/carm/presets");
  //loadPresetDir("/mnt/huge/winxpsave/Program Files/winamp/Plugins/Milkdrop/2001");
 

  //glfInit();   
  //initMenu();  


  //  printf("%d %d\n", gx,gy(;

  //initPCM(maxsamples);
  //initBeatDetect();
 
  
  mutex = SDL_CreateMutex();

  worker_thread = SDL_CreateThread ((void *) worker_func, NULL);
 
}



static void projectM_cleanup(void)
{
  
  //free(pcmdataL); 
  //free(pcmdataR);
  //  free(title);
  
 
  //SDL_WaitThread(worker_thread, NULL);
  SDL_KillThread(worker_thread);
  printf("killed thread\n");

  SDL_DestroyMutex(mutex);
    printf("Destroy Mutex\n");
  SDL_Quit();
  
  printf("projectM plugin: Cleanup completed\n");
}
static void projectM_about(void)
{
  printf("projectM plugin: About\n");
}
static void projectM_configure(void)
{
  printf("projectM plugin: Configure\n");
}
static void projectM_playback_start(void)
{//thread_control = GO;
  printf("projectM plugin: Playback starting\n");
}
static void projectM_playback_stop(void)
{//thread_control = STOP;
  printf("projectM plugin: Playback stopping\n");
}
static void projectM_render_pcm(gint16 pcm_data[2][512])
{

  	SDL_mutexP(mutex);
       
        addPCM16(pcm_data);
	 
	SDL_mutexV(mutex);
	
}

static void projectM_render_freq(gint16 freq_data[2][256])
{
  printf("NO GOOD\n");
 }


void read_config()
{
  
   int n;
   
   char num[80];
   FILE *in; 

 if ((in = fopen("/etc/projectM/config", "r")) != 0) 
   {
     fgets(num, 80, in);  fgets(num, 80, in);  fgets(num, 80, in);
     if(fgets(num, 80, in) != NULL) sscanf (num, "%d", &texsize);  

     fgets(num, 80, in);
     if(fgets(num, 80, in) != NULL) sscanf (num, "%d", &gx);  

     fgets(num, 80, in);
     if(fgets(num, 80, in) != NULL) sscanf (num, "%d", &gy);   

     fgets(num, 80, in);
     if(fgets(num, 80, in) != NULL) sscanf (num, "%d", &wvw);  

     fgets(num, 80, in);
     if(fgets(num, 80, in) != NULL) sscanf (num, "%d", &wvh);  

     fgets(num, 80, in);
     if(fgets(num, 80, in) != NULL) sscanf (num, "%d", &fvw);  

     fgets(num, 80, in);
     if(fgets(num, 80, in) != NULL) sscanf (num, "%d", &fvh);  

     fgets(num, 80, in);
     if(fgets(num, 80, in) != NULL) sscanf (num, "%d", &fps);

     fgets(num, 80, in);
     if(fgets(num, 80, in) != NULL) sscanf (num, "%d", &fullscreen);
     /*
     fgets(num, 80, in);
     fgets(num, 80, in);
     
     n=0;
     while (num[n]!=' ' && num[n]!='\n' && n < 80 && num[n]!=EOF)
       {
	 	 disp[n]=num[n];
		 n++;
       }
     disp[n]=0;

    
     // sprintf(disp,"%s",num );
      setenv("DISPLAY",disp,1);
      printf("%s %d\n", disp,strlen(disp));
      setenv("LD_PRELOAD", "/usr/lib/tls/libGL.so.1.0.4496", 1);
     */
   fclose(in); 
     }
 
} 
