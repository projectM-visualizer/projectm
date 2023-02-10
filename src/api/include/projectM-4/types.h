/**
 * @file types.h
 * @copyright 2003-2023 projectM Team
 * @brief Types and enumerations used in the other API headers.
 *
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2023 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#pragma once

#include "projectM-4/projectM_export.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct projectm;                          //!< Opaque projectM instance type.
typedef struct projectm* projectm_handle; //!< A pointer to the opaque projectM instance.

/**
 * For specifying audio data format.
 */
typedef enum
{
    PROJECTM_MONO = 1,
    PROJECTM_STEREO = 2
} projectm_channels;

/**
 * Placeholder values that can be used to address channel indices in PCM data arrays.
 */
typedef enum
{
    PROJECTM_CHANNEL_L = 0, //!< Left audio channel.
    PROJECTM_CHANNEL_0 = 0, //!< Left audio channel.
    PROJECTM_CHANNEL_R = 1, //!< Right audio channel.
    PROJECTM_CHANNEL_1 = 1  //!< Right audio channel.
} projectm_pcm_channel;

/**
 * Waveform render types used in the touch start method.
 */
typedef enum
{
    PROJECTM_TOUCH_TYPE_RANDOM,          //!< Random waveform type.
    PROJECTM_TOUCH_TYPE_CIRCLE,          //!< Draws a circular waveform.
    PROJECTM_TOUCH_TYPE_RADIAL_BLOB,     //!< Draws a radial blob waveform.
    PROJECTM_TOUCH_TYPE_BLOB2,           //!< Draws a blob-style waveform.
    PROJECTM_TOUCH_TYPE_BLOB3,           //!< Draws another blob-style waveform.
    PROJECTM_TOUCH_TYPE_DERIVATIVE_LINE, //!< Draws a derivative-line waveform.
    PROJECTM_TOUCH_TYPE_BLOB5,           //!< Draws a five-blob waveform.
    PROJECTM_TOUCH_TYPE_LINE,            //!< Draws a single-line waveform.
    PROJECTM_TOUCH_TYPE_DOUBLE_LINE      //!< Draws a double-line waveform.
} projectm_touch_type;

#ifdef __cplusplus
} // extern "C"
#endif

