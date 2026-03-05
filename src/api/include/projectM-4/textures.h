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
 * @brief Loads a texture from raw, uncompressed pixel data (RGB or RGBA).
 *
 * The data buffer must contain at least width * height * channels bytes of pixel data in standard
 * OpenGL format (first row is the bottom of the image).
 *
 * This function can be called at any time to push or pre-load textures, or from within the
 * texture load event callback.
 *
 * @param instance The projectM instance handle.
 * @param texture_name The unqualified texture name (without wrap/filter prefixes). Case-insensitive.
 * @param data Pointer to raw pixel data. Must remain valid until this function returns.
 * @param width Width of the texture in pixels. Must be between 1 and 8192.
 * @param height Height of the texture in pixels. Must be between 1 and 8192.
 * @param channels Number of color channels (3 for RGB, 4 for RGBA).
 * @return true if the texture was loaded successfully, false on error or if a texture with this
 *         name already exists.
 * @since 4.2.0
 */
PROJECTM_EXPORT bool projectm_texture_load_raw_data(projectm_handle instance,
                                                     const char* texture_name,
                                                     const unsigned char* data,
                                                     unsigned int width,
                                                     unsigned int height,
                                                     unsigned int channels);

/**
 * @brief Loads a texture from an existing OpenGL texture ID.
 *
 * projectM will use the texture but will @b not take ownership of it. The application is
 * responsible for deleting the texture after projectM no longer references it. Use the
 * texture unload event callback to get notified when the texture is no longer needed.
 *
 * This function can be called at any time to push or pre-load textures, or from within the
 * texture load event callback.
 *
 * @param instance The projectM instance handle.
 * @param texture_name The unqualified texture name (without wrap/filter prefixes). Case-insensitive.
 * @param texture_id A valid OpenGL texture ID.
 * @param width Width of the texture in pixels. Must be between 1 and 8192.
 * @param height Height of the texture in pixels. Must be between 1 and 8192.
 * @return true if the texture was loaded successfully, false on error or if a texture with this
 *         name already exists.
 * @since 4.2.0
 */
PROJECTM_EXPORT bool projectm_texture_load_gl_texture(projectm_handle instance,
                                                       const char* texture_name,
                                                       unsigned int texture_id,
                                                       unsigned int width,
                                                       unsigned int height);

/**
 * @brief Loads a texture from compressed/encoded image file data.
 *
 * Pass the raw contents of a supported image file (e.g. the bytes read from a JPG, PNG, BMP,
 * TGA, DXT or DDS file) and projectM will decode it internally using stb_image.
 * GIF and PSD are partially supported.
 *
 * This function can be called at any time to push or pre-load textures, or from within the
 * texture load event callback.
 *
 * @param instance The projectM instance handle.
 * @param texture_name The unqualified texture name (without wrap/filter prefixes). Case-insensitive.
 * @param data Pointer to the image file contents. Must remain valid until this function returns.
 * @param data_length Length of the image data buffer in bytes.
 * @return true if the texture was loaded and decoded successfully, false on error or if a texture
 *         with this name already exists.
 * @since 4.2.0
 */
PROJECTM_EXPORT bool projectm_texture_load_compressed_image(projectm_handle instance,
                                                             const char* texture_name,
                                                             const unsigned char* data,
                                                             size_t data_length);

/**
 * @brief Unloads a previously loaded external texture.
 *
 * If the texture is currently in use by an active preset, it will remain in memory until the
 * preset is unloaded. When manually unloading a texture passed by OpenGL ID, the application
 * should wait one or more preset switches before deleting the actual GL texture to avoid
 * rendering issues.
 *
 * @param instance The projectM instance handle.
 * @param texture_name The unqualified texture name as used in the load call. Case-insensitive.
 * @return true if the texture was found and scheduled for removal, false if no texture with the
 *         given name was found.
 * @since 4.2.0
 */
PROJECTM_EXPORT bool projectm_texture_unload(projectm_handle instance,
                                              const char* texture_name);

/**
 * @brief Callback function that is executed when projectM needs to load a texture.
 *
 * This callback allows applications to provide textures from sources other than the filesystem,
 * such as archives, network resources, procedural generators, or pre-loaded data.
 *
 * When called, the application should load the requested texture by calling one of the texture
 * loading functions (@a projectm_texture_load_raw_data, @a projectm_texture_load_gl_texture, or
 * @a projectm_texture_load_compressed_image) and return true. If the application cannot provide
 * the requested texture, it should return false and projectM will fall back to loading from
 * the filesystem.
 *
 * @note The texture_name pointer is only valid inside the callback. Make a copy if it needs to
 *       be retained for later use.
 * @note This callback is always invoked from the same thread that calls projectM rendering
 *       functions. No additional synchronization is required.
 *
 * @param instance The projectM instance handle. Can be used to call texture loading functions.
 * @param texture_name The name of the texture being requested, as used in the preset.
 * @param user_data A user-defined data pointer that was provided when registering the callback.
 * @return true if the application loaded the texture, false to fall back to filesystem loading.
 * @since 4.2.0
 */
typedef bool (*projectm_texture_load_event)(projectm_handle instance,
                                            const char* texture_name,
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
 * @brief Callback function that will be called after projectM unloads a texture.
 *
 * This callback informs the application that a texture with the given name was removed from
 * projectM's texture manager. This is particularly useful when passing a GL texture ID to
 * projectM, as it lets the application know when the texture is no longer needed and can be
 * safely deleted (or at least no longer requires regular updating).
 *
 * @note This callback is not fired for textures loaded from the filesystem by projectM itself.
 *
 * @param texture_name The name of the texture that was unloaded.
 * @param user_data A user-defined data pointer that was provided when registering the callback.
 * @since 4.2.0
 */
typedef void (*projectm_texture_unload_event)(const char* texture_name,
                                              void* user_data);

/**
 * @brief Sets a callback function that will be called when projectM unloads a texture.
 *
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
