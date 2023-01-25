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
#include "PCM.hpp" //Sound data handler (buffering, FFT, etc.)
#include "PipelineMerger.hpp"
#include "Preset.hpp"
#include "PresetFactoryManager.hpp"
#include "Renderer.hpp"
#include "Renderer/PipelineContext.hpp"
#include "TimeKeeper.hpp"

#if PROJECTM_USE_THREADS

#include "libprojectM/BackgroundWorker.hpp"

#endif

ProjectM::ProjectM()
    : m_presetFactoryManager(std::make_unique<PresetFactoryManager>())
    , m_pipelineContext(std::make_unique<class PipelineContext>())
    , m_pipelineContext2(std::make_unique<class PipelineContext>())
#if PROJECTM_USE_THREADS
    , m_workerSync(std::make_unique<BackgroundWorkerSync>())
#endif
{
    Initialize();
}

ProjectM::~ProjectM()
{
#if PROJECTM_USE_THREADS
    m_workerSync->FinishUp();
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
        StartPresetTransition(m_presetFactoryManager->CreatePresetFromFile(presetFilename), !smoothTransition);
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
        StartPresetTransition(m_presetFactoryManager->CreatePresetFromStream(".milk", presetData), !smoothTransition);
    }
    catch (const PresetFactoryException& ex)
    {
        PresetSwitchFailedEvent("", ex.message());
    }
}

void ProjectM::SetTexturePaths(std::vector<std::string> texturePaths)
{
    m_textureSearchPaths = std::move(texturePaths);
    m_renderer->SetTextureSearchPaths(m_textureSearchPaths);
}

void ProjectM::ResetTextures()
{
    m_renderer->ResetTextures();
}

void ProjectM::DumpDebugImageOnNextFrame(const std::string& outputFile)
{
    m_renderer->frameDumpOutputFile = outputFile;
    m_renderer->writeNextFrameToFile = true;
}

#if PROJECTM_USE_THREADS

void ProjectM::ThreadWorker()
{
    while (true)
    {
        if (!m_workerSync->WaitForWork())
        {
            return;
        }
        EvaluateSecondPreset();
        m_workerSync->FinishedWork();
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
    // Don't render if window area is zero.
    if (m_windowWidth == 0 || m_windowHeight == 0)
    {
        return;
    }

    Pipeline pipeline;
    Pipeline* comboPipeline = RenderFrameOnlyPass1(&pipeline);

    RenderFrameOnlyPass2(comboPipeline);

    ProjectM::RenderFrameEndOnSeparatePasses(comboPipeline);
}


auto ProjectM::RenderFrameOnlyPass1(Pipeline* pipeline) -> Pipeline*
{
#if PROJECTM_USE_THREADS
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
            m_presetChangeNotified = true;
            PresetSwitchRequestedEvent(false);
        }
        else if (m_hardCutEnabled &&
                 (m_beatDetect->vol - m_beatDetect->volOld > m_hardCutSensitivity) &&
                 m_timeKeeper->CanHardCut())
        {
            m_presetChangeNotified = true;
            PresetSwitchRequestedEvent(true);
        }
    }

    if (m_timeKeeper->IsSmoothing() && m_transitioningPreset != nullptr)
    {
#if PROJECTM_USE_THREADS
        m_workerSync->WakeUpBackgroundTask();
        // FIXME: Instead of waiting after a single render pass, check every frame if it's done.
        m_workerSync->WaitForBackgroundTaskToFinish();
#endif
        EvaluateSecondPreset();

        if (m_timeKeeper->SmoothRatio() <= 1.0)
        {
            pipeline->setStaticPerPixel(m_meshX, m_meshY);

            PipelineMerger::mergePipelines(
                m_activePreset->pipeline(),
                m_transitioningPreset->pipeline(),
                *pipeline,
                m_timeKeeper->SmoothRatio());

            m_renderer->RenderFrameOnlyPass1(*pipeline, PipelineContext());

            return pipeline;
        }

        m_activePreset = std::move(m_transitioningPreset);
        m_timeKeeper->EndSmoothing();
    }

    m_activePreset->Render(*m_beatDetect, PipelineContext());
    m_renderer->RenderFrameOnlyPass1(m_activePreset->pipeline(), PipelineContext());

    return nullptr; // indicating no transition
}

void ProjectM::RenderFrameOnlyPass2(Pipeline* pipeline)
{
    if (pipeline == nullptr)
    {
        assert(m_activePreset.get());
        pipeline = &m_activePreset->pipeline();
    }

    m_renderer->RenderFrameOnlyPass2(*pipeline, PipelineContext());
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
    return *m_pipelineContext;
}

auto ProjectM::PipelineContext2() -> class PipelineContext&
{
    return *m_pipelineContext2;
}

void ProjectM::Reset()
{
    this->m_count = 0;

    m_presetFactoryManager->initialize(m_meshX, m_meshY);

    ResetEngine();
}

