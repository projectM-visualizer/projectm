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
/**
 * $Id: projectM.hpp,v 1.1.1.1 2005/12/23 18:05:11 psperl Exp $
 *
 * Encapsulation of ProjectM engine
 *
 * $Log$
 */

#pragma once

#ifdef WIN32
#include "dirent.h"
#else

#include <dirent.h>

#endif /** WIN32 */

#include <cmath>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <vector>

#ifndef WIN32

#include <unistd.h>

#endif

#include <sys/types.h>

#ifdef __APPLE__
//#include <MacWindows.h>
//#include <gl.h>
//#include <glu.h>
#else
#ifdef WIN32
#include <windows.h>
#endif /** WIN32 */

#endif /** MACOS */

#include "event.h"
#include "fatal.h"

#include "PCM.hpp"

#include "Common.hpp"
#include "BackgroundWorker.h"

#include <memory>
#include <mutex>
#include <thread>

#ifdef WIN32
#pragma warning (disable:4244)
#pragma warning (disable:4305)
#endif /** WIN32 */

class PipelineContext;

class BeatDetect;

class PCM;

class Func;

class Renderer;

class Preset;

class PresetIterator;

class PresetChooser;

class PresetLoader;

class TimeKeeper;

class Pipeline;

class RenderItemMatcher;

class MasterRenderItemMerge;

class ProjectM
{
public:
    /*
     * Behaviour flags for the projectM instance. Currently, it's only used to prevent automatically filling
     * the preset playlist by traversing the preset path for files.
     */
    enum Flags
    {
        None = 0, //!< No special flags.
        DisablePlaylistLoad = 1 << 0 //!< Prevent automatic playlist loading on startup.
    };

    /**
     * @brief GetSettings class.
     *
     * Stores all (or rather: most) projectM GetSettings.
     */
    class Settings
    {
    public:
        size_t meshX{ 32 }; //!< Per-point mesh X size
        size_t meshY{ 24 }; //!< Per-point mesh Y size
        size_t fps{ 35 }; //!< Target FPS
        size_t textureSize{ 512 }; //!< Internal texture size in pixels.
        size_t windowWidth{ 512 }; //!< Rendering viewport width.
        size_t windowHeight{ 512 }; //!< Rendering viewport height.
        std::string presetURL; //!< Path where the presets are located.
        std::string datadir; //!< Path with additional data, e.g. the default config file.
        double presetDuration{ 15.0 }; //!< Preset display duration in seconds.
        double softCutDuration{ 10.0 }; //!< Time in seconds it takes to softly blend from one preset to another.
        double hardCutDuration{ 60.0 }; //!< Minimum display time before a hard cut can happen.
        bool hardCutEnabled{ false }; //!< Determines if hard cuts are allowed.
        float hardCutSensitivity{ 2.0 }; //!< Hard cut beat sensitivity.
        float beatSensitivity{ 1.0 }; //!< Preset beat sensitivity.
        bool aspectCorrection{ true }; //!< Use aspect ration correction.
        float easterEgg{ 0.0 }; //!< Effectively randomizes the preset display duration a bit.
        bool shuffleEnabled{ true }; //!< Randomly select the next preset in the playlist.
        bool softCutRatingsEnabled{ false }; //!< Enable ratings affecting presets on soft cuts.
    };

    /**
     * Constructor. Loads GetSettings from the given file.
     * @param configFile Filename of the configuration file (config.inp).
     * @param flags Behaviour flags.
     */
    ProjectM(const std::string& configFile, Flags flags = Flags::None);

    /**
     * Constructor. Loads GetSettings from the given Settings class.
     * @param settings The Settings class instance with GetSettings to use.
     * @param flags Behaviour flags.
     */
    ProjectM(const Settings& settings, Flags flags = Flags::None);

    /**
     * Destructor.
     */
    virtual ~ProjectM();

    /**
     * @brief Returns a reference to the audio data buffer.
     * @return A reference to the audio data buffer instance.
     */
    auto PCM() -> class PCM&;

    /**
     * @brief Resets OpenGL rendering.
     *
     * This will reload all textures and recompile shaders.
     *
     * @param width Viewport width.
     * @param height Viewport height.
     */
    void ResetOpenGL(size_t width, size_t height);

