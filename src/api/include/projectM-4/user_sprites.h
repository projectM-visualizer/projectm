/**
 * @file user_sprites.h
 * @copyright 2003-2025 projectM Team
 * @brief Types and enumerations used in the other API headers.
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
 * @brief Loads and displays a new sprite.
 *
 * Currently, these sprite types are supported:
 * <ul>
 * <li><tt>milkdrop</tt>: Original Milkdrop user sprite syntax. Pass the contents of an <tt>imgNN</tt> section in
 * the <tt>code</tt> argument.</li>
 * </ul>
 *
 * Unsupported types and loading errors will result in failure, and no sprite will be added or replaced.
 *
 * @important The same OpenGL context used to create the projectM instance @a must be active when calling this method!
 * @param instance The projectM instance handle.
 * @param type The case-insensitive type name of the sprite to be displayed. See description for supported values.
 * @param code The type-specific sprite code, e.g. the contents of an <tt>imgNN</tt> section from a Milkdrop user
 *             sprite INI file.
 * @return A non-zero identifier if the sprite was successfully created, or zero if the type was
 *         unrecognized or the code couldn't be parsed.
 * @since 4.2.0
 */
PROJECTM_EXPORT uint32_t projectm_sprite_create(projectm_handle instance,
                                                 const char* type,
                                                 const char* code);

/**
 * @brief Destroys a single sprite.
 *
 * If there is no active sprite with the given ID, this method is a no-op.
 *
 * @param instance The projectM instance handle.
 * @param sprite_id The ID of the sprite as returned by <tt>projectm_sprite_create()</tt>.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_sprite_destroy(projectm_handle instance, uint32_t sprite_id);

/**
 * @brief Destroys all active sprites.
 * @param instance The projectM instance handle.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_sprite_destroy_all(projectm_handle instance);

/**
 * @brief Returns the number of currently active sprites.
 *
 * @note Sprites may destroy themselves after a frame has been rendered, and projectM can also
 *       remove a sprite if a new one is added and the sprite limit was already reached.
 *       Keep this in mind when calling <tt>projectm_sprite_get_sprite_ids()</tt> - ideally,
 *       call <tt>projectm_sprite_get_sprite_count()</tt> @a immediately before allocating the
 *       ID list.
 * @param instance The projectM instance handle.
 * @return The current number of sprites being rendered.
 * @since 4.2.0
 */
PROJECTM_EXPORT uint32_t projectm_sprite_get_sprite_count(projectm_handle instance);

/**
 * @brief Returns the number of currently active sprites.
 *
 * Identifiers are ordered by sprite age, with the oldest sprite first and the newest last.
 *
 * @param instance The projectM instance handle.
 * @param sprite_ids A pointer to an already-allocated list which will receive the sprite IDs.
 *                   Call <tt>projectm_sprite_get_sprite_count()</tt> to get the required length
 *                   or allocate a list with the current sprite limit as its size and init all entries
 *                   to zero.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_sprite_get_sprite_ids(projectm_handle instance, uint32_t* sprite_ids);

/**
 * @brief Sets the limit of concurrently displayed sprites.
 *
 * Once the limit is exceeded, the oldest sprite will be destroyed in order to display a new one.
 *
 * @param instance The projectM instance handle.
 * @param max_sprites Maximum number of sprites to be displayed at once. Defaults to 16.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_sprite_set_max_sprites(projectm_handle instance,
                                                      uint32_t max_sprites);

/**
 * @brief Returns the currently set limit of concurrently displayed sprites.
 *
 * @param instance The projectM instance handle.
 * @return The current maximum number of sprites to be displayed at once.
 * @since 4.2.0
 */
PROJECTM_EXPORT uint32_t projectm_sprite_get_max_sprites(projectm_handle instance);

#ifdef __cplusplus
} // extern "C"
#endif
