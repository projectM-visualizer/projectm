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

and some beat detection code was inspired by Frederic Patin @
www.gamedev.net/reference/programming/features/beatdetection/

*/

#include <stdio.h>
#include <xmms/plugin.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <xmms/util.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <xmms/xmmsctrl.h>
#include <math.h>

#include <libprojectM/BeatDetect.hpp>
#include <libprojectM/PCM.hpp>
#include <libprojectM/projectM.hpp>
#include <libprojectM/console_interface.h>
#include "sdltoprojectM.h"
#include "video_init.h"

#define CONFIG_FILE "/share/projectM/config.1.00"
#define FONTS_DIR "/share/projectM/fonts"

// Forward declarations 
extern "C" void projectM_xmms_init(void); 
extern "C" void projectM_cleanup(void);
extern "C" void projectM_about(void);
extern "C" void projectM_configure(void);
extern "C" void projectM_playback_start(void);
extern "C" void projectM_playback_stop(void);
extern "C" void projectM_render_pcm(gint16 pcm_data[2][512]);
extern "C" void projectM_render_freq(gint16 pcm_data[2][256]);
extern "C" int worker_func(void*);
extern "C" VisPlugin *get_vplugin_info();
void read_config();

char preset_dir[1024];
char fonts_dir[1024];

//extern preset_t * active_preset;

FILE * debugFile = fopen("./dwrite-dump", "wb");

// Callback functions
VisPlugin projectM_vtable = {
  NULL, // Handle, filled in by xmms
  NULL, // Filename, filled in by xmms
  0,                     // Session ID
  "projectM v1.00",       // description
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
extern "C" VisPlugin *get_vplugin_info(void)
{
  return &projectM_vtable;
}

// Our worker thread
SDL_Thread *worker_thread;
SDL_sem *sem;
SDL_mutex *mutex;

SDL_Event event;

SDL_Surface *screen;
//SDL_RenderTarget *RenderTarget = NULL;
//GLuint RenderTargetTextureID;

projectM * globalPM = NULL;

int maxsamples=512;

int texsize=512;
int gx=32,gy=24;
int wvw=400,wvh=400;
int fvw=1024,fvh=768;
int fps=35, fullscreen=0;

// char *title;

gint disable_projectm(void *something) {
	projectM_vtable.disable_plugin(&projectM_vtable);
	return 0;
}

Uint32 get_xmms_title(Uint32 something, void *somethingelse) {
	static char check_title = 1;
	static int last_pos;
	static char *last_title = NULL;
	int pos;
	char *title = NULL;

	//Nice optimization, but we want the title no matter what so I can display it when the song changes
#if 0
	if(!(globalPM->showtitle%2)) {
		/* Repeat less often when not showing title */
		return 1000;
	}
#endif

	pos = xmms_remote_get_playlist_pos(projectM_vtable.xmms_session);
	/* Only check every 1 second for title change, otherwise check pos */
	if(check_title || pos != last_pos) {
		title = xmms_remote_get_playlist_title(
				projectM_vtable.xmms_session, pos);
		if(title && (!last_title || strcmp(last_title,title))) {
			globalPM->renderer->title = title;
			globalPM->renderer->drawtitle = 1;
			g_free(last_title);
			last_title = title;
		} else if(title && last_title != title) {
			/* New copy of last title */
			g_free(title);
		}
		check_title = !check_title;
	}
	last_pos = pos;
	/* Repeat every 500ms */
	return 500;
}


int worker_func(void*)
{ 
 char projectM_data[1024]; 
 SDL_TimerID title_timer = NULL;
 read_config();
  init_display(wvw,wvh,&fvw,&fvh,fullscreen);   
  SDL_WM_SetCaption("projectM v1.00", "projectM v1.00");


  /** Initialise projectM */
    
  globalPM = new projectM();
  globalPM->projectM_reset();
  
  globalPM->fullscreen = fullscreen;

  
  	strcpy(projectM_data, PROJECTM_PREFIX);
	strcpy(projectM_data+strlen(PROJECTM_PREFIX), FONTS_DIR);
	projectM_data[strlen(PROJECTM_PREFIX)+strlen(FONTS_DIR)]='\0';
       
	globalPM->fontURL = (char *)malloc( sizeof( char ) * 1024 );
	strcpy( globalPM->fontURL, projectM_data );	
	
	globalPM->presetURL = (char *)malloc( sizeof( char ) * 1024 );
	strcpy( globalPM->presetURL, preset_dir );	
 
      
    globalPM->projectM_init(gx, gy, fps, texsize, fullscreen ? fvw:wvw, fullscreen? fvh:wvh);
  
    globalPM->projectM_resetGL( wvw, wvh );

    title_timer = SDL_AddTimer(500, get_xmms_title, NULL);
    /** Initialise the thread */
  SDL_SemTryWait(sem);
  while ( SDL_SemTryWait(sem) ) {
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
	   
	      if(key == PROJECTM_K_f)
		{
		 

		 int w, h;
                    if (fullscreen == 0) {
                        w = fvw;
                        h = fvh;
                    } else {
                        w = wvw;
                        h = wvh;
                    }
                    globalPM->fullscreen = fullscreen ^= 1;
                    resize_display(w, h, fullscreen);
                    globalPM->projectM_resetGL( w, h ); 
                }
	      else  globalPM->key_handler(evt,key,mod);

              }
	    else if ( evt == PROJECTM_VIDEORESIZE )
	      {

	       

		wvw=event.resize.w;
		wvh=event.resize.h;
	       
		
		resize_display(wvw,wvh,fullscreen);
		globalPM->projectM_resetGL( wvw, wvh ); 
 
              } 
	    else if ( evt == PROJECTM_VIDEOQUIT ) {
	      
	      (void) gtk_idle_add (disable_projectm, NULL);
	    }
	}
	
        /** Add the waveform data */
      

        /** Render the new frame */
	//	 strcpy(title,xmms_remote_get_playlist_title(projectM_vtable.xmms_session, xmms_remote_get_playlist_pos(projectM_vtable.xmms_session))); 

	 //printf("%s\n",title);
	// strcpy(globalPM->title,title);
	  globalPM->renderFrame();

      

        SDL_GL_SwapBuffers();
      }

 
		
  printf("Worker thread: Exiting\n");
 if(title_timer) SDL_RemoveTimer(title_timer);
  g_free(globalPM->renderer->title);
  free(globalPM->presetURL);
  free(globalPM->fontURL);
  free(globalPM);
  close_display();
}

