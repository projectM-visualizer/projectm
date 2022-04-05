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

#include "ProjectM.hpp"

#include "BeatDetect.hpp"
#include "ConfigFile.h"
#include "PCM.hpp"                    //Sound data handler (buffering, FFT, etc.)
#include "PipelineContext.hpp"
#include "PipelineMerger.hpp"
#include "Preset.hpp"
#include "PresetChooser.hpp"
#include "RenderItemMatcher.hpp"
#include "RenderItemMergeFunction.hpp"
#include "Renderer.hpp"
#include "TimeKeeper.hpp"
#include "fatal.h"
#include "projectM-opengl.h"

#ifdef WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

#include <iostream>
#include <map>

namespace {
constexpr int kMaxSwitchRetries = 10;
}

ProjectM::~ProjectM()
{
#if USE_THREADS
    m_workerSync.FinishUp();
    m_workerThread.join();
#endif

    DestroyPresetTools();
}

unsigned ProjectM::InitRenderToTexture()
{
    return m_renderer->initRenderToTexture();
}

void ProjectM::ResetTextures()
{
    m_renderer->ResetTextures();
}


ProjectM::ProjectM(std::string config_file, Flags flags)
    : m_flags(flags)
    , m_pipelineContext(std::make_unique<class PipelineContext>())
    , m_pipelineContext2(std::make_unique<class PipelineContext>())
{
    ReadConfig(config_file);
    Reset();
    ResetOpenGL(m_settings.windowWidth, m_settings.windowHeight);

}

ProjectM::ProjectM(class Settings settings, Flags flags)
    : m_flags(flags)
    , m_pipelineContext(std::make_unique<class PipelineContext>())
    , m_pipelineContext2(std::make_unique<class PipelineContext>())
{
    ReadSettings(settings);
    Reset();
    ResetOpenGL(m_settings.windowWidth, m_settings.windowHeight);
}


bool ProjectM::WriteConfig(const std::string& configFile, const class Settings& settings)
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


void ProjectM::ReadConfig(const std::string& configFile)
{
    std::cout << "[projectM] config file: " << configFile << std::endl;

    ConfigFile config(configFile);
    m_settings.meshX = config.read<int>("Mesh X", 32);
    m_settings.meshY = config.read<int>("Mesh Y", 24);
    m_settings.textureSize = config.read<int>("Texture Size", 512);
    m_settings.fps = config.read<int>("FPS", 35);
    m_settings.windowWidth = config.read<int>("Window Width", 512);
    m_settings.windowHeight = config.read<int>("Window Height", 512);
    m_settings.softCutDuration = config.read<double>
        ("Smooth Preset Duration", config.read<int>("Smooth Transition Duration", 10));
    m_settings.presetDuration = config.read<double>("Preset Duration", 15);

#ifdef __unix__
    m_settings.presetURL = config.read<string>("Preset Path", "/usr/local/share/projectM/presets");
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
    m_settings.titleFontURL = config.read<string>
        ("Title Font", "/usr/local/share/projectM/fonts/Vera.tff");
    m_settings.menuFontURL = config.read<string>
        ("Menu Font", "/usr/local/share/projectM/fonts/VeraMono.tff");
#endif

#ifdef WIN32
    _settings.titleFontURL = config.read<string>
    ( "Title Font", projectM_FONT_TITLE );
    _settings.menuFontURL = config.read<string>
    ( "Menu Font", projectM_FONT_MENU );
#endif


    m_settings.shuffleEnabled = config.read<bool>("Shuffle Enabled", true);

    m_settings.easterEgg = config.read<float>("Easter Egg Parameter", 0.0);
    m_settings.softCutRatingsEnabled =
        config.read<bool>("Soft Cut Ratings Enabled", false);

    // Hard Cuts are preset transitions that occur when your music becomes louder. They only occur after a hard cut duration threshold has passed.
    m_settings.hardCutEnabled = config.read<bool>("Hard Cuts Enabled", false);
    // Hard Cut duration is the number of seconds before you become eligible for a hard cut.
    m_settings.hardCutDuration = config.read<double>("Hard Cut Duration", 60);
    // Hard Cut sensitivity is the volume difference before a "hard cut" is triggered.
    m_settings.hardCutSensitivity = config.read<float>("Hard Cut Sensitivity", 1.0);

    // Beat Sensitivity impacts how reactive your visualizations are to volume, bass, mid-range, and treble. 
    // Preset authors have developed their visualizations with the default of 1.0.
    m_settings.beatSensitivity = config.read<float>("Beat Sensitivity", 1.0);


    Initialize(m_settings.meshX, m_settings.meshY, m_settings.fps,
               m_settings.textureSize, m_settings.windowWidth, m_settings.windowHeight);

    if (config.read("Aspect Correction", true))
    {
        m_settings.aspectCorrection = true;
        m_renderer->correction = true;
    }
    else
    {
        m_settings.aspectCorrection = false;
        m_renderer->correction = false;
    }


}


