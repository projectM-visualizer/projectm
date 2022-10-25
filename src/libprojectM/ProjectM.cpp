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
#include "PCM.hpp" //Sound data handler (buffering, FFT, etc.)
#include "PipelineMerger.hpp"
#include "Preset.hpp"
#include "PresetChooser.hpp"
#include "Renderer.hpp"
#include "TimeKeeper.hpp"

#include <iostream>

namespace {
constexpr int kMaxSwitchRetries = 10;
}

ProjectM::~ProjectM()
{
#if USE_THREADS
    m_workerSync.FinishUp();
    m_workerThread.join();
#endif
}

auto ProjectM::InitRenderToTexture() -> unsigned
{
    return m_renderer->initRenderToTexture();
}

void ProjectM::ResetTextures()
{
    m_renderer->ResetTextures();
}


ProjectM::ProjectM(const std::string& configurationFilename, Flags flags)
    : m_flags(flags)
{
    ReadConfig(configurationFilename);
    Reset();
    ResetOpenGL(m_settings.windowWidth, m_settings.windowHeight);
}

ProjectM::ProjectM(const class Settings& settings, Flags flags)
    : m_flags(flags)
{
    ReadSettings(settings);
    Reset();
    ResetOpenGL(m_settings.windowWidth, m_settings.windowHeight);
}


