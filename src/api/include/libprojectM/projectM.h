/*
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2021 projectM Team
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

#include "libprojectM/projectM_export.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct projectm;                          //!< Opaque projectM instance type.
typedef struct projectm* projectm_handle; //!< A pointer to the opaque projectM instance.

/**
 * @brief projectM instance settings.
 *
 * <p>Use this struct to provide settings for projectM, for example if your application handles projectM configuration
 * internally instead of using the default configuration file.</p>
 *
 * <p>Always allocate the struct using the projectm_alloc_settings() and free it with the projectm_free_settings()
 * function.</p>
 *
 * <p>To allocate memory for char* members, always use projectm_alloc_string(). If any pointer is not NULL,
 * projectm_free_settings() will automatically call projectm_free_string() on it. If you free it on your own, remember
 * to reset the pointer to NULL after doing so!</p>
 */
typedef struct PROJECTM_EXPORT projectm_settings_s {
    int mesh_x;                 //!< Per-pixel mesh X resolution.
    int mesh_y;                 //!< Per-pixel mesh Y resolution.
    int fps;                    //!< Target rendering frames per second.
    int texture_size;           //!< Size of the render texture. Must be a power of 2.
    int window_width;           //!< Width of the rendering viewport.
    int window_height;          //!< Height of the rendering viewport.
    char* texture_path;         //!< Additional path with texture files for use in presets.
    char* data_path;            //!< Path to data files like default textures and presets.
    double preset_duration;     //!< Display duration for each preset in seconds.
    double soft_cut_duration;   //!< Blend-over duration between two presets in seconds.
    double hard_cut_duration;   //!< Minimum time in seconds a preset is displayed before a hard cut can happen.
    bool hard_cut_enabled;      //!< Set to true to enable fast beat-driven preset switches.
    float hard_cut_sensitivity; //!< Beat sensitivity value that must be surpassed for a hard cut.
    float beat_sensitivity;     //!< Beat sensitivity. Standard sensitivity is 1.0.
    bool aspect_correction;     //!< Use aspect ration correction in presets that support it.
    float easter_egg;           //!< Used as the "sigma" value for a gaussian RNG to randomize preset duration. Unused on Windows.
} projectm_settings;

/**
 * For specifying audio data format.
 */
typedef enum
{
    PROJECTM_MONO = 1,
    PROJECTM_STEREO = 2
} projectm_channels;

/**
 * Placeholder values that can be used to address channel indices in PCM data arrays.
 */
typedef enum
{
    PROJECTM_CHANNEL_L = 0, //!< Left audio channel.
    PROJECTM_CHANNEL_0 = 0, //!< Left audio channel.
    PROJECTM_CHANNEL_R = 1, //!< Right audio channel.
    PROJECTM_CHANNEL_1 = 1  //!< Right audio channel.
} projectm_pcm_channel;

/**
 * Waveform render types used in the touch start method.
 */
typedef enum
{
    PROJECTM_TOUCH_TYPE_RANDOM,          //!< Random waveform type.
    PROJECTM_TOUCH_TYPE_CIRCLE,          //!< Draws a circular waveform.
    PROJECTM_TOUCH_TYPE_RADIAL_BLOB,     //!< Draws a radial blob waveform.
    PROJECTM_TOUCH_TYPE_BLOB2,           //!< Draws a blob-style waveform.
    PROJECTM_TOUCH_TYPE_BLOB3,           //!< Draws another blob-style waveform.
    PROJECTM_TOUCH_TYPE_DERIVATIVE_LINE, //!< Draws a derivative-line waveform.
    PROJECTM_TOUCH_TYPE_BLOB5,           //!< Draws a five-blob waveform.
    PROJECTM_TOUCH_TYPE_LINE,            //!< Draws a single-line waveform.
    PROJECTM_TOUCH_TYPE_DOUBLE_LINE      //!< Draws a double-line waveform.
} projectm_touch_type;

/**
 * @brief Allocates memory for a string and returns the pointer.
 *
 * To free the allocated memory, call projectm_free_string(). Do not use free()!
 *
 * @return A pointer to a zero-initialized memory area.
 */
PROJECTM_EXPORT char* projectm_alloc_string(unsigned int length);

/**
 * @brief Frees the memory of an allocated string.
 *
 * <p>Frees the memory allocated by a call to projectm_alloc_string() or any
 * (const) char* pointers returned by a projectM API call.</p>
 *
 * <p>Do not use free() to delete the pointer!</p>
 *
 * @param settings A pointer returned by projectm_alloc_string().
 */
