

#pragma once

#include "projectM-4/playlist_types.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_size(projectm_playlist_handle instance);


PROJECTM_PLAYLIST_EXPORT void projectm_playlist_clear(projectm_playlist_handle instance);


PROJECTM_PLAYLIST_EXPORT char** projectm_playlist_items(projectm_playlist_handle instance, uint32_t start, uint32_t count);


PROJECTM_PLAYLIST_EXPORT char* projectm_playlist_item(projectm_playlist_handle instance, uint32_t index);


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_add_path(projectm_playlist_handle instance, const char* path,
                                                             bool recurse_subdirs, bool allow_duplicates);



PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_insert_path(projectm_playlist_handle instance, const char* path,
                                                                uint32_t index, bool recurse_subdirs, bool allow_duplicates);


PROJECTM_PLAYLIST_EXPORT bool projectm_playlist_add_preset(projectm_playlist_handle instance, const char* filename,
                                                           bool allow_duplicates);


PROJECTM_PLAYLIST_EXPORT bool projectm_playlist_insert_preset(projectm_playlist_handle instance, const char* filename,
                                                              uint32_t index, bool allow_duplicates);


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_add_presets(projectm_playlist_handle instance, const char** filenames,
                                                                uint32_t count, bool allow_duplicates);


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_insert_presets(projectm_playlist_handle instance, const char** filenames,
                                                                   uint32_t count, unsigned int index, bool allow_duplicates);


PROJECTM_PLAYLIST_EXPORT bool projectm_playlist_remove_preset(projectm_playlist_handle instance, uint32_t index);


PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_remove_presets(projectm_playlist_handle instance, uint32_t index,
                                                                   uint32_t count);


PROJECTM_PLAYLIST_EXPORT void projectm_playlist_sort(projectm_playlist_handle instance, uint32_t start_index, uint32_t count,
                                                     projectm_playlist_sort_predicate predicate, projectm_playlist_sort_order order);

#ifdef __cplusplus
}
#endif
