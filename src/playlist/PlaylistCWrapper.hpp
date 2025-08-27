#pragma once

#include "projectM-4/playlist.h"

#include "Playlist.hpp"

#include <cstdint>

namespace libprojectM {
namespace Playlist {

class PlaylistCWrapper : public Playlist
{
public:

    enum class NavigationDirection : uint8_t
    {
        Previous,
        Next,
        Last
    };

    PlaylistCWrapper() = delete;


    explicit PlaylistCWrapper(projectm_handle projectMInstance);


    virtual void Connect(projectm_handle projectMInstance);

    virtual void PlayPresetIndex(uint32_t index, bool hardCut, bool resetFailureCount);


    static void OnPresetSwitchRequested(bool isHardCut, void* userData);


    static void OnPresetSwitchFailed(const char* presetFilename, const char* message,
                                     void* userData);


    virtual void SetRetryCount(uint32_t retryCount);


    virtual auto RetryCount() -> uint32_t;


    virtual void SetPresetSwitchedCallback(projectm_playlist_preset_switched_event callback,
                                           void* userData);


    virtual void SetPresetSwitchFailedCallback(projectm_playlist_preset_switch_failed_event callback,
                                               void* userData);


    void SetLastNavigationDirection(NavigationDirection direction);


    auto GetLastNavigationDirection() const -> NavigationDirection;

private:
    projectm_handle m_projectMInstance{nullptr};

    uint32_t m_presetSwitchRetryCount{5};
    uint32_t m_presetSwitchFailedCount{0};

    bool m_hardCutRequested{false};

    projectm_playlist_preset_switched_event m_presetSwitchedEventCallback{nullptr};
    void* m_presetSwitchedEventUserData{nullptr};

    projectm_playlist_preset_switch_failed_event m_presetSwitchFailedEventCallback{nullptr};
    void* m_presetSwitchFailedEventUserData{nullptr};

    NavigationDirection m_lastNavigationDirection{NavigationDirection::Next};
};

}
}