PROJECTM_EXPORT void projectm_free_string(const char* str);

/**
 * @brief Allocates memory for a projectm_settings struct and returns the pointer.
 *
 * <p>This will not allocate memory for the char* members in the struct. These will be set to NULL initially.
 * To store a string in these members, use projectm_alloc_string() to allocate the required memory. Do not use
 * malloc()!</p>
 *
 * <p>To free the allocated memory, call projectm_free_settings(). Do not use free()!</p>
 *
 * @return A pointer to a zero-initialized projectm_settings struct.
 */
PROJECTM_EXPORT projectm_settings* projectm_alloc_settings();

/**
 * @brief Frees the memory of an allocated projectm_settings structure.
 *
 * <p>Frees the memory allocated by a call to projectm_alloc_settings() or any
 * projectm_settings pointer returned by an API call. Any non-null char pointers
 * will also be free'd using projectm_free_string().</p>
 *
 * <p>Do not use free() to delete the pointer!</p>
 *
 * @param settings A pointer returned by projectm_alloc_settings().
 */
PROJECTM_EXPORT void projectm_free_settings(const projectm_settings* settings);

/**
 * @brief Callback function that is executed whenever projectM wants to switch to a new preset.
 *
 * @param is_hard_cut If true, the transition was triggered by a beat-driven event.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
*                   e.g. context information.
 */
typedef void (*projectm_preset_switch_requested_event)(bool is_hard_cut, void* user_data);

/**
 * @brief Callback function that is executed if a preset change failed.
 *
 * The message and filename pointers are only valid inside the callback. Make a copy if these values
 * need to be retained for later use.
 *
 * @param preset_filename The filename of the failed preset.
 * @param message The error message.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
 *                  e.g. context information.
 */
typedef void (*projectm_preset_switch_failed_event)(const char* preset_filename,
                                                    const char* message, void* user_data);


/**
 * @brief Creates a new projectM instance, reading settings from the given file.
 * @param setting_file_path A path to the settings file to read the configuration from.
 *                          If NULL or an empty path are provided, default settings will be used.
 * @return A projectM handle for the newly created instance that must be used in subsequent API calls.
 *         NULL if the instance could not be created successfully.
 */
PROJECTM_EXPORT projectm_handle projectm_create(const char* setting_file_path);

/**
 * @brief Creates a new projectM instance with given settings.
 * @param settings A pointer to a projectm_settings_t with the settings to be used by the new instance.
 *                 If this pointer is NULL, default settings will be used.
 * @return A projectM handle for the newly created instance that must be used in subsequent API calls.
 *         NULL if the instance could not be created successfully.
 */
PROJECTM_EXPORT projectm_handle projectm_create_settings(const projectm_settings* settings);

/**
 * @brief Destroys the given instance and frees the resources.
 *
 * After destroying the handle, it must not be used for any other calls to the API.
 *
 * @param instance A handle returned by projectm_create() or projectm_create_settings().
 */
PROJECTM_EXPORT void projectm_destroy(projectm_handle instance);

/**
 * @brief Loads a preset from the given filename/URL.
 *
 * Ideally, the filename should be given as a standard local path. projectM also supports loading
 * "file://" URLs. Additionally, the special filename "idle://" can be used to load the default
 * idle preset, displaying the "M" logo.
 *
 * Other URL schemas aren't supported and will cause a loading error.
 *
 * If the preset can't be loaded, no switch takes place and the current preset will continue to
 * be displayed. Note that if there's a transition in progress when calling this function, the
 * transition will be finished immediately, even if the new preset can't be loaded.
 *
 * @param instance The projectM instance handle.
 * @param filename The preset filename or URL to load.
 * @param smooth_transition If true, the new preset is smoothly blended over.
 */
PROJECTM_EXPORT void projectm_load_preset_file(projectm_handle instance, const char* filename,
                                               bool smooth_transition);

/**
 * @brief Loads a preset from the data pointer.
 *
 * Currently, the preset data is assumed to be in Milkdrop format.
 *
 * If the preset can't be loaded, no switch takes place and the current preset will continue to
 * be displayed. Note that if there's a transition in progress when calling this function, the
 * transition will be finished immediately, even if the new preset can't be loaded.
 *
 * @param instance The projectM instance handle.
 * @param data The preset contents to load.
 * @param smooth_transition If true, the new preset is smoothly blended over.
 */
PROJECTM_EXPORT void projectm_load_preset_data(projectm_handle instance, const char* data,
                                               bool smooth_transition);