void ProjectM::ReadSettings(const class Settings& settings)
{
    m_settings.meshX = settings.meshX;
    m_settings.meshY = settings.meshY;
    m_settings.textureSize = settings.textureSize;
    m_settings.fps = settings.fps;
    m_settings.windowWidth = settings.windowWidth;
    m_settings.windowHeight = settings.windowHeight;
    m_settings.softCutDuration = settings.softCutDuration;
    m_settings.presetDuration = settings.presetDuration;
    m_settings.softCutRatingsEnabled = settings.softCutRatingsEnabled;

    m_settings.presetURL = settings.presetURL;
    m_settings.titleFontURL = settings.titleFontURL;
    m_settings.menuFontURL = settings.menuFontURL;
    m_settings.shuffleEnabled = settings.shuffleEnabled;
    m_settings.datadir = settings.datadir;

    m_settings.easterEgg = settings.easterEgg;

    m_settings.hardCutEnabled = settings.hardCutEnabled;
    m_settings.hardCutDuration = settings.hardCutDuration;
    m_settings.hardCutSensitivity = settings.hardCutSensitivity;

    m_settings.beatSensitivity = settings.beatSensitivity;

    Initialize(m_settings.meshX, m_settings.meshY, m_settings.fps,
               m_settings.textureSize, m_settings.windowWidth, m_settings.windowHeight);


    m_settings.aspectCorrection = settings.aspectCorrection;
}

#if USE_THREADS

void ProjectM::ThreadWorker()
{
    while (true)
    {
        if (!m_workerSync.WaitForWork())
        {
            return;
        }
        EvaluateSecondPreset();
        m_workerSync.FinishedWork();
    }
}

#endif

void ProjectM::EvaluateSecondPreset()
{
    PipelineContext2().time = m_timeKeeper->GetRunningTime();
    PipelineContext2().presetStartTime = m_timeKeeper->PresetTimeB();
    PipelineContext2().frame = m_timeKeeper->PresetFrameB();
    PipelineContext2().progress = m_timeKeeper->PresetProgressB();

    m_activePreset2->Render(*m_beatDetect, PipelineContext2());
}

void ProjectM::RenderFrame()
{
    Pipeline pipeline;
    Pipeline* comboPipeline;

    comboPipeline = RenderFrameOnlyPass1(&pipeline);

    RenderFrameOnlyPass2(comboPipeline, 0, 0, 0);

    ProjectM::RenderFrameEndOnSeparatePasses(comboPipeline);
}