    /**
     * @brief Renders a single frame into the current OpenGL context.
     */
    void RenderFrame();

    /**
     * @brief Creates a texture and returns the OpenGL texture ID.
     *
     * @fixme This was supposed to return projectM's rendering output as a texture instead of rendering directly
     *        into the context. Actually, this will only set the output size to the "textureSize" setting, but still
     *        render directly onto the context surface. Using an externally supplied framebuffer object would be the
     *        way to go.
     *
     * @return An actual GLuint with the newly created texture.
     */
    auto InitRenderToTexture() -> unsigned;

    /**
     * @name GetSettings Access
     * Getters and setters for projectM's GetSettings. These are the values supplied either via the config file
     * or the GetSettings class.
     */
    ///@{

    /**
     * @brief Writes the provided GetSettings configuration to the specified file
     *
     * Note this will overwrite any existing file. If the previous config file contained comments,
     * these will be removed.
     *
     * @param configFile The file to write the settings to.
     * @param settings The settings to store.
     * @return True if the file was written successfully, false otherwise.
     */
    static auto WriteConfig(const std::string& configFile, const Settings& settings) -> bool;

    /**
     * @brief Returns a reference to the internal settings class.
     * @return A reference to the internal settings class.
     */
    auto GetSettings() const -> const Settings&;

    /**
     * @brief Sets the size of the internal rendering texture.
     *
     * Will recreate the whole renderer.
     *
     * @param size The new size of the texture in pixels.
     */
    void SetTextureSize(size_t size);

    /**
     * @brief Returns the size of the internal rendering texture.
     * @return The size of the internal rendering texture.
     */
    auto GetTextureSize() const -> size_t;

    /**
     * @brief Sets the soft cut transition duration.
     * @param seconds The number of seconds to transition from one preset to another.
     */
    void SetSoftCutDuration(double seconds);

    /**
     * @brief Returns the current soft cut transition duration.
     * @return The current soft cut transition duration.
     */
    auto GetSoftCutDuration() const -> double;

    void SetHardCutDuration(double seconds);

    auto GetHardCutDuration() const -> double;

    void SetHardCutEnabled(bool enabled);

    auto GetHardCutEnabled() const -> bool;

    void SetHardCutSensitivity(float sensitivity);

    auto GetHardCutSensitivity() const -> float;

    void SetPresetDuration(double seconds);

    auto GetPresetDuration() const -> double;

    void SetAspectCorrection(bool enabled);

    auto GetAspectCorrection() const -> bool;

    auto GetEasterEgg() const -> float;

    void SetEasterEgg(float value);

    void SetMeshSize(size_t width, size_t height);

    void GetMeshSize(size_t& width, size_t& height) const;

    void SetShuffleEnabled(bool enabled);

    auto GetShuffleEnabled() const -> bool;

    auto GetWindowWidth() const -> size_t;

    auto GetWindowHeight() const -> size_t;

    ///@}


    /**
     * @name Input Handling
     * Methods which can be called on user interactions, e.g. key presses.
     */
    ///@{

    /**
     * @brief Internal key handler.
     *
     * Was initially done to create a built-in key mapping that resembles Milkdrop's input mapping.
     * Since projectM is now a library, it totally depends on the integrating app how the user can interact
     * with projectM. Thus:
     * @todo Remove this.
     *
     * @param event The key event.
     * @param keyCode The key scancode.
     * @param modifier The modifier flags, e.g. shift/alt/ctrl/command.
     */
    void KeyHandler(projectMEvent event,
                    projectMKeycode keyCode,
                    projectMModifier modifier);

    void DefaultKeyHandler(projectMEvent event, projectMKeycode keyCode);

    void Touch(float touchX, float touchY, int pressure, int touchType);

    void TouchDrag(float touchX, float touchY, int pressure);

    void TouchDestroy(float touchX, float touchY);

    void TouchDestroyAll();

    ///@}



    /**
     * @name Playlist Management Functions
     * Used to manipulate the playlist and control which preset is currently being displayed.
     */
    ///@{

