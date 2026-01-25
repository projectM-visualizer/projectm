/**
 * @file callbacks.h
 * @copyright 2003-2025 projectM Team
 * @brief Functions and prototypes for projectM callbacks.
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
 * @since 4.0.0
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
 * @since 4.0.0
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
 * @since 4.0.0
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
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_preset_switch_failed_event_callback(projectm_handle instance,
                                                                      projectm_preset_switch_failed_event callback,
                                                                      void* user_data);

/**
 * @brief Structure containing texture data returned by the texture load callback.
 *
 * Applications can provide texture data in one of two ways:
 * 1. Raw pixel data: Set data to a valid pointer, width/height to the dimensions,
 *    and channels to the number of color channels (3 for RGB, 4 for RGBA).
 * 2. Existing OpenGL texture: Set texture_id to a valid OpenGL texture ID.
 *
 * If both are provided, the texture_id takes precedence.
 * If neither is provided (data is NULL and texture_id is 0), projectM will
 * attempt to load the texture from the filesystem.
 *
 * @warning When providing a texture_id, projectM takes ownership of the OpenGL texture
 *          and will delete it (via glDeleteTextures) when it is no longer needed. Do not
 *          delete the texture yourself or reuse the texture ID after passing it here.
 *
 * @since 4.2.0
 */
typedef struct projectm_texture_load_data {
    const unsigned char* data; /**< Pointer to raw pixel data in standard OpenGL format (first row is bottom of image). Can be NULL. */
    unsigned int width;        /**< Width of the texture in pixels. Must be > 0 when providing data or texture_id. */
    unsigned int height;       /**< Height of the texture in pixels. Must be > 0 when providing data or texture_id. */
    unsigned int channels;     /**< Number of color channels (3 for RGB, 4 for RGBA). */
    unsigned int texture_id;   /**< An existing OpenGL texture ID to use. Set to 0 if not used. */
} projectm_texture_load_data;

/**
 * @brief Callback function that is executed when projectM needs to load a texture.
 *
 * This callback allows applications to provide textures from sources other than
 * the filesystem, such as:
 * - Loading textures from archives (e.g., ZIP files)
 * - Loading textures over the network
 * - Generating textures procedurally
 * - Providing pre-loaded textures or video frames
 *
 * When called, the application should populate the provided data structure with
 * either raw pixel data or an OpenGL texture ID. If the application cannot provide
 * the requested texture, it should leave the structure unchanged (data = NULL,
 * texture_id = 0) and projectM will fall back to loading from the filesystem.
 *
 * @note The texture_name pointer is only valid inside the callback. Make a copy if
 *       it needs to be retained for later use.
 * @note If providing raw pixel data, the data pointer must remain valid until
 *       projectM has finished processing it (i.e., until the callback returns).
 * @note This callback is always invoked from the same thread that calls projectM
 *       rendering functions. No additional synchronization is required.
 *
 * @param texture_name The name of the texture being requested, as used in the preset.
 * @param[out] data Pointer to a structure where the application should place texture data.
 * @param user_data A user-defined data pointer that was provided when registering the callback.
 * @since 4.2.0
 */
typedef void (*projectm_texture_load_event)(const char* texture_name,
                                            projectm_texture_load_data* data,
                                            void* user_data);

/**
 * @brief Sets a callback function that will be called when projectM needs to load a texture.
 *
 * This allows applications to provide textures from non-filesystem sources.
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * @param instance The projectM instance handle.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_set_texture_load_event_callback(projectm_handle instance,
                                                              projectm_texture_load_event callback,
                                                              void* user_data);

#ifdef __cplusplus
} // extern "C"
#endif
