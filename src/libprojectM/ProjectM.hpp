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

#include <Renderer/RenderContext.hpp>

#include "projectM-4/projectM_export.h"

#include "libprojectM/Audio/PCM.hpp"

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

#if PROJECTM_USE_THREADS

#include <mutex>
#include <thread>

#endif

class BackgroundWorkerSync;

namespace libprojectM {
namespace Audio {
class BeatDetect;
}
} // namespace libprojectM

class Renderer;

class TextureManager;

class Preset;

class TimeKeeper;

class PresetFactoryManager;

class PROJECTM_EXPORT ProjectM
{
public:
    ProjectM();

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

    /**
     * @brief Sets the texture paths used to find images for presets.
     *
     * Setting new texture paths will clear the texture manager cache and reload textures.
     * This can cause lags in rendering.
     *
     * @param texturePaths A list of paths projectM will look for texture images, in order.
     */
    void SetTexturePaths(std::vector<std::string> texturePaths);

    void ResetTextures();

    void RenderFrame();

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
    auto TargetFramesPerSecond() const -> int32_t;

    /**
     * @brief Sets a new current frames per second value.
     * @param fps The new frames per second value.
     */
    void SetTargetFramesPerSecond(int32_t fps);

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

    /// Turn on or off a lock that prevents projectM from switching to another preset
    void SetPresetLocked(bool locked);

    /// Returns true if the active preset is locked
    auto PresetLocked() const -> bool;

    auto PCM() -> libprojectM::Audio::PCM&;

    auto WindowWidth() -> int;

    auto WindowHeight() -> int;

    /**
     * @brief Dumps a debug image to the working dir when the next frame is rendered.
     *
     * The main texture is dumped after render pass 1, e.g. before shaders are applied.
     */
    void DumpDebugImageOnNextFrame(const std::string& outputFile);

private:
    void Initialize();

    void ResetEngine();

    void StartPresetTransition(std::unique_ptr<Preset>&& preset, bool hardCut);

    void LoadIdlePreset();

    auto GetRenderContext() -> RenderContext;

#if PROJECTM_USE_THREADS

    void ThreadWorker();

#endif

    class libprojectM::Audio::PCM m_pcm; //!< Audio data buffer and analyzer instance.

    size_t m_meshX{32};              //!< Per-point mesh horizontal resolution.
    size_t m_meshY{24};              //!< Per-point mesh vertical resolution.
    size_t m_targetFps{35};          //!< Target frames per second.
    int m_windowWidth{0};         //!< EvaluateFrameData window width. If 0, nothing is rendered.
    int m_windowHeight{0};        //!< EvaluateFrameData window height. If 0, nothing is rendered.
    double m_presetDuration{30.0};   //!< Preset duration in seconds.
    double m_softCutDuration{3.0};   //!< Soft cut transition time.
    double m_hardCutDuration{20.0};  //!< Time after which a hard cut can happen at the earliest.
    bool m_hardCutEnabled{false};    //!< If true, hard cuts based on beat detection are enabled.
    float m_hardCutSensitivity{2.0}; //!< Loudness sensitivity value for hard cuts.
    float m_beatSensitivity{1.0};    //!< General beat sensitivity modifier for presets.
    bool m_aspectCorrection{true};   //!< If true, corrects aspect ratio for non-rectangular windows.
    float m_easterEgg{0.0};          //!< Random preset duration modifier. See TimeKeeper class.

    std::vector<std::string> m_textureSearchPaths; ///!< List of paths to search for texture files

    /** Timing information */
    int m_frameCount{0}; //!< Rendered frame count since start

    bool m_presetLocked{false};         //!< If true, the preset change event will not be sent.
    bool m_presetChangeNotified{false}; //!< Stores whether the user has been notified that projectM wants to switch the preset.

    std::unique_ptr<PresetFactoryManager> m_presetFactoryManager; //!< Provides access to all available preset factories.

    std::unique_ptr<Renderer> m_renderer;                         //!< The Preset renderer.
    std::unique_ptr<TextureManager> m_textureManager;             //!< The texture manager.
    std::unique_ptr<libprojectM::Audio::BeatDetect> m_beatDetect; //!< The beat detection class.
    std::unique_ptr<Preset> m_activePreset;                       //!< Currently loaded preset.
    std::unique_ptr<Preset> m_transitioningPreset;                //!< Destination preset when smooth preset switching.
    std::unique_ptr<TimeKeeper> m_timeKeeper;                     //!< Keeps the different timers used to render and switch presets.

#if PROJECTM_USE_THREADS
    mutable std::recursive_mutex m_presetSwitchMutex;   //!< Mutex for locking preset switching while rendering and vice versa.
    std::thread m_workerThread;                         //!< Background worker for preloading presets.
    std::unique_ptr<BackgroundWorkerSync> m_workerSync; //!< Background work synchronizer.
#endif
};
