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

#include "libprojectM/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Writes a .bmp framedump after rendering the next main texture, before shaders are applied.
 *
 * The image is written to the current working directory and is named "frame_texture_contents-[date].bmp".
 *
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_write_debug_image_on_next_frame(projectm_handle instance);

#ifdef __cplusplus
} // extern "C"
#endif
