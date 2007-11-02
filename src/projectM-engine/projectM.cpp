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
#include "wipemalloc.h"
#include "BuiltinFuncs.hpp"
#include "fatal.h"
#include "Common.hpp"
#include "compare.h"
#ifdef WIN32
#include "win32-dirent.h"
#endif

#include "timer.h"
#include <iostream>
#ifdef LINUX
#include "time.h"
#endif

#ifdef WIN32
#include <time.h>
#endif

//#include <xmms/plugin.h>
#include <iostream>
#include "projectM.hpp"
#include "BeatDetect.hpp"
#include "Eval.hpp"
#include "Param.hpp"
#include "Parser.hpp"
#include "Preset.hpp"
#include "PerPixelEqn.hpp"
#include "PresetMerge.hpp"
//#include "menu.h"
#include "PCM.hpp"                    //Sound data handler (buffering, FFT, etc.)
#include "CustomWave.hpp"
#include "CustomShape.hpp"
#include "IdlePreset.hpp"
#include "MoodBar.hpp"

#include <map>

#include "Renderer.hpp"
#include "PresetFrameIO.hpp"
#include "PresetChooser.hpp"
#include "ConfigFile.h"
#include "TextureManager.hpp"



double presetDuration = 15;
double smoothDuration = 5;
//int smoothFrame = 0;
int oldFrame = 1;

DLLEXPORT projectM::projectM ( int gx, int gy, int fps, int texsize, int width, int height, std::string preset_url,std::string title_fonturl, std::string title_menuurl ) :beatDetect ( 0 ),  renderer ( 0 ), presetURL ( preset_url ),   title_fontURL ( title_fonturl ), menu_fontURL ( menu_fontURL ), smoothFrame ( 0 )
{
	presetURL = preset_url;
	projectM_reset();
	projectM_init ( gx, gy, fps, texsize, width, height );
	projectM_resetGL ( width, height );
}

projectM::~projectM()
{

	destroyPresetTools();

	if ( renderer )
		delete ( renderer );
	if ( beatDetect )
		delete ( beatDetect );
	if ( pcm )
		delete ( pcm );
}

DLLEXPORT unsigned projectM::initRenderToTexture()
{
	return renderer->initRenderToTexture();
}

DLLEXPORT void projectM::projectM_resetTextures()
{
	renderer->ResetTextures();
}

DLLEXPORT  projectM::projectM ( std::string config_file ) :
		beatDetect ( 0 ), renderer ( 0 ), smoothFrame ( 0 )
{

	projectM_reset();
	readConfig ( config_file );

}

void projectM::readConfig ( std::string config_file )
{

	ConfigFile config ( config_file );

	int gx = config.read<int> ( "Mesh X", 32 );
	int gy = config.read<int> ( "Mesh Y", 24 );
	int texsize = config.read<int> ( "Texsize", 512 );
	int fps = config.read<int> ( "FPS", 35 );
	int wvw = config.read<int> ( "Window Width", 512 );
	int wvh = config.read<int> ( "Window Height", 512 );
	smoothDuration = config.read<int> ( "Smooth Transition Duration", 5 );
	presetDuration = config.read<int> ( "Preset Duration", 15 );


	presetURL = config.read<string> ( "Preset Path", "/usr/share/projectM/presets" );

	title_fontURL = config.read<string> ( "Title Font", "/usr/share/projectM/fonts/Vera.ttf" );
	menu_fontURL = config.read<string> ( "Menu Font", "/usr/share/projectM/fonts/VeraMono.ttf" );

	projectM_init ( gx, gy, fps, texsize, wvw, wvh );

	beatDetect->beat_sensitivity = config.read<float> ( "Hard Cut Sensitivity", 10.0 );
	if ( config.read ( "Aspect Correction", true ) ) renderer->correction = 1;
	else renderer->correction = 0;

	projectM_resetGL ( wvw, wvh );
}


