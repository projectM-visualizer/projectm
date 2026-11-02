/**
 * @file debug.h
 * @copyright 2003-2025 projectM Team
 * @brief Debug functions for both libprojectM and preset developers.
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
 * @brief Writes a .bmp main texture dump after rendering the next main texture, before shaders are applied.
 *
 * If no file name is given, the image is written to the current working directory
 * and will be named "frame_texture_contents-YYYY-mm-dd-HH:MM:SS-frame.bmp".
 *
 * Note this is the main texture contents, not the final rendering result. If the active preset
 * uses a composite shader, the dumped image will not have it applied. The main texture is what is
 * passed over to the next frame, the composite shader is only applied to the display framebuffer
 * after updating the main texture.
 *
 * To capture the actual output, dump the contents of the main framebuffer after calling
 * @a projectm_render_frame() on the application side.
 *
 * @param instance The projectM instance handle.
 * @param output_file The filename to write the dump to or NULL.
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_write_debug_image_on_next_frame(projectm_handle instance,
                                                              const char* output_file);

/**
 * @brief Structure containing the values of a single expression variable.
 *
 * This struct is used to return the values of a single, monitored expression variable. Depending
 * on how often a certain code block is being executed, this struct will contain one or more values.
 * For example, per_frame variables will only have one entry, while variables in custom waveform
 * per_point expressions will have as many entries as there were points rendered in the last frame.
 *
 * For user sprites, @a index is the sprite slot returned by  @a projectm_sprite_create().
 *
 * If a code block wasn't executed at all, e.g. if a preset doesn't use a certain effect, the count
 * will be zero and the values array a NULL pointer.
 *
 * The application must not change the contents of the returned structure, as it is considered
 * read-only.
 *
 * @note There is no last NULL element in the values array. Use value_count to iterate over the
 *       correct number of values!
 * @since 4.2.0
 */
struct projectm_expression_variable_values {
    uint32_t value_count; //!< The number of entries in @a values.
    uint32_t index;       //!< The custom shape/waveform or user sprite index. 0 for any other block.
    double** values;      //!< An array of double pointers, containing the values of the expression values as they were set to at the end of the last rendered frame.
};

/**
 * @brief Available expression blocks for watches.
 *
 * Currently, the five code blocks executed per frame are available for watching, plus the Milkdrop
 * user sprite per_frame code.
 *
 * Init blocks cannot be watched, as they are only executed once whenever a preset is loaded, and use
 * the same variables as their respective per_frame counterparts.
 *
 * @since 4.2.0
 */
typedef enum
{
    PROJECTM_EXPR_PER_FRAME,       //!< Variables in the "per_frame_" code block
    PROJECTM_EXPR_PER_POINT,       //!< Variables in the "per_point_" (AKA per vertex) code block
    PROJECTM_EXPR_SHAPE_PER_FRAME, //!< Variables in
    PROJECTM_EXPR_WAVE_PER_FRAME,
    PROJECTM_EXPR_WAVE_PER_POINT,
    PROJECTM_EXPR_MILKDROP_SPRITE
} projectm_expression_blocks;

/**
 * @brief Adds a new variable watch and returns a pointer to the data holder structure.
 *
 * This function will add a new watch for a single variable in the specified code block. If the code
 * block was valid, a pointer to a @a projectm_expression_variable_values will be returned. This
 * pointer will stay valid until this specific or all watches are removed, or the watched projectM
 * instance is being destroyed.
 *
 * Only the "active" preset is being watched. During a transition, the newly loaded preset is not
 * the active one - it will become active once the transition is finished and the previous preset
 * was unloaded. Hard cuts will change the active preset immediately.
 *
 * Variables which are not used/defined by a preset will always return 0.0. The value count will
 * still match the number of block invocations.
 *
 * User sprites will be watched based on their slot index, as returned by @a projectm_sprite_create().
 *
 * Adding watches only have a very small performance impact, with most of the work being done on
 * preset load. During runtime, the overhead basically consists of copying the watched double values
 * from the expression context into the watch structure.
 *
 * @note The returned structure is owned by projectM. Do not free the structure externally!
 * @param instance The projectM instance handle.
 * @param block The code block to add the watch for.
 * @param index The custom shape/waveform or user sprite index. Ignored for other block types.
 * @param variable_name The name of the variable to watch.
 * @return A pointer to a @a projectm_expression_variable_values structure which will contain the
 *         variable data, or NULL if the watch could not be added.
 * @since 4.2.0
 */
PROJECTM_EXPORT const projectm_expression_variable_values* projectm_expression_watch_add(projectm_handle instance,
                                                                                         projectm_expression_blocks block,
                                                                                         uint32_t index,
                                                                                         const char* variable_name);

/**
 * @brief Removes a previously added variable watch.
 *
 * @note The returned structure pointer will be invalid after calling this function, and is only
 *       supplied to match it to a stored value within the application for easier removal.
 * @param instance The projectM instance handle.
 * @param block The code block to remove the watch from.
 * @param index The custom shape/waveform or user sprite index. Ignored for other block types.
 * @param variable_name The name of the variable to remove the watch for.
 * @return The pointer to the previously registered @a projectm_expression_variable_values structure,
 *         if the watch was found, or NULL if the watch could not be found and wasn't removed.
 * @since 4.2.0
 */
PROJECTM_EXPORT const projectm_expression_variable_values* projectm_expression_watch_remove(projectm_handle instance,
                                                                                            projectm_expression_blocks block,
                                                                                            uint32_t index,
                                                                                            const char* variable_name);

/**
 * @brief Removes all active variable watches from this projectM instance.
 *
 * All previously returned pointers to @a projectm_expression_variable_values will no longer be valid
 * and thus must not be used/dereferenced after calling this function.
 *
 * @param instance The projectM instance handle.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_expression_watch_remove_all(projectm_handle instance);

#ifdef __cplusplus
} // extern "C"
#endif
