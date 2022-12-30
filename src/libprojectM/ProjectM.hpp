/*
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
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
#pragma once

#include "Common.hpp"
#include "PCM.hpp"
#include "PresetFactoryManager.hpp"
#include "fatal.h"

#ifdef _WIN32

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)
#endif /* _MSC_VER */
#endif /** _WIN32 */

#include <sys/types.h>

#include <memory>
#include <string>
#include <vector>

#if USE_THREADS

#include "BackgroundWorker.h"

#include <mutex>
#include <thread>

#endif

class BeatDetect;

class Pcm;

class Func;

class Renderer;

class Preset;

class TimeKeeper;

class Pipeline;

class PipelineContext;

class ProjectM
{
public:
    class Settings
    {
    public:
        size_t meshX{32};
        size_t meshY{24};
        size_t fps{35};
        size_t textureSize{512};
        size_t windowWidth{512};
        size_t windowHeight{512};
        std::string texturePath;
        std::string dataPath;
        double presetDuration{15.0};
        double softCutDuration{10.0};
        double hardCutDuration{60.0};
        bool hardCutEnabled{false};
        float hardCutSensitivity{2.0};
        float beatSensitivity{1.0};
        bool aspectCorrection{true};
        float easterEgg{0.0};
    };

    explicit ProjectM(const std::string& configurationFilename);

    explicit ProjectM(const class Settings& configurationFilename);

    virtual ~ProjectM();

    /**
     * @brief Callback for notifying the integrating app that projectM wants to switch to a new preset.
     *
     * It is safe to call LoadPreset() from inside the callback. The app can decide when to actually
     * call the function or even ignore the request completely.
     *
     * @param isHardCut True if the switch event was caused by a hard cut, false if it is a soft cut.
     */
    virtual void PresetSwitchRequestedEvent(bool isHardCut) const;

    /**
     * @brief Callback for notifying the integrating app that the requested preset file couldn't be loaded.
     * @param presetFilename The filename of the preset that failed to load. Empty if loaded from a stream.
     * @param message The error message with the failure reason.
     */
    virtual void PresetSwitchFailedEvent(const std::string& presetFilename, const std::string& message) const;

    /**
     * @brief Loads the given preset file and performs a smooth or immediate transition.
     * @param presetFilename The preset filename to load.
     * @param smoothTransition If set to true, old and new presets will be blended over smoothly.
     *                         If set to false, the new preset will be rendered immediately.
     */
    void LoadPresetFile(const std::string& presetFilename, bool smoothTransition);

    /**
     * @brief Loads the given preset data and performs a smooth or immediate transition.
     *
     * This function assumes the data to be in Milkdrop format.
     *
     * @param presetData The preset data stream to load from.
     * @param smoothTransition If set to true, old and new presets will be blended over smoothly.
     *                         If set to false, the new preset will be rendered immediately.
     */
    void LoadPresetData(std::istream& presetData, bool smoothTransition);

    void ResetOpenGL(size_t width, size_t height);

    void ResetTextures();

    void RenderFrame();

    auto InitRenderToTexture() -> unsigned;

    void SetTextureSize(size_t size);

    auto TextureSize() const -> size_t;

    void SetBeatSensitivity(float sensitivity);

    auto GetBeatSensitivity() const -> float;

    auto SoftCutDuration() const -> double;

    void SetSoftCutDuration(double seconds);

    auto HardCutDuration() const -> double;

    void SetHardCutDuration(double seconds);

    auto HardCutEnabled() const -> bool;

    void SetHardCutEnabled(bool enabled);

    auto HardCutSensitivity() const -> float;

    void SetHardCutSensitivity(float sensitivity);

    /**
     * @brief Returns the currently set preset duration in seconds.
     * @return The currently set preset duration in seconds.
     */
    auto PresetDuration() const -> double;

    void SetPresetDuration(double seconds);

    /**
     * @brief Returns the current frames per second value.
     * @return The current frames per second value.
     */
    auto FramesPerSecond() const -> int32_t;