/**
 * @brief Sets a callback function that will be called when a preset change is requested.
 *
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * @param instance The projectM instance handle.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 */
PROJECTM_EXPORT void projectm_set_preset_switch_requested_event_callback(projectm_handle instance,
                                                                         projectm_preset_switch_requested_event callback,
                                                                         void* user_data);

/**
 * @brief Sets a callback function that will be called when a preset change failed.
 *
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * @param instance The projectM instance handle.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 */
PROJECTM_EXPORT void projectm_set_preset_switch_failed_event_callback(projectm_handle instance,
                                                                      projectm_preset_switch_failed_event callback,
                                                                      void* user_data);

/**
 * @brief Reloads all textures.
 *
 * Also resets the OpenGL renderer without changing the viewport size. Useful if preset paths were changed.
 *
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_reset_textures(projectm_handle instance);

/**
 * @brief Renders a single frame.
 *
 * @note Separate two-pass frame rendering is currently not supported by the C API as it is rarely used
 *       and also depends on the loaded preset.
 *
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_render_frame(projectm_handle instance);

/**
 * @brief Enables render-to-texture.
 *
 * Useful if projectM output will be part of a more complex OpenGL scene. The size of the texture is determined by the
 * given viewport size and the dimensions should be a power of 2.
 *
 * @param instance The projectM instance handle.
 * @return A GLuint value with the texture ID projectM will render to.
 */
PROJECTM_EXPORT unsigned int projectm_init_render_to_texture(projectm_handle instance);

/**
 * @brief Returns the size of the internal render texture.
 * @param instance The projectM instance handle.
 * @return The size of the internal rendering texture.
 */
PROJECTM_EXPORT size_t projectm_get_texture_size(projectm_handle instance);

/**
 * @brief Changes the size of the internal render texture.
 * @note This will recreate the internal renderer.
 * @param instance The projectM instance handle.
 * @param size The new size of the render texture. Must be a power of 2.
 */
PROJECTM_EXPORT void projectm_set_texture_size(projectm_handle instance, size_t size);

/**
 * @brief Sets the beat sensitivity.
 *
 * The beat sensitivity to be used.
 *
 * @param instance The projectM instance handle.
 * @param sensitivity The sensitivity setting.
 */
PROJECTM_EXPORT void projectm_set_beat_sensitivity(projectm_handle instance, float sensitivity);

/**
 * @brief Returns the beat sensitivity.
 * @param instance The projectM instance handle.
 * @return The current sensitivity setting.
 */
PROJECTM_EXPORT float projectm_get_beat_sensitivity(projectm_handle instance);

/**
 * @brief Returns the minimum display time before a hard cut can happen.
 * @param instance The projectM instance handle.
 * @return The minimum number of seconds the preset will be displayed before a hard cut.
 */
PROJECTM_EXPORT double projectm_get_hard_cut_duration(projectm_handle instance);

/**
 * @brief Sets the minimum display time before a hard cut can happen.
 *
 * <p>Hard cuts are beat-sensitive preset transitions, immediately changing from
 * one preset to the next without a smooth blending period.</p>
 *
 * <p>Set this to a higher value than preset duration to disable hard cuts.</p>
 *
 * @param instance The projectM instance handle.
 * @param seconds Minimum number of seconds the preset will be displayed before a hard cut.
 */
PROJECTM_EXPORT void projectm_set_hard_cut_duration(projectm_handle instance, double seconds);

/**
 * @brief Returns whether hard cuts are enabled or not.
 * @param instance The projectM instance handle.
 * @return True if hard cuts are enabled, false otherwise.
 */
PROJECTM_EXPORT bool projectm_get_hard_cut_enabled(projectm_handle instance);

/**
 * @brief Enables or disables hard cuts.
 *
 * Even if enabled, the hard cut duration must be set to a value lower than the preset duration
 * to work properly.
 *
 * @param instance The projectM instance handle.
 * @param enabled True to enable hard cuts, false to disable.
 */
PROJECTM_EXPORT void projectm_set_hard_cut_enabled(projectm_handle instance, bool enabled);

/**
 * @brief Returns the current hard cut sensitivity.
 * @param instance The projectM instance handle.
 * @return The current hard cut sensitivity.
 */
PROJECTM_EXPORT float projectm_get_hard_cut_sensitivity(projectm_handle instance);

/**
 * @brief Sets the hard cut volume sensitivity.
 *
 * The beat detection volume difference that must be surpassed to trigger a hard cut.
 *
 * @param instance The projectM instance handle.
 * @param sensitivity The volume threshold that triggers a hard cut if surpassed.
 */
