#pragma once

#include "playlist.h"

#include "Playlist.hpp"

#include <string>
#include <vector>

class PlaylistCWrapper : public ProjectM::Playlist::Playlist
{
public:
    PlaylistCWrapper() = delete;
    explicit PlaylistCWrapper(projectm_handle projectMInstance);

    /**
     * @brief Reconnects the playlist instance to another projectM instance, or disconnects it.
     * @param projectMInstance A pointer to an existing projectM instance or nullptr to disconnect.
     */
    void Connect(projectm_handle projectMInstance);

private:
    projectm_handle m_projectMInstance{nullptr}; //!< The projectM instance handle this instance is connected to.
};
