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

#include "RenderItemMatcher.hpp"
#include "RenderItemMergeFunction.hpp"
#include "fatal.h"
#include "Common.hpp"

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
#include "PipelineContext.hpp"
//#include <xmms/plugin.h>
#include <iostream>
#include "projectM.hpp"
#include "BeatDetect.hpp"
#include "Preset.hpp"
#include "PipelineMerge.hpp"
//#include "menu.h"
#include "PCM.hpp"                    //Sound data handler (buffering, FFT, etc.)

#include <map>

#include "Renderer.hpp"
#include "PresetChooser.hpp"
#include "ConfigFile.h"
#include "TextureManager.hpp"
#include "TimeKeeper.hpp"
#include "RenderItemMergeFunction.hpp"
#ifdef USE_THREADS
#include "pthread.h"
#endif

projectM::~projectM()
{

 #ifdef USE_THREADS
	std::cout << "[projectM] thread ";
  	printf("c");
	running = false;
	printf("l");
	pthread_cond_signal(&condition);
	printf("e");
	pthread_mutex_unlock( &mutex );
	printf("a");
	pthread_detach(thread);
	printf("n");
	pthread_cond_destroy(&condition);
	printf("u");
	pthread_mutex_destroy( &mutex );
	printf("p");
	std::cout << std::endl;
#endif
	destroyPresetTools();

	if ( renderer )
		delete ( renderer );
	if ( beatDetect )
		delete ( beatDetect );
	if ( _pcm ) {
		delete ( _pcm );
		_pcm = 0;
	}

delete(_pipelineContext);
}

DLLEXPORT unsigned projectM::initRenderToTexture()
{
	return renderer->initRenderToTexture();
}

DLLEXPORT void projectM::projectM_resetTextures()
{
	renderer->ResetTextures();
}


DLLEXPORT  projectM::projectM ( std::string config_file, int flags) :
		beatDetect ( 0 ), renderer ( 0 ),  _pcm(0), m_presetPos(0), m_flags(flags), _pipelineContext(new PipelineContext())
{
	readConfig(config_file);
	projectM_reset();
	projectM_resetGL(_settings.windowWidth, _settings.windowHeight);

}

DLLEXPORT projectM::projectM(Settings settings, int flags):
        beatDetect ( 0 ), renderer ( 0 ),  _pcm(0), m_presetPos(0), m_flags(flags), _pipelineContext(new PipelineContext())
{
    readSettings(settings);
    projectM_reset();
    projectM_resetGL(_settings.windowWidth, _settings.windowHeight);
}


bool projectM::writeConfig(const std::string & configFile, const Settings & settings) {

	ConfigFile config ( configFile );

	config.add("Mesh X", settings.meshX);
	config.add("Mesh Y", settings.meshY);
	config.add("Texture Size", settings.textureSize);
	config.add("FPS", settings.fps);
	config.add("Window Width", settings.windowWidth);
	config.add("Window Height", settings.windowHeight);
	config.add("Smooth Preset Duration", settings.smoothPresetDuration);
	config.add("Preset Duration", settings.presetDuration);
	config.add("Preset Path", settings.presetURL);
	config.add("Title Font", settings.titleFontURL);
	config.add("Menu Font", settings.menuFontURL);
	config.add("Hard Cut Sensitivity", settings.beatSensitivity);
	config.add("Aspect Correction", settings.aspectCorrection);
	config.add("Easter Egg Parameter", settings.easterEgg);
	config.add("Shuffle Enabled", settings.shuffleEnabled);

	std::fstream file(configFile.c_str());
	if (file) {
		file << config;
		return true;
	} else
		return false;
}