Pipeline* ProjectM::RenderFrameOnlyPass1(
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

    m_timeKeeper->UpdateTimers();

    /// @bug who is responsible for updating this now?"
    auto& context = PipelineContext();
    context.time = m_timeKeeper->GetRunningTime();
    context.presetStartTime = m_timeKeeper->PresetTimeA();
    context.frame = m_timeKeeper->PresetFrameA();
    context.progress = m_timeKeeper->PresetProgressA();
    m_renderer->UpdateContext(context);

    m_beatDetect->CalculateBeatStatistics();

    //m_activePreset->evaluateFrame();

    //if the preset isn't locked and there are more presets
    if (m_renderer->noSwitch == false && !m_presetChooser->empty())
    {
        //if preset is done and we're not already switching
        if (m_timeKeeper->PresetProgressA() >= 1.0 && !m_timeKeeper->IsSmoothing())
        {
            if (Settings().shuffleEnabled)
            {
                SelectRandom(false);
            }
            else
            {
                SelectNext(false);
            }
        }
        else if (Settings().hardCutEnabled && (m_beatDetect->vol - m_beatDetect->volOld > Settings().hardCutSensitivity) &&
                 m_timeKeeper->CanHardCut())
        {
            // Hard Cuts must be enabled, must have passed the hardcut duration, and the volume must be a greater difference than the hardcut sensitivity.
            if (Settings().shuffleEnabled)
            {
                SelectRandom(true);
            }
            else
            {
                SelectNext(true);
            }
        }
    }


    if (m_timeKeeper->IsSmoothing() && m_timeKeeper->SmoothRatio() <= 1.0 && !m_presetChooser->empty())
    {
        //	 printf("start thread\n");
        assert(m_activePreset2.get());

#if USE_THREADS
        m_workerSync.WakeUpBackgroundTask();
#endif

        m_activePreset->Render(*m_beatDetect, PipelineContext());

#if USE_THREADS
        m_workerSync.WaitForBackgroundTaskToFinish();
#else
        evaluateSecondPreset();
#endif

        pPipeline->setStaticPerPixel(Settings().meshX, Settings().meshY);

        assert(m_matcher);
        PipelineMerger::mergePipelines(m_activePreset->pipeline(),
                                       m_activePreset2->pipeline(), *pPipeline,
                                       m_matcher->matchResults(),
                                       *m_merger, m_timeKeeper->SmoothRatio());

        m_renderer->RenderFrameOnlyPass1(*pPipeline, PipelineContext());


        return pPipeline;

    }
    else
    {


        if (m_timeKeeper->IsSmoothing() && m_timeKeeper->SmoothRatio() > 1.0)
        {
            //printf("End Smooth\n");
            m_activePreset = std::move(m_activePreset2);
            m_timeKeeper->EndSmoothing();
        }
        //printf("Normal\n");

        m_activePreset->Render(*m_beatDetect, PipelineContext());
        m_renderer->RenderFrameOnlyPass1(m_activePreset->pipeline(), PipelineContext());
        return NULL; // indicating no transition

    }

    //	std::cout<< m_activePreset->absoluteFilePath()<<std::endl;
    //	renderer->presetName = m_activePreset->absoluteFilePath();




}


/* eye is 0,or 1, or who knows?*/
void ProjectM::RenderFrameOnlyPass2(Pipeline* pPipeline, int xoffset, int yoffset,
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

        m_renderer->RenderFrameOnlyPass2(*pPipeline, PipelineContext(), xoffset, yoffset, eye);

    }
    else
    {


        m_renderer->RenderFrameOnlyPass2(m_activePreset->pipeline(), PipelineContext(), xoffset, yoffset, eye);


    }

}


void ProjectM::RenderFrameEndOnSeparatePasses(Pipeline* pPipeline)
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

    m_count++;
}

void ProjectM::Reset()
{
    this->m_count = 0;

    ResetEngine();
}

void ProjectM::Initialize(int gx, int gy, int fps, int texsize, int width, int height)
{
    /** Initialise start time */
    m_timeKeeper = std::make_unique<TimeKeeper>(m_settings.presetDuration, m_settings.softCutDuration, m_settings.hardCutDuration,
                                m_settings.easterEgg);

    /** Nullify frame stash */

    /** Initialise per-pixel matrix calculations */
    /** We need to initialise this before the builtin param db otherwise bass/mid etc won't bind correctly */
    assert(!m_beatDetect);

    m_beatDetect = std::make_unique<BeatDetect>(m_pcm);

    this->m_renderer = std::make_unique<Renderer>(width, height, gx, gy, m_beatDetect.get(), Settings().presetURL, Settings().titleFontURL,
                                                  Settings().menuFontURL, Settings().datadir);

    InitializePresetTools(gx, gy);

#if USE_THREADS
    m_workerSync.Reset();
    m_workerThread = std::thread(&ProjectM::ThreadWorker, this);
#endif

    /// @bug order of operatoins here is busted
    //renderer->setPresetName ( m_activePreset->name() );
    m_timeKeeper->StartPreset();

    // ToDo: Calculate the real FPS instead
    PipelineContext().fps = fps;
    PipelineContext2().fps = fps;

}

/* Reinitializes the engine variables to a default (conservative and sane) value */
void ProjectM::ResetEngine()
{

    if (m_beatDetect != NULL)
    {
        m_beatDetect->Reset();
        m_beatDetect->beatSensitivity = m_settings.beatSensitivity;
    }

}

