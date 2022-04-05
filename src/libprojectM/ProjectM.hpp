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
#include "event.h"
#include "fatal.h"

#ifdef WIN32
// libs required for win32
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "kernel32.lib")

#pragma warning (disable:4244)
#pragma warning (disable:4305)

#include <windows.h>
#else
#include <unistd.h>
#endif /** WIN32 */

#include <sys/types.h>

#include <memory>
#include <string>
#include <vector>

#if USE_THREADS

#include <mutex>
#include <thread>
#include "BackgroundWorker.h"

#endif

class PipelineContext;

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

class RenderItemMatcher;

class MasterRenderItemMerge;

/** Interface types */
typedef enum
{
    MENU_INTERFACE,
    SHELL_INTERFACE,
    EDITOR_INTERFACE,
    DEFAULT_INTERFACE,
    BROWSER_INTERFACE
} interface_t;

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

    class Settings
    {
    public:
        size_t meshX{ 32 };
        size_t meshY{ 24 };
        size_t fps{ 35 };
        size_t textureSize{ 512 };
        size_t windowWidth{ 512 };
        size_t windowHeight{ 512 };
        std::string presetURL;
        std::string titleFontURL;
        std::string menuFontURL;
        std::string datadir;
        double presetDuration{ 15.0 };
        double softCutDuration{ 10.0 };
        double hardCutDuration{ 60.0 };
        bool hardCutEnabled{ false };
        float hardCutSensitivity{ 2.0 };
        float beatSensitivity{ 1.0 };
        bool aspectCorrection{ true };
        float easterEgg{ 0.0 };
        bool shuffleEnabled{ true };
        bool softCutRatingsEnabled{ false };
    };

    ProjectM(std::string config_file, Flags flags = Flags::None);

    ProjectM(Settings settings, Flags flags = Flags::None);

    virtual ~ProjectM();

    void ResetOpenGL(size_t width, size_t height);

    void ResetTextures();

    std::string Title() const;

    void SetTitle(const std::string& title);

    void RenderFrame();

    unsigned InitRenderToTexture();

    void KeyHandler(projectMEvent event,
                    projectMKeycode keycode, projectMModifier modifier);

    void SetTextureSize(size_t size);

    size_t TextureSize() const;

    double SoftCutDuration() const;

    void SetSoftCutDuration(int seconds);

    void SetSoftCutDuration(double seconds);

    double HardCutDuration() const;

    void SetHardCutDuration(int seconds);

    void SetHardCutDuration(double seconds);

    bool HardCutEnabled() const;

    void SetHardCutEnabled(bool enabled);

    float HardCutSensitivity() const;

    void SetHardCutSensitivity(float sensitivity);

    void SetPresetDuration(int seconds);

    void SetPresetDuration(double seconds);

    bool AspectCorrection() const;

    void SetAspectCorrection(bool enabled);

    float EasterEgg() const;

    void SetEasterEgg(float value);

    void MeshSize(size_t& w, size_t& h) const;

    void SetMeshSize(size_t w, size_t h);

    void Touch(float x, float y, int pressure, int touchtype);

    void TouchDrag(float x, float y, int pressure);

    void TouchDestroy(float x, float y);

    void TouchDestroyAll();

    void SetHelpText(const std::string& helpText);

    void ToggleSearchText(); // turn search text input on / off
    void SetToastMessage(const std::string& toastMessage);

    const Settings& Settings() const
    {
        return m_settings;
    }

    /// Writes a Settings configuration to the specified file
    static bool WriteConfig(const std::string& configFile, const class Settings& settings);

    /// Sets preset iterator position to the passed in index
    void SelectPresetPosition(unsigned int index);

    /// Plays a preset immediately
    void SelectPreset(unsigned int index, bool hardCut = true);

    /// Populates a page full of presets for the renderer to use.
    void PopulatePresetMenu();

    /// Removes a preset from the play list. If it is playing then it will continue as normal until next switch
    void RemovePreset(unsigned int index);

    /// Removes entire playlist, The currently loaded preset will end up sticking until new presets are added
    void ClearPlaylist();

    /// Turn on or off a lock that prevents projectM from switching to another preset
    void SetPresetLocked(bool isLocked);

    /// Returns true if the active preset is locked
    bool PresetLocked() const;

    /// Returns true if the text based search menu is up.
    bool TextInputActive(bool nomin = false) const;

    unsigned int PresetIndex(const std::string& url) const;

    /// Plays a preset immediately when given preset name
    void SelectPresetByName(std::string name, bool hardCut = true);

    // search based on keystroke
    std::string SearchText() const;

    // search based on keystroke
    void SetSearchText(const std::string& searchKey);

    // delete part of search term (backspace)
    void DeleteSearchText();

    // reset search term (blank)
    void ResetSearchText();

    /// Returns index of currently active preset. In the case where the active
    /// preset was removed from the playlist, this function will return the element
    /// before active preset (thus the next in order preset is invariant with respect
    /// to the removal)
    bool SelectedPresetIndex(unsigned int& index) const;

    /// Add a preset url to the play list. Appended to bottom. Returns index of preset
    unsigned int
    AddPresetURL(const std::string& presetURL, const std::string& presetName, const RatingList& ratingList);

    /// Insert a preset url to the play list at the suggested index.
    void InsertPresetURL(unsigned int index,
                         const std::string& presetURL, const std::string& presetName, const RatingList& ratingList);

    /// Returns true if the selected preset position points to an actual preset in the
    /// currently loaded playlist
    bool PresetPositionValid() const;

    /// Returns the url associated with a preset index
    std::string PresetURL(unsigned int index) const;

    /// Returns the preset name associated with a preset index
    std::string PresetName(unsigned int index) const;

    void ChangePresetName(unsigned int index, std::string name);

    /// Returns the rating associated with a preset index
    int PresetRating(unsigned int index, const PresetRatingType ratingType) const;

    void ChangePresetRating(unsigned int index, int rating, const PresetRatingType ratingType);

    /// Returns the size of the play list
    unsigned int PlaylistSize() const;


    inline void SetShuffleEnabled(bool value)
    {
        m_settings.shuffleEnabled = value;

        /// idea@ call a virtualfunction shuffleChanged()
    }


    inline bool ShuffleEnabled() const
    {
        return m_settings.shuffleEnabled;
    }

    /// Occurs when active preset has switched. Switched to index is returned
    virtual void PresetSwitchedEvent(bool /*isHardCut*/, size_t /*index*/) const {};

    virtual void ShuffleEnabledValueChanged(bool /*isEnabled*/) const {};

    virtual void PresetSwitchFailedEvent(bool /*hardCut*/, unsigned int /*index*/, const std::string& /*message*/) const {};


    /// Occurs whenever preset rating has changed via ChangePresetRating() method
    virtual void PresetRatingChanged(unsigned int /*index*/, int /*rating*/, PresetRatingType /*ratingType*/) const {};


    inline Pcm& Pcm()
    {
        return m_pcm;
    }

    /// Get the preset index given a name
    unsigned int SearchIndex(const std::string& name) const;

    void SelectPrevious(const bool);

    void SelectNext(const bool);

    void SelectRandom(const bool);

    int WindowWidth()
    {
        return m_settings.windowWidth;
    }

    int WindowHeight()
    {
        return m_settings.windowHeight;
    }

    bool ErrorLoadingCurrentPreset() const
    {
        return m_errorLoadingCurrentPreset;
    }

    void DefaultKeyHandler(projectMEvent event, projectMKeycode keycode);

