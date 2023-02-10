/**
 * @file touch.h
 * @copyright 2003-2023 projectM Team
 * @brief Touch-related functions to add random waveforms.
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
 * @brief Starts a touch event or moves an existing waveform.
 *
 * This will add or move waveforms in addition to the preset waveforms. If there is an existing waveform
 * at the given coordinates, it will be centered on the new coordinates. If there is no waveform, a new one
 * will be added.
 *
 * @param instance The projectM instance handle.
 * @param x The x coordinate of the touch event.
 * @param y The y coordinate of the touch event.
 * @param pressure  The amount of pressure applied in a range from 0.0 to 1.0.
 * @param touch_type The waveform type that will be rendered on touch.
 */
PROJECTM_EXPORT void projectm_touch(projectm_handle instance, float x, float y,
                                    int pressure, projectm_touch_type touch_type);

/**
 * @brief Centers any waveforms under the coordinates to simulate dragging.
 * @param instance The projectM instance handle.
 * @param x The x coordinate of the drag.
 * @param y the y coordinate of the drag.
 * @param pressure The amount of pressure applied in a range from 0.0 to 1.0.
 */
PROJECTM_EXPORT void projectm_touch_drag(projectm_handle instance, float x, float y, int pressure);

/**
 * @brief Removes any additional touch waveforms under the given coordinates.
 * @param instance The projectM instance handle.
 * @param x The last known x touch coordinate.
 * @param y The last known y touch coordinate.
 */
PROJECTM_EXPORT void projectm_touch_destroy(projectm_handle instance, float x, float y);

/**
 * @brief Removes all touch waveforms from the screen.
 *
 * Preset-defined waveforms will still be displayed.
 *
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_touch_destroy_all(projectm_handle instance);

#ifdef __cplusplus
} // extern "C"
#endif