DLLEXPORT void projectM::renderFrame()
{

#ifdef DEBUG
	char fname[1024];
	FILE *f = NULL;
	int index = 0;
	int x, y;
#endif

	//     printf("Start of loop at %d\n",timestart);
	mspf= ( int ) ( 1000.0/ ( float ) presetInputs.fps ); //milliseconds per frame

#ifndef WIN32
	presetInputs.time = getTicks ( &startTime ) * 0.001;
#else
	presetInputs.time = getTicks ( startTime ) * 0.001;
#endif /** !WIN32 */


	//DWRITE ( "frame: %d\ttime: %f\tprogress: %f\tavgtime: %d\tang: %f\trot: %f\n",
	// this->presetInputs.frame, presetInputs.time, this->presetInputs.progress, this->avgtime, this->presetInputs.ang_per_pixel,
	//this->presetOutputs.rot );


//       printf("start:%d at:%d min:%d stop:%d on:%d %d\n",startframe, frame frame-startframe,avgtime,  noSwitch,progress);
	presetInputs.ResetMesh();

//     printf("%f %d\n",Time,frame);


	beatDetect->detectFromSamples();

	DWRITE ( "=== vol: %f\tbass: %f\tmid: %f\ttreb: %f ===\n",
	         beatDetect->vol,beatDetect->bass,beatDetect->mid,beatDetect->treb );
	DWRITE ( "=== bass_att: %f ===\n",
	         beatDetect->bass_att );

	presetInputs.bass = beatDetect->bass;
	presetInputs.mid = beatDetect->mid;
	presetInputs.treb = beatDetect->treb;
	presetInputs.bass_att = beatDetect->bass_att;
	presetInputs.mid_att = beatDetect->mid_att;
	presetInputs.treb_att = beatDetect->treb_att;

	assert ( m_activePreset.get() );

	m_activePreset->evaluateFrame();

	if ( renderer->noSwitch==false && !m_presetChooser->empty() )
	{
		if ( presetInputs.progress>1.0 )
		{
			oldFrame = presetInputs.frame;
			presetInputs.progress=0.0;
			presetInputs.frame = 1;

			*m_presetPos = m_presetChooser->weightedRandom<PresetChooser::UniformRandomFunctor>();

			m_activePreset2 = m_presetPos->allocate
			                  ( presetInputs, &m_activePreset->presetOutputs() == &presetOutputs ? presetOutputs2 : presetOutputs );

			assert ( m_activePreset2.get() );
			renderer->setPresetName ( m_activePreset2->presetName() );

			//nohard=(int)(presetInputs.fps*3.5);
			smoothFrame = ( int ) ( presetInputs.fps * smoothDuration );
			presetSwitchedEvent(false, **m_presetPos);
//              printf("SOFT CUT - Smooth started\n");
		}
		else if ( ( beatDetect->vol-beatDetect->vol_old>beatDetect->beat_sensitivity ) && nohard<0 )
		{
			//            printf("%f %d %d\n", beatDetect->bass-beatDetect->bass_old,this->frame,this->avgtime);
			*m_presetPos = m_presetChooser->weightedRandom<PresetChooser::UniformRandomFunctor> ();

			m_activePreset = m_presetPos->allocate ( presetInputs, presetOutputs );
			renderer->setPresetName ( m_activePreset->presetName() );

			assert ( m_activePreset.get() );
			//nohard=presetInputs.fps*1;
			smoothFrame=0;
			presetInputs.progress=0.0;
			presetInputs.frame = 1;
			presetSwitchedEvent(true, **m_presetPos);
		}
		else nohard--;
	}



	if ( smoothFrame > 1 && !m_presetChooser->empty() )
	{
		int frame = ++presetInputs.frame;
		presetInputs.frame = ++oldFrame;
		presetInputs.progress= 1.0;
		assert ( m_activePreset.get() );
		m_activePreset->evaluateFrame();
		renderer->PerPixelMath ( &m_activePreset->presetOutputs(), &presetInputs );
		renderer->WaveformMath ( &m_activePreset->presetOutputs(), &presetInputs, true );

		presetInputs.frame = frame;
		presetInputs.progress= frame / ( float ) avgtime;
		assert ( m_activePreset2.get() );
		m_activePreset2->evaluateFrame();
		renderer->PerPixelMath ( &m_activePreset2->presetOutputs(), &presetInputs );
		renderer->WaveformMath ( &m_activePreset2->presetOutputs(), &presetInputs, true );

		//double pos = -((smoothFrame / (presetInputs.fps * smoothDuration))-1);
		//double ratio = 1/(1 + exp((pos-0.5)*4*M_PI));
		double ratio = smoothFrame / ( presetInputs.fps * smoothDuration );
		// printf("f(%f)=%f\n",pos, ratio);
		PresetMerger::MergePresets ( m_activePreset->presetOutputs(),m_activePreset2->presetOutputs(),ratio,presetInputs.gx, presetInputs.gy );

		//printf("Smooth:%d\n",smoothFrame);

		smoothFrame--;

	}
	else
	{
		if ( smoothFrame == 1 )
		{
			m_activePreset = m_activePreset2;
			smoothFrame=0;
//		printf("Smooth Finished\n");
		}

		presetInputs.frame++;  //number of frames for current preset
		presetInputs.progress= presetInputs.frame/ ( float ) avgtime;
		m_activePreset->evaluateFrame();

		renderer->PerPixelMath ( &m_activePreset->presetOutputs(), &presetInputs );
		renderer->WaveformMath ( &m_activePreset->presetOutputs(), &presetInputs, false );

	}

	//	std::cout<< m_activePreset->absoluteFilePath()<<std::endl;
	//	renderer->presetName = m_activePreset->absoluteFilePath();

	renderer->RenderFrame ( &m_activePreset->presetOutputs(), &presetInputs );

	count++;
#ifndef WIN32
	/** Frame-rate limiter */
	/** Compute once per preset */
	if ( this->count%100==0 )
	{
		this->renderer->realfps=100.0/ ( ( getTicks ( &this->startTime )-this->fpsstart ) /1000 );
		this->fpsstart=getTicks ( &this->startTime );
	}

	int timediff = getTicks ( &this->startTime )-this->timestart;

	if ( timediff < this->mspf )
	{
		// printf("%s:",this->mspf-timediff);
		int sleepTime = ( unsigned int ) ( this->mspf-timediff ) * 1000;
//		DWRITE ( "usleep: %d\n", sleepTime );
		if ( sleepTime > 0 && sleepTime < 100000 )
		{
			if ( usleep ( sleepTime ) != 0 ) {}}
	}
	this->timestart=getTicks ( &this->startTime );
#endif /** !WIN32 */

	DWRITE ( "exiting renderFrame()\n" );
}

