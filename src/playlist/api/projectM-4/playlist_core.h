

#pragma once

#include "projectM-4/playlist_types.h"
#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_PLAYLIST_EXPORT projectm_playlist_handle projectm_playlist_create(projectm_handle projectm_instance);


PROJECTM_PLAYLIST_EXPORT void projectm_playlist_destroy(projectm_playlist_handle instance);


PROJECTM_PLAYLIST_EXPORT void projectm_playlist_connect(projectm_playlist_handle instance, projectm_handle projectm_instance);

#ifdef __cplusplus
}
#endif
