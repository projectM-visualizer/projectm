#pragma once

#include "projectM-4/playlist.h"

#include "Playlist.hpp"

#include <string>
#include <vector>

class PlaylistCWrapper : public ProjectM::Playlist::Playlist
{
public:
    PlaylistCWrapper() = delete;

    /**
     * Constructor.
     * @param projectMInstance The projectM instance to connect to.
     */
    explicit PlaylistCWrapper(projectm_handle projectMInstance);

    /**
     * @brief Reconnects the playlist instance to another projectM instance, or disconnects it.
     * @param projectMInstance A pointer to an existing projectM instance or nullptr to disconnect.
     */
    virtual void Connect(projectm_handle projectMInstance);

    virtual void PlayPresetIndex(size_t index, bool hardCut, bool resetFailureCount);

    /**
     * @brief Callback executed by projectM if a preset switch should be done.
     * @param isHardCut True, if the switch should be immediate, false if a smooth transition.
     * @param userData Context data pointer ("this" pointer of the wrapper class).
     */
    static void OnPresetSwitchRequested(bool isHardCut, void* userData);

    /**
     * @brief Callback executed by projectM if a preset switch has failed.
     * @param presetFilename The preset filename that was being loaded.
     * @param message The failure message.
     * @param userData Context data pointer ("this" pointer of the wrapper class).
     */
    static void OnPresetSwitchFailed(const char* presetFilename, const char* message,
                                     void* userData);

    /**
     * @brief Sets the retry count for preset switches before giving up.
     * @param retryCount The number of retries.
     */
    virtual void SetRetryCount(uint32_t retryCount);

    /**
     * @brief Sets the retry count for preset switches before giving up.
     * @param retryCount The number of retries.
     */
    virtual auto RetryCount() -> uint32_t;

    /**
     * @brief Sets the preset switched callback.
     * @param callback The callback pointer.
     * @param userData The callback context data.
     */
    virtual void SetPresetSwitchedCallback(projectm_playlist_preset_switched_event callback,
                                           void* userData);

    /**
     * @brief Sets the preset switch failed callback.
     * @param callback The callback pointer.
     * @param userData The callback context data.
     */
    virtual void SetPresetSwitchFailedCallback(projectm_playlist_preset_switch_failed_event callback,
                                               void* userData);

private:
    projectm_handle m_projectMInstance{nullptr}; //!< The projectM instance handle this instance is connected to.

    uint32_t m_presetSwitchRetryCount{5};  //!< Number of switch retries before sending the failure event to the application.
    uint32_t m_presetSwitchFailedCount{0}; //!< Number of retries since the last preset switch.

    bool m_hardCutRequested{false}; //!< Stores the type of the last requested switch attempt.

    projectm_playlist_preset_switched_event m_presetSwitchedEventCallback{nullptr}; //!< Preset switched callback pointer set by the application.
    void* m_presetSwitchedEventUserData{nullptr};                                   //!< Context data pointer set by the application.

    projectm_playlist_preset_switch_failed_event m_presetSwitchFailedEventCallback{nullptr}; //!< Preset switch failed callback pointer set by the application.
    void* m_presetSwitchFailedEventUserData{nullptr};                                        //!< Context data pointer set by the application.
};
