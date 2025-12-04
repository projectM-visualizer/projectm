/**
 * @file types.h
 * @copyright 2003-2025 projectM Team
 * @brief Types and enumerations used in the other API headers.
 * @since 4.0.0
 *
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2024 projectM Team
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
 * @since 4.0.0
 */
typedef enum
{
    PROJECTM_MONO = 1,
    PROJECTM_STEREO = 2
} projectm_channels;

/**
 * Placeholder values that can be used to address channel indices in PCM data arrays.
 * @since 4.0.0
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
 * @since 4.0.0
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

/**
 * Log level constants for use with the logging API functions.
 * @since 4.2.0
 */
typedef enum
{
    PROJECTM_LOG_LEVEL_NOTSET = 0, //!< No specific log level, use default (INFO).
    PROJECTM_LOG_LEVEL_TRACE = 1,  //!< Verbose trace logging. Only enabled in debug builds by default.
    PROJECTM_LOG_LEVEL_DEBUG = 2,  //!< Development-related debug logging. Only enabled in debug builds by default.
    PROJECTM_LOG_LEVEL_INFO = 3,   //!< Informational messages.
    PROJECTM_LOG_LEVEL_WARN = 4,   //!< Warnings about non-critical issues.
    PROJECTM_LOG_LEVEL_ERROR = 5,  //!< Recoverable errors, e.g. shader compilation or I/O errors.
    PROJECTM_LOG_LEVEL_FATAL = 6   //!< Irrecoverable errors preventing projectM from working.
} projectm_log_level;

#ifdef __cplusplus
} // extern "C"
#endif