void projectM::readConfig (const std::string & configFile )
{
	std::cout << "[projectM] config file: " << configFile << std::endl;

	ConfigFile config ( configFile );
	_settings.meshX = config.read<int> ( "Mesh X", 32 );
	_settings.meshY = config.read<int> ( "Mesh Y", 24 );
	_settings.textureSize = config.read<int> ( "Texture Size", 512 );
	_settings.fps = config.read<int> ( "FPS", 35 );
	_settings.windowWidth  = config.read<int> ( "Window Width", 512 );
	_settings.windowHeight = config.read<int> ( "Window Height", 512 );
	_settings.smoothPresetDuration =  config.read<int>
			( "Smooth Preset Duration", config.read<int>("Smooth Transition Duration", 10));
	_settings.presetDuration = config.read<int> ( "Preset Duration", 15 );

	#ifdef LINUX
	_settings.presetURL = config.read<string> ( "Preset Path", CMAKE_INSTALL_PREFIX "/share/projectM/presets" );
	#endif

	#ifdef __APPLE__
	/// @bug awful hardcoded hack- need to add intelligence to cmake wrt bundling - carm
	_settings.presetURL = config.read<string> ( "Preset Path", "../Resources/presets" );
	#endif

	#ifdef WIN32
	_settings.presetURL = config.read<string> ( "Preset Path", CMAKE_INSTALL_PREFIX "/share/projectM/presets" );
	#endif

	#ifdef __APPLE__
	_settings.titleFontURL = config.read<string>
			( "Title Font",  "../Resources/fonts/Vera.tff");
	_settings.menuFontURL = config.read<string>
			( "Menu Font", "../Resources/fonts/VeraMono.ttf");
	#endif

	#ifdef LINUX
	_settings.titleFontURL = config.read<string>
			( "Title Font", CMAKE_INSTALL_PREFIX  "/share/projectM/fonts/Vera.ttf" );
	_settings.menuFontURL = config.read<string>
			( "Menu Font", CMAKE_INSTALL_PREFIX  "/share/projectM/fonts/VeraMono.ttf" );
	#endif

	#ifdef WIN32
	_settings.titleFontURL = config.read<string>
			( "Title Font", CMAKE_INSTALL_PREFIX  "/share/projectM/fonts/Vera.ttf" );
	_settings.menuFontURL = config.read<string>
			( "Menu Font", CMAKE_INSTALL_PREFIX  "/share/projectM/fonts/VeraMono.ttf" );
	#endif


	_settings.shuffleEnabled = config.read<bool> ( "Shuffle Enabled", true);

	_settings.easterEgg = config.read<float> ( "Easter Egg Parameter", 0.0);


	 projectM_init ( _settings.meshX, _settings.meshY, _settings.fps,
			 _settings.textureSize, _settings.windowWidth,_settings.windowHeight);


	 _settings.beatSensitivity = beatDetect->beat_sensitivity = config.read<float> ( "Hard Cut Sensitivity", 10.0 );

	if ( config.read ( "Aspect Correction", true ) )
		_settings.aspectCorrection = renderer->correction = true;
	else
		_settings.aspectCorrection = renderer->correction = false;


}


void projectM::readSettings (const Settings & settings )
{
    _settings.meshX = settings.meshX;
    _settings.meshY = settings.meshY;
    _settings.textureSize = settings.textureSize;
    _settings.fps = settings.fps;
    _settings.windowWidth  = settings.windowWidth;
    _settings.windowHeight = settings.windowHeight;
    _settings.smoothPresetDuration = settings.smoothPresetDuration;
    _settings.presetDuration = settings.presetDuration;


    _settings.presetURL = settings.presetURL;
    _settings.titleFontURL = settings.titleFontURL;
    _settings.menuFontURL =  settings.menuFontURL;
    _settings.shuffleEnabled = settings.shuffleEnabled;

    _settings.easterEgg = settings.easterEgg;

    projectM_init ( _settings.meshX, _settings.meshY, _settings.fps,
                    _settings.textureSize, _settings.windowWidth,_settings.windowHeight);


    _settings.beatSensitivity = settings.beatSensitivity;
    _settings.aspectCorrection = settings.aspectCorrection;

}

#ifdef USE_THREADS
static void *thread_callback(void *prjm) {
 projectM *p = (projectM *)prjm;

 p->thread_func(prjm);
return NULL;}


void *projectM::thread_func(void *vptr_args)
{
   pthread_mutex_lock( &mutex );
  //  printf("in thread: %f\n", timeKeeper->PresetProgressB());
  while (true)
    {
      pthread_cond_wait( &condition, &mutex );
      if(!running)
	{
	  pthread_mutex_unlock( &mutex );
	  return NULL;
	}
     evaluateSecondPreset();
    }
}
#endif

void projectM::evaluateSecondPreset()
{
/*
      m_activePreset2->presetInputs().frame = timeKeeper->PresetFrameB();
      m_activePreset2->presetInputs().progress= timeKeeper->PresetProgressB();

      assert ( m_activePreset2.get() );
      m_activePreset2->evaluateFrame();
      m_activePreset2->presetOutputs().Render(*beatDetect,presetInputs2);
*/

}