/** Resets OpenGL state */
void ProjectM::ResetOpenGL(size_t width, size_t height)
{
    /** Stash the new dimensions */
    m_settings.windowWidth = width;
    m_settings.windowHeight = height;
    m_renderer->reset(width, height);
}

/** Sets the title to display */
std::string ProjectM::Title() const
{
    return m_renderer->title;
}

/** Sets the title to display */
void ProjectM::SetTitle(const std::string& title)
{
    if (title != m_renderer->title)
    {
        m_renderer->title = title;
        m_renderer->drawtitle = 1;
    }
}


int ProjectM::InitializePresetTools(int gx, int gy)
{

    /* Set the seed to the current time in seconds */
    srand(time(NULL));

    std::string url;
    if ((m_flags & Flags::DisablePlaylistLoad) == Flags::DisablePlaylistLoad)
    {
        url = Settings().presetURL;
    }

    if ((m_presetLoader = std::make_unique<PresetLoader>(gx, gy, url)) == 0)
    {
        m_presetLoader = 0;
        std::cerr << "[projectM] error allocating preset loader" << std::endl;
        return PROJECTM_FAILURE;
    }

    if ((m_presetChooser = std::make_unique<PresetChooser>(*m_presetLoader, Settings().softCutRatingsEnabled)) == 0)
    {
        m_presetLoader.reset();

        m_presetChooser = 0;
        m_presetLoader = 0;

        std::cerr << "[projectM] error allocating preset chooser" << std::endl;
        return PROJECTM_FAILURE;
    }

    // Start the iterator
    if (!m_presetPos)
    {
        m_presetPos = std::make_unique<PresetIterator>();
    }

    // Initialize a preset queue position as well
    //	m_presetQueuePos = new PresetIterator();

    // Start at end ptr- this allows next/previous to easily be done from this position.
    *m_presetPos = m_presetChooser->end();

    // Load idle preset
//        std::cerr << "[projectM] Allocating idle preset..." << std::endl;
    m_activePreset = m_presetLoader->loadPreset
        ("idle://Geiss & Sperl - Feedback (projectM idle HDR mix).milk");
    m_renderer->setPresetName("Geiss & Sperl - Feedback (projectM idle HDR mix)");

    PopulatePresetMenu();

    m_renderer->SetPipeline(m_activePreset->pipeline());

    // Case where no valid presets exist in directory. Could also mean
    // playlist initialization was deferred
    if (m_presetChooser->empty())
    {
        //std::cerr << "[projectM] warning: no valid files found in preset directory \""
        ///< m_presetLoader->directoryName() << "\"" << std::endl;
    }

    m_matcher = std::make_unique<RenderItemMatcher>();
    m_merger = std::make_unique<MasterRenderItemMerge>();
    //_merger->add(new WaveFormMergeFunction());
    m_merger->add(new ShapeMerge());
    m_merger->add(new BorderMerge());
    //_merger->add(new BorderMergeFunction());

    /// @bug These should be requested by the preset factories.
    m_matcher->distanceFunction().addMetric(new ShapeXYDistance());

    //std::cerr << "[projectM] Idle preset allocated." << std::endl;

    ResetEngine();

    //std::cerr << "[projectM] engine has been reset." << std::endl;
    return PROJECTM_SUCCESS;
}

void ProjectM::DestroyPresetTools()
{
    m_activePreset.reset();
    m_activePreset2.reset();
    m_presetPos.reset();
    m_presetChooser.reset();
    m_presetLoader.reset();
    m_matcher.reset();
    m_merger.reset();
}

/// @bug queuePreset case isn't handled
void ProjectM::RemovePreset(unsigned int index)
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
ProjectM::AddPresetURL(const std::string& presetURL, const std::string& presetName, const RatingList& ratingList)
{
    bool restorePosition = false;

    if (*m_presetPos == m_presetChooser->end())
    {
        restorePosition = true;
    }

    int index = m_presetLoader->addPresetURL(presetURL, presetName, ratingList);

    if (restorePosition)
    {
        *m_presetPos = m_presetChooser->end();
    }

    return index;
}

