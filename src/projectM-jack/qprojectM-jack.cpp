/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <projectM.h>
#include <qprojectm_mainwindow.hpp>
#include <QApplication>

#include "ConfigFile.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h> // for mkdir
#include <jack/jack.h>

#ifdef HTTP_REMOTE_CONTROL
#include "HTTPRemoteControl.h"
#endif

#define QPROJECTM_JACK_CONFIG_FILE "/config.inp"

QString read_config();

jack_port_t *input_port;
jack_client_t *client;
jack_default_audio_sample_t out[1024*1000];

#ifdef DEBUG
FILE *debugFile = NULL;
#endif

volatile enum ClientState {
	Init,
	Run,
	Exit
} client_state = Init;


projectm* globalPM = nullptr;

int dumpFrame = 0;
int frameNumber = 0;

int texsize=512;
int gx=32,gy=24;
int wvw=512,wvh=512;
int fvw=1024,fvh=768;
int fps=30, fullscreen=0;

class ProjectMApplication : public QApplication {
	public:
		ProjectMApplication(int& argc, char ** argv) :
			QApplication(argc, argv) { }
		virtual ~ProjectMApplication() { }

		// catch exceptions which are thrown in slots
		virtual bool notify(QObject * receiver, QEvent * event) {
			try {
				return QApplication::notify(receiver, event);
			} catch (std::exception& e) {
				qCritical() << "Exception thrown:" << e.what();
			}
			return false;
		}
};

QString read_config()
{

   char num[512];
   FILE *f_in;
   FILE *f_out;

   char * home;
   char projectM_home[1024];
   char projectM_config[1024];

   int len;
   strcpy(projectM_config, PROJECTM_PREFIX);
   strcpy(projectM_config + (len = strlen(PROJECTM_PREFIX)), "/");

   strcpy(projectM_config+(len += strlen("/")), RESOURCE_PREFIX);
   strcpy(projectM_config+(len += strlen(RESOURCE_PREFIX)), QPROJECTM_JACK_CONFIG_FILE);
   projectM_config[len += strlen(QPROJECTM_JACK_CONFIG_FILE)]='\0';

   printf("dir:%s \n",projectM_config);
   home=getenv("HOME");
   strcpy(projectM_home, home);
   strcpy(projectM_home+strlen(home), "/.projectM/config.inp");
   projectM_home[strlen(home)+strlen("/.projectM/config.inp")]='\0';


 if ((f_in = fopen(projectM_home, "r")) != 0)
   {
     printf("reading ~/.projectM/config.inp \n");
     fclose(f_in);
     return projectM_home;
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

     if((f_out = fopen(projectM_home,"w"))!=0)
       {

	 if ((f_in = fopen(projectM_config, "r")) != 0)
	   {

	     while(fgets(num,80,f_in)!=NULL)
	       {
		 fputs(num,f_out);
	       }
	     fclose(f_in);
	     fclose(f_out);


	     if ((f_in = fopen(projectM_home, "r")) != 0)
	       {
		 printf("created ~/.projectM/config.inp successfully\n");
		 fclose(f_in);
		 return projectM_home;
	       }
	     else{printf("This shouldn't happen, using implementation defaults\n");abort();}
	   }
	 else{printf("Cannot find projectM default config, using implementation defaults\n");abort();}
       }
     else
       {
	 printf("Cannot create ~/.projectM/config.inp, using default config file\n");
	 if ((f_in = fopen(projectM_config, "r")) != 0)
	   { printf("Successfully opened default config file\n");
	     fclose(f_in);
	     return projectM_config;}
	 else{ printf("Using implementation defaults, your system is really messed up, I'm surprised we even got this far\n");  abort();}

       }

   }


 abort();
}

int
process (jack_nframes_t nframes, void *arg)
{
	Q_UNUSED(arg);

	jack_default_audio_sample_t *in;

	in = (jack_default_audio_sample_t*)jack_port_get_buffer (input_port, nframes);

	//memcpy (out, in,sizeof (jack_default_audio_sample_t) * nframes);

    projectm_pcm_add_float_1ch_data(globalPM, in, nframes);
//		printf("%x %f\n",nframes,in[128]);

	return 0;
}

void jack_shutdown (void *arg)
{
	Q_UNUSED(arg);
	exit (1);
}

int main (int argc, char **argv) {
	const char **ports;
	const char *client_name;
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
	int i;

	// Start a new application
	ProjectMApplication app(argc, argv);

	auto config_file = read_config();


	QProjectM_MainWindow * mainWindow = new QProjectM_MainWindow(config_file, 0);
	mainWindow->show();

	globalPM = mainWindow->GetProjectM();



	//JACK INIT
	//----------------------------------------------
	if (argc >= 2) {		/* client name specified? */
		client_name = argv[1];
		if (argc >= 3) {	/* server name specified? */
			server_name = argv[2];
			//			options |= JackServerName;
		}
	} else {			/* use basename of argv[0] */
		client_name = strrchr(argv[0], '/');
		if (client_name == 0) {
			client_name = argv[0];
		} else {
			client_name++;
		}
	}

	/* open a client connection to the JACK server */

	client = jack_client_open (client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}

	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}

	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	/* tell the JACK server to call `process()' whenever
	   there is work to be done.
	*/

	if (jack_set_process_callback (client, process, 0)) {
		std::cerr << "qprojectM-jack: failed to set process callbank!. quitting..." << std::endl;
		exit(EXIT_FAILURE);
	}

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown (client, jack_shutdown, 0);

	/* display the current sample rate.
	 */

	printf ("engine sample rate: %d\n",
		jack_get_sample_rate (client));

	/* create two ports */

	input_port = jack_port_register (client, "input",
					 JACK_DEFAULT_AUDIO_TYPE,
					 JackPortIsInput, 0);

	if (input_port == NULL) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	// END JACK INIT ----------------------------------

        //JACK BEGIN-----------------------------

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */

	ports = jack_get_ports (client, NULL, NULL, JackPortIsOutput);
	if (ports == NULL) {
		fprintf(stderr, "no physical capture ports\n");
		exit (1);
	}


        i=0;
        while (ports[i]!=NULL)
	 {
            printf("Connecting to Jack port %s\n",ports[i]);
	    if (jack_connect (client, ports[i], jack_port_name (input_port))) {
	      fprintf (stderr, "cannot connect input ports\n");
	    }
	      i++;
	     }

	free (ports);
	
	
#ifdef HTTP_REMOTE_CONTROL
  uint port=qgetenv("HTTP_PORT").toUInt();
  QxtHttpServerConnector connector;

  QxtHttpSessionManager session;
  printf("session.setPort(%i\n", port); fflush(stdout);
  session.setPort(port);
  session.setConnector(&connector);

  HTTPRemoteControl s1(&session, mainWindow->qprojectM());
  session.setStaticContentService ( &s1);

  printf("session.start()\n"); fflush(stdout);
  if(port>0) { // I think it didn't work when the conditional was further up
    session.start();
  }
#endif


	//----------------------------------END

	//return 1;
	return app.exec();
}