DLLEXPORT void projectM::renderFrame()
{

#ifdef DEBUG
	char fname[1024];
	FILE *f = NULL;
	int index = 0;
	int x, y;
#endif

	timeKeeper->UpdateTimers();

	//printf("A:%f, B:%f, S:%f\n", timeKeeper->PresetProgressA(), timeKeeper->PresetProgressB(), timeKeeper->SmoothRatio());

	mspf= ( int ) ( 1000.0/ ( float ) settings().fps ); //milliseconds per frame

	/// @bug whois is responsible for updating this now?"
	pipelineContext().time = timeKeeper->GetRunningTime();
	pipelineContext().frame = timeKeeper->PresetFrameA();
	pipelineContext().progress = timeKeeper->PresetProgressA();

	m_activePreset->Render(*beatDetect, pipelineContext());

	beatDetect->detectFromSamples();

	//m_activePreset->evaluateFrame();

	if ( renderer->noSwitch==false && !m_presetChooser->empty() )
	{
		if ( timeKeeper->PresetProgressA()>=1.0 && !timeKeeper->IsSmoothing())
		{

			timeKeeper->StartSmoothing();
			switchPreset(m_activePreset2);

			// Compute best matching between the render items.
			(*_matcher)(m_activePreset.get()->pipeline().drawables,
				m_activePreset2.get()->pipeline().drawables);

			presetSwitchedEvent(false, **m_presetPos);
		}

		else if ((beatDetect->vol-beatDetect->vol_old>beatDetect->beat_sensitivity ) &&
			 timeKeeper->CanHardCut())
		{
		  	// printf("Hard Cut\n");

			switchPreset(m_activePreset);

			//fz(m_activePreset, presetInputs, presetOutputs);

			timeKeeper->StartPreset();
			presetSwitchedEvent(true, **m_presetPos);
		}
	}


	if ( timeKeeper->IsSmoothing() && timeKeeper->SmoothRatio() <= 1.0 && !m_presetChooser->empty() )
	{

	  //	 printf("start thread\n");
		assert ( m_activePreset.get() );

#ifdef USE_THREADS
		pthread_cond_signal(&condition);
		pthread_mutex_unlock( &mutex );
#endif
		m_activePreset->Render(*beatDetect, pipelineContext());

#ifdef USE_THREADS
		pthread_mutex_lock( &mutex );
#else
		evaluateSecondPreset();
#endif

	//PresetMerger::MergePresets(m_activePreset->presetOutputs(), m_activePreset2->presetOutputs(),
	//	timeKeeper->SmoothRatio(), presetInputs.gx, presetInputs.gy);

	Pipeline pipeline;

	pipeline.setStaticPerPixel(settings().meshX, settings().meshY);

	assert(_matcher);
	PipelineMerger::MergePipelines( m_activePreset->pipeline(),
		m_activePreset2->pipeline(), pipeline, _matcher->matchResults(),
	*_merger, 1.0 - timeKeeper->SmoothRatio());

	/// @bug not sure if this is correct
	renderer->RenderFrame(pipeline, pipelineContext());

	for (int i = 0; i < _matcher->matchResults().matches.size(); i++) {		
		//delete(pipeline.drawables[i]);
	}
	pipeline.drawables.clear();
	}
	else
	{
		if ( timeKeeper->IsSmoothing() && timeKeeper->SmoothRatio() > 1.0 )
		{
		  //printf("End Smooth\n");
			m_activePreset = m_activePreset2;
			timeKeeper->EndSmoothing();
		}
		//printf("Normal\n");


		m_activePreset->Render(*beatDetect, pipelineContext());

//		m_activePreset->evaluateFrame();
//		m_activePreset->presetOutputs().Render(*beatDetect,presetInputs);

		renderer->RenderFrame (m_activePreset->pipeline(), pipelineContext());
	}

	//	std::cout<< m_activePreset->absoluteFilePath()<<std::endl;
	//	renderer->presetName = m_activePreset->absoluteFilePath();



	count++;
#ifndef WIN32
	/** Frame-rate limiter */
	/** Compute once per preset */
	if ( this->count%100==0 )
	{
		this->renderer->realfps=100.0/ ( ( getTicks ( &timeKeeper->startTime )-this->fpsstart ) /1000 );
		this->fpsstart=getTicks ( &timeKeeper->startTime );
	}

	int timediff = getTicks ( &timeKeeper->startTime )-this->timestart;

	if ( timediff < this->mspf )
	{
		// printf("%s:",this->mspf-timediff);
		int sleepTime = ( unsigned int ) ( this->mspf-timediff ) * 1000;
//		DWRITE ( "usleep: %d\n", sleepTime );
		if ( sleepTime > 0 && sleepTime < 100000 )
		{
			if ( usleep ( sleepTime ) != 0 ) {}}
	}
	this->timestart=getTicks ( &timeKeeper->startTime );
#endif /** !WIN32 */


}

