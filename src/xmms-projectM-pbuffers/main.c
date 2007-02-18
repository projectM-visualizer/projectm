/* 
projectM v0.99 - xmms-projectm.sourceforge.net
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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <xmms/xmmsctrl.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <pthread.h>

#include <projectM/projectM.h>
#include <projectM/console_interface.h>
#include "xtoprojectm.h"

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
static void make_window(const char *name, int x, int y, int width, int height);
static void make_pbuffer(int width, int height);
static int current_time(void);
static void lock(void);
static void unlock(void);

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


static Display *dpy        = NULL;
static Window win          = 0;
static GLXPbuffer pbuf     = 0;
static GLXContext ctx_win  = NULL,
                  ctx_pbuf = NULL;

  pthread_t thread;  

projectM_t *globalPM = NULL;

int maxsamples=512;

int texsize=512;
int gx=32,gy=24;
int wvw=512,wvh=512;
int fvw=1024,fvh=768;
int fps=30, fullscreen=0;
char *disp;
// char *title;
int run=1;

int disable_projectm(void) {
	projectM_vtable.disable_plugin(&projectM_vtable);
	return 0;
}

void worker_func()
{ 
   
   char projectM_data[PATH_MAX];
   char *dpyName       = NULL;
   GLboolean printInfo = GL_FALSE;
   run=1;
 read_config();
 printf("texsize:%d\n",texsize);
   /* parse arguments */
  
   
   /* open the display */
   dpy = XOpenDisplay(dpyName);
   if (!dpy) {
      printf("Error: couldn't open display %s\n", dpyName);
      return -1;
   }

   /* setup and initial window, pbuffer and texture */
   make_window("projectM 0.99", 0, 0, wvw,wvh);
   
   XMapWindow(dpy, win);

   if (printInfo) {
      printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
      printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
      printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
      printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
   }

    make_pbuffer(texsize,texsize);
   
   glXMakeCurrent(dpy, win, ctx_win);

  
   //glPixelStorei(GL_UNPACK_ALIGNMENT,1);

   

   /* stuff for pbuffer */
#ifdef LINUX
     glXMakeCurrent(dpy, pbuf, ctx_pbuf);

     //glPixelStorei(GL_UNPACK_ALIGNMENT,1);
     glViewport( 0, 0, texsize, texsize );
     glClearColor(0,0,0,0);
#endif

  /** Initialise projectM */
    
  globalPM = (projectM_t *)malloc( sizeof( projectM_t ) );

 projectM_reset( globalPM );
  
  globalPM->fullscreen = fullscreen;
  globalPM->renderTarget->texsize = texsize;
  globalPM->gx=gx;
  globalPM->gy=gy;
  globalPM->fps=fps;

    globalPM->renderTarget->usePbuffers =  1;  
    globalPM->renderTarget->unlock_func =  &unlock;  
    globalPM->renderTarget->lock_func = &lock;
 
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
    
    /** Initialise the thread */
 
    
    while ( run ) {
      projectMEvent evt;
      projectMKeycode key;
      projectMModifier mod;
      
      
      while (XPending(dpy) > 0) {
	XEvent event;
	XNextEvent(dpy, &event);

	evt = x2pmEvent( event );
	key = x2pmKeycode( XLookupKeysym(&event.xkey, 0) );
	mod = x2pmModifier(0 );

	switch (evt) {
	
	case PROJECTM_VIDEORESIZE:
	  glXMakeCurrent(dpy, win, ctx_win);
	  projectM_resetGL(globalPM,event.xconfigure.width, event.xconfigure.height);       
	  break;
	case PROJECTM_VIDEOQUIT:
	  if (!strcmp( XGetAtomName( dpy, event.xclient.message_type ), "WM_PROTOCOLS" ) )
	  { (void) gtk_idle_add (disable_projectm, NULL);}
	  
	  break;
	case PROJECTM_KEYDOWN:
	  
	  key_handler(globalPM,evt,key,mod);
	  break;
	  
	default: 
	  break; 
      
	}
      }
      
      /* next frame */
     

      //drawGears();
        
      renderFrame( globalPM );
  
      glXSwapBuffers(dpy, win);
      
    }
					    

    
 
		
  printf("Worker thread: Exiting\n");
  free(globalPM->presetURL);
  free(globalPM->fontURL);
  free(globalPM);
  
   glXDestroyContext(dpy, ctx_win);
   XDestroyWindow(dpy, win);

   glXDestroyContext(dpy, ctx_pbuf);
   glXDestroyPbuffer(dpy, pbuf);

   XCloseDisplay(dpy);


}

static void projectM_xmms_init(void) 
{
  
  int  iret;
  
  printf("projectM plugin: Initializing\n");
  // run=1;
  iret = pthread_create( &thread, NULL, worker_func, NULL);
   
}