PROJECTM_EXPORT void projectm_set_hard_cut_sensitivity(projectm_handle instance, float sensitivity);

/**
 * @brief Returns the time in seconds for a soft transition between two presets.
 * @param instance The projectM instance handle.
 * @return Time in seconds it takes to smoothly transition from one preset to another.
 */
PROJECTM_EXPORT double projectm_get_soft_cut_duration(projectm_handle instance);

/**
 * @brief Sets the time in seconds for a soft transition between two presets.
 *
 * During a soft cut, both presets are rendered and slowly transitioned from one
 * to the other.
 *
 * @param instance The projectM instance handle.
 * @param seconds Time in seconds it takes to smoothly transition from one preset to another.
 */
PROJECTM_EXPORT void projectm_set_soft_cut_duration(projectm_handle instance, double seconds);

/**
 * @brief Returns the preset display duration before switching to the next using a soft cut.
 *
 * This can be considered as the maximum time a preset is displayed. If this time is reached,
 * a smooth cut will be initiated. A hard cut, if any, will always happen before this time.
 *
 * @param instance The projectM instance handle.
 * @return The currently set preset display duration in seconds.
 */
PROJECTM_EXPORT double projectm_get_preset_duration(projectm_handle instance);

/**
 * @brief Sets the preset display duration before switching to the next using a soft cut.
 *
 * This can be considered as the maximum time a preset is displayed. If this time is reached,
 * a smooth cut will be initiated. A hard cut, if any, will always happen before this time.
 *
 * @param instance The projectM instance handle.
 * @param seconds The number of seconds a preset will be displayed before the next is shown.
 */
PROJECTM_EXPORT void projectm_set_preset_duration(projectm_handle instance, double seconds);

/**
 * @brief Returns the per-pixel equation mesh size in units.
 * @param instance The projectM instance handle.
 * @param width The width of the mesh.
 * @param height The height of the mesh.
 */
PROJECTM_EXPORT void projectm_get_mesh_size(projectm_handle instance, size_t* width, size_t* height);

/**
 * @brief Sets the per-pixel equation mesh size in units.
 * @note This will recreate the renderer.
 * @param instance The projectM instance handle.
 * @param width The new width of the mesh.
 * @param height The new height of the mesh.
 */
PROJECTM_EXPORT void projectm_set_mesh_size(projectm_handle instance, size_t width, size_t height);

/**
 * @brief Returns the current/average frames per second.
 *
 * This value needs to be set by the application. If it wasn't set, a default value of 60 is used.
 *
 * @param instance The projectM instance handle.
 * @return The current/average frames per second.
 */
PROJECTM_EXPORT int32_t projectm_get_fps(projectm_handle instance);

/**
 * @brief Sets the current/average frames per second.
 *
 * Applications running projectM should update this value regularly and set it to the calculated
 * (and possibly averaged) FPS value the output rendered with. The value is passed on to presets,
 * which may choose to use it for calculations. It is not used in any other way by the library.
 *
 * @param instance The projectM instance handle.
 * @param fps The current FPS value projectM is running with.
 */
PROJECTM_EXPORT void projectm_set_fps(projectm_handle instance, int32_t fps);

/**
 * @brief Returns the search path for additional textures.
 * @param instance The projectM instance handle.
 * @return The path used to search for additional textures.
 */
PROJECTM_EXPORT const char* projectm_get_texture_path(projectm_handle instance);

/**
 * @brief Returns the path projectM uses to search for additional data.
 * @param instance The projectM instance handle.
 * @return The data dir path.
 */
PROJECTM_EXPORT const char* projectm_get_data_path(projectm_handle instance);

/**
 * @brief Enabled or disables aspect ratio correction in presets that support it.
 *
 * This sets a flag presets can use to aspect-correct rendered shapes, which otherwise would
 * be distorted if the viewport isn't exactly square.
 *
 * @param instance The projectM instance handle.
 * @param enabled True to enable aspect correction, false to disable it.
 */
PROJECTM_EXPORT void projectm_set_aspect_correction(projectm_handle instance, bool enabled);

/**
 * @brief Returns whether aspect ratio correction is enabled or not.
 * @param instance The projectM instance handle.
 * @return True if aspect ratio correction is enabled, false otherwise.
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
 * @param value The new "easter egg" value.
 */
PROJECTM_EXPORT void projectm_set_easter_egg(projectm_handle instance, float value);

/**
 * @brief Returns the current "easter egg" value.
 * @param instance The projectM instance handle.
 * @return The current "easter egg" value.
 */
PROJECTM_EXPORT float projectm_get_easter_egg(projectm_handle instance);

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

