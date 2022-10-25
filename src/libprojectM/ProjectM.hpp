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
#include "PipelineContext.hpp"
#include "event.h"
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

class PresetIterator;

class PresetChooser;

class PresetLoader;

class TimeKeeper;

class Pipeline;

class ProjectM
{
public:
    /*
     * Behaviour flags for the projectM instance. Currently, it's only used to prevent automatically filling
     * the preset playlist by traversing the preset path for files.
     */
    enum Flags
    {
        None = 0,                    //!< No special flags.
        DisablePlaylistLoad = 1 << 0 //!< Prevent automatic playlist loading on startup.
    };

    class Settings
    {
    public:
        size_t meshX{32};
        size_t meshY{24};
        size_t fps{35};
        size_t textureSize{512};
        size_t windowWidth{512};
        size_t windowHeight{512};
        std::string presetPath;
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
        bool shuffleEnabled{true};
        bool softCutRatingsEnabled{false};
    };

    explicit ProjectM(const std::string& configurationFilename, Flags flags = Flags::None);

    explicit ProjectM(const class Settings& configurationFilename, Flags flags = Flags::None);

    virtual ~ProjectM();

    void ResetOpenGL(size_t width, size_t height);

    void ResetTextures();

    void RenderFrame();

    auto InitRenderToTexture() -> unsigned;

    void KeyHandler(projectMEvent event,
                    projectMKeycode keyCode,
                    projectMModifier modifier);

    void SetTextureSize(size_t size);

    auto TextureSize() const -> size_t;

    void SetBeatSensitivity(float sensitivity);

    auto SoftCutDuration() const -> double;

    void SetSoftCutDuration(int seconds);

    void SetSoftCutDuration(double seconds);

    auto HardCutDuration() const -> double;

    void SetHardCutDuration(int seconds);

    void SetHardCutDuration(double seconds);

    auto HardCutEnabled() const -> bool;

    void SetHardCutEnabled(bool enabled);

    auto HardCutSensitivity() const -> float;

    void SetHardCutSensitivity(float sensitivity);

    void SetPresetDuration(int seconds);

    void SetPresetDuration(double seconds);

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

    /// Sets preset iterator position to the passed in index
    void SelectPresetPosition(unsigned int index);

    /// Plays a preset immediately
    void SelectPreset(unsigned int index, bool hardCut = true);

    /// Removes a preset from the play list. If it is playing then it will continue as normal until next switch
    void RemovePreset(unsigned int index);

    /// Removes entire playlist, The currently loaded preset will end up sticking until new presets are added
    void ClearPlaylist();

    /// Turn on or off a lock that prevents projectM from switching to another preset
    void SetPresetLocked(bool locked);

    /// Returns true if the active preset is locked
    auto PresetLocked() const -> bool;

    auto PresetIndex(const std::string& presetFilename) const -> unsigned int;

    /// Plays a preset immediately when given preset name
    void SelectPresetByName(std::string presetName, bool hardCut = true);

    /// Returns index of currently active preset. In the case where the active
    /// preset was removed from the playlist, this function will return the element
    /// before active preset (thus the next in order preset is invariant with respect
    /// to the removal)
    auto SelectedPresetIndex(unsigned int& index) const -> bool;

    /// Add a preset url to the play list. Appended to bottom. Returns index of preset
    auto AddPresetURL(const std::string& presetFilename,
                      const std::string& presetName,
                      const RatingList& ratingList) -> unsigned int;

    /// Insert a preset url to the play list at the suggested index.
    void InsertPresetURL(unsigned int index,
                         const std::string& presetFilename,
                         const std::string& presetName,
                         const RatingList& ratingList);

    /// Returns true if the selected preset position points to an actual preset in the
    /// currently loaded playlist
    auto PresetPositionValid() const -> bool;

    /// Returns the url associated with a preset index
    auto PresetURL(unsigned int index) const -> std::string;

    /// Returns the preset name associated with a preset index
    auto PresetName(unsigned int index) const -> std::string;

    void ChangePresetName(unsigned int index, std::string presetName);

    /// Returns the rating associated with a preset index
    auto PresetRating(unsigned int index, PresetRatingType ratingType) const -> int;

    void ChangePresetRating(unsigned int index, int rating, PresetRatingType ratingType);

