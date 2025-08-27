

#pragma once

#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_EXPORT void projectm_touch(projectm_handle instance, float x, float y,
                                    int pressure, projectm_touch_type touch_type);


PROJECTM_EXPORT void projectm_touch_drag(projectm_handle instance, float x, float y, int pressure);


PROJECTM_EXPORT void projectm_touch_destroy(projectm_handle instance, float x, float y);


PROJECTM_EXPORT void projectm_touch_destroy_all(projectm_handle instance);

#ifdef __cplusplus
}
#endif
