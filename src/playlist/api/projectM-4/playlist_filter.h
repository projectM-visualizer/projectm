

#pragma once

#include "projectM-4/playlist_types.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_filter(projectm_playlist_handle instance, const char** filter_list,
                                                           size_t count);


PROJECTM_PLAYLIST_EXPORT char** projectm_playlist_get_filter(projectm_playlist_handle instance, size_t* count);


PROJECTM_PLAYLIST_EXPORT size_t projectm_playlist_apply_filter(projectm_playlist_handle instance);

#ifdef __cplusplus
}
#endif