void projectM::projectM_reset()
{
	this->mspf = 0;
	this->timed = 0;
	this->timestart = 0;
	this->count = 0;

	this->fpsstart = 0;

	projectM_resetengine();
}

void projectM::projectM_init ( int gx, int gy, int fps, int texsize, int width, int height )
{

	/** Initialise start time */
        timeKeeper = new TimeKeeper(_settings.presetDuration,_settings.smoothPresetDuration, _settings.easterEgg);

	/** Nullify frame stash */

	/** Initialise per-pixel matrix calculations */
	/** We need to initialise this before the builtin param db otherwise bass/mid etc won't bind correctly */
	assert ( !beatDetect );

	if (!_pcm)
		_pcm = new PCM();
	assert(pcm());
	beatDetect = new BeatDetect ( _pcm );

	if ( _settings.fps > 0 )
		mspf= ( int ) ( 1000.0/ ( float ) _settings.fps );
	else mspf = 0;

	this->renderer = new Renderer ( width, height, gx, gy, texsize,  beatDetect, settings().presetURL, settings().titleFontURL, settings().menuFontURL );

	std::cerr << "set pipeline broken FIX ME" << std::endl;
	//renderer->SetPipeline(presetOutputs);
	running = true;

	initPresetTools(gx, gy);


#ifdef USE_THREADS
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condition, NULL);
	if (pthread_create(&thread, NULL, thread_callback, this) != 0)
	    {

	      std::cerr << "[projectM] failed to allocate a thread! try building with option USE_THREADS turned off" << std::endl;;
	      exit(EXIT_FAILURE);
	    }
	pthread_mutex_lock( &mutex );
#endif

	/// @bug order of operatoins here is busted
	//renderer->setPresetName ( m_activePreset->name() );
	timeKeeper->StartPreset();
	assert(pcm());

}

/* Reinitializes the engine variables to a default (conservative and sane) value */
void projectM::projectM_resetengine()
{

	if ( beatDetect != NULL )
	{
		beatDetect->reset();
	}

	/// @bug call factory clear here?
	std::cerr << "call factory clear here?" << std::endl;
//	abort();


}

/** Resets OpenGL state */
DLLEXPORT void projectM::projectM_resetGL ( int w, int h )
{

	/** Stash the new dimensions */

	renderer->reset ( w,h );
}

/** Sets the title to display */
DLLEXPORT void projectM::projectM_setTitle ( std::string title ) {

	if ( title != renderer->title )
	{
		renderer->title=title;
		renderer->drawtitle=1;
	}
}


int projectM::initPresetTools(int gx, int gy)
{

	/* Set the seed to the current time in seconds */
	srand ( time ( NULL ) );

	std::string url = (m_flags & FLAG_DISABLE_PLAYLIST_LOAD) ? std::string() : settings().presetURL;

	if ( ( m_presetLoader = new PresetLoader ( gx, gy, url) ) == 0 )
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
	if (!m_presetPos)
		m_presetPos = new PresetIterator();

	// Initialize a preset queue position as well
//	m_presetQueuePos = new PresetIterator();

	// Start at end ptr- this allows next/previous to easily be done from this position.
	*m_presetPos = m_presetChooser->end();

	// Load idle preset
	std::cerr << "[projectM] Allocating idle preset..." << std::endl;
	m_activePreset = m_presetLoader->loadPreset
		("idle://Geiss & Sperl - Feedback (projectM idle HDR mix).milk");

	renderer->SetPipeline(m_activePreset->pipeline());

	// Case where no valid presets exist in directory. Could also mean
	// playlist initialization was deferred
	if (m_presetChooser->empty())
	{
		std::cerr << "[projectM] warning: no valid files found in preset directory \""
		<< m_presetLoader->directoryName() << "\"" << std::endl;
	}

	_matcher = new RenderItemMatcher();
	_merger = new MasterRenderItemMerge();
	//_merger->add(new WaveFormMergeFunction());
	_merger->add(new ShapeMerge());
	_merger->add(new BorderMerge());
	//_merger->add(new BorderMergeFunction());

	/// @bug These should be requested by the preset factories.
	_matcher->distanceFunction().addMetric(new ShapeXYDistance());

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

	if ( m_presetChooser )
		delete ( m_presetChooser );

	/// @slow might not be necessary
	m_presetChooser = 0;

	if ( m_presetLoader )
		delete ( m_presetLoader );

	/// @slow might not be necessary
	m_presetLoader = 0;

}