void projectM::projectM_reset()
{

	/** Default variable settings */

	this->wvw = 512;
	this->wvh = 512;


	/** Frames per preset */
	this->avgtime = 500;


	/** More other stuff */
	this->mspf = 0;
	this->timed = 0;
	this->timestart = 0;
	this->nohard = 0;
	this->count = 0;

	this->fpsstart = 0;

	projectM_resetengine();
}

void projectM::projectM_init ( int gx, int gy, int fps, int texsize, int width, int height )
{

	/** Initialise engine variables */

	projectM_initengine();
	presetInputs.Initialize ( gx, gy );
	presetOutputs.Initialize ( gx, gy );
	presetOutputs2.Initialize ( gx, gy );

	DWRITE ( "projectM plugin: Initializing\n" );

	/** Initialise start time */
#ifndef WIN32
	gettimeofday ( &this->startTime, NULL );
#else
	startTime = GetTickCount();
#endif /** !WIN32 */

	/** Nullify frame stash */


	/** Initialise per-pixel matrix calculations */


	presetInputs.fps = fps;
	this->nohard=fps*5;
	/** We need to initialise this before the builtin param db otherwise bass/mid etc won't bind correctly */
	assert ( !beatDetect );

	pcm = new PCM();
	beatDetect = new BeatDetect ( pcm );

	initPresetTools();
#if 0
	/* Load default preset directory */
#ifdef MACOS2
	/** Probe the bundle for info */
	CFBundleRef bundle = CFBundleGetMainBundle();
	char msg[1024];
	sprintf ( msg, "bundle: %X\n", bundle );
	DWRITE ( msg );
	if ( bundle != NULL )
	{
		CFPlugInRef pluginRef = CFBundleGetPlugIn ( bundle );
		if ( pluginRef != NULL )
		{
			DWRITE ( "located plugin ref\n" );
		}
		else
		{
			DWRITE ( "failed to find plugin ref\n" );
		}

		CFURLRef bundleURL = CFBundleCopyBundleURL ( bundle );
		if ( bundleURL == NULL )
		{
			DWRITE ( "bundleURL failed\n" );
		}
		else
		{
			DWRITE ( "bundleURL OK\n" );
		}
		char *bundleName =
		    ( char * ) CFStringGetCStringPtr ( CFURLGetString ( bundleURL ), kCFStringEncodingMacRoman );
		DWRITE ( "bundleURL: %s\n", bundleName );

		presetURL = CFBundleCopyResourceURL ( bundle, purl, NULL, NULL );
		if ( presetURL != NULL )
		{
			this->presetURL = ( char * ) CFStringGetCStringPtr ( CFURLCopyPath ( presetURL ), kCFStringEncodingMacRoman );
			sprintf ( msg, "Preset: %s\n", presetURL );
			DWRITE ( msg );
			printf ( msg );

			/** Stash the short preset name */

		}
		else
		{
			DWRITE ( "Failed to probe 'presets' bundle ref\n" );
			this->presetURL = NULL;
		}

		fontURL = CFBundleCopyResourceURL ( bundle, furl, NULL, NULL );
		if ( fontURL != NULL )
		{
			fontURL = ( char * ) CFStringGetCStringPtr ( CFURLCopyPath ( fontURL ), kCFStringEncodingMacRoman );
			sprintf ( msg, "Font: %s\n", fontURL );
			DWRITE ( msg );
			printf ( msg );
		}
		else
		{
			DWRITE ( "Failed to probe 'fonts' bundle ref\n" );
			fontURL = NULL;
		}
	}

	/** Sanity check */
	if ( bundle == NULL || presetURL == NULL || fontURL == NULL )
	{
		sprintf ( msg, "defaulting presets\n" );
		DWRITE ( msg );
		this->fontURL = ( char * ) wipemalloc ( sizeof ( char ) * 512 );
//        strcpy( this->fontURL, "../../fonts/" );
		strcpy ( fontURL, "/Users/descarte/tmp/projectM/fonts" );
		this->fontURL[34] = '\0';
//        loadPresetDir( "../../presets/" );
//        loadPresetDir( "/Users/descarte/tmp/projectM-1.00/presets_projectM" );
	}
	else
	{
		printf ( "PresetDir: %s\n", this->presetURL );
		loadPresetDir ( presetURL );
	}
#else
	if ( presetURL == NULL || fontURL == NULL )
	{
		char msg[1024];
		sprintf ( msg, "defaulting presets\n" );
		DWRITE ( msg );
		fontURL = ( char * ) wipemalloc ( sizeof ( char ) * 512 );
#ifdef WIN32
		strcpy ( this->fontURL, "c:\\tmp\\projectM\\fonts" );
		fontURL[24] = '\0';
#else
	strcpy ( this->fontURL, "/Users/descarte/tmp/projectM/fonts" );
	fontURL[34] = '\0';
#endif
		DWRITE ( "loading font URL directly: %s\n", this->fontURL );
#ifdef WIN32
		//      loadPresetDir( "c:\\tmp\\projectM\\presets_projectM" );
#else
	//    loadPresetDir( "/Users/descarte/tmp/projectM-1.00/presets_projectM" );
#endif
	}
	else
	{
		printf ( "PresetDir: %s\n", this->presetURL );
		//loadPresetDir( presetURL );
	}

#endif
#endif

	if ( presetInputs.fps > 0 )
		mspf= ( int ) ( 1000.0/ ( float ) presetInputs.fps );
	else mspf = 0;


	this->avgtime= ( int ) ( this->presetInputs.fps*presetDuration );




	this->presetInputs.gx = gx;
	this->presetInputs.gy = gy;

	this->renderer = new Renderer ( width, height, gx, gy, texsize,  beatDetect, presetURL, title_fontURL, menu_fontURL );


	renderer->setPresetName ( m_activePreset->presetName() );

//	printf ( "exiting projectM_init()\n" );
}


