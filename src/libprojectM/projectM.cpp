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

#include "projectM-opengl.h"
#include "RenderItemMatcher.hpp"
#include "RenderItemMergeFunction.hpp"
#include "fatal.h"
#include "Common.hpp"

#ifdef WIN32
#include "dirent.h"
#endif

#include <iostream>

#include "PipelineContext.hpp"
#include "projectM.hpp"
#include "BeatDetect.hpp"
#include "Preset.hpp"
#include "PipelineMerger.hpp"
#include "PCM.hpp"                    //Sound data handler (buffering, FFT, etc.)

#include <map>

#include "Renderer.hpp"
#include "PresetChooser.hpp"
#include "ConfigFile.h"
#include "TextureManager.hpp"
#include "TimeKeeper.hpp"
#include "RenderItemMergeFunction.hpp"

namespace {
constexpr int kMaxSwitchRetries = 10;
}

projectM::~projectM()
{
#if USE_THREADS
    m_workerSync.FinishUp();
    m_workerThread.join();
#endif

    destroyPresetTools();

    if (renderer)
    {
        delete (renderer);
    }
    if (beatDetect)
    {
        delete (beatDetect);
    }
    if (_pcm)
    {
        delete (_pcm);
        _pcm = 0;
    }

    if (timeKeeper)
    {
        delete timeKeeper;
        timeKeeper = NULL;
    }

    delete (_pipelineContext);
    delete (_pipelineContext2);
}

unsigned projectM::initRenderToTexture()
{
    return renderer->initRenderToTexture();
}

void projectM::projectM_resetTextures()
{
    renderer->ResetTextures();
}


projectM::projectM(std::string config_file, int flags)
    :
    renderer(0)
    , _pcm(0)
    , beatDetect(0)
    , _pipelineContext(new PipelineContext())
    , _pipelineContext2(new PipelineContext())
    , m_presetPos(0)
    , timeKeeper(NULL)
    , m_flags(flags)
    , _matcher(NULL)
    , _merger(NULL)
{
    readConfig(config_file);
    projectM_reset();
    projectM_resetGL(_settings.windowWidth, _settings.windowHeight);

}

projectM::projectM(Settings settings, int flags)
    :
    renderer(0)
    , _pcm(0)
    , beatDetect(0)
    , _pipelineContext(new PipelineContext())
    , _pipelineContext2(new PipelineContext())
    , m_presetPos(0)
    , timeKeeper(NULL)
    , m_flags(flags)
    , _matcher(NULL)
    , _merger(NULL)
{
    readSettings(settings);
    projectM_reset();
    projectM_resetGL(_settings.windowWidth, _settings.windowHeight);
}


bool projectM::writeConfig(const std::string& configFile, const Settings& settings)
{

    ConfigFile config(configFile);

    config.add("Mesh X", settings.meshX);
    config.add("Mesh Y", settings.meshY);
    config.add("Texture Size", settings.textureSize);
    config.add("FPS", settings.fps);
    config.add("Window Width", settings.windowWidth);
    config.add("Window Height", settings.windowHeight);
    config.add("Smooth Preset Duration", settings.softCutDuration);
    config.add("Preset Duration", settings.presetDuration);
    config.add("Preset Path", settings.presetURL);
    config.add("Title Font", settings.titleFontURL);
    config.add("Menu Font", settings.menuFontURL);
    config.add("Hard Cut Sensitivity", settings.beatSensitivity);
    config.add("Aspect Correction", settings.aspectCorrection);
    config.add("Easter Egg Parameter", settings.easterEgg);
    config.add("Shuffle Enabled", settings.shuffleEnabled);
    config.add("Soft Cut Ratings Enabled", settings.softCutRatingsEnabled);
    std::fstream file(configFile.c_str(), std::ios_base::trunc | std::ios_base::out);
    if (file)
    {
        file << config;
        return true;
    }
    else
    {
        return false;
    }
}