    /**
     * @brief Sets a new current frames per second value.
     * @param fps The new frames per second value.
     */
    void SetFramesPerSecond(int32_t fps);

    auto AspectCorrection() const -> bool;

    void SetAspectCorrection(bool enabled);

    auto EasterEgg() const -> float;

    void SetEasterEgg(float value);

    void MeshSize(size_t& meshResolutionX, size_t& meshResolutionY) const;

    void SetMeshSize(size_t meshResolutionX, size_t meshResolutionY);

    void Touch(float touchX, float touchY, int pressure, int touchType);

    void TouchDrag(float touchX, float touchY, int pressure);

    void TouchDestroy(float touchX, float touchY);

    void TouchDestroyAll();

    auto Settings() const -> const class Settings&;

    /// Writes a Settings configuration to the specified file
    static auto WriteConfig(const std::string& configurationFilename,
                            const class Settings& settings) -> bool;

    /// Turn on or off a lock that prevents projectM from switching to another preset
    void SetPresetLocked(bool locked);

    /// Returns true if the active preset is locked
    auto PresetLocked() const -> bool;

    auto Pcm() -> class Pcm&;

    auto WindowWidth() -> int;

    auto WindowHeight() -> int;

    /**
     * @brief Dumps a debug image to the working dir when the next frame is rendered.
     *
     * The main texture is dumped after render pass 1, e.g. before shaders are applied.
     */
    void DumpDebugImageOnNextFrame();

private:
    void EvaluateSecondPreset();

    /**
     * @brief Renders the first pass of a frame.
     * @param pipeline A pointer to a Pipeline for use in pass 2.
     * @return Returns the pointer passed in pipeline if in a transition, else returns nullptr.
     */
    auto RenderFrameOnlyPass1(Pipeline* pipeline) -> Pipeline*;

    void RenderFrameOnlyPass2(Pipeline* pipeline);

    void RenderFrameEndOnSeparatePasses(Pipeline* pipeline);

    auto PipelineContext() -> class PipelineContext&;

    auto PipelineContext2() -> class PipelineContext&;

    void ReadConfig(const std::string& configurationFilename);

    void ReadSettings(const class Settings& settings);

    void Initialize();

    void Reset();

    void ResetEngine();

    void StartPresetTransition(std::unique_ptr<Preset>&& preset, bool hardCut);

    void RecreateRenderer();

#if USE_THREADS

    void ThreadWorker();

#endif

    class Pcm m_pcm; //!< Audio data buffer and analyzer instance.

    class Settings m_settings; //!< The projectM Settings.

    std::vector<std::string> m_textureSearchPaths; ///!< List of paths to search for texture files

    /** Timing information */
    int m_count{0}; //!< Rendered frame count since start

    bool m_presetLocked{false}; //!< If true, the preset change event will not be sent.
    bool m_presetChangeNotified{false}; //!< Stores whether the user has been notified that projectM wants to switch the preset.

    PresetFactoryManager m_presetFactoryManager; //!< Provides access to all available preset factories.

    std::unique_ptr<class PipelineContext> m_pipelineContext;  //!< Pipeline context for the first/current preset.
    std::unique_ptr<class PipelineContext> m_pipelineContext2; //!< Pipeline context for the next/transitioning preset.

    std::unique_ptr<Renderer> m_renderer;     //!< The Preset renderer.
    std::unique_ptr<BeatDetect> m_beatDetect; //!< The beat detection class.
    std::unique_ptr<Preset> m_activePreset;   //!< Currently loaded preset.
    std::unique_ptr<Preset> m_transitioningPreset;  //!< Destination preset when smooth preset switching.
    std::unique_ptr<TimeKeeper> m_timeKeeper; //!< Keeps the different timers used to render and switch presets.

#if USE_THREADS
    mutable std::recursive_mutex m_presetSwitchMutex; //!< Mutex for locking preset switching while rendering and vice versa.
    std::thread m_workerThread;                       //!< Background worker for preloading presets.
    BackgroundWorkerSync m_workerSync;                //!< Background work synchronizer.
#endif
};
