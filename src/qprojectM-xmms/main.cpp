/* 
projectM v1.01 - xmms-projectm.sourceforge.net
--------------------------------------------------

Lead Developers:  Carmelo Piccione (carmelo.piccione@gmail.com) &
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
#include <sys/types.h>
#include <string.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <xmms/util.h>
#include <pthread.h>

#include <xmms/xmmsctrl.h>
#include <math.h>
#include "ConfigFile.h"

#include <libprojectM/projectM.hpp>
#include <libprojectM/QProjectM_MainWindow.hpp>
#include <QApplication>

#define CONFIG_FILE "/share/projectM/config.inp"

// Forward declarations 
extern "C" void projectM_xmms_init(void); 
extern "C" void projectM_cleanup(void);
extern "C" void projectM_about(void);
extern "C" void projectM_configure(void);
extern "C" void projectM_playback_start(void);
extern "C" void projectM_playback_stop(void);
extern "C" void projectM_render_pcm(gint16 pcm_data[2][512]);
extern "C" void projectM_render_freq(gint16 pcm_data[2][256]);
extern "C" VisPlugin *get_vplugin_info();
//void *worker(void *);
int worker();
std::string read_config();
void saveSnapshotToFile();
//extern preset_t * active_preset;

//FILE * debugFile = fopen("./dwrite-dump", "wb");

// Callback functions
VisPlugin projectM_vtable = {
  NULL, // Handle, filled in by xmms
  NULL, // Filename, filled in by xmms
  0,                     // Session ID
  "projectM-qt v1.02",       // description
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

unsigned get_xmms_title() {
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
		  //globalPM->renderer->title = title;
			//globalPM->renderer->drawtitle = 1;

		  std::string titlepp(title);
		  globalPM->projectM_setTitle(titlepp);
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

int capture = 0;

 
QApplication *app;
QProjectM_MainWindow *mainWindow;
int retval;
pthread_t thread;

pid_t pID;

extern "C" void projectM_xmms_init(void) 
{    
  //  retval = pthread_create( &thread, NULL, worker, NULL);

  pID = clone();
   if (pID == 0)                // child
   {
      // Code only executed by child process

     worker();
     exit(1);
    }
    else if (pID < 0)            // failed to fork
    {
      std::cerr << "Failed to fork" << std::endl;
        exit(1);
        // Throw exception
    }
    else                                   // parent
    {
      // Code only executed by parent process

  
    }


}

bool initialized = false;

int worker()
//void *worker(void *)
{
  int argc; char **argv;
  argc = 0;
  
  
  // Start a new qapplication 
  QApplication app(argc,argv);
  //app = new QApplication(argc,argv);
  std::string config_file;
  config_file = read_config();
  
  mainWindow = new QProjectM_MainWindow(config_file);
  mainWindow->show();

  globalPM = mainWindow->getQProjectM(); 
  initialized = true;
  return app.exec();

}


extern "C" void projectM_cleanup(void)
{
  initialized = false;
  mainWindow->close(); ;
  //pthread_join(thread, NULL);

  //  delete(mainWindow);
  //  mainWindow = NULL;
  //  delete(app);
  //  app = NULL;
  
 // printf("projectM plugin: Cleanup completed\n");
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
  if (initialized)
    {
        globalPM->pcm->addPCM16(pcm_data);	      
	get_xmms_title();
    }
}

extern "C" void projectM_render_freq(gint16 freq_data[2][256])
{
  printf("NO GOOD\n");
 }

std::string read_config()
{

//   int n;
   
   char num[512];
   FILE *in; 
   FILE *out;

   char* home;
   char projectM_home[1024];
   char projectM_config[1024];

   strcpy(projectM_config, PROJECTM_PREFIX);
   strcpy(projectM_config+strlen(PROJECTM_PREFIX), CONFIG_FILE);
   projectM_config[strlen(PROJECTM_PREFIX)+strlen(CONFIG_FILE)]='\0';
   //printf("dir:%s \n",projectM_config);
   home=getenv("HOME");
   strcpy(projectM_home, home);
   strcpy(projectM_home+strlen(home), "/.projectM/config.inp");
   projectM_home[strlen(home)+strlen("/.projectM/config.inp")]='\0';

  
 if ((in = fopen(projectM_home, "r")) != 0) 
   {
     //printf("reading ~/.projectM/config.inp \n");
     fclose(in);
     return std::string(projectM_home);
   }
 else
   {
     printf("trying to create ~/.projectM/config.inp \n");

     strcpy(projectM_home, home);
     strcpy(projectM_home+strlen(home), "/.projectM");
     projectM_home[strlen(home)+strlen("/.projectM")]='\0';
     mkdir(projectM_home,0755);

     strcpy(projectM_home, home);
     strcpy(projectM_home+strlen(home), "/.projectM/config.inp");
     projectM_home[strlen(home)+strlen("/.projectM/config.inp")]='\0';

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
	       { 
		 printf("created ~/.projectM/config.inp successfully\n");  
		 fclose(in);
		 return std::string(projectM_home);
	       }
	     else{printf("This shouldn't happen, using implementation defualts\n");abort();}
	   }
	 else{printf("Cannot find projectM default config, using implementation defaults\n");abort();}
       }
     else
       {
	 printf("Cannot create ~/.projectM/config.inp, using default config file\n");
	 if ((in = fopen(projectM_config, "r")) != 0) 
	   { printf("Successfully opened default config file\n");
	     fclose(in);
	     return std::string(projectM_config);}
	 else{ printf("Using implementation defaults, your system is really messed up, I'm suprised we even got this far\n");  abort();}
       }

   }

 abort();
}