auto ProjectM::WriteConfig(const std::string& configurationFilename, const class Settings& settings) -> bool
{

    ConfigFile config(configurationFilename);

    config.add("Mesh X", settings.meshX);
    config.add("Mesh Y", settings.meshY);
    config.add("Texture Size", settings.textureSize);
    config.add("FPS", settings.fps);
    config.add("Window Width", settings.windowWidth);
    config.add("Window Height", settings.windowHeight);
    config.add("Smooth Preset Duration", settings.softCutDuration);
    config.add("Preset Duration", settings.presetDuration);
    config.add("Preset Path", settings.presetPath);
    config.add("Hard Cut Sensitivity", settings.beatSensitivity);
    config.add("Aspect Correction", settings.aspectCorrection);
    config.add("Easter Egg Parameter", settings.easterEgg);
    config.add("Shuffle Enabled", settings.shuffleEnabled);
    config.add("Soft Cut Ratings Enabled", settings.softCutRatingsEnabled);
    std::fstream file(configurationFilename.c_str(), std::ios_base::trunc | std::ios_base::out);
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


void ProjectM::ReadConfig(const std::string& configurationFilename)
{
    std::cout << "[projectM] config file: " << configurationFilename << std::endl;

    ConfigFile config(configurationFilename);
    m_settings.meshX = config.read<int>("Mesh X", 32);
    m_settings.meshY = config.read<int>("Mesh Y", 24);
    m_settings.textureSize = config.read<int>("Texture Size", 512);
    m_settings.fps = config.read<int>("FPS", 35);
    m_settings.windowWidth = config.read<int>("Window Width", 512);
    m_settings.windowHeight = config.read<int>("Window Height", 512);
    m_settings.softCutDuration = config.read<double>("Smooth Preset Duration", config.read<int>("Smooth Transition Duration", 10));
    m_settings.presetDuration = config.read<double>("Preset Duration", 15);

#ifdef __unix__
    m_settings.presetPath = config.read<string>("Preset Path", "/usr/local/share/projectM/presets");
#endif

#ifdef __APPLE__
    /// @bug awful hardcoded hack- need to add intelligence to cmake wrt bundling - carm
    m_settings.presetPath = config.read<string>("Preset Path", "../Resources/presets");
#endif

#ifdef _WIN32
    m_settings.presetPath = config.read<string>("Preset Path", "/usr/local/share/projectM/presets");
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

    m_settings.aspectCorrection = config.read<bool>("Aspect Correction", true);

    Initialize();
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

    m_settings.presetPath = settings.presetPath;
    m_settings.texturePath = settings.texturePath;
    m_settings.shuffleEnabled = settings.shuffleEnabled;
    m_settings.dataPath = settings.dataPath;

    m_settings.easterEgg = settings.easterEgg;

    m_settings.hardCutEnabled = settings.hardCutEnabled;
    m_settings.hardCutDuration = settings.hardCutDuration;
    m_settings.hardCutSensitivity = settings.hardCutSensitivity;

    m_settings.beatSensitivity = settings.beatSensitivity;

    m_settings.aspectCorrection = settings.aspectCorrection;

    Initialize();
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
    Pipeline* comboPipeline = RenderFrameOnlyPass1(&pipeline);

    RenderFrameOnlyPass2(comboPipeline, 0, 0);

    ProjectM::RenderFrameEndOnSeparatePasses(comboPipeline);
}


auto ProjectM::RenderFrameOnlyPass1(Pipeline* pipeline) -> Pipeline*
{
#if USE_THREADS
    std::lock_guard<std::recursive_mutex> guard(m_presetSwitchMutex);
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

    //if the preset isn't locked and there are more presets
    if (!m_renderer->noSwitch && !m_presetChooser->empty())
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
        else if (Settings().hardCutEnabled &&
                 (m_beatDetect->vol - m_beatDetect->volOld > Settings().hardCutSensitivity) &&
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
        assert(m_activePreset2.get());

#if USE_THREADS
        m_workerSync.WakeUpBackgroundTask();
#endif

        m_activePreset->Render(*m_beatDetect, PipelineContext());

#if USE_THREADS
        // FIXME: Instead of waiting after a single render pass, check every frame if it's done.
        m_workerSync.WaitForBackgroundTaskToFinish();
#else
        EvaluateSecondPreset();
#endif

        pipeline->setStaticPerPixel(Settings().meshX, Settings().meshY);

        PipelineMerger::mergePipelines(
            m_activePreset->pipeline(),
            m_activePreset2->pipeline(),
            *pipeline,
            m_timeKeeper->SmoothRatio());

        m_renderer->RenderFrameOnlyPass1(*pipeline, PipelineContext());

        return pipeline;
    }

    if (m_timeKeeper->IsSmoothing() && m_timeKeeper->SmoothRatio() > 1.0)
    {
        m_activePreset = std::move(m_activePreset2);
        m_timeKeeper->EndSmoothing();
    }

    m_activePreset->Render(*m_beatDetect, PipelineContext());
    m_renderer->RenderFrameOnlyPass1(m_activePreset->pipeline(), PipelineContext());

    return nullptr; // indicating no transition
}

void ProjectM::RenderFrameOnlyPass2(Pipeline* pipeline,
                                    int offsetX,
                                    int offsetY)
{
    if (pipeline == nullptr)
    {
        assert(m_activePreset.get());
        pipeline = &m_activePreset->pipeline();
    }

    m_renderer->RenderFrameOnlyPass2(*pipeline, PipelineContext(), offsetX, offsetY, 0);
}


void ProjectM::RenderFrameEndOnSeparatePasses(Pipeline* pipeline)
{
    if (pipeline != nullptr)
    {
        // mergePipelines() sets masterAlpha for each RenderItem, reset it before we forget
        for (RenderItem* drawable : pipeline->drawables)
        {
            drawable->masterAlpha = 1.0;
        }
        pipeline->drawables.clear();
    }

    m_count++;
}

auto ProjectM::PipelineContext() -> class PipelineContext&
{
    return m_pipelineContext;
}

auto ProjectM::PipelineContext2() -> class PipelineContext&
{
    return m_pipelineContext2;
}

void ProjectM::Reset()
{
    this->m_count = 0;

    ResetEngine();
}

void ProjectM::Initialize()
{
    /** Initialise start time */
    m_timeKeeper = std::make_unique<TimeKeeper>(m_settings.presetDuration,
                                                m_settings.softCutDuration,
                                                m_settings.hardCutDuration,
                                                m_settings.easterEgg);

    /** Nullify frame stash */

    /** Initialise per-pixel matrix calculations */
    /** We need to initialise this before the builtin param db otherwise bass/mid etc won't bind correctly */
    assert(!m_beatDetect);

    m_beatDetect = std::make_unique<BeatDetect>(m_pcm);

    // Create texture search path list
    if (!m_settings.presetPath.empty())
    {
        m_textureSearchPaths.emplace_back(m_settings.presetPath);
    }
    if (!m_settings.texturePath.empty())
    {
        m_textureSearchPaths.emplace_back(m_settings.texturePath);
    }
    if (!m_settings.dataPath.empty())
    {
        m_textureSearchPaths.emplace_back(m_settings.dataPath + pathSeparator + "presets");
        m_textureSearchPaths.emplace_back(m_settings.dataPath + pathSeparator + "textures");
    }

    this->m_renderer = std::make_unique<Renderer>(m_settings.windowWidth,
                                                  m_settings.windowHeight,
                                                  m_settings.meshX,
                                                  m_settings.meshY,
                                                  m_beatDetect.get(),
                                                  m_textureSearchPaths);

    InitializePresetTools();

#if USE_THREADS
    m_workerSync.Reset();
    m_workerThread = std::thread(&ProjectM::ThreadWorker, this);
#endif

    /// @bug order of operatoins here is busted
    //renderer->setPresetName ( m_activePreset->name() );
    m_timeKeeper->StartPreset();

    // ToDo: Calculate the real FPS instead
    PipelineContext().fps = static_cast<int>(m_settings.fps);
    PipelineContext2().fps = static_cast<int>(m_settings.fps);
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

auto ProjectM::InitializePresetTools() -> void
{
    /* Set the seed to the current time in seconds */
    srand(time(nullptr));

    std::string url;
    if ((m_flags & Flags::DisablePlaylistLoad) != Flags::DisablePlaylistLoad)
    {
        url = Settings().presetPath;
    }

    m_presetLoader = std::make_unique<PresetLoader>(m_settings.meshX, m_settings.meshY, url);
    m_presetChooser = std::make_unique<PresetChooser>(*m_presetLoader, Settings().softCutRatingsEnabled);
    m_presetPos = std::make_unique<PresetIterator>();

    // Start at end ptr- this allows next/previous to easily be done from this position.
    *m_presetPos = m_presetChooser->end();

    // Load idle preset
    m_activePreset = m_presetLoader->loadPreset("idle://Geiss & Sperl - Feedback (projectM idle HDR mix).milk");
    m_renderer->setPresetName("Geiss & Sperl - Feedback (projectM idle HDR mix)");
    m_renderer->SetPipeline(m_activePreset->pipeline());

    ResetEngine();
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

auto ProjectM::AddPresetURL(const std::string& presetFilename, const std::string& presetName, const RatingList& ratingList) -> unsigned int
{
    bool restorePosition = false;

    if (*m_presetPos == m_presetChooser->end())
    {
        restorePosition = true;
    }

    int index = m_presetLoader->addPresetURL(presetFilename, presetName, ratingList);

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

    *m_presetPos = m_presetChooser->begin(index);
    if (!StartPresetTransition(hardCut))
    {
        SelectRandom(hardCut);
    }
}

bool ProjectM::StartPresetTransition(bool hardCut)
{
    std::unique_ptr<Preset> new_preset = SwitchToCurrentPreset();
    if (new_preset == nullptr)
    {
        PresetSwitchFailedEvent(hardCut, **m_presetPos, "fake error");
        m_errorLoadingCurrentPreset = true;

        return false;
    }

    if (hardCut)
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

    PresetSwitchedEvent(hardCut, **m_presetPos);
    m_errorLoadingCurrentPreset = false;

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

auto ProjectM::WindowWidth() -> int
{
    return m_settings.windowWidth;
}

auto ProjectM::WindowHeight() -> int
{
    return m_settings.windowHeight;
}

auto ProjectM::ErrorLoadingCurrentPreset() const -> bool
{
    return m_errorLoadingCurrentPreset;
}

void ProjectM::SelectPrevious(const bool hardCut)
{
    if (m_presetChooser->empty())
    {
        return;
    }

    if (Settings().shuffleEnabled && m_presetHistory.size() >= 1 &&
             static_cast<std::size_t>(m_presetHistory.back()) != m_presetLoader->size())
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

    if (Settings().shuffleEnabled && m_presetFuture.size() >= 1 &&
             static_cast<std::size_t>(m_presetFuture.front()) != m_presetLoader->size())
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
auto ProjectM::SwitchToCurrentPreset() -> std::unique_ptr<Preset>
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

void ProjectM::SetPresetLocked(bool locked)
{
    m_renderer->noSwitch = locked;
}

auto ProjectM::PresetLocked() const -> bool
{
    return m_renderer->noSwitch;
}

auto ProjectM::PresetURL(unsigned int index) const -> std::string
{
    return m_presetLoader->getPresetURL(index);
}

auto ProjectM::PresetRating(unsigned int index, const PresetRatingType ratingType) const -> int
{
    return m_presetLoader->getPresetRating(index, ratingType);
}

auto ProjectM::PresetName(unsigned int index) const -> std::string
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

auto ProjectM::SelectedPresetIndex(unsigned int& index) const -> bool
{
    if (*m_presetPos == m_presetChooser->end())
    {
        return false;
    }

    index = **m_presetPos;
    return true;
}


auto ProjectM::PresetPositionValid() const -> bool
{
    return (*m_presetPos != m_presetChooser->end());
}

auto ProjectM::PlaylistSize() const -> unsigned int
{
    return m_presetLoader->size();
}

void ProjectM::SetShuffleEnabled(bool value)
{
    m_settings.shuffleEnabled = value;
}

auto ProjectM::ShuffleEnabled() const -> bool
{
    return m_settings.shuffleEnabled;
}

void ProjectM::PresetSwitchedEvent(bool, size_t) const {}

void ProjectM::ShuffleEnabledValueChanged(bool) const {}

void ProjectM::PresetSwitchFailedEvent(bool, unsigned int, const std::string&) const {}

void ProjectM::PresetRatingChanged(unsigned int, int, PresetRatingType) const {}

void ProjectM::ChangePresetRating(unsigned int index, int rating, const PresetRatingType ratingType)
{
    m_presetLoader->setRating(index, rating, ratingType);
    PresetRatingChanged(index, rating, ratingType);
}

void ProjectM::InsertPresetURL(unsigned int index, const std::string& presetFilename, const std::string& presetName,
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

    else // Case: inserting at selected preset
    {
        newSelectedIndex++;
    }

    m_presetLoader->insertPresetURL(index, presetFilename, presetName, ratingList);

    if (atEndPosition)
    {
        *m_presetPos = m_presetChooser->end();
    }
    else
    {
        *m_presetPos = m_presetChooser->begin(newSelectedIndex);
    }
}

void ProjectM::ChangePresetName(unsigned int index, std::string presetName)
{
    m_presetLoader->setPresetName(index, presetName);
}


void ProjectM::SetTextureSize(size_t size)
{
    m_settings.textureSize = size;

    RecreateRenderer();
}

auto ProjectM::TextureSize() const -> size_t
{
    return m_settings.textureSize;
}

void ProjectM::SetBeatSensitivity(float sensitivity)
{
    m_beatDetect->beatSensitivity = sensitivity;
}

auto ProjectM::SoftCutDuration() const -> double
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

auto ProjectM::HardCutDuration() const -> double
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

auto ProjectM::HardCutEnabled() const -> bool
{
    return m_settings.hardCutEnabled;
}

void ProjectM::SetHardCutEnabled(bool enabled)
{
    m_settings.hardCutEnabled = enabled;
}

auto ProjectM::HardCutSensitivity() const -> float
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

auto ProjectM::AspectCorrection() const -> bool
{
    return m_settings.aspectCorrection;
}

void ProjectM::SetAspectCorrection(bool enabled)
{
    m_settings.aspectCorrection = enabled;
    m_renderer->correction = enabled;
}

auto ProjectM::EasterEgg() const -> float
{
    return m_settings.easterEgg;
}

void ProjectM::SetEasterEgg(float value)
{
    m_settings.easterEgg = value;
    m_timeKeeper->ChangeEasterEgg(value);
}

void ProjectM::MeshSize(size_t& meshResolutionX, size_t& meshResolutionY) const
{
    meshResolutionX = m_settings.meshX;
    meshResolutionY = m_settings.meshY;
}

void ProjectM::SetMeshSize(size_t meshResolutionX, size_t meshResolutionY)
{
    m_settings.meshX = meshResolutionX;
    m_settings.meshY = meshResolutionY;

    RecreateRenderer();
}

auto ProjectM::Pcm() -> class Pcm&
{
    return m_pcm;
}

// get index from search results based on preset name
auto ProjectM::SearchIndex(const std::string& presetName) const -> unsigned int
{
    for (auto& it : m_renderer->m_presetList)
    {
        if (it.name == presetName)
        {
            return it.id;
        }
    }
    return 0;
}

// get preset index based on preset name
auto ProjectM::PresetIndex(const std::string& presetFilename) const -> unsigned int
{
    return m_presetLoader->getPresetIndex(presetFilename);
}

// load preset based on name
void ProjectM::SelectPresetByName(std::string presetName, bool hardCut)
{
    if (presetName == "")
    {
        return;
    }
    unsigned int index = PresetIndex(presetName);
    if (m_presetChooser->empty())
    {
        return;
    }
    SelectPreset(index);
}

auto ProjectM::Settings() const -> const class ProjectM::Settings&
{
    return m_settings;
}

void ProjectM::Touch(float touchX, float touchY, int pressure, int touchType)
{
    if (m_renderer)
    {
        m_renderer->touch(touchX, touchY, pressure, touchType);
    }
}

void ProjectM::TouchDrag(float touchX, float touchY, int pressure)
{
    if (m_renderer)
    {
        m_renderer->touchDrag(touchX, touchY, pressure);
    }
}


void ProjectM::TouchDestroy(float touchX, float touchY)
{
    if (m_renderer)
    {
        m_renderer->touchDestroy(touchX, touchY);
    }
}

void ProjectM::TouchDestroyAll()
{
    if (m_renderer)
    {
        m_renderer->touchDestroyAll();
    }
}

void ProjectM::RecreateRenderer()
{
    m_renderer = std::make_unique<Renderer>(m_settings.windowWidth, m_settings.windowHeight,
                                            m_settings.meshX, m_settings.meshY,
                                            m_beatDetect.get(), m_textureSearchPaths);
}
