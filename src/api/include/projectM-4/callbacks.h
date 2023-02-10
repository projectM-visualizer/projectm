/**
 * @file callbacks.h
 * @copyright 2003-2023 projectM Team
 * @brief Functions and prototypes for projectM callbacks.
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

#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback function that is executed whenever projectM wants to switch to a new preset.
 *
 * @param is_hard_cut If true, the transition was triggered by a beat-driven event.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
*                   e.g. context information.
 */
typedef void (*projectm_preset_switch_requested_event)(bool is_hard_cut, void* user_data);

/**
 * @brief Callback function that is executed if a preset change failed.
 *
 * The message and filename pointers are only valid inside the callback. Make a copy if these values
 * need to be retained for later use.
 *
 * @param preset_filename The filename of the failed preset.
 * @param message The error message.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
 *                  e.g. context information.
 */
typedef void (*projectm_preset_switch_failed_event)(const char* preset_filename,
                                                    const char* message, void* user_data);


/**
 * @brief Sets a callback function that will be called when a preset change is requested.
 *
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * @param instance The projectM instance handle.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 */
PROJECTM_EXPORT void projectm_set_preset_switch_requested_event_callback(projectm_handle instance,
                                                                         projectm_preset_switch_requested_event callback,
                                                                         void* user_data);

/**
 * @brief Sets a callback function that will be called when a preset change failed.
 *
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * @param instance The projectM instance handle.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 */
PROJECTM_EXPORT void projectm_set_preset_switch_failed_event_callback(projectm_handle instance,
                                                                      projectm_preset_switch_failed_event callback,
                                                                      void* user_data);

#ifdef __cplusplus
} // extern "C"
#endif