/// @bug queuePreset case isn't handled
void projectM::removePreset(unsigned int index) {

	unsigned int chooserIndex = **m_presetPos;

	m_presetLoader->removePreset(index);


	// Case: no more presets, set iterator to end
	if (m_presetChooser->empty())
		*m_presetPos = m_presetChooser->end();

	// Case: chooser index has become one less due to removal of an index below it
	else if (chooserIndex > index) {
		chooserIndex--;
		*m_presetPos = m_presetChooser->begin(chooserIndex);
	}

	// Case: we have deleted the active preset position
	// Set iterator to end of chooser
	else if (chooserIndex == index) {
		*m_presetPos = m_presetChooser->end();
	}



}

unsigned int projectM::addPresetURL ( const std::string & presetURL, const std::string & presetName, int rating )
{
	bool restorePosition = false;

	if (*m_presetPos == m_presetChooser->end())
		restorePosition = true;

	int index = m_presetLoader->addPresetURL ( presetURL, presetName, rating);

	if (restorePosition)
		*m_presetPos = m_presetChooser->end();

	return index;
}

void projectM::selectPreset ( unsigned int index )
{

	if ( m_presetChooser->empty() )
		return;

	assert ( index < m_presetLoader->size());
	assert ( index >= 0 );

	*m_presetPos = m_presetChooser->begin ( index );

	m_activePreset = m_presetPos->allocate ();

	renderer->setPresetName ( m_activePreset->name() );

	timeKeeper->StartPreset();
}

void projectM::switchPreset(std::auto_ptr<Preset> & targetPreset) {

	if (_settings.shuffleEnabled)
		*m_presetPos = m_presetChooser->weightedRandom();
	else
		m_presetChooser->nextPreset(*m_presetPos);

	targetPreset = m_presetPos->allocate();

	// Set preset name here- event is not done because at the moment this function is oblivious to smooth/hard switches
	renderer->setPresetName ( targetPreset->name() );
	renderer->SetPipeline(targetPreset->pipeline());

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

int projectM::getPresetRating ( unsigned int index ) const
{
	return m_presetLoader->getPresetRating(index);
}

std::string projectM::getPresetName ( unsigned int index ) const
{
	return m_presetLoader->getPresetName(index);
}

void projectM::clearPlaylist ( )
{
	m_presetLoader->clear();
	*m_presetPos = m_presetChooser->end();
}

void projectM::selectPresetPosition(unsigned int index) {
	*m_presetPos = m_presetChooser->begin(index);
}

bool projectM::selectedPresetIndex(unsigned int & index) const {

	if (*m_presetPos == m_presetChooser->end())
		return false;

	index = **m_presetPos;
	return true;
}


bool projectM::presetPositionValid() const {

	return (*m_presetPos != m_presetChooser->end());
}

unsigned int projectM::getPlaylistSize() const
{
	return m_presetLoader->size();
}

void projectM:: changePresetRating (unsigned int index, int rating) {
	m_presetLoader->setRating(index, rating);
}

void projectM::insertPresetURL(unsigned int index, const std::string & presetURL, const std::string & presetName, int rating)
{
	bool atEndPosition = false;

	int newSelectedIndex;


	if (*m_presetPos == m_presetChooser->end()) // Case: preset not selected
	{
		atEndPosition = true;
	}

	else if (**m_presetPos < index) // Case: inserting before selected preset
	{
		newSelectedIndex = **m_presetPos;
	}

	else if (**m_presetPos > index) // Case: inserting after selected preset
	{
		newSelectedIndex++;
	}

	else  // Case: inserting at selected preset
	{
		newSelectedIndex++;
	}

	m_presetLoader->insertPresetURL (index, presetURL, presetName, rating);

	if (atEndPosition)
		*m_presetPos = m_presetChooser->end();
	else
		*m_presetPos = m_presetChooser->begin(newSelectedIndex);


}




