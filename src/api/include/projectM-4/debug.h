/**
 * @file debug.h
 * @copyright 2003-2023 projectM Team
 * @brief Debug functions for both libprojectM and preset developers.
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
 * @brief Writes a .bmp main texture dump after rendering the next main texture, before shaders are applied.
 *
 * If no file name is given, the image is written to the current working directory
 * and will be named named "frame_texture_contents-YYYY-mm-dd-HH:MM:SS-frame.bmp".
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
 */
PROJECTM_EXPORT void projectm_write_debug_image_on_next_frame(projectm_handle instance, const char* output_file);

#ifdef __cplusplus
} // extern "C"
#endif
