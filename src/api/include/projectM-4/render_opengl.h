/**
 * @file render_opengl.h
 * @copyright 2003-2025 projectM Team
 * @brief Functions to configure and render projectM visuals using OpenGL.
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
 * @brief Renders a single frame.
 *
 * @param instance The projectM instance handle.
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_opengl_render_frame(projectm_handle instance);

/**
 * @brief Renders a single frame into a user-defined framebuffer object.
 *
 * @param instance The projectM instance handle.
 * @param framebuffer_object_id The OpenGL FBO ID to render to.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_opengl_render_frame_fbo(projectm_handle instance, uint32_t framebuffer_object_id);

/**
 * @brief Burn-in the provided texture into the active preset(s) main texture.
 *
 * During transitions, the image is drawn onto both active presets.
 *
 * @param instance The projectM instance handle.
 * @param texture The OpenGL texture ID to draw onto the current preset.
 * @param left The left offset in screen coordinates.
 * @param top The top offset in screen coordinates.
 * @param width The width in screen coordinates. Negative values will flip the image horizontally.
 * @param height The height in screen coordinates. Negative values will flip the image vertically.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_opengl_burn_texture(projectm_handle instance, uint32_t texture, int left, int top, int width, int height);

#ifdef __cplusplus
} // extern "C"
#endif
