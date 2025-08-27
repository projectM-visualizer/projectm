

#pragma once

#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_EXPORT projectm_handle projectm_create();


PROJECTM_EXPORT void projectm_destroy(projectm_handle instance);


PROJECTM_EXPORT void projectm_load_preset_file(projectm_handle instance, const char* filename,
                                               bool smooth_transition);


PROJECTM_EXPORT void projectm_load_preset_data(projectm_handle instance, const char* data,
                                               bool smooth_transition);


PROJECTM_EXPORT void projectm_reset_textures(projectm_handle instance);


PROJECTM_EXPORT void projectm_get_version_components(int* major, int* minor, int* patch);


PROJECTM_EXPORT char* projectm_get_version_string();


PROJECTM_EXPORT char* projectm_get_vcs_version_string();

#ifdef __cplusplus
}
#endif
