

#pragma once

#include "projectM-4/projectM_export.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct projectm;
typedef struct projectm* projectm_handle;


typedef enum
{
    PROJECTM_MONO = 1,
    PROJECTM_STEREO = 2
} projectm_channels;


typedef enum
{
    PROJECTM_CHANNEL_L = 0,
    PROJECTM_CHANNEL_0 = 0,
    PROJECTM_CHANNEL_R = 1,
    PROJECTM_CHANNEL_1 = 1
} projectm_pcm_channel;


typedef enum
{
    PROJECTM_TOUCH_TYPE_RANDOM,
    PROJECTM_TOUCH_TYPE_CIRCLE,
    PROJECTM_TOUCH_TYPE_RADIAL_BLOB,
    PROJECTM_TOUCH_TYPE_BLOB2,
    PROJECTM_TOUCH_TYPE_BLOB3,
    PROJECTM_TOUCH_TYPE_DERIVATIVE_LINE,
    PROJECTM_TOUCH_TYPE_BLOB5,
    PROJECTM_TOUCH_TYPE_LINE,
    PROJECTM_TOUCH_TYPE_DOUBLE_LINE
} projectm_touch_type;

#ifdef __cplusplus
}
#endif
