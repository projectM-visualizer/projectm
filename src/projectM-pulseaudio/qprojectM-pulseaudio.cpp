
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

/* $Id: parec-simple.c 1418 2007-01-04 13:43:45Z ossman $ */

/***
  This file was copied from PulseAudio.

  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2 of the License,
  or (at your option) any later version.

  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <projectM.hpp>
#include <qprojectm_mainwindow.hpp>
#include <QApplication>
#include <QtDebug>

#include "ConfigFile.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <QAction>
#include <QThread>
#include <QTimer>
#define CONFIG_FILE "/share/projectM/config.inp"

std::string read_config();

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <src/libprojectM/TestRunner.hpp>

//#include <pulsecore/gccmacro.h>

#include "QPulseAudioThread.hpp"
#include "QPulseAudioDeviceChooser.hpp"

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
			QApplication(argc, argv) {
		}
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

int main ( int argc, char*argv[] )
{
//	if (!TestRunner::run()) exit(1);

	ProjectMApplication app ( argc, argv );

	std::string config_file;
	config_file = read_config();


	QMutex audioMutex;

	QProjectM_MainWindow * mainWindow = new QProjectM_MainWindow ( config_file, &audioMutex);

	QAction pulseAction("Pulse audio settings...", mainWindow);


      	mainWindow->registerSettingsAction(&pulseAction);
	mainWindow->show();

	QPulseAudioThread * pulseThread = new QPulseAudioThread(argc, argv, mainWindow);

	pulseThread->start();

	//QApplication::connect
	//		(mainWindow->qprojectMWidget(), SIGNAL(projectM_Initialized(QProjectM *)), pulseThread, SLOT(setQrojectMWidget(QProjectMWidget*)));

	QPulseAudioDeviceChooser devChooser(pulseThread, mainWindow);
	QApplication::connect(&pulseAction, SIGNAL(triggered()), &devChooser, SLOT(open()));
	//QApplication::connect(pulseThread, SIGNAL(threadCleanedUp()), mainWindow, SLOT(close()));

	//QApplication::connect(mainWindow, SIGNAL(shuttingDown()), pulseThread, SLOT(cleanup()), Qt::DirectConnection);
 	int ret = app.exec();
	if (pulseThread != 0)
	  devChooser.writeSettings();

	if (mainWindow)
        	mainWindow->unregisterSettingsAction(&pulseAction);

	pulseThread->cleanup();

	delete(pulseThread);
	return ret;
}


std::string read_config()
{
	char num[512];
	FILE *in;
	FILE *out;

	char* home;
        char* xdg_home;
        
	char projectM_config[1024];
	char default_config[1024];

	strcpy(default_config, PROJECTM_PREFIX);
        strcat(default_config, CONFIG_FILE);
	printf("Default config: %s\n", default_config);

	home=getenv("HOME");
        xdg_home = getenv("XDG_CONFIG_HOME");

	strcpy(projectM_config, home);
        strcat(projectM_config, "/.projectM/config.inp");

	if ( ( in = fopen ( projectM_config, "r" ) ) != 0 )
	{
		printf ( "reading %s \n", projectM_config );
		fclose ( in );
		return std::string ( projectM_config );
	}
	else {
                if (xdg_home) {
                        strcpy(projectM_config, xdg_home);
                        strcat(projectM_config, "/projectM/config.inp");
                        if ( ( in = fopen ( projectM_config, "r" ) ) != 0 ) {
                                printf ( "reading %s \n", projectM_config );
                                fclose ( in );
                                return std::string ( projectM_config );
                        }
                        else {
                                strcpy(projectM_config, xdg_home);
                                strcat(projectM_config, "/projectM");
                        }
                }
                else {
                      strcpy(projectM_config, home);
                      strcat(projectM_config, "/.projectM");
                }
                mkdir(projectM_config, 0755);
                strcat(projectM_config, "/config.inp");
                printf ("trying to create %s \n", projectM_config);

                if ( ( out = fopen ( projectM_config,"w" ) ) !=0 )
                        {

			if ( ( in = fopen ( default_config, "r" ) ) != 0 )
			{

				while ( fgets ( num,80,in ) !=NULL )
				{
					fputs ( num,out );
				}
				fclose ( in );
				fclose ( out );


				if ( ( in = fopen ( projectM_config, "r" ) ) != 0 )
				{
					printf ( "created %s successfully\n", projectM_config );
					fclose ( in );
					return std::string ( projectM_config );
				}
				else{printf ( "This shouldn't happen, using implementation defaults\n" );abort();}
			}
			else{printf ( "Cannot find projectM default config, using implementation defaults\n" );abort();}
		}
		else
		{
			printf ( "Cannot create %s, using default config file\n", projectM_config );
			if ( ( in = fopen ( default_config, "r" ) ) != 0 )
			{
				printf ( "Successfully opened default config file\n" );
				fclose ( in );
				return std::string ( default_config );
			}
			else{ printf ( "Using implementation defaults, your system is really messed up, I'm surprised we even got this far\n" );  abort();}

		}

	}


	abort();
}