void projectM::projectM_initengine()
{

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


	this->presetInputs.progress = 0;
	this->presetInputs.frame = 1;

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

}

/* Reinitializes the engine variables to a default (conservative and sane) value */
void projectM::projectM_resetengine()
{

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


	if ( beatDetect != NULL )
	{
		beatDetect->reset();
	}
	this->presetInputs.progress = 0;
	this->presetInputs.frame = 1;

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

}

/** Resets OpenGL state */
DLLEXPORT void projectM::projectM_resetGL ( int w, int h )
{

	/** Stash the new dimensions */

	renderer->reset ( w,h );
}

/** Sets the title to display */
DLLEXPORT void projectM::projectM_setTitle ( std::string title )
{
	if ( title != renderer->title )
	{
		renderer->title=title;
		renderer->drawtitle=1;
	}
}


int projectM::initPresetTools()
{

	/* Initializes the builtin function database */
	BuiltinFuncs::init_builtin_func_db();

	/* Initializes all infix operators */
	Eval::init_infix_ops();

	/* Set the seed to the current time in seconds */
	srand ( time ( NULL ) );

	if ( ( m_presetLoader = new PresetLoader ( presetURL ) ) == 0 )
	{
		m_presetLoader = 0;
		std::cerr << "[projectM] error allocating preset loader" << std::endl;
		return PROJECTM_FAILURE;
	}

	if ( ( m_presetChooser = new PresetChooser ( *m_presetLoader ) ) == 0 )
	{
		delete ( m_presetLoader );

		m_presetChooser = 0;
		m_presetLoader = 0;

		std::cerr << "[projectM] error allocating preset chooser" << std::endl;
		return PROJECTM_FAILURE;
	}

	// Start the iterator
	m_presetPos = new PresetIterator();

	// Initialize a preset queue position as well
	m_presetQueuePos = new PresetIterator();

	// Start at end ptr- this allows next/previous to easily be done from this position.
	*m_presetPos = *m_presetQueuePos = m_presetChooser->end();

	// Load idle preset
	//std::cerr << "[projectM] Allocating idle preset..." << std::endl;
	m_activePreset = IdlePreset::allocate ( presetInputs, presetOutputs );

	// Case where no valid presets exist in directory
	if ( m_presetChooser->empty() )
	{
		std::cerr << "[projectM] warning: no valid files found in preset directory \""
		<< m_presetLoader->directoryName() << "\"" << std::endl;
	}

	//std::cerr << "[projectM] Idle preset allocated." << std::endl;

	projectM_resetengine();

	//std::cerr << "[projectM] engine has been reset." << std::endl;
	return PROJECTM_SUCCESS;
}