private:
    void EvaluateSecondPreset();

    Pipeline* RenderFrameOnlyPass1(Pipeline* pPipeline);

    void RenderFrameOnlyPass2(Pipeline* pPipeline, int xoffset, int yoffset, int eye);

    void RenderFrameEndOnSeparatePasses(Pipeline* pPipeline);

    class PipelineContext& PipelineContext()
    {
        return *m_pipelineContext;
    }

    class PipelineContext& PipelineContext2()
    {
        return *m_pipelineContext2;
    }

    void ReadConfig(const std::string& configFile);

    void ReadSettings(const class Settings& settings);

    void Initialize(int gx, int gy, int fps, int texsize, int width, int height);

    void Reset();

    void ResetEngine();

    /// Initializes preset loading / management libraries
    int InitializePresetTools(int gx, int gy);

    /// Deinitialize all preset related tools. Usually done before projectM cleanup
    void DestroyPresetTools();

    std::unique_ptr<Preset> SwitchToCurrentPreset();

    bool StartPresetTransition(bool hard_cut);

    void RecreateRenderer();

    #if USE_THREADS

        void ThreadWorker();

    #endif

    class Pcm m_pcm;//!< Audio data buffer and analyzer instance.

    class Settings m_settings;//!< The projectM Settings.

    Flags m_flags{Flags::None};//!< Behaviour flags.

    std::vector<int> m_presetHistory;//!< List of previously played preset indices.
    std::vector<int> m_presetFuture; //!< List of preset indices queued for playing.

    /** Timing information */
    int m_count{0};//!< Rendered frame count since start

    bool m_errorLoadingCurrentPreset{false};//!< Error flag for preset loading errors.

    std::unique_ptr<Renderer> m_renderer;                     //!< The Preset renderer.
    std::unique_ptr<BeatDetect> m_beatDetect;                 //!< The beat detection class.
    std::unique_ptr<class PipelineContext> m_pipelineContext; //!< Pipeline context for the first/current preset.
    std::unique_ptr<class PipelineContext> m_pipelineContext2;//!< Pipeline context for the next/transitioning preset.
    std::unique_ptr<PresetIterator> m_presetPos;              //!< The current position of the directory iterator.
    std::unique_ptr<PresetLoader> m_presetLoader;             //!< Required by the preset chooser. Manages a loaded preset directory.
    std::unique_ptr<PresetChooser> m_presetChooser;           //!< Provides accessor functions to choose presets.
    std::unique_ptr<Preset> m_activePreset;                   //!< Currently loaded preset.
    std::unique_ptr<Preset> m_activePreset2;                  //!< Destination preset when smooth preset switching.
    std::unique_ptr<TimeKeeper> m_timeKeeper;                 //!< Keeps the different timers used to render and switch presets.
    std::unique_ptr<RenderItemMatcher> m_matcher;             //!< Render item matcher for preset transitions.
    std::unique_ptr<MasterRenderItemMerge> m_merger;          //!< Render item merger for preset transitions.

#if USE_THREADS
    mutable std::recursive_mutex m_presetSwitchMutex;//!< Mutex for locking preset switching while rendering and vice versa.
    std::thread m_workerThread;                      //!< Background worker for preloading presets.
    BackgroundWorkerSync m_workerSync;               //!< Background work synchronizer.
#endif
};
