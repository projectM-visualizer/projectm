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

/**
 * @brief Draws lines so they keep their share of the picture at large render sizes.
 *
 * By default, waveforms, custom waves, shape outlines and motion vectors are drawn as 1 px wide
 * OpenGL lines, as in Milkdrop. The higher the render resolution, the smaller the share of the
 * picture they cover, so many presets get thin and dark on 4K screens.
 *
 * With a reference size, lines are drawn as quads instead, one instance per segment with miter
 * joins. They are 1 px wide at and below the reference size's area and widen by the square root of
 * the area ratio above it, so a line covers the same share of the picture at any size and aspect
 * ratio. Below the reference area, lines stay 1 px wide at full brightness.
 *
 * Thick lines are drawn like Milkdrop's: the thin line four times with small offsets, which scale
 * the same way. Waveform dots are drawn with a matching point size and their alpha is adjusted to
 * the drawn area. Milkdrop's waveform sample count rule and the spiro/hash wave fade, which both
 * depend on the render size, follow the reference size above its area.
 *
 * Edges are hard, like Milkdrop's lines, unless projectm_opengl_set_line_antialiasing() is enabled.
 * If the driver rejects the line shaders, lines are drawn as OpenGL lines.
 *
 * Milkdrop's authoring resolution, 1024x768, is a good reference size.
 *
 * @param instance The projectM instance handle.
 * @param width Width of the reference size in pixels, or 0 to draw 1 px OpenGL lines (the default).
 * @param height Height of the reference size in pixels, or 0 to draw 1 px OpenGL lines (the default).
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_opengl_set_line_reference_size(projectm_handle instance, uint32_t width, uint32_t height);

/**
 * @brief Returns the line reference size set with projectm_opengl_set_line_reference_size().
 *
 * @param instance The projectM instance handle.
 * @param width Receives the reference width, or 0 if lines are drawn as OpenGL lines.
 * @param height Receives the reference height, or 0 if lines are drawn as OpenGL lines.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_opengl_get_line_reference_size(projectm_handle instance, uint32_t* width, uint32_t* height);

/**
 * @brief Gives scaled lines a one-pixel anti-aliased edge.
 *
 * Off by default: lines have hard edges like Milkdrop's, which presets that feed their image back
 * are tuned for (a soft edge made such a preset 14% darker in testing). Has no effect while no line
 * reference size is set (see projectm_opengl_set_line_reference_size()), and none on dots.
 *
 * @param instance The projectM instance handle.
 * @param enabled True for anti-aliased edges.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_opengl_set_line_antialiasing(projectm_handle instance, bool enabled);

/**
 * @brief Returns whether scaled lines have an anti-aliased edge.
 *
 * @param instance The projectM instance handle.
 * @return True if anti-aliased edges are enabled.
 * @since 4.2.0
 */
PROJECTM_EXPORT bool projectm_opengl_get_line_antialiasing(projectm_handle instance);

#ifdef __cplusplus
} // extern "C"
#endif