void ProjectM::SelectPreset(unsigned int index, bool hardCut)
{
    if (m_presetChooser->empty())
    {
        return;
    }

    PopulatePresetMenu();

    *m_presetPos = m_presetChooser->begin(index);
    if (!StartPresetTransition(hardCut))
    {
        SelectRandom(hardCut);
    }
}

// populatePresetMenu is called when a preset is loaded.
void ProjectM::PopulatePresetMenu()
{
    if (m_renderer->showmenu)
    { // only track a preset list buffer if the preset menu is up.
        m_renderer->m_presetList.clear(); // clear preset list buffer from renderer.

        if (TextInputActive())
        {
            // if a searchTerm is active, we will populate the preset menu with search terms instead of the page we are on.
            int h = 0;
            std::string presetName = m_renderer->presetName();
            int presetIndex = SearchIndex(presetName);
            for (unsigned int i = 0; i < PlaylistSize(); i++)
            { // loop over all presets
                if (CaseInsensitiveSubstringFind(PresetName(i), m_renderer->searchText()) != std::string::npos)
                { // if term matches
                    if (h < m_renderer->textMenuPageSize) // limit to just one page, pagination is not needed.
                    {
                        h++;
                        m_renderer->m_presetList.push_back(
                            { h, PresetName(i), "" }); // populate the renders preset list.
                        if (h == presetIndex)
                        {
                            m_renderer->m_activePresetID = h;
                        }
                    }
                }
            }
        }
        else
        {
            // normal preset menu, based on pagination.
            m_renderer->m_activePresetID = m_presetPos->lastIndex(); // tell renderer about the active preset ID (so it can be highlighted)
            int page_start = 0;
            if (m_presetPos->lastIndex() != m_presetLoader->size())
            {
                page_start = m_renderer->m_activePresetID; // if it's not the idle preset, then set it to the true value
            }
            if (page_start < m_renderer->textMenuPageSize)
            {
                page_start = 0; // if we are on page 1, start at the first preset.
            }
            if (page_start % m_renderer->textMenuPageSize == 0)
            {
                // if it's a perfect division of the page size, we are good.
            }
            else
            {
                page_start = page_start - (page_start %
                                           m_renderer->textMenuPageSize); // if not, find closest divisable number for page start
            }
            int page_end = page_start + m_renderer->textMenuPageSize; // page end is page start + page size
            if (page_end > m_presetLoader->size())
            {
                page_end = m_presetLoader->size();
            }
            while (page_start < page_end)
            {
                m_renderer->m_presetList.push_back(
                    { page_start, PresetName(page_start), "" }); // populate the renders preset list.
                page_start++;
            }
        }
    }
}

bool ProjectM::StartPresetTransition(bool hard_cut)
{
    std::unique_ptr<Preset> new_preset = SwitchToCurrentPreset();
    if (new_preset == nullptr)
    {
        PresetSwitchFailedEvent(hard_cut, **m_presetPos, "fake error");
        m_errorLoadingCurrentPreset = true;
        PopulatePresetMenu();
        return false;
    }

    if (hard_cut)
    {
        m_activePreset = std::move(new_preset);
        m_timeKeeper->StartPreset();
    }
    else
    {
        m_activePreset2 = std::move(new_preset);
        m_timeKeeper->StartPreset();
        m_timeKeeper->StartSmoothing();
    }

    PresetSwitchedEvent(hard_cut, **m_presetPos);
    m_errorLoadingCurrentPreset = false;

    PopulatePresetMenu();

    return true;
}

void ProjectM::SelectRandom(const bool hardCut)
{
    if (m_presetChooser->empty())
    {
        return;
    }
    m_presetHistory.push_back(m_presetPos->lastIndex());

    for (int i = 0; i < kMaxSwitchRetries; ++i)
    {
        *m_presetPos = m_presetChooser->weightedRandom(hardCut);
        if (StartPresetTransition(hardCut))
        {
            break;
        }
    }
    // If presetHistory is tracking more than 10, then delete the oldest entry so we cap to a history of 10.
    if (m_presetHistory.size() >= 10)
    {
        m_presetHistory.erase(m_presetHistory.begin());
    }
    m_presetFuture.clear();

}