extern "C" void projectM_xmms_init(void) 
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

  worker_thread = SDL_CreateThread ( *worker_func, NULL);
 
}



extern "C"void projectM_cleanup(void)
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
extern "C" void projectM_about(void)
{
  printf("projectM plugin: About\n");
}
extern "C" void projectM_configure(void)
{
  printf("projectM plugin: Configure\n");
}
extern "C" void projectM_playback_start(void)
{//thread_control = GO;
  printf("projectM plugin: Playback starting\n");
}
extern "C" void projectM_playback_stop(void)
{//thread_control = STOP;
  printf("projectM plugin: Playback stopping\n");
}
extern "C" void projectM_render_pcm(gint16 pcm_data[2][512])
{
  	SDL_mutexP(mutex);
      
       	/// @bug sperl: might want to look at this. crashes here sometimes 
        globalPM->beatDetect->pcm->addPCM16(pcm_data);
	 
	SDL_mutexV(mutex);
	
}

extern "C" void projectM_render_freq(gint16 freq_data[2][256])
{
  printf("NO GOOD\n");
 }

void read_config()
{

   int n;
   
   char num[512];
   FILE *in; 
   FILE *out;

   char* home;
   char projectM_home[1024];
   char projectM_config[1024];

   strcpy(projectM_config, PROJECTM_PREFIX);
   strcpy(projectM_config+strlen(PROJECTM_PREFIX), CONFIG_FILE);
   projectM_config[strlen(PROJECTM_PREFIX)+strlen(CONFIG_FILE)]='\0';
   printf("dir:%s \n",projectM_config);
   home=getenv("HOME");
   strcpy(projectM_home, home);
   strcpy(projectM_home+strlen(home), "/.projectM/config.1.00");
   projectM_home[strlen(home)+strlen("/.projectM/config.1.00")]='\0';

  
 if ((in = fopen(projectM_home, "r")) != 0) 
   {
     printf("reading ~/.projectM/config.1.00 \n");
   }
 else
   {
     printf("trying to create ~/.projectM/config.1.00 \n");

     strcpy(projectM_home, home);
     strcpy(projectM_home+strlen(home), "/.projectM");
     projectM_home[strlen(home)+strlen("/.projectM")]='\0';
     mkdir(projectM_home,0755);

     strcpy(projectM_home, home);
     strcpy(projectM_home+strlen(home), "/.projectM/config.1.00");
     projectM_home[strlen(home)+strlen("/.projectM/config.1.00")]='\0';
     
     if((out = fopen(projectM_home,"w"))!=0)
       {
	
	 if ((in = fopen(projectM_config, "r")) != 0) 
	   {
	     
	     while(fgets(num,80,in)!=NULL)
	       {
		 fputs(num,out);
	       }
	     fclose(in);
	     fclose(out);
	    

	     if ((in = fopen(projectM_home, "r")) != 0) 
	       { printf("created ~/.projectM/config.1.00 successfully\n");  }
	     else{printf("This shouldn't happen, using implementation defualts\n");return;}
	   }
	 else{printf("Cannot find projectM default config, using implementation defaults\n");return;}
       }
     else
       {
	 printf("Cannot create ~/.projectM/config.1.00, using default config file\n");
	 if ((in = fopen(projectM_config, "r")) != 0) 
	   { printf("Successfully opened default config file\n");}
	 else{ printf("Using implementation defaults, your system is really messed up, I'm suprised we even got this far\n");	   return;}
	   
       }

   }



     fgets(num, 512, in);  fgets(num, 512, in);  fgets(num, 512, in);
     if(fgets(num, 512, in) != NULL) sscanf (num, "%d", &texsize);  

     fgets(num, 512, in);
     if(fgets(num, 512, in) != NULL) sscanf (num, "%d", &gx);  

     fgets(num, 512, in);
     if(fgets(num, 512, in) != NULL) sscanf (num, "%d", &gy);   

     fgets(num, 512, in);
     if(fgets(num, 512, in) != NULL) sscanf (num, "%d", &wvw);  

     fgets(num, 512, in);
     if(fgets(num, 512, in) != NULL) sscanf (num, "%d", &wvh);  
   
     fgets(num, 512, in);
     if(fgets(num, 512, in) != NULL) sscanf (num, "%d", &fps);

     fgets(num, 512, in);
     if(fgets(num, 512, in) != NULL) sscanf (num, "%d", &fullscreen);
     fgets(num, 512, in);

     if(fgets(num, 512, in) != NULL)  strcpy(preset_dir, num);
     preset_dir[strlen(preset_dir)-1]='\0';
    
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




