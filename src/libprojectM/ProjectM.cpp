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

#include "Preset.hpp"
#include "PresetFactoryManager.hpp"
#include "TimeKeeper.hpp"

#include "Audio/PCM.hpp" //Sound data handler (buffering, FFT, etc.)

#include "Renderer/CopyTexture.hpp"
#include "Renderer/PresetTransition.hpp"
#include "Renderer/TextureManager.hpp"
#include "Renderer/TransitionShaderManager.hpp"


#if PROJECTM_USE_THREADS

#include "libprojectM/BackgroundWorker.hpp"

#endif

ProjectM::ProjectM()
    : m_presetFactoryManager(std::make_unique<PresetFactoryManager>())
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
        m_textureManager->PurgeTextures();
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
        m_textureManager->PurgeTextures();
        StartPresetTransition(m_presetFactoryManager->CreatePresetFromStream(".milk", presetData), !smoothTransition);
    }
    catch (const PresetFactoryException& ex)
    {
        m_activePreset.reset();
        PresetSwitchFailedEvent("", ex.message());
    }
}

void ProjectM::SetTexturePaths(std::vector<std::string> texturePaths)
{
    m_textureSearchPaths = std::move(texturePaths);
    m_textureManager = std::make_unique<TextureManager>(m_textureSearchPaths);
}

void ProjectM::ResetTextures()
{
    m_textureManager = std::make_unique<TextureManager>(m_textureSearchPaths);
}

void ProjectM::DumpDebugImageOnNextFrame(const std::string& outputFile)
{
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
        m_workerSync->FinishedWork();
    }
}

#endif

void ProjectM::RenderFrame()
{
    // Don't render if window area is zero.
    if (m_windowWidth == 0 || m_windowHeight == 0)
    {
        return;
    }

#if PROJECTM_USE_THREADS
    std::lock_guard<std::recursive_mutex> guard(m_presetSwitchMutex);
#endif

    // Update FPS and other timer values.
    m_timeKeeper->UpdateTimers();

    // Update and retrieve audio data
    m_audioStorage.UpdateFrameAudioData(m_timeKeeper->SecondsSinceLastFrame(), m_frameCount);
    auto audioData = m_audioStorage.GetFrameAudioData();

    // Check if the preset isn't locked, and we've not already notified the user
    if (!m_presetChangeNotified)
    {
        // If preset is done and we're not already switching
        if (m_timeKeeper->PresetProgressA() >= 1.0 && !m_timeKeeper->IsSmoothing())
        {
            m_presetChangeNotified = true;
            PresetSwitchRequestedEvent(false);
        }
        else if (m_hardCutEnabled &&
                 m_frameCount > 50 &&
                 (audioData.vol - m_previousFrameVolume > m_hardCutSensitivity) &&
                 m_timeKeeper->CanHardCut())
        {
            m_presetChangeNotified = true;
            PresetSwitchRequestedEvent(true);
        }
    }

    // If no preset is active, load the idle preset.
    if (!m_activePreset)
    {
        LoadIdlePreset();
        if (!m_activePreset)
        {
            return;
        }

        m_activePreset->Initialize(GetRenderContext());
    }

    if (m_timeKeeper->IsSmoothing() && m_transitioningPreset != nullptr)
    {
#if PROJECTM_USE_THREADS
        m_workerSync->WakeUpBackgroundTask();
        // FIXME: Instead of waiting after a single render pass, check every frame if it's done.
        m_workerSync->WaitForBackgroundTaskToFinish();
#endif

        // ToDo: check if new preset is loaded.

        if (m_timeKeeper->SmoothRatio() >= 1.0)
        {
            m_timeKeeper->EndSmoothing();
        }
    }

    auto renderContext = GetRenderContext();

    if (m_transition != nullptr && m_transitioningPreset != nullptr)
    {
        if (m_transition->IsDone())
        {
            m_activePreset = std::move(m_transitioningPreset);
            m_transitioningPreset.reset();
            m_transition.reset();
        }
        else
        {
            m_transitioningPreset->RenderFrame(audioData, renderContext);
        }
    }


    // ToDo: Call the to-be-implemented render method in Renderer
    m_activePreset->RenderFrame(audioData, renderContext);

    // ToDo: Allow external apps to provide a custom target framebuffer.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    if (m_transition != nullptr && m_transitioningPreset != nullptr)
    {
        m_transition->Draw(*m_activePreset, *m_transitioningPreset, renderContext, audioData);
    }
    else
    {
        m_textureCopier->Draw(m_activePreset->OutputTexture(), false, false);
    }

    m_frameCount++;
    m_previousFrameVolume = audioData.vol;
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
    m_textureManager = std::make_unique<TextureManager>(m_textureSearchPaths);

    m_transitionShaderManager = std::make_unique<TransitionShaderManager>();

    m_textureCopier = std::make_unique<CopyTexture>();

    m_presetFactoryManager->initialize();

    /* Set the seed to the current time in seconds */
    srand(time(nullptr));

    LoadIdlePreset();

#if PROJECTM_USE_THREADS
    m_workerSync->Reset();
    m_workerThread = std::thread(&ProjectM::ThreadWorker, this);
#endif

    m_timeKeeper->StartPreset();
}