void ProjectM::SelectPrevious(const bool hardCut)
{
    if (m_presetChooser->empty())
    {
        return;
    }

    if (TextInputActive() && m_renderer->m_presetList.size() >= 1)
    {
        // if search menu is up, previous is based on search terms.
        if (m_renderer->m_activePresetID <= 1)
        {
            // loop to bottom of page is at top
            m_renderer->m_activePresetID = m_renderer->m_presetList.size();
            SelectPresetByName(m_renderer->m_presetList[m_renderer->m_activePresetID - 1].name, true);
        }
        else
        {
            // otherwise move back
            m_renderer->m_activePresetID--;
            SelectPresetByName(m_renderer->m_presetList[m_renderer->m_activePresetID - 1].name, true);
        }
    }
    else if (Settings().shuffleEnabled && m_presetHistory.size() >= 1 &&
             static_cast<std::size_t>(m_presetHistory.back()) != m_presetLoader->size() && !m_renderer->showmenu)
    { // if randomly browsing presets, "previous" should return to last random preset not the index--. Avoid returning to size() because that's the idle:// preset.
        m_presetFuture.push_back(m_presetPos->lastIndex());
        SelectPreset(m_presetHistory.back());
        m_presetHistory.pop_back();
    }
    else
    {
        // if we are not shuffling or there is no random future history, then let's not track a random vector and move backwards in the preset index.
        m_presetHistory.clear();
        m_presetFuture.clear();
        m_presetChooser->previousPreset(*m_presetPos);
        if (!StartPresetTransition(hardCut))
        {
            SelectRandom(hardCut);
        }
    }
}

void ProjectM::SelectNext(const bool hardCut)
{
    if (m_presetChooser->empty())
    {
        return;
    }

    if (TextInputActive() && m_renderer->m_presetList.size() >= 1) // if search is active and there are search results
    {
        // if search menu is down, next is based on search terms.
        if (static_cast<std::size_t>(m_renderer->m_activePresetID) >= m_renderer->m_presetList.size())
        {
            // loop to top of page is at bottom
            m_renderer->m_activePresetID = 1;
            SelectPresetByName(m_renderer->m_presetList[0].name, true);
        }
        else
        {
            // otherwise move forward
            m_renderer->m_activePresetID++;
            SelectPresetByName(m_renderer->m_presetList[m_renderer->m_activePresetID - 1].name, true);
        }
    }
    else if (Settings().shuffleEnabled && m_presetFuture.size() >= 1 &&
             static_cast<std::size_t>(m_presetFuture.front()) != m_presetLoader->size() && !m_renderer->showmenu)
    { // if shuffling and we have future presets already stashed then let's go forward rather than truely move randomly.
        m_presetHistory.push_back(m_presetPos->lastIndex());
        SelectPreset(m_presetFuture.back());
        m_presetFuture.pop_back();
    }
    else
    {
        // if we are not shuffling or there is no random history, then let's not track a random vector and move forwards in the preset index.
        m_presetFuture.clear();
        m_presetHistory.clear();
        m_presetChooser->nextPreset(*m_presetPos);
        if (!StartPresetTransition(hardCut))
        {
            SelectRandom(hardCut);
        }
    }
}

/**
 * Switches the pipeline and renderer to the current preset.
 * @return the resulting Preset object, or nullptr on failure.
 */
std::unique_ptr<Preset> ProjectM::SwitchToCurrentPreset()
{
    std::unique_ptr<Preset> new_preset;
#if USE_THREADS
    std::lock_guard<std::recursive_mutex> guard(m_presetSwitchMutex);
#endif

    try
    {
        new_preset = m_presetPos->allocate();
    }
    catch (const PresetFactoryException& e)
    {
        std::cerr << "problem allocating target preset: " << e.message()
                  << std::endl;
    }

    if (new_preset == nullptr)
    {
        std::cerr << "Could not switch to current preset" << std::endl;
        return nullptr;
    }

    // Set preset name here- event is not done because at the moment this function
    // is oblivious to smooth/hard switches
    m_renderer->setPresetName(new_preset->name());
    std::string result = m_renderer->SetPipeline(new_preset->pipeline());
    if (!result.empty())
    {
        std::cerr << "problem setting pipeline: " << result << std::endl;
    }

    return new_preset;
}

void ProjectM::SetPresetLocked(bool isLocked)
{
    m_renderer->noSwitch = isLocked;
    if (PresetLocked())
    {
        m_renderer->setToastMessage("Preset Locked");
    }
    else
    {
        m_renderer->setToastMessage("Unlocked");
    }
}