static void projectM_cleanup(void)
{
  
  //free(pcmdataL); 
  //free(pcmdataR);
  //  free(title);
  run=0;
  pthread_join( thread, NULL);
//  printf("killed thread\n");

    
  
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
       
  addPCM16Data(pcm_data,512);
	 	
}

static void projectM_render_freq(gint16 freq_data[2][256])
{
  printf("NO GOOD\n");
 }



/*
 * Create an RGBA, double-buffered window.
 */

static void make_window(const char *name, int x, int y, int width, int height)
{
   int attrib[] = { GLX_RGBA,
		    GLX_RED_SIZE, 1,
		    GLX_GREEN_SIZE, 1,
		    GLX_BLUE_SIZE, 1,
		    GLX_DOUBLEBUFFER,
		    GLX_DEPTH_SIZE, 1,
		    None };
   int scrnum;
   XSetWindowAttributes attr;
   unsigned long mask;
   Window root;
   XVisualInfo *visinfo;
   Atom wm_delete;

   scrnum = DefaultScreen( dpy );
   root   = RootWindow( dpy, scrnum );

   visinfo = glXChooseVisual( dpy, scrnum, attrib );
   if (!visinfo) {
      printf("Error: couldn't get an RGBA, Double-buffered visual\n");
      exit(1);
   }

   /* window attributes */
   attr.background_pixel = 0;
   attr.border_pixel = 0;
   attr.colormap = XCreateColormap( dpy, root, visinfo->visual, AllocNone);
   attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

   win = XCreateWindow( dpy, root, 0, 0, width, height,
		        0, visinfo->depth, InputOutput,
		        visinfo->visual, mask, &attr );

   /* set hints and properties */
   {
      XSizeHints sizehints;
      sizehints.x = x;
      sizehints.y = y;
      sizehints.width  = width;
      sizehints.height = height;
      sizehints.flags = USSize | USPosition;
      XSetNormalHints(dpy, win, &sizehints);
      XSetStandardProperties(dpy, win, name, name,
                              None, (char **)NULL, 0, &sizehints);
   }

   wm_delete = XInternAtom( dpy, "WM_DELETE_WINDOW", 1 );
   XSetWMProtocols( dpy, win, &wm_delete, 1 );


   ctx_win = glXCreateContext( dpy, visinfo, NULL, GL_TRUE );
   if (!ctx_win) {
      printf("Error: glXCreateContext failed\n");
      exit(1);
   }

   XFree(visinfo);

   glXMakeCurrent(dpy, win, ctx_win);


}

/*
 * Create an RGBA, double-buffered pbuffer.
 */
static void make_pbuffer(int width, int height)
{
   int scrnum;
   GLXFBConfig *fbconfig;
   XVisualInfo *visinfo;
   int nitems;

   int attrib[] = {
      GLX_DOUBLEBUFFER,  False,
      GLX_RED_SIZE,      1,
      GLX_GREEN_SIZE,    1,
      GLX_BLUE_SIZE,     1,
      GLX_DEPTH_SIZE,    1,
      GLX_RENDER_TYPE,   GLX_RGBA_BIT,
      GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT | GLX_WINDOW_BIT,
      None
   };
   int pbufAttrib[] = {
      GLX_PBUFFER_WIDTH,   width,
      GLX_PBUFFER_HEIGHT,  height,
      GLX_LARGEST_PBUFFER, False,
      None
   };


   scrnum   = DefaultScreen( dpy );

   fbconfig = glXChooseFBConfig(dpy,
                                scrnum,
                                attrib,
                                &nitems);
   if (NULL == fbconfig) {
      fprintf(stderr,"Error: couldn't get fbconfig\n");
      exit(1);
   }

   pbuf = glXCreatePbuffer(dpy, fbconfig[0], pbufAttrib);

   visinfo = glXGetVisualFromFBConfig(dpy, fbconfig[0]);
   if (!visinfo) {
      fprintf(stderr, "Error: couldn't get an RGBA, double-buffered visual\n");
      exit(1);
   }

   ctx_pbuf = glXCreateContext( dpy, visinfo, ctx_win, GL_TRUE );
   if (!ctx_pbuf) {
      fprintf(stderr, "Error: glXCreateContext failed\n");
      exit(1);
   }

   XFree(fbconfig);
   XFree(visinfo);
}

static int
current_time(void)
{
   struct timeval tv;
   struct timezone tz;
   (void) gettimeofday(&tv, &tz);
   return (int) tv.tv_sec;
}

static void lock(void)
{
   glXMakeCurrent(dpy, pbuf, ctx_pbuf);

 
  
   //PASS1

  
}


static void unlock(void)
{


    glXMakeCurrent(dpy, win, ctx_win);

   
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