/**
 * @brief Returns a structure with the current projectM settings.
 * @param instance The projectM instance handle.
 * @return A struct with all currently used settings.
 */
PROJECTM_EXPORT projectm_settings* projectm_get_settings(projectm_handle instance);

/**
 * @brief Saves the given settings struct into a file.
 *
 * The file can be loaded during projectM initialization. This is useful if the application needs to
 * keep settings separate from the global system/user configuration.
 *
 * @param config_file The filename to store the settings in.
 * @param settings The settings struct to store.
 */
PROJECTM_EXPORT void projectm_write_config(const char* config_file, const projectm_settings* settings);

/**
 * @brief Returns whether the current preset is locked or not.
 * @param instance The projectM instance handle.
 * @return True if the preset lock is enabled, false otherwise.
 */
PROJECTM_EXPORT bool projectm_get_preset_locked(projectm_handle instance);

/**
 * @brief Locks or unlocks the current preset.
 *
 * Locking effectively disables automatic preset transitions, both hard and soft cuts. Programmatic
 * preset switches will still be executed.
 *
 * @param instance The projectM instance handle.
 * @param lock True to lock the current preset, false to enable automatic transitions.
 */
PROJECTM_EXPORT void projectm_set_preset_locked(projectm_handle instance, bool lock);

/**
 * @brief Returns the current viewport size in pixels.
 * @param instance The projectM instance handle.
 * @param width Valid pointer to a size_t variable that will receive the window width value.
 * @param height Valid pointer to a size_t variable that will receive the window height value.
 */
PROJECTM_EXPORT void projectm_get_window_size(projectm_handle instance, size_t* width, size_t* height);

/**
 * @brief Sets the current viewport size in pixels.
 *
 * Calling this function will reset the OpenGL renderer.
 *
 * @param instance The projectM instance handle.
 * @param width New viewport width in pixels.
 * @param height New viewport height in pixels.
 */
PROJECTM_EXPORT void projectm_set_window_size(projectm_handle instance, size_t width, size_t height);

/**
 * @brief Returns the maximum number of audio samples that can be stored.
 *
 * Each PCM data update should not exceed this number of samples. If more samples
 * are added, only this number of samples is stored and the remainder discarded.
 *
 * @return The number of audio samples that are stored, per channel.
 */
PROJECTM_EXPORT unsigned int projectm_pcm_get_max_samples();

/**
 * @brief Adds 32-bit floating-point audio samples.
 *
 * This function is used to add new audio data to projectM's internal audio buffer. It is internally converted
 * to 2-channel float data, duplicating the channel.
 *
 * If stereo, the channel order in samples is LRLRLR.
 *
 * @param instance The projectM instance handle.
 * @param samples An array of PCM samples.
 * Each sample is expected to be within the range -1 to 1.
 * @param count The number of audio samples in a channel.
 * @param channels If the buffer is mono or stereo.
 * Can be PROJECTM_MONO or PROJECTM_STEREO.
 */
PROJECTM_EXPORT void projectm_pcm_add_float(projectm_handle instance, const float* samples,
                                            unsigned int count, projectm_channels channels);

/**
 * @brief Adds 16-bit integer audio samples.
 *
 * This function is used to add new audio data to projectM's internal audio buffer. It is internally converted
 * to 2-channel float data, duplicating the channel.
 *
 * If stereo, the channel order in samples is LRLRLR.
 *
 * @param instance The projectM instance handle.
 * @param samples An array of PCM samples.
 * @param count The number of audio samples in a channel.
 * @param channels If the buffer is mono or stereo.
 * Can be PROJECTM_MONO or PROJECTM_STEREO.
 */
PROJECTM_EXPORT void projectm_pcm_add_int16(projectm_handle instance, const int16_t* samples,
                                            unsigned int count, projectm_channels channels);

/**
 * @brief Adds 8-bit unsigned integer audio samples.
 *
 * This function is used to add new audio data to projectM's internal audio buffer. It is internally converted
 * to 2-channel float data, duplicating the channel.
 *
 * If stereo, the channel order in samples is LRLRLR.
 *
 * @param instance The projectM instance handle.
 * @param samples An array of PCM samples.
 * @param count The number of audio samples in a channel.
 * @param channels If the buffer is mono or stereo.
 * Can be PROJECTM_MONO or PROJECTM_STEREO.
 */
PROJECTM_EXPORT void projectm_pcm_add_uint8(projectm_handle instance, const uint8_t* samples,
                                            unsigned int count, projectm_channels channels);

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
