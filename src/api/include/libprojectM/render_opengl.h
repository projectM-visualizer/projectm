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

#include "libprojectM/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Renders a single frame.
 *
 * @note Separate two-pass frame rendering is currently not supported by the C API as it is rarely used
 *       and also depends on the loaded preset.
 *
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_opengl_render_frame(projectm_handle instance);

/**
 * @brief Enables render-to-texture.
 *
 * Useful if projectM output will be part of a more complex OpenGL scene. The size of the texture is determined by the
 * given viewport size and the dimensions should be a power of 2.
 *
 * @param instance The projectM instance handle.
 * @return A GLuint value with the texture ID projectM will render to.
 */
PROJECTM_EXPORT unsigned int projectm_opengl_init_render_to_texture(projectm_handle instance);

#ifdef __cplusplus
} // extern "C"
#endif
