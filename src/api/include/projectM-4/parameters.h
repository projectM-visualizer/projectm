/**
 * @file parameters.h
 * @copyright 2003-2025 projectM Team
 * @brief Functions to set and retrieve all sorts of projectM parameters and setting.
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
 * @brief Sets the texture search paths.
 *
 * Calling this method will clear and reload all textures, including the main rendering texture.
 * Can cause a small delay/lag in rendering. Only use if texture paths were changed.
 *
 * @param instance The projectM instance handle.
 * @param texture_search_paths A list of texture search paths.
 * @param count The number of paths in the list.
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_texture_search_paths(projectm_handle instance,
                                                       const char** texture_search_paths,
                                                       size_t count);

/**
 * @brief Sets a user-specified frame time in fractional seconds.
 *
 * Setting this to any value equal to or larger than zero will make projectM use this time value for
 * animations instead of the system clock. Any value less than zero will use the system time instead,
 * which is the default behavior.
 *
 * This method can be used to render visualizations at non-realtime frame rates, e.g. encoding a video
 * as fast as projectM can render frames.
 *
 * While switching back and forth between system and user time values is possible, it will cause
 * visual artifacts in the rendering as the time value will make large jumps between frames. Thus,
 * it is recommended to stay with one type of timing value.
 *
 * If using this feature, it is further recommended to set the time to 0.0 on the first frame.
 *
 * @param instance The projectM instance handle.
 * @param seconds_since_first_frame Any value >= 0 to use user-specified timestamps, values < 0 will use the system clock. Default: -1.0
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_set_frame_time(projectm_handle instance, double seconds_since_first_frame);

/**
 * @brief Returns the fractional seconds time value used rendering the last frame.
 * @note This will not return the value set with projectm_set_frame_time, but the actual time
 *       used to render the last frame. If a user-specified frame time was set, this value is
 *       returned. Otherwise, the frame time measured via the system clock will be returned.
 * @param instance The projectM instance handle.
 * @return Time elapsed since projectM was started, or the value of the user-specified time value used
 *         to render the last frame.
 * @since 4.2.0
 */
PROJECTM_EXPORT double projectm_get_last_frame_time(projectm_handle instance);

/**
 * @brief Sets the beat sensitivity.
 *
 * The beat sensitivity to be used.
 *
 * @param instance The projectM instance handle.
 * @param sensitivity The sensitivity setting. Default: 1.0
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_beat_sensitivity(projectm_handle instance, float sensitivity);

/**
 * @brief Returns the beat sensitivity.
 * @param instance The projectM instance handle.
 * @return The current sensitivity setting.
 * @since 4.0.0
 */
PROJECTM_EXPORT float projectm_get_beat_sensitivity(projectm_handle instance);

/**
 * @brief Sets the minimum display time before a hard cut can happen.
 *
 * <p>Hard cuts are beat-sensitive preset transitions, immediately changing from
 * one preset to the next without a smooth blending period.</p>
 *
 * <p>Set this to a higher value than preset duration to disable hard cuts.</p>
 *
 * @param instance The projectM instance handle.
 * @param seconds Minimum number of seconds the preset will be displayed before a hard cut. Default: 20.0
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_hard_cut_duration(projectm_handle instance, double seconds);

/**
 * @brief Returns the minimum display time before a hard cut can happen.
 * @param instance The projectM instance handle.
 * @return The minimum number of seconds the preset will be displayed before a hard cut.
 * @since 4.0.0
 */
PROJECTM_EXPORT double projectm_get_hard_cut_duration(projectm_handle instance);

/**
 * @brief Enables or disables hard cuts.
 *
 * Even if enabled, the hard cut duration must be set to a value lower than the preset duration
 * to work properly.
 *
 * @param instance The projectM instance handle.
 * @param enabled True to enable hard cuts, false to disable. Default: false
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_hard_cut_enabled(projectm_handle instance, bool enabled);

/**
 * @brief Returns whether hard cuts are enabled or not.
 * @param instance The projectM instance handle.
 * @return True if hard cuts are enabled, false otherwise.
 * @since 4.0.0
 */
PROJECTM_EXPORT bool projectm_get_hard_cut_enabled(projectm_handle instance);

/**
 * @brief Sets the hard cut volume sensitivity.
 *
 * The beat detection volume difference that must be surpassed to trigger a hard cut.
 *
 * @param instance The projectM instance handle.
 * @param sensitivity The volume threshold that triggers a hard cut if surpassed. Default: 2.0
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_hard_cut_sensitivity(projectm_handle instance, float sensitivity);

/**
 * @brief Returns the current hard cut sensitivity.
 * @param instance The projectM instance handle.
 * @return The current hard cut sensitivity.
 * @since 4.0.0
 */