void projectM::readConfig(const std::string& configFile)
{
    std::cout << "[projectM] config file: " << configFile << std::endl;

    ConfigFile config(configFile);
    _settings.meshX = config.read<int>("Mesh X", 32);
    _settings.meshY = config.read<int>("Mesh Y", 24);
    _settings.textureSize = config.read<int>("Texture Size", 512);
    _settings.fps = config.read<int>("FPS", 35);
    _settings.windowWidth = config.read<int>("Window Width", 512);
    _settings.windowHeight = config.read<int>("Window Height", 512);
    _settings.softCutDuration = config.read<double>
        ("Smooth Preset Duration", config.read<int>("Smooth Transition Duration", 10));
    _settings.presetDuration = config.read<double>("Preset Duration", 15);

#ifdef __unix__
    _settings.presetURL = config.read<string>("Preset Path", "/usr/local/share/projectM/presets");
#endif

#ifdef __APPLE__
    /// @bug awful hardcoded hack- need to add intelligence to cmake wrt bundling - carm
    _settings.presetURL = config.read<string> ( "Preset Path", "../Resources/presets" );
#endif

#ifdef WIN32
    _settings.presetURL = config.read<string> ( "Preset Path", "/usr/local/share/projectM/presets" );
#endif

#ifdef __APPLE__
    _settings.titleFontURL = config.read<string>
    ( "Title Font",  "../Resources/fonts/Vera.tff");
    _settings.menuFontURL = config.read<string>
    ( "Menu Font", "../Resources/fonts/VeraMono.ttf");
#endif

#ifdef __unix__
    _settings.titleFontURL = config.read<string>
        ("Title Font", "/usr/local/share/projectM/fonts/Vera.tff");
    _settings.menuFontURL = config.read<string>
        ("Menu Font", "/usr/local/share/projectM/fonts/VeraMono.tff");
#endif

#ifdef WIN32
    _settings.titleFontURL = config.read<string>
    ( "Title Font", projectM_FONT_TITLE );
    _settings.menuFontURL = config.read<string>
    ( "Menu Font", projectM_FONT_MENU );
#endif


    _settings.shuffleEnabled = config.read<bool>("Shuffle Enabled", true);

    _settings.easterEgg = config.read<float>("Easter Egg Parameter", 0.0);
    _settings.softCutRatingsEnabled =
        config.read<bool>("Soft Cut Ratings Enabled", false);

    // Hard Cuts are preset transitions that occur when your music becomes louder. They only occur after a hard cut duration threshold has passed.
    _settings.hardCutEnabled = config.read<bool>("Hard Cuts Enabled", false);
    // Hard Cut duration is the number of seconds before you become eligible for a hard cut.
    _settings.hardCutDuration = config.read<double>("Hard Cut Duration", 60);
    // Hard Cut sensitivity is the volume difference before a "hard cut" is triggered.
    _settings.hardCutSensitivity = config.read<float>("Hard Cut Sensitivity", 1.0);

    // Beat Sensitivity impacts how reactive your visualizations are to volume, bass, mid-range, and treble. 
    // Preset authors have developed their visualizations with the default of 1.0.
    _settings.beatSensitivity = config.read<float>("Beat Sensitivity", 1.0);


    projectM_init(_settings.meshX, _settings.meshY, _settings.fps,
                  _settings.textureSize, _settings.windowWidth, _settings.windowHeight);

    if (config.read("Aspect Correction", true))
    {
        _settings.aspectCorrection = true;
        renderer->correction = true;
    }
    else
    {
        _settings.aspectCorrection = false;
        renderer->correction = false;
    }


}


void projectM::readSettings(const Settings& settings)
{
    _settings.meshX = settings.meshX;
    _settings.meshY = settings.meshY;
    _settings.textureSize = settings.textureSize;
    _settings.fps = settings.fps;
    _settings.windowWidth = settings.windowWidth;
    _settings.windowHeight = settings.windowHeight;
    _settings.softCutDuration = settings.softCutDuration;
    _settings.presetDuration = settings.presetDuration;
    _settings.softCutRatingsEnabled = settings.softCutRatingsEnabled;

    _settings.presetURL = settings.presetURL;
    _settings.titleFontURL = settings.titleFontURL;
    _settings.menuFontURL = settings.menuFontURL;
    _settings.shuffleEnabled = settings.shuffleEnabled;
    _settings.datadir = settings.datadir;

    _settings.easterEgg = settings.easterEgg;

    _settings.hardCutEnabled = settings.hardCutEnabled;
    _settings.hardCutDuration = settings.hardCutDuration;
    _settings.hardCutSensitivity = settings.hardCutSensitivity;

    _settings.beatSensitivity = settings.beatSensitivity;

    projectM_init(_settings.meshX, _settings.meshY, _settings.fps,
                  _settings.textureSize, _settings.windowWidth, _settings.windowHeight);


    _settings.aspectCorrection = settings.aspectCorrection;
}

#if USE_THREADS
void projectM::ThreadWorker()
{
    while (true)
    {
        if (!m_workerSync.WaitForWork())
        {
            return;
        }
        evaluateSecondPreset();
        m_workerSync.FinishedWork();
    }
}

#endif

void projectM::evaluateSecondPreset()
{
    pipelineContext2().time = timeKeeper->GetRunningTime();
    pipelineContext2().presetStartTime = timeKeeper->PresetTimeB();
    pipelineContext2().frame = timeKeeper->PresetFrameB();
    pipelineContext2().progress = timeKeeper->PresetProgressB();

    m_activePreset2->Render(*beatDetect, pipelineContext2());
}

