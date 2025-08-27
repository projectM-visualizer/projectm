

#pragma once

#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_EXPORT void projectm_set_texture_search_paths(projectm_handle instance,
                                                       const char** texture_search_paths,
                                                       size_t count);


PROJECTM_EXPORT void projectm_set_frame_time(projectm_handle instance, double seconds_since_first_frame);


PROJECTM_EXPORT double projectm_get_last_frame_time(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_beat_sensitivity(projectm_handle instance, float sensitivity);


PROJECTM_EXPORT float projectm_get_beat_sensitivity(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_hard_cut_duration(projectm_handle instance, double seconds);


PROJECTM_EXPORT double projectm_get_hard_cut_duration(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_hard_cut_enabled(projectm_handle instance, bool enabled);


PROJECTM_EXPORT bool projectm_get_hard_cut_enabled(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_hard_cut_sensitivity(projectm_handle instance, float sensitivity);


PROJECTM_EXPORT float projectm_get_hard_cut_sensitivity(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_soft_cut_duration(projectm_handle instance, double seconds);


PROJECTM_EXPORT double projectm_get_soft_cut_duration(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_preset_duration(projectm_handle instance, double seconds);


PROJECTM_EXPORT double projectm_get_preset_duration(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_mesh_size(projectm_handle instance, size_t width, size_t height);


PROJECTM_EXPORT void projectm_get_mesh_size(projectm_handle instance, size_t* width, size_t* height);


PROJECTM_EXPORT void projectm_set_fps(projectm_handle instance, int32_t fps);


PROJECTM_EXPORT int32_t projectm_get_fps(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_aspect_correction(projectm_handle instance, bool enabled);


PROJECTM_EXPORT bool projectm_get_aspect_correction(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_easter_egg(projectm_handle instance, float value);


PROJECTM_EXPORT float projectm_get_easter_egg(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_preset_locked(projectm_handle instance, bool lock);


PROJECTM_EXPORT bool projectm_get_preset_locked(projectm_handle instance);


PROJECTM_EXPORT void projectm_set_window_size(projectm_handle instance, size_t width, size_t height);


PROJECTM_EXPORT void projectm_get_window_size(projectm_handle instance, size_t* width, size_t* height);

#ifdef __cplusplus
}
#endif
