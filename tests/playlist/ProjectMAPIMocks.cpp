/**
 * This file defines the few projectM API calls used in the playlist library. We're not interested
 * in their implementation in this test suite.
 */

#include <projectM-4/projectM.h>
#include <projectM-4/projectM_export.h>

PROJECTM_EXPORT void projectm_set_preset_switch_requested_event_callback(projectm_handle instance,
                                                         projectm_preset_switch_requested_event callback,
                                                         void* user_data)
{
}

PROJECTM_EXPORT void projectm_set_preset_switch_failed_event_callback(projectm_handle instance,
                                                      projectm_preset_switch_failed_event callback,
                                                      void* user_data)
{
}

PROJECTM_EXPORT void projectm_load_preset_file(projectm_handle instance, const char* filename,
                               bool smooth_transition)
{
}
