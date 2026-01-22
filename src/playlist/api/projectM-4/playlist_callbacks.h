/**
 * @file playlist_callbacks.h
 * @copyright 2003-2025 projectM Team
 * @brief Functions and prototypes for projectM playlist callbacks.
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

#include "projectM-4/playlist_types.h"

#include <stdbool.h>

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
 * @since 4.0.0
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
 * @since 4.0.0
 */
typedef void (*projectm_playlist_preset_switch_failed_event)(const char* preset_filename,
                                                             const char* message, void* user_data);

/**
 * @brief Callback function that is executed when the playlist wants to load a preset.
 *
 * This callback allows applications to handle preset loading themselves instead of
 * letting the playlist library load presets from the filesystem. This is useful for:
 * - Loading presets from archives (e.g., ZIP files)
 * - Loading presets from network sources (e.g., HTTP)
 * - Asynchronous preset loading
 * - Custom preset storage solutions
 *
 * When this callback is set and returns true, the playlist library will NOT attempt
 * to load the preset file itself. The application is responsible for calling
 * projectm_load_preset_file() or projectm_load_preset_data() with the preset content.
 *
 * If the callback returns false or is not set, the playlist library will use the
 * default behavior of loading the preset from the filesystem.
 *
 * @note The filename pointer is only valid inside the callback. Make a copy if it needs
 *       to be retained for later use.
 * @param index The playlist index of the preset to be loaded.
 * @param filename The preset filename/URL at this index. Can be used as a key or path.
 * @param hard_cut True if this should be a hard cut, false for a smooth transition.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
 *                  e.g. context information.
 * @return True if the application handled the preset loading, false to use default behavior.
 * @since 4.2.0
 */
typedef bool (*projectm_playlist_preset_load_event)(unsigned int index, const char* filename,
                                                    bool hard_cut, void* user_data);


/**
 * @brief Sets a callback function that will be called when a preset changes.
 *
 * Only one callback can be registered per playlist instance. To remove the callback, use NULL.
 *
 * @param instance The playlist manager instance.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 * @since 4.0.0
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
 * @since 4.0.0
 */
PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_preset_switch_failed_event_callback(projectm_playlist_handle instance,
                                                                                        projectm_playlist_preset_switch_failed_event callback,
                                                                                        void* user_data);

/**
 * @brief Sets a callback function that will be called when the playlist wants to load a preset.
 *
 * This allows applications to handle preset loading themselves, e.g., from archives,
 * network sources, or using custom storage. When set, this callback is called before
 * the playlist library attempts to load a preset file.
 *
 * Only one callback can be registered per playlist instance. To remove the callback, use NULL.
 *
 * @param instance The playlist manager instance.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 * @since 4.2.0
 */
PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_preset_load_event_callback(projectm_playlist_handle instance,
                                                                               projectm_playlist_preset_load_event callback,
                                                                               void* user_data);

#ifdef __cplusplus
} // extern "C"
#endif