    /// Sets preset iterator position to the passed in index
    void SelectPresetPosition(unsigned int index);

    /// Plays a preset immediately
    void ShowPreset(unsigned int index, bool hardCut = true);

    /// Plays a preset immediately when given preset name
    void ShowPresetByName(std::string name, bool hardCut = true);

    void ShowPreviousPreset(bool hardCut);

    void ShowNextPreset(bool hardCut);

    void ShowRandomPreset(bool hardCut);

    /// Removes a preset from the play list. If it is playing then it will continue as normal until next switch
    void RemovePreset(unsigned int index);

    /// Removes entire playlist, The currently loaded preset will end up sticking until new presets are added
    void ClearPlaylist();

    /// Turn on or off a lock that prevents projectM from switching to another preset
    void SetPresetLock(bool isLocked);

    /// Returns true if the active preset is locked
    auto GetPresetLock() const -> bool;

    auto GetPresetIndex(const std::string& presetFilename) const -> unsigned int;

    /// Returns index of currently active preset. In the case where the active
    /// preset was removed from the playlist, this function will return the element
    /// before active preset (thus the next in order preset is invariant with respect
    /// to the removal)
    auto GetSelectedPresetIndex(unsigned int& index) const -> bool;

    /// Add a preset to the play list. Appended to bottom. Returns index of preset
    auto AddPreset(const std::string& presetFilename,
                   const std::string& presetName,
                   const RatingList& ratingList) -> unsigned int;

    /// Inserts a preset to the play list at the suggested index.
    void InsertPreset(unsigned int index,
                      const std::string& presetFilename,
                      const std::string& presetName,
                      const RatingList& ratingList);

    /// Returns true if the selected preset position points to an actual preset in the
    /// currently loaded playlist
    auto IsPresetPositionValid() const -> bool;

    /// Returns the filename associated with a preset index
    auto GetPresetFilename(unsigned int index) const -> std::string;

    void SetPresetName(unsigned int index, std::string presetName);

    /// Returns the preset name associated with a preset index
    auto GetPresetName(unsigned int index) const -> std::string;

    void SetPresetRating(unsigned int index, int rating, const PresetRatingType ratingType);

    /// Returns the rating associated with a preset index
    auto GetPresetRating(unsigned int index, const PresetRatingType ratingType) const -> int;

    /// Returns the size of the play list
    auto GetPlaylistSize() const -> unsigned int;

    auto GetErrorLoadingCurrentPreset() const -> bool;

    ///@}



    /**
     * @name On-Screen Menu Functions
     * These functions are used for interacting with the built-in on-screen menu, help text and info overlays.
     */
    ///@{

    void SetHelpText(const std::string& helpText);

    void ToggleSearchText();

    void ShowToastMessage(const std::string& toastMessage);

    /// Populates a page full of presets for the renderer to use.
    void PopulatePresetMenu();

    /// Returns true if the text based search menu is up.
    auto IsTextInputActive(bool nomin = false) const -> bool;

    // search based on keystroke
    auto GetSearchText() const -> std::string;

    // search based on keystroke
    void SetSearchText(const std::string& searchKey);

    // delete part of search term (backspace)
    void DeleteSearchTextChar();

    // reset search term (blank)
    void ResetSearchText();

    /// Get the preset index given a name
    auto GetSearchIndex(const std::string& name) const -> unsigned int;

    ///@}


    /**
     * @name Event Callbacks
     * Classes inheriting this projectM base class can overload these methods to act on certain events.
     */
    ///@{

    /**
     * @brief The active preset has changed.
     *
     * This can happen either automatically if the preset duration caused a switch, or manually from a programmatic
     * preset switch.
     *
     * @param isHardCut True if the cut is a hard cut.
     * @param index The playlist index of the preset which will now be played.
     */
    virtual void OnPresetSwitched(bool isHardCut, size_t index) const
    {
    };

    /**
     * @brief The next preset could not be loaded or changed..
     *
     * This can happen if either a preset file is broken, or the playlist is empty.
     *
     * @param isHardCut True if the cut is a hard cut.
     * @param index The playlist index of the preset which should have been played.
     * @param errorMessage The error message with additional details.
     */
    virtual void OnPresetSwitchFailed(bool isHardCut, unsigned int index, const std::string& errorMessage) const
    {
    };

