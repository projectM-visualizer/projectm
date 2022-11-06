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
#include "PresetFactoryManager.hpp"
#include "Renderer.hpp"
#include "TimeKeeper.hpp"

#include <iostream>

ProjectM::ProjectM(const std::string& configurationFilename)
{
    ReadConfig(configurationFilename);
    Reset();
    ResetOpenGL(m_settings.windowWidth, m_settings.windowHeight);
}

ProjectM::ProjectM(const class Settings& settings)
{
    ReadSettings(settings);
    Reset();
    ResetOpenGL(m_settings.windowWidth, m_settings.windowHeight);
}

ProjectM::~ProjectM()
{
#if USE_THREADS
    m_workerSync.FinishUp();
    m_workerThread.join();
#endif
}

void ProjectM::PresetSwitchRequestedEvent(bool isHardCut) const
{
}

void ProjectM::PresetSwitchFailedEvent(const std::string&, const std::string&) const
{
}

void ProjectM::LoadPresetFile(const std::string& presetFilename, bool smoothTransition)
{
    // If already in a transition, force immediate completion.
    if (m_transitioningPreset != nullptr)
    {
        m_activePreset = std::move(m_transitioningPreset);
    }

    try
    {
        StartPresetTransition(m_presetFactoryManager.CreatePresetFromFile(presetFilename), !smoothTransition);
    }
    catch (const PresetFactoryException& ex)
    {
        PresetSwitchFailedEvent(presetFilename, ex.message());
    }
}

void ProjectM::LoadPresetData(std::istream& presetData, bool smoothTransition)
{
    // If already in a transition, force immediate completion.
    if (m_transitioningPreset != nullptr)
    {
        m_activePreset = std::move(m_transitioningPreset);
    }

    try
    {
        StartPresetTransition(m_presetFactoryManager.CreatePresetFromStream("milk", presetData), !smoothTransition);
    }
    catch (const PresetFactoryException& ex)
    {
        PresetSwitchFailedEvent("", ex.message());
    }
}

auto ProjectM::InitRenderToTexture() -> unsigned
{
    return m_renderer->initRenderToTexture();
}

void ProjectM::ResetTextures()
{
    m_renderer->ResetTextures();
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
    config.add("Hard Cut Sensitivity", settings.beatSensitivity);
    config.add("Aspect Correction", settings.aspectCorrection);
    config.add("Easter Egg Parameter", settings.easterEgg);
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
    m_settings.easterEgg = config.read<float>("Easter Egg Parameter", 0.0);

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

    m_settings.texturePath = settings.texturePath;
    m_settings.dataPath = settings.dataPath;

    m_settings.easterEgg = settings.easterEgg;

    m_settings.hardCutEnabled = settings.hardCutEnabled;
    m_settings.hardCutDuration = settings.hardCutDuration;
    m_settings.hardCutSensitivity = settings.hardCutSensitivity;

    m_settings.beatSensitivity = settings.beatSensitivity;

    m_settings.aspectCorrection = settings.aspectCorrection;

    Initialize();
}


void ProjectM::DumpDebugImageOnNextFrame()
{
    m_renderer->writeNextFrameToFile = true;
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

    m_transitioningPreset->Render(*m_beatDetect, PipelineContext2());
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

    // Check if the preset isn't locked, and we've not already notified the user
    if (!m_presetChangeNotified)
    {
        //if preset is done and we're not already switching
        if (m_timeKeeper->PresetProgressA() >= 1.0 && !m_timeKeeper->IsSmoothing())
        {
            // Call preset change callback
        }
        else if (Settings().hardCutEnabled &&
                 (m_beatDetect->vol - m_beatDetect->volOld > Settings().hardCutSensitivity) &&
                 m_timeKeeper->CanHardCut())
        {
            // Call preset change callback
        }
        m_presetChangeNotified = true;
    }


    if (m_timeKeeper->IsSmoothing() && m_timeKeeper->SmoothRatio() <= 1.0 && m_transitioningPreset != nullptr)
    {
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
            m_transitioningPreset->pipeline(),
            *pipeline,
            m_timeKeeper->SmoothRatio());

        m_renderer->RenderFrameOnlyPass1(*pipeline, PipelineContext());

        return pipeline;
    }

    if (m_timeKeeper->IsSmoothing() && m_timeKeeper->SmoothRatio() > 1.0)
    {
        m_activePreset = std::move(m_transitioningPreset);
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

    m_presetFactoryManager.initialize(m_settings.meshX, m_settings.meshY);

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
    try
    {
        m_renderer->reset(width, height);
    }
    catch (const RenderException& ex)
    {
        // ToDo: Add generic error callback
    }
}

auto ProjectM::InitializePresetTools() -> void
{
    /* Set the seed to the current time in seconds */
    srand(time(nullptr));

    m_renderer->SetPipeline(m_activePreset->pipeline());

    ResetEngine();
}

void ProjectM::StartPresetTransition(std::unique_ptr<Preset>&& preset, bool hardCut)
{
    if (preset == nullptr)
    {
        return;
    }

    try {
        m_renderer->SetPipeline(preset->pipeline());
    }
    catch(const RenderException& ex)
    {
        PresetSwitchFailedEvent(preset->Filename(), ex.message());
        return;
    }

    if (hardCut)
    {
        m_activePreset = std::move(preset);
        m_timeKeeper->StartPreset();
    }
    else
    {
        m_transitioningPreset = std::move(preset);
        m_timeKeeper->StartPreset();
        m_timeKeeper->StartSmoothing();
    }

    m_presetChangeNotified = m_presetLocked;
}

auto ProjectM::WindowWidth() -> int
{
    return m_settings.windowWidth;
}

auto ProjectM::WindowHeight() -> int
{
    return m_settings.windowHeight;
}

void ProjectM::SetPresetLocked(bool locked)
{
    // ToDo: Add a preset switch timer separate from the display timer and reset to 0 when
    //       disabling the preset switch lock.
    m_presetLocked = locked;
    m_presetChangeNotified = locked;
}

auto ProjectM::PresetLocked() const -> bool
{
    return m_presetLocked;
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

auto ProjectM::GetBeatSensitivity() const -> float
{
    return m_beatDetect->beatSensitivity;
}

auto ProjectM::SoftCutDuration() const -> double
{
    return m_settings.softCutDuration;
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

void ProjectM::SetPresetDuration(double seconds)
{
    m_timeKeeper->ChangePresetDuration(seconds);
}

auto ProjectM::PresetDuration() const -> double
{
    return m_timeKeeper->PresetDuration();
}

auto ProjectM::FramesPerSecond() const -> int32_t
{
    return m_settings.fps;
}

void ProjectM::SetFramesPerSecond(int32_t fps)
{
    m_settings.fps = fps;
    m_renderer->setFPS(fps);
    m_pipelineContext.fps = fps;
    m_pipelineContext2.fps = fps;
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
