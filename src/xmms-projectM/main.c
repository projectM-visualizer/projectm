/* 
xmms-projectM v0.99 - xmms-projectm.sourceforge.net
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
#include <gtk/gtk.h>
#include <xmms/util.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <xmms/xmmsctrl.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <projectM/projectM.h>
#include <projectM/console_interface.h>
#include "sdltoprojectM.h"
#include "video_init.h"

#if HAVE_CONFIG_H
#include <config.h>
#endif
#define CONFIG_FILE "/config"
#define PRESETS_DIR "/presets"
#define FONTS_DIR "/fonts"

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
  "projectM v0.99",       // description
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

SDL_sem *sem;

SDL_Event event;

SDL_Surface *screen;
//SDL_RenderTarget *RenderTarget = NULL;
//GLuint RenderTargetTextureID;

projectM_t *globalPM = NULL;

int maxsamples=512;

int texsize=512;
int gx=32,gy=24;
int wvw=640,wvh=480;
int fvw=1280,fvh=960;
int fps=30, fullscreen=0;
char *disp;

int disable_projectm(void) {
	projectM_vtable.disable_plugin(&projectM_vtable);
	return 0;
}

int get_xmms_title(void) {
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
			globalPM->title = title;
			globalPM->drawtitle = 1;
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

void worker_func()
{ 
  char projectM_data[PATH_MAX];
 
  SDL_TimerID title_timer = NULL;

 
  read_config();
 
  init_display(wvw,wvh,fullscreen);

  SDL_WM_SetCaption("projectM v0.99", "projectM v0.99");
 

  /** Initialise projectM */
    
  globalPM = (projectM_t *)malloc( sizeof( projectM_t ) );

  projectM_reset( globalPM );
  
  globalPM->fullscreen = fullscreen;
  globalPM->renderTarget->texsize = texsize;
  globalPM->gx=gx;
  globalPM->gy=gy;
  globalPM->fps=fps;
  globalPM->renderTarget->usePbuffers=0;
  
  strcpy(projectM_data, PROJECTM_DATADIR);
  strcpy(projectM_data+strlen(PROJECTM_DATADIR), FONTS_DIR);
  projectM_data[strlen(PROJECTM_DATADIR)+strlen(FONTS_DIR)]='\0';
  
  globalPM->fontURL = (char *)malloc( sizeof( char ) * 512 );
  strcpy( globalPM->fontURL, projectM_data );
  
  strcpy(projectM_data+strlen(PROJECTM_DATADIR), PRESETS_DIR);
  projectM_data[strlen(PROJECTM_DATADIR)+strlen(PRESETS_DIR)]='\0';
  
  globalPM->presetURL = (char *)malloc( sizeof( char ) * 512 );
  strcpy( globalPM->presetURL, projectM_data );
  
  
  projectM_init( globalPM );
  
  projectM_resetGL( globalPM, wvw, wvh );

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
     

                if(key == SDLK_f)
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
                    projectM_resetGL( globalPM, w, h ); 
                }
                else  key_handler(globalPM,evt,key,mod);

            }
            else if ( evt == PROJECTM_VIDEORESIZE )
            {
                wvw=event.resize.w;
                wvh=event.resize.h;
                resize_display(wvw, wvh, 0);
                projectM_resetGL( globalPM, wvw, wvh ); 

            } 
            else if ( evt == PROJECTM_VIDEOQUIT ) {

	(void) gtk_idle_add (disable_projectm, NULL);
	/*				if(quit_timer == NULL)
	                quit_timer = SDL_AddTimer(500, disable_projectm, NULL);*/
            }

        }
             

        /** Render the new frame */

	renderFrame( globalPM );
 
        SDL_GL_SwapBuffers();
      }

 
		
  printf("Worker thread: Exiting\n"); 
  if(title_timer) SDL_RemoveTimer(title_timer);
  g_free(globalPM->title);
  free(globalPM->presetURL);
  free(globalPM->fontURL);
  free(globalPM);
  close_display();
}

static void projectM_xmms_init(void) 
{
  
  printf("projectM plugin: Initializing\n");

  SDL_EnableUNICODE(1); 
  
  mutex = SDL_CreateMutex();

  sem = SDL_CreateSemaphore(1);

  worker_thread = SDL_CreateThread ((void *) worker_func, NULL);
 
}



static void projectM_cleanup(void)
{
   
    SDL_SemPost(sem);
    SDL_WaitThread(worker_thread, NULL);

    SDL_DestroySemaphore(sem);
    printf("Destroy Semaphore\n");
    SDL_DestroyMutex(mutex);
    printf("Destroy Mutex\n");
  
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
{
  printf("projectM plugin: Playback starting\n");
}
static void projectM_playback_stop(void)
{
  printf("projectM plugin: Playback stopping\n");
}
static void projectM_render_pcm(gint16 pcm_data[2][512])
{

    if (0 < SDL_SemValue(sem)) return;
  	SDL_mutexP(mutex);
       
        addPCM16Data(pcm_data,512);
	 
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
   FILE *out;

   char* home;
   char projectM_home[PATH_MAX];
   char projectM_config[PATH_MAX];

   strcpy(projectM_config, PROJECTM_DATADIR);
   strcpy(projectM_config+strlen(PROJECTM_DATADIR), CONFIG_FILE);
   projectM_config[strlen(PROJECTM_DATADIR)+strlen(CONFIG_FILE)]='\0';

   home=getenv("HOME");
   strcpy(projectM_home, home);
   strcpy(projectM_home+strlen(home), "/.projectM/config");
   projectM_home[strlen(home)+strlen("/.projectM/config")]='\0';

  
 if ((in = fopen(projectM_home, "r")) != 0) 
   {
     printf("reading ~/.projectM/config \n");
   }
 else
   {
     printf("trying to create ~/.projectM/config \n");

     strcpy(projectM_home, home);
     strcpy(projectM_home+strlen(home), "/.projectM");
     projectM_home[strlen(home)+strlen("/.projectM")]='\0';
     mkdir(projectM_home,0755);

     strcpy(projectM_home, home);
     strcpy(projectM_home+strlen(home), "/.projectM/config");
     projectM_home[strlen(home)+strlen("/.projectM/config")]='\0';
     
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
	       { printf("created ~/.projectM/config successfully\n");  }
	     else{printf("This shouldn't happen, using implementation defualts\n");return;}
	   }
	 else{printf("Cannot find projectM default config, using implementation defaults\n");return;}
       }
     else
       {
	 printf("Cannot create ~/.projectM/config, using default config file\n");
	 if ((in = fopen(projectM_config, "r")) != 0) 
	   { printf("Successfully opened default config file\n");}
	 else{ printf("Using implementation defaults, your system is really messed up, I'm suprised we even got this far\n");	   return;}
	   
       }

   }



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
