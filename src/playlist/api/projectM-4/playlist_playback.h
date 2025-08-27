

#pragma once

#include "projectM-4/playlist_types.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_shuffle(projectm_playlist_handle instance, bool shuffle);


PROJECTM_PLAYLIST_EXPORT bool projectm_playlist_get_shuffle(projectm_playlist_handle instance);


PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_retry_count(projectm_playlist_handle instance, uint32_t retry_count);


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_get_retry_count(projectm_playlist_handle instance);


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_set_position(projectm_playlist_handle instance, uint32_t new_position,
                                                                 bool hard_cut);


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_get_position(projectm_playlist_handle instance);


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_play_next(projectm_playlist_handle instance, bool hard_cut);


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_play_previous(projectm_playlist_handle instance, bool hard_cut);


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_play_last(projectm_playlist_handle instance, bool hard_cut);

#ifdef __cplusplus
}
#endif