    /**
     * @brief The rating of a preset was changed by ChangePresetRating().
     * @param index The playlist index of the preset which rating has been changed.
     * @param rating The new rating score.
     * @param ratingType The type of the rating that was changed.
     */
    virtual void OnPresetRatingChanged(unsigned int index, int rating, PresetRatingType ratingType) const
    {
    };

    ///@}


private:

    void EvaluateSecondPreset();

    auto RenderFrameOnlyPass1(Pipeline* pPipeline) -> Pipeline*;

    void RenderFrameOnlyPass2(Pipeline* pPipeline, int offsetX, int offsetY, int eye);

    void RenderFrameEndOnSeparatePasses(Pipeline* pPipeline);

    auto PipelineContext() -> class PipelineContext&;

    auto PipelineContext2() -> class PipelineContext&;

    void ReadConfig(const std::string& configFile);

    void ReadSettings(const Settings& settings);

    void Initialize(int meshX, int meshY, int fps, int textureSize, int width, int height);

    void Reset();

    void ResetBeatDetection();

    /**
     * @brief Initializes preset loading / management libraries
     * @param meshX Per-point mesh X size.
     * @param meshY Per-point mesh Y size.
     * @return Either PROJECTM_FAILURE or PROJECTM_SUCCESS.
     */
    auto InitializePresetTools(int meshX, int meshY) -> int;

    /**
     * @brief Deinitialize all preset related tools. Usually done before projectM cleanup
     */
    void DestroyPresetTools();

    auto SwitchToCurrentPreset() -> std::unique_ptr<Preset>;

    auto StartPresetTransition(bool hardCut) -> bool;

    void RecreateRenderer();

#if USE_THREADS

    void ThreadWorker();

#endif

    class PCM m_pcm; //!< Audio data buffer and analyzer instance.

    Settings m_settings; //!< The projectM settings.

    int m_flags{ 0 }; //!< Behaviour flags.

    std::vector<int> m_presetHistory; //!< List of previously played preset indices.
    std::vector<int> m_presetFuture; //!< List of preset indices queued for playing.

    /** Timing information */
    int m_count{ 0 }; //!< Rendered frame count since start.
    float m_fpsStart{ 0.0f }; //!< FPS at start of the frame, used for FPS limiting.

    bool m_errorLoadingCurrentPreset{ false }; //!< Error flag for preset loading errors.

    std::unique_ptr<Renderer> m_renderer; //!< The Preset renderer.
    std::unique_ptr<BeatDetect> m_beatDetect; //!< The beat detection class.
    std::unique_ptr<class PipelineContext> m_pipelineContext; //!< Pipeline context for the first/current preset.
    std::unique_ptr<class PipelineContext> m_pipelineContext2; //!< Pipeline context for the next/transitioning preset.
    std::unique_ptr<PresetIterator> m_presetPos; //!< The current position of the directory iterator.
    std::unique_ptr<PresetLoader> m_presetLoader; //!< Required by the preset chooser. Manages a loaded preset directory.
    std::unique_ptr<PresetChooser> m_presetChooser; //!< Provides accessor functions to choose presets.
    std::unique_ptr<Preset> m_activePreset; //!< Currently loaded preset.
    std::unique_ptr<Preset> m_activePreset2; //!< Destination preset when smooth preset switching.
    std::unique_ptr<TimeKeeper> m_timeKeeper; //!< Keeps the different timers used to render and switch presets.
    std::unique_ptr<RenderItemMatcher> m_matcher; //!< Render item matcher for preset transitions.
    std::unique_ptr<MasterRenderItemMerge> m_merger; //!< Render item merger for preset transitions.

#if USE_THREADS
    mutable std::mutex m_presetSwitchMutex; //!< Mutex for locking preset switching while rendering and vice versa.
    std::thread m_workerThread; //!< Background worker for preloading presets.
    BackgroundWorkerSync m_workerSync; //!< Background work synchronizer.
#endif
};
