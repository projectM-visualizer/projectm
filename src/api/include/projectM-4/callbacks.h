

#pragma once

#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*projectm_preset_switch_requested_event)(bool is_hard_cut, void* user_data);


typedef void (*projectm_preset_switch_failed_event)(const char* preset_filename,
                                                    const char* message, void* user_data);



PROJECTM_EXPORT void projectm_set_preset_switch_requested_event_callback(projectm_handle instance,
                                                                         projectm_preset_switch_requested_event callback,
                                                                         void* user_data);


PROJECTM_EXPORT void projectm_set_preset_switch_failed_event_callback(projectm_handle instance,
                                                                      projectm_preset_switch_failed_event callback,
                                                                      void* user_data);

#ifdef __cplusplus
}
#endif
