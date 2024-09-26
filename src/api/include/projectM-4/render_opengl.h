/**
 * @file render_opengl.h
 * @copyright 2003-2023 projectM Team
 * @brief Functions to configure and render projectM visuals using OpenGL.
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
 * @brief Renders a single frame.
 *
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_opengl_render_frame(projectm_handle instance);

/**
 * @brief Renders a single frame into a user-defined framebuffer object.
 *
 * @param instance The projectM instance handle.
 * @param framebuffer_object_id The OpenGL FBO ID to render to.
 */
PROJECTM_EXPORT void projectm_opengl_render_frame_fbo(projectm_handle instance, uint32_t framebuffer_object_id);

/**
 * @brief Sets the viewport of the target framebuffer and the texture filter used to draw in the target framebuffer.
 *
 * Setting a width and a height of zero uses default logic.
 * Calling this function is cheap, i.e. no resources are recreated.
 *
 * @param instance The projectM instance handle.
 * @param targetX New viewport x in pixels.
 * @param targetY New viewport y in pixels.
 * @param targetWidth New viewport width in pixels.
 * @param targetHeight New viewport height in pixels.
 * @param targetTextureFilter New texture filter, nearest or linear.
 */
PROJECTM_EXPORT void projectm_set_target_options(projectm_handle instance, uint32_t targetX, uint32_t targetY, uint32_t targetWidth, uint32_t targetHeight, projectm_texture_filter targetTextureFilter);

/**
 * @brief Returns the current target framebuffer viewport in pixels and the current texture filter.
 * @param instance The projectM instance handle.
 * @param targetX Valid pointer to a uint32_t variable that will receive the viewport x value.
 * @param targetY Valid pointer to a uint32_t variable that will receive the viewport y value.
 * @param targetWidth Valid pointer to a uint32_t variable that will receive the viewport width value.
 * @param targetHeight Valid pointer to a uint32_t variable that will receive the viewport height value.
 * @param targetTextureFilter Valid pointer to a projectm_texture_filter variable that will receive the texture filter value.
 */
PROJECTM_EXPORT void projectm_get_target_options(projectm_handle instance, uint32_t* targetX, uint32_t* targetY, uint32_t* targetWidth, uint32_t* targetHeight, projectm_texture_filter* targetTextureFilter);

#ifdef __cplusplus
} // extern "C"
#endif
