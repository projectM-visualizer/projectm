#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <libvisual/libvisual.h>
#include <projectM/projectM.h>
#include <projectM/console_interface.h>
#include "lvtoprojectM.h"

#if HAVE_CONFIG_H
#include <config.h>
#endif
#define CONFIG_FILE "/config"
#define PRESETS_DIR "/presets"
#define FONTS_DIR "/fonts"

void read_config();

int texsize=512;
int gx=32,gy=24;
int wvw=512,wvh=512;
int fvw=1024,fvh=768;
int fps=60, fullscreen=0;

projectM_t *globalPM;

/* Private context sensitive data goes here, */
typedef struct {
	projectM_t *PM;
} ProjectmPrivate;

int lv_projectm_init (VisPluginData *plugin);
int lv_projectm_cleanup (VisPluginData *plugin);
int lv_projectm_requisition (VisPluginData *plugin, int *width, int *height);
int lv_projectm_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_projectm_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_projectm_palette (VisPluginData *plugin);
int lv_projectm_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

/* Main plugin stuff */
/* The get_plugin_info function provides the libvisual plugin registry, and plugin loader
 * with the very basic plugin information */
const VisPluginInfo *get_plugin_info (int *count)
{
	/* Initialize the plugin specific data structure
	 * with pointers to the functions that represent
	 * the plugin interface it's implementation, more info:
	 * http://libvisual.sourceforge.net/newdocs/docs/html/struct__VisActorPlugin.html */
	static VisActorPlugin actor[] = {{
		.requisition = lv_projectm_requisition,
		.palette = lv_projectm_palette,
		.render = lv_projectm_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_GL /* We want GL clearly */
	}};

	/* This is the non plugin specific data structure, containing
	 * general informatie about the plugin itself, more info:
	 * http://libvisual.sourceforge.net/newdocs/docs/html/struct__VisPluginInfo.html */
	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "projectm",
		.name = "libvisual ProjectM",
		.author = "Peter Sperl",
		.version = "0.98",
		.about = "projectM",
		.help =  "",

		.init = lv_projectm_init,
		.cleanup = lv_projectm_cleanup,
		.events = lv_projectm_events,

		.plugin = (void *) &actor[0]
	}};

	*count = sizeof (info) / sizeof (*info);

	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_ALPHA_SIZE, 8);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_DEPTH_SIZE, 16);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER, 1);

	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_RED_SIZE, 8);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_GREEN_SIZE, 8);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_BLUE_SIZE, 8);

	return info;
}

/* This function is called before we really start rendering, it's the init function */
int lv_projectm_init (VisPluginData *plugin)
{
        char projectM_data[1024];
	ProjectmPrivate *priv;

        read_config();
	/* Allocate the projectm private data structure, and register it as a private */
	priv = visual_mem_new0 (ProjectmPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	globalPM = visual_mem_new0 (projectM_t, 1);
	projectM_reset( globalPM );

	globalPM->fullscreen = 0; //fullscreen;
	globalPM->renderTarget->texsize = texsize;
	globalPM->renderTarget->usePbuffers = 0;
	globalPM->gx=gx;
	globalPM->gy=gy;
	globalPM->fps=fps;


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

	return 0;
}

int lv_projectm_cleanup (VisPluginData *plugin)
{
	ProjectmPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	/* Cleanup, and thus also free our private */
	visual_mem_free (priv);
	visual_mem_free (globalPM);
	return 0;
}

/* This is used to ask a plugin if it can handle a certain size, and if not, to
 * set the size it wants by putting a value in width, height that represents the
 * required size */
int lv_projectm_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	/* Size negotiate with the window */
	reqw = *width;
	reqh = *height;

	if (reqw < 64)
		reqw = 64;

	if (reqh < 64)
		reqh = 64;

	*width = reqw;
	*height = reqh;

	return 0;
}

int lv_projectm_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	ProjectmPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_video_set_dimension (video, width, height);

	projectM_resetGL( globalPM, width, height ); 

	return 0;
}

/* This is the main event loop, where all kind of events can be handled, more information
 * regarding these can be found at:
 * http://libvisual.sourceforge.net/newdocs/docs/html/union__VisEvent.html 
 */
int lv_projectm_events (VisPluginData *plugin, VisEventQueue *events)
{
	ProjectmPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;
	VisParamEntry *param;

	projectMEvent evt;
        projectMKeycode key;
        projectMModifier mod;
	
	while (visual_event_queue_poll (events, &ev)) 
	  {
	    switch (ev.type) 
	      {
	      case VISUAL_EVENT_KEYUP:

		evt = lv2pmEvent( ev.type );
		key = lv2pmKeycode( ev.event.keyboard.keysym.sym );
		mod = lv2pmModifier( ev.event.keyboard.keysym.mod );
		key_handler(globalPM, PROJECTM_KEYDOWN, key,mod);

		break;
	      case VISUAL_EVENT_RESIZE:
		lv_projectm_dimension (plugin, ev.event.resize.video,
				       ev.event.resize.width, ev.event.resize.height);
		break;
		
	      default: /* to avoid warnings */
		break;
	      }
	}
	
	return 0;
}

/* Using this function we can update the palette when we're in 8bits mode, which
 * we aren't with projectm, so just ignore :) */
VisPalette *lv_projectm_palette (VisPluginData *plugin)
{
	return NULL;
}

/* This is where the real rendering happens! This function is what we call, many times
 * a second to get our graphical frames. */
int lv_projectm_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	ProjectmPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisBuffer pcmb;
	float pcm[2][512];
	//short pcms[2][512];
	int i;

	visual_buffer_set_data_pair (&pcmb, pcm[0], sizeof (pcm[0]));
	visual_audio_get_sample (audio, &pcmb, VISUAL_AUDIO_CHANNEL_LEFT);

	visual_buffer_set_data_pair (&pcmb, pcm[1], sizeof (pcm[1]));
	visual_audio_get_sample (audio, &pcmb, VISUAL_AUDIO_CHANNEL_RIGHT);

	/*
	for (i = 0; i < 512; i++) {
		pcms[0][i] = pcm[0][i] * 32768.0;
		pcms[1][i] = pcm[1][i] * 32768.0;
	}

	addPCM16Data(pcms,512);
	*/

	addPCMfloat(pcm,512);
	renderFrame( globalPM );

	return 0;
}



void read_config()
{

   int n;
   
   char num[80];
   FILE *in; 
   FILE *out;

   char* home;
   char projectM_home[1024];
   char projectM_config[1024];

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
