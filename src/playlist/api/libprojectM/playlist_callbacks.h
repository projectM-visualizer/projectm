/**
 * @file playlist_callbacks.h
 * @copyright 2003-2023 projectM Team
 * @brief Functions and prototypes for projectM playlist callbacks.
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

#include "projectM-4/playlist_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback function that is executed on each preset change.
 *
 * Can be used for example to UpdateMeshSize the application window title. Applications must not
 * switch presets inside this callback, as it can lead to infinite recursion.
 *
 * @param is_hard_cut True if the preset was switched using a hard cut via beat detection.
 * @param index The playlist index of the new preset.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
 *                  e.g. context information.
 */
typedef void (*projectm_playlist_preset_switched_event)(bool is_hard_cut, unsigned int index,
                                                        void* user_data);

/**
 * @brief Callback function that is executed if a preset change failed too often.
 *
 * Similar to the projectM API function, but will only be called if the preset switch failed
 * multiple times in a row, e.g. due to missing files or broken presets. The application should
 * decide what action to take.
 *
 * @note Do NOT call any functions that switch presets inside the callback, at it might
 *       lead to infinite recursion and thus a stack overflow!
 * @note The message pointer is only valid inside the callback. Make a copy if it need to be
 *       retained for later use.
 * @param preset_filename The filename of the failed preset.
 * @param message The last error message.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
 *                  e.g. context information.
 */
typedef void (*projectm_playlist_preset_switch_failed_event)(const char* preset_filename,
                                                             const char* message, void* user_data);


/**
 * @brief Sets a callback function that will be called when a preset changes.
 *
 * Only one callback can be registered per playlist instance. To remove the callback, use NULL.
 *
 * @param instance The playlist manager instance.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 */
PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_preset_switched_event_callback(projectm_playlist_handle instance,
                                                                                   projectm_playlist_preset_switched_event callback,
                                                                                   void* user_data);

/**
 * @brief Sets a callback function that will be called when a preset change failed.
 *
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * If the application want to receive projectM's analogous callback directly, use the
 * projectm_set_preset_switch_failed_event_callback() function after calling
 * projectm_playlist_create() or projectm_playlist_connect(), respectively, as these will both
 * override the callback set in projectM.
 *
 * @param instance The playlist manager instance.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 */
PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_preset_switch_failed_event_callback(projectm_playlist_handle instance,
                                                                                        projectm_playlist_preset_switch_failed_event callback,
                                                                                        void* user_data);

#ifdef __cplusplus
} // extern "C"
#endif