PROJECTM_EXPORT float projectm_get_hard_cut_sensitivity(projectm_handle instance);

/**
 * @brief Sets the time in seconds for a soft transition between two presets.
 *
 * During a soft cut, both presets are rendered and slowly transitioned from one
 * to the other.
 *
 * @param instance The projectM instance handle.
 * @param seconds Time in seconds it takes to smoothly transition from one preset to another. Default: 3.0
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_soft_cut_duration(projectm_handle instance, double seconds);

/**
 * @brief Returns the time in seconds for a soft transition between two presets.
 * @param instance The projectM instance handle.
 * @return Time in seconds it takes to smoothly transition from one preset to another.
 * @since 4.0.0
 */
PROJECTM_EXPORT double projectm_get_soft_cut_duration(projectm_handle instance);

/**
 * @brief Sets the preset display duration before switching to the next using a soft cut.
 *
 * This can be considered as the maximum time a preset is displayed. If this time is reached,
 * a smooth cut will be initiated. A hard cut, if any, will always happen before this time.
 *
 * @param instance The projectM instance handle.
 * @param seconds The number of seconds a preset will be displayed before the next is shown. Default: 30.0
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_preset_duration(projectm_handle instance, double seconds);

/**
 * @brief Returns the preset display duration before switching to the next using a soft cut.
 *
 * This can be considered as the maximum time a preset is displayed. If this time is reached,
 * a smooth cut will be initiated. A hard cut, if any, will always happen before this time.
 *
 * @param instance The projectM instance handle.
 * @return The currently set preset display duration in seconds.
 * @since 4.0.0
 */
PROJECTM_EXPORT double projectm_get_preset_duration(projectm_handle instance);

/**
 * @brief Sets the per-pixel equation mesh size in units.
 *
 * Will internally be clamped to [8,300] in each axis. If any dimension is set to an odd value, it will be incremented by 1
 * so only multiples of two are used.
 *
 * @param instance The projectM instance handle.
 * @param width The new width of the mesh. Default: 32
 * @param height The new height of the mesh. Default: 24
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_mesh_size(projectm_handle instance, size_t width, size_t height);

/**
 * @brief Returns the per-pixel equation mesh size in units.
 * @param instance The projectM instance handle.
 * @param width The width of the mesh.
 * @param height The height of the mesh.
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_get_mesh_size(projectm_handle instance, size_t* width, size_t* height);

/**
 * @brief Applies a sub-texel offset for main texture lookups in the warp shader.
 *
 * Original Milkdrop uses 0.5 here, but it doesn't seem to be required in OpenGL as this value
 * introduces a slight warp drift to the top left. As this may be vendor-specific, this value can
 * be configured externally to fix any possible drift.
 *
 * @param instance The projectM instance handle.
 * @param offset_X The offset in texels in the horizontal direction. Milkdrop uses 0.5. Default: 0.0
 * @param offset_y The offset in texels in the vertical direction. Milkdrop uses 0.5. Default: 0.0
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_set_texel_offset(projectm_handle instance, float offset_X, float offset_y);

/**
 * @brief Retrieves the current sub-texel offsets for main texture lookups in the warp shader.
 *
 * Original Milkdrop uses 0.5 here, but it doesn't seem to be required in OpenGL as this value
 * introduces a slight warp drift to the top left. As this may be vendor-specific, this value can
 * be configured externally to fix any possible drift.
 *
 * @param instance The projectM instance handle.
 * @param offset_X A valid pointer to a float variable that will receive the currently set horizontal texel offset.
 * @param offset_y A valid pointer to a float variable that will receive the currently set vertical texel offset.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_get_texel_offset(projectm_handle instance, float* offset_X, float* offset_y);

/**
 * @brief Sets the current/average frames per second.
 *
 * Applications running projectM should UpdateMeshSize this value regularly and set it to the calculated
 * (and possibly averaged) FPS value the output rendered with. The value is passed on to presets,
 * which may choose to use it for calculations. It is not used in any other way by the library.
 *
 * @param instance The projectM instance handle.
 * @param fps The current FPS value projectM is running with. Default: 35
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_fps(projectm_handle instance, int32_t fps);

/**
 * @brief Returns the current/average frames per second.
 *
 * This value needs to be set by the application. If it wasn't set, a default value of 60 is used.
 *
 * @param instance The projectM instance handle.
 * @return The current/average frames per second.
 * @since 4.0.0
 */