    /// Returns the size of the play list
    auto PlaylistSize() const -> unsigned int;

    void SetShuffleEnabled(bool value);

    auto ShuffleEnabled() const -> bool;

    /// Occurs when active preset has switched. Switched to index is returned
    virtual void PresetSwitchedEvent(bool hardCut, size_t index) const;

    virtual void ShuffleEnabledValueChanged(bool enabled) const;

    virtual void PresetSwitchFailedEvent(bool hardCut, unsigned int index, const std::string& message) const;

    /// Occurs whenever preset rating has changed via ChangePresetRating() method
    virtual void PresetRatingChanged(unsigned int index, int rating, PresetRatingType ratingType) const;

    auto Pcm() -> class Pcm&;

    /// Get the preset index given a name
    auto SearchIndex(const std::string& presetName) const -> unsigned int;

    void SelectPrevious(bool hardCut);

    void SelectNext(bool hardCut);

    void SelectRandom(bool hardCut);

    auto WindowWidth() -> int;

    auto WindowHeight() -> int;

    auto ErrorLoadingCurrentPreset() const -> bool;

    void DefaultKeyHandler(projectMEvent event, projectMKeycode keycode);

private:
    void EvaluateSecondPreset();

    /**
     * @brief Renders the first pass of a frame.
     * @param pipeline A pointer to a Pipeline for use in pass 2.
     * @return Returns the pointer passed in pipeline if in a transition, else returns nullptr.
     */
    auto RenderFrameOnlyPass1(Pipeline* pipeline) -> Pipeline*;

    void RenderFrameOnlyPass2(Pipeline* pipeline, int offsetX, int offsetY);

    void RenderFrameEndOnSeparatePasses(Pipeline* pipeline);

    auto PipelineContext() -> class PipelineContext&;

    auto PipelineContext2() -> class PipelineContext&;

    void ReadConfig(const std::string& configurationFilename);

    void ReadSettings(const class Settings& settings);

    void Initialize();

    void Reset();

    void ResetEngine();

    /// Initializes preset loading / management libraries
    auto InitializePresetTools() -> void;

    auto SwitchToCurrentPreset() -> std::unique_ptr<Preset>;

    auto StartPresetTransition(bool hardCut) -> bool;

    void RecreateRenderer();

#if USE_THREADS

    void ThreadWorker();

#endif

    class Pcm m_pcm; //!< Audio data buffer and analyzer instance.

    class Settings m_settings; //!< The projectM Settings.

    Flags m_flags{Flags::None}; //!< Behaviour flags.

    std::vector<int> m_presetHistory; //!< List of previously played preset indices.
    std::vector<int> m_presetFuture;  //!< List of preset indices queued for playing.

    std::vector<std::string> m_textureSearchPaths; ///!< List of paths to search for texture files

    /** Timing information */
    int m_count{0}; //!< Rendered frame count since start

    bool m_errorLoadingCurrentPreset{false}; //!< Error flag for preset loading errors.

    class PipelineContext m_pipelineContext;  //!< Pipeline context for the first/current preset.
    class PipelineContext m_pipelineContext2; //!< Pipeline context for the next/transitioning preset.

    std::unique_ptr<Renderer> m_renderer;                      //!< The Preset renderer.
    std::unique_ptr<BeatDetect> m_beatDetect;                  //!< The beat detection class.
    std::unique_ptr<PresetIterator> m_presetPos;               //!< The current position of the directory iterator.
    std::unique_ptr<PresetLoader> m_presetLoader;              //!< Required by the preset chooser. Manages a loaded preset directory.
    std::unique_ptr<PresetChooser> m_presetChooser;            //!< Provides accessor functions to choose presets.
    std::unique_ptr<Preset> m_activePreset;                    //!< Currently loaded preset.
    std::unique_ptr<Preset> m_activePreset2;                   //!< Destination preset when smooth preset switching.
    std::unique_ptr<TimeKeeper> m_timeKeeper;                  //!< Keeps the different timers used to render and switch presets.

#if USE_THREADS
    mutable std::recursive_mutex m_presetSwitchMutex; //!< Mutex for locking preset switching while rendering and vice versa.
    std::thread m_workerThread;                       //!< Background worker for preloading presets.
    BackgroundWorkerSync m_workerSync;                //!< Background work synchronizer.
#endif
};