void ProjectM::LoadIdlePreset()
{
    LoadPresetFile("idle://Geiss & Sperl - Feedback (projectM idle HDR mix).milk", false);
    assert(m_activePreset);
}

/** Resets OpenGL state */
void ProjectM::ResetOpenGL(size_t width, size_t height)
{
    /** Stash the new dimensions */
    m_windowWidth = width;
    m_windowHeight = height;
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
        preset->Initialize(GetRenderContext());
    }
    catch (std::exception& ex)
    {
        PresetSwitchFailedEvent(preset->Filename(), ex.what());
    }

    // ToDo: Continue only if preset is fully loaded.

    m_transition.reset();

    if (m_activePreset)
    {
        preset->DrawInitialImage(m_activePreset->OutputTexture(), GetRenderContext());
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
        m_transition = std::make_unique<PresetTransition>(m_transitionShaderManager->RandomTransition(), m_softCutDuration);
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
    m_beatSensitivity = std::min(std::max(0.0f, sensitivity), 2.0f);
}

auto ProjectM::GetBeatSensitivity() const -> float
{
    return m_beatSensitivity;
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
}

auto ProjectM::AspectCorrection() const -> bool
{
    return m_aspectCorrection;
}

void ProjectM::SetAspectCorrection(bool enabled)
{
    m_aspectCorrection = enabled;
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

    // Need multiples of two, otherwise will not render a horizontal and/or vertical bar in the center of the warp mesh.
    if (m_meshX % 2 == 1)
    {
        m_meshX++;
    }

    if (m_meshY % 2 == 1)
    {
        m_meshY++;
    }

    // Constrain per-pixel mesh size to sensible limits
    m_meshX = std::max(static_cast<size_t>(8), std::min(static_cast<size_t>(400), m_meshX));
    m_meshY = std::max(static_cast<size_t>(8), std::min(static_cast<size_t>(400), m_meshY));
}

auto ProjectM::PCM() -> libprojectM::Audio::PCM&
{
    return m_audioStorage;
}

void ProjectM::Touch(float touchX, float touchY, int pressure, int touchType)
{
}

void ProjectM::TouchDrag(float touchX, float touchY, int pressure)
{
}

void ProjectM::TouchDestroy(float touchX, float touchY)
{
}

void ProjectM::TouchDestroyAll()
{
}

auto ProjectM::GetRenderContext() -> RenderContext
{
    RenderContext ctx{};
    ctx.viewportSizeX = m_windowWidth;
    ctx.viewportSizeY = m_windowHeight;
    ctx.time = static_cast<float>(m_timeKeeper->GetRunningTime());
    ctx.progress = static_cast<float>(m_timeKeeper->PresetProgressA());
    ctx.fps = static_cast<float>(m_targetFps);
    ctx.frame = m_frameCount;
    ctx.aspectX = (m_windowHeight > m_windowWidth) ? static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight) : 1.0f;
    ctx.aspectY = (m_windowWidth > m_windowHeight) ? static_cast<float>(m_windowHeight) / static_cast<float>(m_windowWidth) : 1.0f;
    ctx.invAspectX = 1.0f / ctx.aspectX;
    ctx.invAspectY = 1.0f / ctx.aspectY;
    ctx.perPixelMeshX = static_cast<int>(m_meshX);
    ctx.perPixelMeshY = static_cast<int>(m_meshY);
    ctx.textureManager = m_textureManager.get();

    return ctx;
}