PROJECTM_EXPORT int32_t projectm_get_fps(projectm_handle instance);

/**
 * @brief Enabled or disables aspect ratio correction in presets that support it.
 *
 * This sets a flag presets can use to aspect-correct rendered shapes, which otherwise would
 * be distorted if the viewport isn't exactly square.
 *
 * @param instance The projectM instance handle.
 * @param enabled True to enable aspect correction, false to disable it. Default: true
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_aspect_correction(projectm_handle instance, bool enabled);

/**
 * @brief Returns whether aspect ratio correction is enabled or not.
 * @param instance The projectM instance handle.
 * @return True if aspect ratio correction is enabled, false otherwise.
 * @since 4.0.0
 */
PROJECTM_EXPORT bool projectm_get_aspect_correction(projectm_handle instance);

/**
 * @brief Sets the "easter egg" value.
 *
 * <p>This doesn't enable any fancy feature, it only influences the randomized display time of presets. It's
 * passed as the "sigma" value of the gaussian random number generator used to determine the maximum display time,
 * effectively multiplying the generated number of seconds by this amount.</p>
 *
 * <p>See function sampledPresetDuration() of the TimeKeeper class on how it is used.</p>
 *
 * @param instance The projectM instance handle.
 * @param value The new "easter egg" value. Must be greater than zero, otherwise a default sigma value of 1.0 will be used. Default: 1.0
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_easter_egg(projectm_handle instance, float value);

/**
 * @brief Returns the current "easter egg" value.
 * @param instance The projectM instance handle.
 * @return The current "easter egg" value.
 * @since 4.0.0
 */
PROJECTM_EXPORT float projectm_get_easter_egg(projectm_handle instance);

/**
 * @brief Locks or unlocks the current preset.
 *
 * Locking effectively disables automatic preset transitions, both hard and soft cuts. Programmatic
 * preset switches will still be executed.
 *
 * @param instance The projectM instance handle.
 * @param lock True to lock the current preset, false to enable automatic transitions. Default: false
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_preset_locked(projectm_handle instance, bool lock);

/**
 * @brief Returns whether the current preset is locked or not.
 * @param instance The projectM instance handle.
 * @return True if the preset lock is enabled, false otherwise.
 * @since 4.0.0
 */
PROJECTM_EXPORT bool projectm_get_preset_locked(projectm_handle instance);

/**
 * @brief Sets the current viewport size in pixels.
 *
 * This method must be called with a non-zero size for each dimension for projectM to render
 * anything. The viewport size should be the same size or smaller as the surface being rendered to.
 *
 * The internal textures will only be replaced and resized when rendering the next frame and only
 * if any of the two dimensions actually changed.
 *
 * Calling this function on every frame with the same size doesn't have any negative
 * effects as it only updates the size with the same values.
 *
 * @param instance The projectM instance handle.
 * @param width New viewport width in pixels. Default: 0
 * @param height New viewport height in pixels. Default: 0
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_set_window_size(projectm_handle instance, size_t width, size_t height);

/**
 * @brief Returns the current viewport size in pixels.
 * @param instance The projectM instance handle.
 * @param width Valid pointer to a size_t variable that will receive the window width value.
 * @param height Valid pointer to a size_t variable that will receive the window height value.
 * @since 4.0.0
 */
PROJECTM_EXPORT void projectm_get_window_size(projectm_handle instance, size_t* width, size_t* height);

/**
 * @brief Sets whether newly loaded presets should start with a clean (black) canvas.
 *
 * By default, when switching presets, the last frame of the previous preset is copied into
 * the new preset's main texture, creating a visual continuity. Setting this flag to true
 * will cause each new preset to start with a black canvas instead.
 *
 * This is useful for applications that want a clean start for each preset, avoiding the
 * "ghosting" effect from the previous preset.
 *
 * @param instance The projectM instance handle.
 * @param enabled True to start new presets with a clean canvas, false to copy the previous frame. Default: false
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_set_preset_start_clean(projectm_handle instance, bool enabled);

/**
 * @brief Returns whether newly loaded presets start with a clean canvas.
 * @param instance The projectM instance handle.
 * @return True if presets start with a clean canvas, false if the previous frame is copied.
 * @since 4.2.0
 */
PROJECTM_EXPORT bool projectm_get_preset_start_clean(projectm_handle instance);

#ifdef __cplusplus
} // extern "C"
#endif
