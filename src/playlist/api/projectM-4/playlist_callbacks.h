

#pragma once

#include "projectM-4/playlist_types.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*projectm_playlist_preset_switched_event)(bool is_hard_cut, unsigned int index,
                                                        void* user_data);


typedef void (*projectm_playlist_preset_switch_failed_event)(const char* preset_filename,
                                                             const char* message, void* user_data);



PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_preset_switched_event_callback(projectm_playlist_handle instance,
                                                                                   projectm_playlist_preset_switched_event callback,
                                                                                   void* user_data);


PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_preset_switch_failed_event_callback(projectm_playlist_handle instance,
                                                                                        projectm_playlist_preset_switch_failed_event callback,
                                                                                        void* user_data);

#ifdef __cplusplus
}
#endif
