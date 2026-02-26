/**
 * @file textures.h
 * @copyright 2003-2025 projectM Team
 * @brief Functions, callbacks and prototypes for loading textures.
 * @since 4.2.0
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
 * Placeholder values that can be used to address channel indices in PCM data arrays.
 * @since 4.2.0
 */
typedef enum
{
    PROJECTM_TEXTURE_RAW = 0,       //!< Load raw 3/4 channel pixel data in standard OpenGL format (first row is bottom of image).
    PROJECTM_TEXTURE_GL_TEX_ID = 1, //!< Specify an existing OpenGL texture ID to use.
    /**
     * Pass a compressed/plain image file and let projectM decompress/decode it.
     * Supported formats are: JPG, PNG, BMP, TGA, DXT and DDS, while GIF and
     * PSD are partially supported.
     */
    PROJECTM_TEXTURE_COMPRESSED_FILE = 2
} projectm_texture_load_type;

/**
 * @brief Structure containing texture data returned by the texture load callback.
 *
 * Applications can provide texture data in one of three ways:
 * 1. Raw pixel data: Set data to a valid pointer, width/height to the dimensions,
 *    and channels to the number of color channels (3 for RGB, 4 for RGBA).
 * 2. Existing OpenGL texture: Set texture_id to a valid OpenGL texture ID.
 * 3. Data read from a supported image file type, see @a PROJECTM_TEXTURE_COMPRESSED_FILE for
 *    details. projectM will internally use stb_image to load the textures.
 *
 * If no image or texture ID is provided for the given type(data is NULL or texture_id is 0), or
 * image loading fails, projectM will attempt to load the texture from the filesystem as usual.
 *
 * After
 *
 * @warning When providing a texture_id, projectM takes ownership of the OpenGL texture
 *          and will delete it (via glDeleteTextures) when it is no longer needed. Do not
 *          delete the texture yourself or reuse the texture ID after passing it here.
 *
 * @since 4.2.0
 */
typedef struct projectm_texture_load_data {
    projectm_texture_load_type type; //!< The format of the passed-in texture.
    const unsigned char* data;       //!< Pointer to raw pixel or compressed image data.
    unsigned int width;              //!< Width of the texture in pixels. Must be > 0 when providing data or texture_id. */
    unsigned int height;             //!< Height of the texture in pixels. Must be > 0 when providing data or texture_id. */
    unsigned int channels;           //!< Number of color channels (3 for RGB, 4 for RGBA). */
    unsigned int texture_id;         //!< An existing OpenGL texture ID to use. Only used if type is @a PROJECTM_TEXTURE_GL_TEX_ID, ignored otherwise. */
} projectm_texture_load_data;

PROJECTM_EXPORT bool projectm_load_texture(projectm_handle instance,
                                           const char* texture_name,
                                           const projectm_texture_load_data* texture_data);

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

/**
 * @brief Sets a callback function that will be called after projectM unloaded a texture.
 *
 * This callback will inform the application that a texture with a given name was removed from
 * projectM's texture manager. This callback is only useful when passing a texture ID to projectM,
 * as this enabled the application to know when this specific texture is no longer needed and can
 * be deleted (or at least doesn't require regular updating anymore).
 * @param texture_name The name of the texture being requested, as used in the preset.
 * @param user_data A user-defined data pointer that was provided when registering the callback.
 * @since 4.2.0
 */
typedef void (*projectm_texture_unload_event)(const char* texture_name,
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
PROJECTM_EXPORT void projectm_set_texture_unload_event_callback(projectm_handle instance,
                                                                projectm_texture_unload_event callback,
                                                                void* user_data);

#ifdef __cplusplus
} // extern "C"
#endif