void ProjectM::Initialize()
{
    /** Initialise start time */
    m_timeKeeper = std::make_unique<TimeKeeper>(m_presetDuration,
                                                m_softCutDuration,
                                                m_hardCutDuration,
                                                m_easterEgg);

    /** Nullify frame stash */

    /** Initialise per-pixel matrix calculations */
    /** We need to initialise this before the builtin param db otherwise bass/mid etc won't bind correctly */
    assert(!m_beatDetect);

    m_beatDetect = std::make_unique<BeatDetect>(m_pcm);

    this->m_renderer = std::make_unique<Renderer>(m_windowWidth,
                                                  m_windowHeight,
                                                  m_meshX,
                                                  m_meshY,
                                                  m_beatDetect.get(),
                                                  m_textureSearchPaths);

    m_presetFactoryManager->initialize(m_meshX, m_meshY);

    /* Set the seed to the current time in seconds */
    srand(time(nullptr));

    ResetEngine();
    LoadIdlePreset();

#if PROJECTM_USE_THREADS
    m_workerSync->Reset();
    m_workerThread = std::thread(&ProjectM::ThreadWorker, this);
#endif

    m_timeKeeper->StartPreset();

    // ToDo: Calculate the real FPS instead
    PipelineContext().fps = static_cast<int>(m_targetFps);
    PipelineContext2().fps = static_cast<int>(m_targetFps);
}

void ProjectM::LoadIdlePreset()
{
    LoadPresetFile("idle://Geiss & Sperl - Feedback (projectM idle HDR mix).milk", false);
    assert(m_activePreset);
    m_renderer->SetPipeline(m_activePreset->pipeline());
}

/* Reinitializes the engine variables to a default (conservative and sane) value */
void ProjectM::ResetEngine()
{

    if (m_beatDetect != NULL)
    {
        m_beatDetect->Reset();
        m_beatDetect->beatSensitivity = m_beatSensitivity;
    }
}

/** Resets OpenGL state */
void ProjectM::ResetOpenGL(size_t width, size_t height)
{
    /** Stash the new dimensions */
    m_windowWidth = width;
    m_windowHeight = height;
    try
    {
        m_renderer->reset(width, height);
    }
    catch (const RenderException& ex)
    {
        // ToDo: Add generic error callback
    }
}

void ProjectM::StartPresetTransition(std::unique_ptr<Preset>&& preset, bool hardCut)
{
    m_presetChangeNotified = m_presetLocked;

    if (preset == nullptr)
    {
        return;
    }

    try
    {
        m_renderer->SetPipeline(preset->pipeline());
    }
    catch (const RenderException& ex)
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
        m_timeKeeper->StartSmoothing();
    }
}

auto ProjectM::WindowWidth() -> int
{
    return m_windowWidth;
}

auto ProjectM::WindowHeight() -> int
{
    return m_windowHeight;
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

void ProjectM::SetBeatSensitivity(float sensitivity)
{
    m_beatDetect->beatSensitivity = std::min(std::max(0.0f, sensitivity), 2.0f);
}

auto ProjectM::GetBeatSensitivity() const -> float
{
    return m_beatDetect->beatSensitivity;
}

auto ProjectM::SoftCutDuration() const -> double
{
    return m_softCutDuration;
}

void ProjectM::SetSoftCutDuration(double seconds)
{
    m_softCutDuration = seconds;
    m_timeKeeper->ChangeSoftCutDuration(seconds);
}

auto ProjectM::HardCutDuration() const -> double
{
    return m_hardCutDuration;
}

void ProjectM::SetHardCutDuration(double seconds)
{
    m_hardCutDuration = static_cast<int>(seconds);
    m_timeKeeper->ChangeHardCutDuration(seconds);
}

auto ProjectM::HardCutEnabled() const -> bool
{
    return m_hardCutEnabled;
}

void ProjectM::SetHardCutEnabled(bool enabled)
{
    m_hardCutEnabled = enabled;
}

auto ProjectM::HardCutSensitivity() const -> float
{
    return m_hardCutSensitivity;
}

void ProjectM::SetHardCutSensitivity(float sensitivity)
{
    m_hardCutSensitivity = sensitivity;
}

void ProjectM::SetPresetDuration(double seconds)
{
    m_timeKeeper->ChangePresetDuration(seconds);
}

auto ProjectM::PresetDuration() const -> double
{
    return m_timeKeeper->PresetDuration();
}

auto ProjectM::TargetFramesPerSecond() const -> int32_t
{
    return m_targetFps;
}

void ProjectM::SetTargetFramesPerSecond(int32_t fps)
{
    m_targetFps = fps;
    m_renderer->setFPS(fps);
    m_pipelineContext->fps = fps;
    m_pipelineContext2->fps = fps;
}

auto ProjectM::AspectCorrection() const -> bool
{
    return m_aspectCorrection;
}

void ProjectM::SetAspectCorrection(bool enabled)
{
    m_aspectCorrection = enabled;
    m_renderer->correction = enabled;
}

auto ProjectM::EasterEgg() const -> float
{
    return m_easterEgg;
}

void ProjectM::SetEasterEgg(float value)
{
    m_easterEgg = value;
    m_timeKeeper->ChangeEasterEgg(value);
}

void ProjectM::MeshSize(size_t& meshResolutionX, size_t& meshResolutionY) const
{
    meshResolutionX = m_meshX;
    meshResolutionY = m_meshY;
}

void ProjectM::SetMeshSize(size_t meshResolutionX, size_t meshResolutionY)
{
    m_meshX = meshResolutionX;
    m_meshY = meshResolutionY;

    // Update mesh size in all sorts of classes.
    m_renderer->SetPerPixelMeshSize(m_meshX, m_meshY);
    m_presetFactoryManager->initialize(m_meshX, m_meshY);

    // Unload all presets and reload idle preset
    m_activePreset.reset();
    m_transitioningPreset.reset();
    LoadIdlePreset();
}

auto ProjectM::Pcm() -> class Pcm&
{
    return m_pcm;
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
    m_renderer = std::make_unique<Renderer>(m_windowWidth, m_windowHeight,
                                            m_meshX, m_meshY,
                                            m_beatDetect.get(), m_textureSearchPaths);
}