void projectM::renderFrame()
{
    Pipeline pipeline;
    Pipeline* comboPipeline;

    comboPipeline = renderFrameOnlyPass1(&pipeline);

    renderFrameOnlyPass2(comboPipeline, 0, 0, 0);

    projectM::renderFrameEndOnSeparatePasses(comboPipeline);
}


Pipeline* projectM::renderFrameOnlyPass1(
    Pipeline* pPipeline) /*pPipeline is a pointer to a Pipeline for use in pass 2. returns the pointer if it was used, else returns NULL */
{
#if USE_THREADS
    std::lock_guard<std::recursive_mutex> guard(m_presetSwitchMutex);
#endif

#ifdef DEBUG
    char fname[1024];
    FILE* f = NULL;
    int index = 0;
    int x, y;
#endif

    timeKeeper->UpdateTimers();
/*
    if (timeKeeper->IsSmoothing())
    {
        printf("Smoothing A:%f, B:%f, S:%f\n", timeKeeper->PresetProgressA(), timeKeeper->PresetProgressB(), timeKeeper->SmoothRatio());
    }
    else
    {
        printf("          A:%f\n", timeKeeper->PresetProgressA());
    }*/

    mspf = (int) (1000.0 / (float) settings().fps); //milliseconds per frame

    /// @bug who is responsible for updating this now?"
    auto& context = pipelineContext();
    context.time = timeKeeper->GetRunningTime();
    context.presetStartTime = timeKeeper->PresetTimeA();
    context.frame = timeKeeper->PresetFrameA();
    context.progress = timeKeeper->PresetProgressA();
    renderer->UpdateContext(context);

    beatDetect->calculateBeatStatistics();

    //m_activePreset->evaluateFrame();

    //if the preset isn't locked and there are more presets
    if (renderer->noSwitch == false && !m_presetChooser->empty())
    {
        //if preset is done and we're not already switching
        if (timeKeeper->PresetProgressA() >= 1.0 && !timeKeeper->IsSmoothing())
        {
            if (settings().shuffleEnabled)
            {
                selectRandom(false);
            }
            else
            {
                selectNext(false);
            }
        }
        else if (settings().hardCutEnabled && (beatDetect->vol - beatDetect->vol_old > settings().hardCutSensitivity) &&
                 timeKeeper->CanHardCut())
        {
            // Hard Cuts must be enabled, must have passed the hardcut duration, and the volume must be a greater difference than the hardcut sensitivity.
            if (settings().shuffleEnabled)
            {
                selectRandom(true);
            }
            else
            {
                selectNext(true);
            }
        }
    }


    if (timeKeeper->IsSmoothing() && timeKeeper->SmoothRatio() <= 1.0 && !m_presetChooser->empty())
    {
        //	 printf("start thread\n");
        assert(m_activePreset2.get());

#if USE_THREADS
        m_workerSync.WakeUpBackgroundTask();
#endif

        m_activePreset->Render(*beatDetect, pipelineContext());

#if USE_THREADS
        m_workerSync.WaitForBackgroundTaskToFinish();
#else
        evaluateSecondPreset();
#endif

        pPipeline->setStaticPerPixel(settings().meshX, settings().meshY);

        assert(_matcher);
        PipelineMerger::mergePipelines(m_activePreset->pipeline(),
                                       m_activePreset2->pipeline(), *pPipeline,
                                       _matcher->matchResults(),
                                       *_merger, timeKeeper->SmoothRatio());

        renderer->RenderFrameOnlyPass1(*pPipeline, pipelineContext());


        return pPipeline;

    }
    else
    {


        if (timeKeeper->IsSmoothing() && timeKeeper->SmoothRatio() > 1.0)
        {
            //printf("End Smooth\n");
            m_activePreset = std::move(m_activePreset2);
            timeKeeper->EndSmoothing();
        }
        //printf("Normal\n");

        m_activePreset->Render(*beatDetect, pipelineContext());
        renderer->RenderFrameOnlyPass1(m_activePreset->pipeline(), pipelineContext());
        return NULL; // indicating no transition

    }

    //	std::cout<< m_activePreset->absoluteFilePath()<<std::endl;
    //	renderer->presetName = m_activePreset->absoluteFilePath();




}