// check if search menu is up and you have search terms (2 chars). nomin means you don't care about search terms.
bool ProjectM::TextInputActive(bool nomin) const
{
    if (m_renderer->showsearch && (m_renderer->searchText().length() >= 2 || nomin))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ProjectM::PresetLocked() const
{
    return m_renderer->noSwitch;
}

std::string ProjectM::PresetURL(unsigned int index) const
{
    return m_presetLoader->getPresetURL(index);
}

int ProjectM::PresetRating(unsigned int index, const PresetRatingType ratingType) const
{
    return m_presetLoader->getPresetRating(index, ratingType);
}

std::string ProjectM::PresetName(unsigned int index) const
{
    return m_presetLoader->getPresetName(index);
}

void ProjectM::ClearPlaylist()
{
    m_presetLoader->clear();
    *m_presetPos = m_presetChooser->end();
}

void ProjectM::SelectPresetPosition(unsigned int index)
{
    *m_presetPos = m_presetChooser->begin(index);
}

bool ProjectM::SelectedPresetIndex(unsigned int& index) const
{

    if (*m_presetPos == m_presetChooser->end())
    {
        return false;
    }

    index = **m_presetPos;
    return true;
}


bool ProjectM::PresetPositionValid() const
{

    return (*m_presetPos != m_presetChooser->end());
}

unsigned int ProjectM::PlaylistSize() const
{
    return m_presetLoader->size();
}

void ProjectM::ChangePresetRating(unsigned int index, int rating, const PresetRatingType ratingType)
{
    m_presetLoader->setRating(index, rating, ratingType);
    PresetRatingChanged(index, rating, ratingType);
}

void ProjectM::InsertPresetURL(unsigned int index, const std::string& presetURL, const std::string& presetName,
                               const RatingList& ratingList)
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

    m_presetLoader->insertPresetURL(index, presetURL, presetName, ratingList);

    if (atEndPosition)
    {
        *m_presetPos = m_presetChooser->end();
    }
    else
    {
        *m_presetPos = m_presetChooser->begin(newSelectedIndex);
    }


}

void ProjectM::ChangePresetName(unsigned int index, std::string name)
{
    m_presetLoader->setPresetName(index, name);
}


void ProjectM::SetTextureSize(size_t size)
{
    m_settings.textureSize = size;

    RecreateRenderer();
}

size_t ProjectM::TextureSize() const
{
    return m_settings.textureSize;
}

double ProjectM::SoftCutDuration() const
{
    return m_settings.softCutDuration;
}

void ProjectM::SetSoftCutDuration(int seconds)
{
    m_settings.softCutDuration = static_cast<double>(seconds);
    m_timeKeeper->ChangeSoftCutDuration(seconds);
}

void ProjectM::SetSoftCutDuration(double seconds)
{
    m_settings.softCutDuration = seconds;
    m_timeKeeper->ChangeSoftCutDuration(seconds);
}

double ProjectM::HardCutDuration() const
{
    return m_settings.hardCutDuration;
}

void ProjectM::SetHardCutDuration(int seconds)
{
    m_settings.hardCutDuration = seconds;
    m_timeKeeper->ChangeHardCutDuration(seconds);
}

void ProjectM::SetHardCutDuration(double seconds)
{
    m_settings.hardCutDuration = static_cast<int>(seconds);
    m_timeKeeper->ChangeHardCutDuration(seconds);
}

bool ProjectM::HardCutEnabled() const
{
    return m_settings.hardCutEnabled;
}

void ProjectM::SetHardCutEnabled(bool enabled)
{
    m_settings.hardCutEnabled = enabled;
}

float ProjectM::HardCutSensitivity() const
{
    return m_settings.hardCutSensitivity;
}

void ProjectM::SetHardCutSensitivity(float sensitivity)
{
    m_settings.hardCutSensitivity = sensitivity;
}

void ProjectM::SetPresetDuration(int seconds)
{
    m_timeKeeper->ChangePresetDuration(seconds);
}

void ProjectM::SetPresetDuration(double seconds)
{
    m_timeKeeper->ChangePresetDuration(seconds);
}

bool ProjectM::AspectCorrection() const
{
    return m_settings.aspectCorrection;
}

