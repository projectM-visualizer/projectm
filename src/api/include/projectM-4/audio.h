

#pragma once

#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_EXPORT unsigned int projectm_pcm_get_max_samples();


PROJECTM_EXPORT void projectm_pcm_add_float(projectm_handle instance, const float* samples,
                                            unsigned int count, projectm_channels channels);


PROJECTM_EXPORT void projectm_pcm_add_int16(projectm_handle instance, const int16_t* samples,
                                            unsigned int count, projectm_channels channels);


PROJECTM_EXPORT void projectm_pcm_add_uint8(projectm_handle instance, const uint8_t* samples,
                                            unsigned int count, projectm_channels channels);

#ifdef __cplusplus
}
#endif
