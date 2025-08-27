

#pragma once

#include "projectM-4/playlist_types.h"

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_PLAYLIST_EXPORT void projectm_playlist_free_string(char* string);


PROJECTM_PLAYLIST_EXPORT void projectm_playlist_free_string_array(char** array);

#ifdef __cplusplus
}
#endif