void ProjectM::SetAspectCorrection(bool enabled)
{
    m_settings.aspectCorrection = enabled;
    m_renderer->correction = enabled;
}

float ProjectM::EasterEgg() const
{
    return m_settings.easterEgg;
}

void ProjectM::SetEasterEgg(float value)
{
    m_settings.easterEgg = value;
    m_timeKeeper->ChangeEasterEgg(value);
}

void ProjectM::MeshSize(size_t& w, size_t& h) const
{
    w = m_settings.meshX;
    h = m_settings.meshY;
}

void ProjectM::SetMeshSize(size_t w, size_t h)
{
    m_settings.meshX = w;
    m_settings.meshY = h;

    RecreateRenderer();
}

// toggleSearchText
void ProjectM::ToggleSearchText()
{
    if (m_renderer)
    {
        m_renderer->toggleSearchText();
    }
}

// get index from search results based on preset name
unsigned int ProjectM::SearchIndex(const std::string& name) const
{
    for (auto& it: m_renderer->m_presetList)
    {
        if (it.name == name)
        {
            return it.id;
        }
    }
    return 0;
}

// get preset index based on preset name
unsigned int ProjectM::PresetIndex(const std::string& url) const
{
    return m_presetLoader->getPresetIndex(url);
}

// load preset based on name
void ProjectM::SelectPresetByName(std::string name, bool hardCut)
{
    if (name == "")
    {
        return;
    }
    unsigned int index = PresetIndex(name);
    if (m_presetChooser->empty())
    {
        return;
    }
    SelectPreset(index);
}

std::string ProjectM::SearchText() const
{
    return m_renderer->getSearchText();
}

// update search text based on new keystroke
void ProjectM::SetSearchText(const std::string& searchKey)
{
    if (m_renderer)
    {
        m_renderer->setSearchText(searchKey);
    }
    PopulatePresetMenu();
    if (m_renderer->m_presetList.size() >= 1)
    {
        std::string topPreset = m_renderer->m_presetList.front().name;
        m_renderer->m_activePresetID = 1;
        SelectPresetByName(topPreset);
    }
}

// update search text based on new backspace
void ProjectM::DeleteSearchText()
{
    if (m_renderer)
    {
        m_renderer->deleteSearchText();
    }
    PopulatePresetMenu();
    if (m_renderer->m_presetList.size() >= 1)
    {
        m_renderer->m_activePresetID = 1;
        std::string topPreset = m_renderer->m_presetList.front().name;
        SelectPresetByName(topPreset);
    }
}

// reset search text
void ProjectM::ResetSearchText()
{
    if (m_renderer)
    {
        m_renderer->resetSearchText();
    }
    PopulatePresetMenu();
    if (m_renderer->m_presetList.size() >= 1)
    {
        m_renderer->m_activePresetID = 1;
        std::string topPreset = m_renderer->m_presetList.front().name;
        SelectPresetByName(topPreset);
    }
}

void ProjectM::SetToastMessage(const std::string& toastMessage)
{
    if (m_renderer)
    {
        m_renderer->setToastMessage(toastMessage);
    }
}

void ProjectM::Touch(float x, float y, int pressure, int touchtype)
{
    if (m_renderer)
    {
        m_renderer->touch(x, y, pressure, touchtype);
    }
}

void ProjectM::TouchDrag(float x, float y, int pressure)
{
    if (m_renderer)
    {
        m_renderer->touchDrag(x, y, pressure);
    }
}


void ProjectM::TouchDestroy(float x, float y)
{
    if (m_renderer)
    {
        m_renderer->touchDestroy(x, y);
    }
}

void ProjectM::TouchDestroyAll()
{
    if (m_renderer)
    {
        m_renderer->touchDestroyAll();
    }
}

void ProjectM::SetHelpText(const std::string& helpText)
{
    if (m_renderer)
    {
        m_renderer->setHelpText(helpText);
    }
}

void ProjectM::RecreateRenderer()
{
    m_renderer = std::make_unique<Renderer>(m_settings.windowWidth, m_settings.windowHeight,
                                            m_settings.meshX, m_settings.meshY,
                                            m_beatDetect.get(), m_settings.presetURL,
                                            m_settings.titleFontURL, m_settings.menuFontURL,
                                            m_settings.datadir);
}