/* eye is 0,or 1, or who knows?*/
void projectM::renderFrameOnlyPass2(Pipeline* pPipeline, int xoffset, int yoffset,
                                    int eye) /*pPipeline can be null if we re not in transition */
{
/* eye is currently ignored */


#ifdef DEBUG
    char fname[1024];
    FILE* f = NULL;
    int index = 0;
    int x, y;
#endif

    if (pPipeline)
//    if ( timeKeeper->IsSmoothing() && timeKeeper->SmoothRatio() <= 1.0 && !m_presetChooser->empty() )
    {
        //	 printf("start thread\n");
        assert(m_activePreset2.get());


        /* was other stuff */

        renderer->RenderFrameOnlyPass2(*pPipeline, pipelineContext(), xoffset, yoffset, eye);

    }
    else
    {


        renderer->RenderFrameOnlyPass2(m_activePreset->pipeline(), pipelineContext(), xoffset, yoffset, eye);


    }

}


void projectM::renderFrameEndOnSeparatePasses(Pipeline* pPipeline)
{

    if (pPipeline)
    {
        // mergePipelines() sets masterAlpha for each RenderItem, reset it before we forget
        for (RenderItem* drawable: pPipeline->drawables)
        {
            drawable->masterAlpha = 1.0;
        }
        pPipeline->drawables.clear();
    }

    count++;
#ifndef WIN32
    /** Frame-rate limiter */
    /** Compute once per preset */
    if (this->count % 100 == 0)
    {
        auto ticks = static_cast<float>(timeKeeper->GetRunningTime());
        this->renderer->realfps = 100.0f / ((ticks - this->fpsstart) * 0.001f);
        this->fpsstart = ticks;
    }

#ifndef UNLOCK_FPS
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
#endif

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

void projectM::projectM_init(int gx, int gy, int fps, int texsize, int width, int height)
{
    /** Initialise start time */
    timeKeeper = new TimeKeeper(_settings.presetDuration, _settings.softCutDuration, _settings.hardCutDuration,
                                _settings.easterEgg);

    /** Nullify frame stash */

    /** Initialise per-pixel matrix calculations */
    /** We need to initialise this before the builtin param db otherwise bass/mid etc won't bind correctly */
    assert(!beatDetect);

    if (!_pcm)
    {
        _pcm = new Pcm();
    }
    assert(pcm());
    beatDetect = new BeatDetect(*_pcm);

    if (_settings.fps > 0)
    {
        mspf = (int) (1000.0 / (float) _settings.fps);
    }
    else
    {
        mspf = 0;
    }

    this->renderer = new Renderer(width, height, gx, gy, beatDetect, settings().presetURL, settings().titleFontURL,
                                  settings().menuFontURL, settings().datadir);

    initPresetTools(gx, gy);

#if USE_THREADS
    m_workerSync.Reset();
    m_workerThread = std::thread(&projectM::ThreadWorker, this);
#endif

    /// @bug order of operatoins here is busted
    //renderer->setPresetName ( m_activePreset->name() );
    timeKeeper->StartPreset();
    assert(pcm());

    pipelineContext().fps = fps;
    pipelineContext2().fps = fps;

}

/* Reinitializes the engine variables to a default (conservative and sane) value */
void projectM::projectM_resetengine()
{

    if (beatDetect != NULL)
    {
        beatDetect->reset();
        beatDetect->beatSensitivity = _settings.beatSensitivity;
    }

}

/** Resets OpenGL state */
void projectM::projectM_resetGL(size_t width, size_t height)
{
    /** Stash the new dimensions */
    _settings.windowWidth = width;
    _settings.windowHeight = height;
    renderer->reset(width, height);
}

/** Sets the title to display */
std::string projectM::getTitle() const
{
    return renderer->title;
}

/** Sets the title to display */
void projectM::setTitle(const std::string& title)
{
    if (title != renderer->title)
    {
        renderer->title = title;
        renderer->drawtitle = 1;
    }
}


int projectM::initPresetTools(int gx, int gy)
{

    /* Set the seed to the current time in seconds */
    srand(time(NULL));

    std::string url = (m_flags & FLAG_DISABLE_PLAYLIST_LOAD) ? std::string() : settings().presetURL;

    if ((m_presetLoader = new PresetLoader(gx, gy, url)) == 0)
    {
        m_presetLoader = 0;
        std::cerr << "[projectM] error allocating preset loader" << std::endl;
        return PROJECTM_FAILURE;
    }

    if ((m_presetChooser = new PresetChooser(*m_presetLoader, settings().softCutRatingsEnabled)) == 0)
    {
        delete (m_presetLoader);

        m_presetChooser = 0;
        m_presetLoader = 0;

        std::cerr << "[projectM] error allocating preset chooser" << std::endl;
        return PROJECTM_FAILURE;
    }

    // Start the iterator
    if (!m_presetPos)
    {
        m_presetPos = new PresetIterator();
    }

    // Initialize a preset queue position as well
    //	m_presetQueuePos = new PresetIterator();

    // Start at end ptr- this allows next/previous to easily be done from this position.
    *m_presetPos = m_presetChooser->end();

    // Load idle preset
//        std::cerr << "[projectM] Allocating idle preset..." << std::endl;
    m_activePreset = m_presetLoader->loadPreset
        ("idle://Geiss & Sperl - Feedback (projectM idle HDR mix).milk");
    renderer->setPresetName("Geiss & Sperl - Feedback (projectM idle HDR mix)");

    populatePresetMenu();

    renderer->SetPipeline(m_activePreset->pipeline());

    // Case where no valid presets exist in directory. Could also mean
    // playlist initialization was deferred
    if (m_presetChooser->empty())
    {
        //std::cerr << "[projectM] warning: no valid files found in preset directory \""
        ///< m_presetLoader->directoryName() << "\"" << std::endl;
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
    m_activePreset.reset();
    m_activePreset2.reset();

    if (m_presetPos)
    {
        delete (m_presetPos);
    }

    m_presetPos = 0;

    if (m_presetChooser)
    {
        delete (m_presetChooser);
    }

    m_presetChooser = 0;

    if (m_presetLoader)
    {
        delete (m_presetLoader);
    }

    m_presetLoader = 0;

    if (_matcher)
    {
        delete _matcher;
        _matcher = NULL;
    }

    if (_merger)
    {
        delete _merger;
        _merger = NULL;
    }
}

/// @bug queuePreset case isn't handled
void projectM::removePreset(unsigned int index)
{

    size_t chooserIndex = **m_presetPos;

    m_presetLoader->removePreset(index);


    // Case: no more presets, set iterator to end
    if (m_presetChooser->empty())
    {
        *m_presetPos = m_presetChooser->end();

        // Case: chooser index has become one less due to removal of an index below it
    }
    else if (chooserIndex > index)
    {
        chooserIndex--;
        *m_presetPos = m_presetChooser->begin(chooserIndex);
    }

        // Case: we have deleted the active preset position
        // Set iterator to end of chooser
    else if (chooserIndex == index)
    {
        *m_presetPos = m_presetChooser->end();
    }


}

unsigned int
projectM::addPresetURL(const std::string& presetURL, const std::string& presetName, const RatingList& ratings)
{
    bool restorePosition = false;

    if (*m_presetPos == m_presetChooser->end())
    {
        restorePosition = true;
    }

    int index = m_presetLoader->addPresetURL(presetURL, presetName, ratings);

    if (restorePosition)
    {
        *m_presetPos = m_presetChooser->end();
    }

    return index;
}

void projectM::selectPreset(unsigned int index, bool hardCut)
{
    if (m_presetChooser->empty())
    {
        return;
    }

    populatePresetMenu();

    *m_presetPos = m_presetChooser->begin(index);
    if (!startPresetTransition(hardCut))
    {
        selectRandom(hardCut);
    }
}

// populatePresetMenu is called when a preset is loaded.
void projectM::populatePresetMenu()
{
    if (renderer->showmenu)
    { // only track a preset list buffer if the preset menu is up.
        renderer->m_presetList.clear(); // clear preset list buffer from renderer.

        if (isTextInputActive())
        {
            // if a searchTerm is active, we will populate the preset menu with search terms instead of the page we are on.
            int h = 0;
            std::string presetName = renderer->presetName();
            int presetIndex = getSearchIndex(presetName);
            for (unsigned int i = 0; i < getPlaylistSize(); i++)
            { // loop over all presets
                if (caseInsensitiveSubstringFind(getPresetName(i), renderer->searchText()) != std::string::npos)
                { // if term matches
                    if (h < renderer->textMenuPageSize) // limit to just one page, pagination is not needed.
                    {
                        h++;
                        renderer->m_presetList.push_back(
                            { h, getPresetName(i), "" }); // populate the renders preset list.
                        if (h == presetIndex)
                        {
                            renderer->m_activePresetID = h;
                        }
                    }
                }
            }
        }
        else
        {
            // normal preset menu, based on pagination.
            renderer->m_activePresetID = m_presetPos->lastIndex(); // tell renderer about the active preset ID (so it can be highlighted)
            int page_start = 0;
            if (m_presetPos->lastIndex() != m_presetLoader->size())
            {
                page_start = renderer->m_activePresetID; // if it's not the idle preset, then set it to the true value
            }
            if (page_start < renderer->textMenuPageSize)
            {
                page_start = 0; // if we are on page 1, start at the first preset.
            }
            if (page_start % renderer->textMenuPageSize == 0)
            {
                // if it's a perfect division of the page size, we are good.
            }
            else
            {
                page_start = page_start - (page_start %
                                           renderer->textMenuPageSize); // if not, find closest divisable number for page start
            }
            int page_end = page_start + renderer->textMenuPageSize; // page end is page start + page size
            if (page_end > m_presetLoader->size())
            {
                page_end = m_presetLoader->size();
            }
            while (page_start < page_end)
            {
                renderer->m_presetList.push_back(
                    { page_start, getPresetName(page_start), "" }); // populate the renders preset list.
                page_start++;
            }
        }
    }
}

bool projectM::startPresetTransition(bool hard_cut)
{
    std::unique_ptr<Preset> new_preset = switchToCurrentPreset();
    if (new_preset == nullptr)
    {
        presetSwitchFailedEvent(hard_cut, **m_presetPos, "fake error");
        errorLoadingCurrentPreset = true;
        populatePresetMenu();
        return false;
    }

    if (hard_cut)
    {
        m_activePreset = std::move(new_preset);
        timeKeeper->StartPreset();
    }
    else
    {
        m_activePreset2 = std::move(new_preset);
        timeKeeper->StartPreset();
        timeKeeper->StartSmoothing();
    }

    presetSwitchedEvent(hard_cut, **m_presetPos);
    errorLoadingCurrentPreset = false;

    populatePresetMenu();

    return true;
}

void projectM::selectRandom(const bool hardCut)
{
    if (m_presetChooser->empty())
    {
        return;
    }
    presetHistory.push_back(m_presetPos->lastIndex());

    for (int i = 0; i < kMaxSwitchRetries; ++i)
    {
        *m_presetPos = m_presetChooser->weightedRandom(hardCut);
        if (startPresetTransition(hardCut))
        {
            break;
        }
    }
    // If presetHistory is tracking more than 10, then delete the oldest entry so we cap to a history of 10.
    if (presetHistory.size() >= 10)
    {
        presetHistory.erase(presetHistory.begin());
    }
    presetFuture.clear();

}

void projectM::selectPrevious(const bool hardCut)
{
    if (m_presetChooser->empty())
    {
        return;
    }

    if (isTextInputActive() && renderer->m_presetList.size() >= 1)
    {
        // if search menu is up, previous is based on search terms.
        if (renderer->m_activePresetID <= 1)
        {
            // loop to bottom of page is at top
            renderer->m_activePresetID = renderer->m_presetList.size();
            selectPresetByName(renderer->m_presetList[renderer->m_activePresetID - 1].name, true);
        }
        else
        {
            // otherwise move back
            renderer->m_activePresetID--;
            selectPresetByName(renderer->m_presetList[renderer->m_activePresetID - 1].name, true);
        }
    }
    else if (settings().shuffleEnabled && presetHistory.size() >= 1 &&
             static_cast<std::size_t>(presetHistory.back()) != m_presetLoader->size() && !renderer->showmenu)
    { // if randomly browsing presets, "previous" should return to last random preset not the index--. Avoid returning to size() because that's the idle:// preset.
        presetFuture.push_back(m_presetPos->lastIndex());
        selectPreset(presetHistory.back());
        presetHistory.pop_back();
    }
    else
    {
        // if we are not shuffling or there is no random future history, then let's not track a random vector and move backwards in the preset index.
        presetHistory.clear();
        presetFuture.clear();
        m_presetChooser->previousPreset(*m_presetPos);
        if (!startPresetTransition(hardCut))
        {
            selectRandom(hardCut);
        }
    }
}

void projectM::selectNext(const bool hardCut)
{
    if (m_presetChooser->empty())
    {
        return;
    }

    if (isTextInputActive() && renderer->m_presetList.size() >= 1) // if search is active and there are search results
    {
        // if search menu is down, next is based on search terms.
        if (static_cast<std::size_t>(renderer->m_activePresetID) >= renderer->m_presetList.size())
        {
            // loop to top of page is at bottom
            renderer->m_activePresetID = 1;
            selectPresetByName(renderer->m_presetList[0].name, true);
        }
        else
        {
            // otherwise move forward 
            renderer->m_activePresetID++;
            selectPresetByName(renderer->m_presetList[renderer->m_activePresetID - 1].name, true);
        }
    }
    else if (settings().shuffleEnabled && presetFuture.size() >= 1 &&
             static_cast<std::size_t>(presetFuture.front()) != m_presetLoader->size() && !renderer->showmenu)
    { // if shuffling and we have future presets already stashed then let's go forward rather than truely move randomly.
        presetHistory.push_back(m_presetPos->lastIndex());
        selectPreset(presetFuture.back());
        presetFuture.pop_back();
    }
    else
    {
        // if we are not shuffling or there is no random history, then let's not track a random vector and move forwards in the preset index.
        presetFuture.clear();
        presetHistory.clear();
        m_presetChooser->nextPreset(*m_presetPos);
        if (!startPresetTransition(hardCut))
        {
            selectRandom(hardCut);
        }
    }
}

/**
 * Switches the pipeline and renderer to the current preset.
 * @return the resulting Preset object, or nullptr on failure.
 */
std::unique_ptr<Preset> projectM::switchToCurrentPreset() {
  std::unique_ptr<Preset> new_preset;

#if USE_THREADS
    std::lock_guard<std::recursive_mutex> guard(m_presetSwitchMutex);
#endif

  try {
    new_preset = m_presetPos->allocate();
  } catch (const PresetFactoryException &e) {
    std::cerr << "problem allocating target preset: " << e.message()
              << std::endl;
  }

  if (new_preset == nullptr) {
    std::cerr << "Could not switch to current preset" << std::endl;
    return nullptr;
  }

    // Set preset name here- event is not done because at the moment this function
    // is oblivious to smooth/hard switches
    renderer->setPresetName(new_preset->name());
    std::string result = renderer->SetPipeline(new_preset->pipeline());
    if (!result.empty())
    {
        std::cerr << "problem setting pipeline: " << result << std::endl;
    }

  return new_preset;
}

void projectM::setPresetLock(bool isLocked)
{
    renderer->noSwitch = isLocked;
    if (isPresetLocked())
    {
        renderer->setToastMessage("Preset Locked");
    }
    else
    {
        renderer->setToastMessage("Unlocked");
    }
}

// check if search menu is up and you have search terms (2 chars). nomin means you don't care about search terms.
bool projectM::isTextInputActive(bool nomin) const
{
    if (renderer->showsearch && (renderer->searchText().length() >= 2 || nomin))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool projectM::isPresetLocked() const
{
    return renderer->noSwitch;
}

std::string projectM::getPresetURL(unsigned int index) const
{
    return m_presetLoader->getPresetURL(index);
}

int projectM::getPresetRating(unsigned int index, const PresetRatingType ratingType) const
{
    return m_presetLoader->getPresetRating(index, ratingType);
}

std::string projectM::getPresetName(unsigned int index) const
{
    return m_presetLoader->getPresetName(index);
}

void projectM::clearPlaylist()
{
    m_presetLoader->clear();
    *m_presetPos = m_presetChooser->end();
}

void projectM::selectPresetPosition(unsigned int index)
{
    *m_presetPos = m_presetChooser->begin(index);
}

bool projectM::selectedPresetIndex(unsigned int& index) const
{

    if (*m_presetPos == m_presetChooser->end())
    {
        return false;
    }

    index = **m_presetPos;
    return true;
}


bool projectM::presetPositionValid() const
{

    return (*m_presetPos != m_presetChooser->end());
}

unsigned int projectM::getPlaylistSize() const
{
    return m_presetLoader->size();
}

void projectM::changePresetRating(unsigned int index, int rating, const PresetRatingType ratingType)
{
    m_presetLoader->setRating(index, rating, ratingType);
    presetRatingChanged(index, rating, ratingType);
}

void projectM::insertPresetURL(unsigned int index, const std::string& presetURL, const std::string& presetName,
                               const RatingList& ratings)
{
    bool atEndPosition = false;

    int newSelectedIndex = 0;


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

    m_presetLoader->insertPresetURL(index, presetURL, presetName, ratings);

    if (atEndPosition)
    {
        *m_presetPos = m_presetChooser->end();
    }
    else
    {
        *m_presetPos = m_presetChooser->begin(newSelectedIndex);
    }


}

void projectM::changePresetName(unsigned int index, std::string name)
{
    m_presetLoader->setPresetName(index, name);
}


void projectM::setTextureSize(size_t size)
{
    _settings.textureSize = size;

    recreateRenderer();
}

size_t projectM::getTextureSize() const
{
    return _settings.textureSize;
}

double projectM::getSoftCutDuration() const
{
    return _settings.softCutDuration;
}

void projectM::setSoftCutDuration(int seconds)
{
    _settings.softCutDuration = static_cast<double>(seconds);
    timeKeeper->ChangeSoftCutDuration(seconds);
}

void projectM::setSoftCutDuration(double seconds)
{
    _settings.softCutDuration = seconds;
    timeKeeper->ChangeSoftCutDuration(seconds);
}

double projectM::getHardCutDuration() const
{
    return _settings.hardCutDuration;
}

void projectM::setHardCutDuration(int seconds)
{
    _settings.hardCutDuration = seconds;
    timeKeeper->ChangeHardCutDuration(seconds);
}

void projectM::setHardCutDuration(double seconds)
{
    _settings.hardCutDuration = static_cast<int>(seconds);
    timeKeeper->ChangeHardCutDuration(seconds);
}

bool projectM::getHardCutEnabled() const
{
    return _settings.hardCutEnabled;
}

void projectM::setHardCutEnabled(bool enabled)
{
    _settings.hardCutEnabled = enabled;
}

float projectM::getHardCutSensitivity() const
{
    return _settings.hardCutSensitivity;
}

void projectM::setHardCutSensitivity(float sensitivity)
{
    _settings.hardCutSensitivity = sensitivity;
}

void projectM::setPresetDuration(int seconds)
{
    timeKeeper->ChangePresetDuration(seconds);
}

void projectM::setPresetDuration(double seconds)
{
    timeKeeper->ChangePresetDuration(seconds);
}

bool projectM::getAspectCorrection() const
{
    return _settings.aspectCorrection;
}

void projectM::setAspectCorrection(bool enabled)
{
    _settings.aspectCorrection = enabled;
    renderer->correction = enabled;
}

float projectM::getEasterEgg() const
{
    return _settings.easterEgg;
}

void projectM::setEasterEgg(float value)
{
    _settings.easterEgg = value;
    timeKeeper->ChangeEasterEgg(value);
}

void projectM::getMeshSize(size_t& w, size_t& h) const
{
    w = _settings.meshX;
    h = _settings.meshY;
}

void projectM::setMeshSize(size_t w, size_t h)
{
    _settings.meshX = w;
    _settings.meshY = h;

    recreateRenderer();
}

// toggleSearchText
void projectM::toggleSearchText()
{
    if (renderer)
    {
        renderer->toggleSearchText();
    }
}

// get index from search results based on preset name
unsigned int projectM::getSearchIndex(const std::string& name) const
{
    for (auto& it: renderer->m_presetList)
    {
        if (it.name == name)
        {
            return it.id;
        }
    }
    return 0;
}

// get preset index based on preset name
unsigned int projectM::getPresetIndex(const std::string& name) const
{
    return m_presetLoader->getPresetIndex(name);
}

// load preset based on name
void projectM::selectPresetByName(std::string name, bool hardCut)
{
    if (name == "")
    {
        return;
    }
    unsigned int index = getPresetIndex(name);
    if (m_presetChooser->empty())
    {
        return;
    }
    selectPreset(index);
}

std::string projectM::getSearchText() const
{
    return renderer->getSearchText();
}

// update search text based on new keystroke
void projectM::setSearchText(const std::string& searchKey)
{
    if (renderer)
    {
        renderer->setSearchText(searchKey);
    }
    populatePresetMenu();
    if (renderer->m_presetList.size() >= 1)
    {
        std::string topPreset = renderer->m_presetList.front().name;
        renderer->m_activePresetID = 1;
        selectPresetByName(topPreset);
    }
}

// update search text based on new backspace
void projectM::deleteSearchText()
{
    if (renderer)
    {
        renderer->deleteSearchText();
    }
    populatePresetMenu();
    if (renderer->m_presetList.size() >= 1)
    {
        renderer->m_activePresetID = 1;
        std::string topPreset = renderer->m_presetList.front().name;
        selectPresetByName(topPreset);
    }
}

// reset search text
void projectM::resetSearchText()
{
    if (renderer)
    {
        renderer->resetSearchText();
    }
    populatePresetMenu();
    if (renderer->m_presetList.size() >= 1)
    {
        renderer->m_activePresetID = 1;
        std::string topPreset = renderer->m_presetList.front().name;
        selectPresetByName(topPreset);
    }
}

void projectM::setToastMessage(const std::string& toastMessage)
{
    if (renderer)
    {
        renderer->setToastMessage(toastMessage);
    }
}

void projectM::touch(float x, float y, int pressure, int touchtype)
{
    if (renderer)
    {
        renderer->touch(x, y, pressure, touchtype);
    }
}

void projectM::touchDrag(float x, float y, int pressure)
{
    if (renderer)
    {
        renderer->touchDrag(x, y, pressure);
    }
}


void projectM::touchDestroy(float x, float y)
{
    if (renderer)
    {
        renderer->touchDestroy(x, y);
    }
}

void projectM::touchDestroyAll()
{
    if (renderer)
    {
        renderer->touchDestroyAll();
    }
}

void projectM::setHelpText(const std::string& helpText)
{
    if (renderer)
    {
        renderer->setHelpText(helpText);
    }
}

void projectM::recreateRenderer()
{
    delete renderer;
    renderer = new Renderer(_settings.windowWidth, _settings.windowHeight,
                            _settings.meshX, _settings.meshY,
                            beatDetect, _settings.presetURL,
                            _settings.titleFontURL, _settings.menuFontURL,
                            _settings.datadir);
}