void projectM::destroyPresetTools()
{

	if ( m_presetPos )
		delete ( m_presetPos );

	/// @slow might not be necessary
	m_presetPos = 0;


	if ( m_presetQueuePos )
		delete ( m_presetQueuePos );

	/// @slow might not be necessary
	m_presetQueuePos = 0;


	if ( m_presetChooser )
		delete ( m_presetChooser );

	/// @slow might not be necessary
	m_presetChooser = 0;

	if ( m_presetLoader )
		delete ( m_presetLoader );

	/// @slow might not be necessary
	m_presetLoader = 0;

	Eval::destroy_infix_ops();
	BuiltinFuncs::destroy_builtin_func_db();

}


void projectM::removePreset(unsigned int index) {
	m_presetLoader->removePreset(index);
}

unsigned int projectM::addPresetURL ( const std::string & presetURL, const std::string & presetName )
{	
	return m_presetLoader->addPresetURL ( presetURL, presetName );
}

void projectM::selectPreset ( unsigned int index )
{

	if ( m_presetChooser->empty() )
		return;

	assert ( index < m_presetLoader->getNumPresets() );
	assert ( index >= 0 );

	*m_presetPos = m_presetChooser->begin ( index );

	m_activePreset = m_presetPos->allocate ( presetInputs, presetOutputs );

	renderer->setPresetName ( m_activePreset->presetName() );

	presetInputs.frame = 0;
	smoothFrame = 0;
}

void projectM::setPresetLock ( bool isLocked )
{
	renderer->noSwitch = isLocked;
}

bool projectM::isPresetLocked() const
{
	return renderer->noSwitch;
}

std::string projectM::getPresetURL ( unsigned int index ) const
{
	return m_presetLoader->getPresetURL(index);
}


std::string projectM::getPresetName ( unsigned int index ) const
{
	return m_presetLoader->getPresetName(index);
}

void projectM::clearPlaylist ( ) 
{
	m_presetLoader->clear();
}


void projectM::queuePreset(unsigned int index) {
	*m_presetQueuePos = m_presetChooser->begin(index);
}

bool projectM::isPresetQueued() const {
	return (*m_presetQueuePos != m_presetChooser->end());
}


unsigned int projectM::getPlaylistSize() const
{
	return m_presetLoader->getNumPresets();
}
