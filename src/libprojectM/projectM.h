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
#include "libprojectM/event.h"

#ifdef __cplusplus
extern "C" {
#endif

struct projectm; //!< Opaque projectM instance type.
typedef projectm* projectm_handle; //!< A pointer to the opaque projectM instance.

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
struct PROJECTM_EXPORT projectm_settings
{
    int mesh_x; //!< Per-pixel mesh X resolution.
    int mesh_y; //!< Per-pixel mesh Y resolution.
    int fps; //!< Target rendering frames per second.
    int texture_size; //!< Size of the render texture. Must be a power of 2.
    int window_width; //!< Width of the rendering viewport.
    int window_height; //!< Height of the rendering viewport.
    char* preset_url; //!< Path to a preset playlist in XML format to be loaded. Use FLAG_DISABLE_PLAYLIST_LOAD to skip loading a playlist.
    char* title_font_url; //!< Path to the "title" font that is used to render the preset name.
    char* menu_font_url; //!< Path to the "menu" font that is used to render the built-in on-screen menu.
    char* data_dir; //!< Path to data files like default fonts and presets.
    double smooth_preset_duration; //!< Blend-over duration between two presets in seconds.
    double preset_duration; //!< Display duration for each preset in seconds.
    bool hardcut_enabled; //!< Set to true to enable fast beat-driven preset switches.
    int hardcut_duration; //!< Minimum time a preset is displayed before a hardcut can happen in seconds.
    float hardcut_sensitivity; //!< Beat sensitivity value that must be surpassed for a hardcut.
    float beat_sensitivity; //!< Beat sensitivity. Standard sensitivity is 1.0.
    bool aspect_correction; //!< Use aspect ration correction in presets that support it.
    float easter_egg; //!< Used as the "sigma" value for a gaussian RNG to randomize preset duration. Unused on Windows.
    bool shuffle_enabled; //!< Enable playlist shuffle, selecting a random preset on each switch instead of the next in list.
    bool soft_cut_ratings_enabled; //!< If true, use soft cut ratings on soft cuts and hard cut ratings on hard cuts. If false, the hard cut rating is always used.
};

/**
 * Flags that influence projectM instance creation.
 */
enum projectm_flags
{
    PROJECTM_FLAG_NONE = 0, //!< No flags.
    PROJECTM_FLAG_DISABLE_PLAYLIST_LOAD = 1 << 0 //!< Set this flag to disable loading a preset playlist on startup.
};

/**
 * Rating types supported by projectM. Used to control preset selection for different types
 * of transitions (hard/soft).
 */
enum projectm_preset_rating_type
{
    PROJECTM_HARD_CUT_RATING_TYPE, //!< Rating for hard cuts.
    PROJECTM_SOFT_CUT_RATING_TYPE //!< Rating for soft cuts.
};

/**
 * Placeholder values that can be used to address channel indices in PCM data arrays.
 */
enum projectm_pcm_channel
{
    PROJECTM_CHANNEL_L = 0, //!< Left audio channel.
    PROJECTM_CHANNEL_0 = 0, //!< Left audio channel.
    PROJECTM_CHANNEL_R = 1, //!< Right audio channel.
    PROJECTM_CHANNEL_1 = 1 //!< Right audio channel.
};

/**
 * Waveform render types used in the touch start method.
 */
enum projectm_touch_type
{
    PROJECTM_TOUCH_TYPE_RANDOM, //!< Random waveform type.
    PROJECTM_TOUCH_TYPE_CIRCLE, //!< Draws a circular waveform.
    PROJECTM_TOUCH_TYPE_RADIAL_BLOB, //!< Draws a radial blob waveform.
    PROJECTM_TOUCH_TYPE_BLOB2,//!< Draws a blob-style waveform.
    PROJECTM_TOUCH_TYPE_BLOB3, //!< Draws another blob-style waveform.
    PROJECTM_TOUCH_TYPE_DERIVATIVE_LINE, //!< Draws a derivative-line waveform.
    PROJECTM_TOUCH_TYPE_BLOB5, //!< Draws a five-blob waveform.
    PROJECTM_TOUCH_TYPE_LINE, //!< Draws a single-line waveform.
    PROJECTM_TOUCH_TYPE_DOUBLE_LINE //!< Draws a double-line waveform.
};

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
 * (const) char* pointers returned by an API call.</p>
 *
 * <p>Do not use free() to delete the pointer!</p>
 *
 * @param settings A pointer returned by projectm_alloc_string().
 */
PROJECTM_EXPORT void projectm_free_string(const char* str);

/**
 * @brief Allocates memory for a projectm_settings struct and returns the pointer.
 *
 * To free the allocated memory, call projectm_free_settings(). Do not use free()!
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
 * @brief Callback function that is executed on each preset change.
 *
 * Can be used for example to update the application window title.
 *
 * @param is_hard_cut True if the preset was switched using a hard cut via beat detection.
 * @param index The playlist index of the new preset.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
 *                  e.g. context information.
 */
typedef void(* projectm_preset_switched_event)(bool is_hard_cut, unsigned int index, void* user_data);

/**
 * @brief Callback function that is executed is the shuffle setting has changed.
 * @param shuffle_enabled True if shuffle is enabled, false if it was disabled.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
 *                  e.g. context information.
 */
typedef void(* projectm_shuffle_enable_changed_event)(bool shuffle_enabled, void* user_data);

/**
 * @brief Callback function that is executed if a preset change failed.
 *
 * The message pointer is only valid inside the callback. Make a copy if it must be kept
 * for later use.
 *
 * @param is_hard_cut True if the preset was switched using a hard cut via beat detection.
 * @param index The playlist index of the new preset.
 * @param message The error message.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
 *                  e.g. context information.
 */
typedef void(* projectm_preset_switch_failed_event)(bool is_hard_cut, unsigned int index, const char* message,
                                                    void* user_data);

/**
 * @brief Callback function that is executed if a preset rating has been changed.
 *
 * Can be used for example to update the rating display in the host application.
 *
 * @param index The playlist index of the new preset.
 * @param rating The new rating value.
 * @param rating_type The rating type that has been changed.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
 *                  e.g. context information.
 */
typedef void(* projectm_preset_rating_changed_event)(unsigned int index, int rating,
                                                     projectm_preset_rating_type rating_type, void* user_data);



/**
 * @brief Creates a new projectM instance, reading settings from the given file.
 * @param setting_file_path A path to the settings file to read the configuration from.
 *                          If NULL or an empty path are provided, default settings will be used.
 * @param flags Any combination of values from the projectm_flags enumeration.
 * @return A projectM handle for the newly created instance that must be used in subsequent API calls.
 *         NULL if the instance could not be created successfully.
 */
PROJECTM_EXPORT projectm_handle projectm_create(const char* setting_file_path, int flags);

/**
 * @brief Creates a new projectM instance, reading settings from the given file.
 * @param settings A pointer to a projectm_settings_t with the settings to be used by the new instance.
 *                 If this pointer is NULL, default settings will be used.
 * @param flags Any combination of values from the projectm_flags enumeration.
 * @return A projectM handle for the newly created instance that must be used in subsequent API calls.
 *         NULL if the instance could not be created successfully.
 */
PROJECTM_EXPORT projectm_handle projectm_create_settings(const projectm_settings* settings, int flags);

/**
 * @brief Destroys the given instance and frees the resources.
 *
 * After destroying the handle, it must not be used for any other calls to the API.
 *
 * @param instance A handle returned by projectm_create() or projectm_create_settings().
 */
PROJECTM_EXPORT void projectm_destroy(projectm_handle instance);

/**
 * @brief Sets a callback function that will be called when a preset changes.
 *
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * @param instance The projectM instance handle.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context information.
 */
PROJECTM_EXPORT void projectm_set_preset_switched_event_callback(projectm_handle instance,
                                                                 projectm_preset_switched_event callback,
                                                                 void* user_data);

/**
 * @brief Sets a callback function that will be called when the shuffle setting changes.
 *
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * @param instance The projectM instance handle.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context information.
 */
PROJECTM_EXPORT void projectm_set_shuffle_enable_changed_event_callback(projectm_handle instance,
                                                                        projectm_shuffle_enable_changed_event callback,
                                                                        void* user_data);

/**
 * @brief Sets a callback function that will be called when a preset change failed.
 *
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * @param instance The projectM instance handle.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context information.
 */
PROJECTM_EXPORT void projectm_set_preset_switch_failed_event_callback(projectm_handle instance,
                                                                      projectm_preset_switch_failed_event callback,
                                                                      void* user_data);

/**
 * @brief Sets a callback function that will be called when a preset rating changed.
 *
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * @param instance The projectM instance handle.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context information.
 */
PROJECTM_EXPORT void projectm_set_preset_rating_changed_event_callback(projectm_handle instance,
                                                                       projectm_preset_rating_changed_event callback,
                                                                       void* user_data);

/**
 * @brief Reset the projectM OpenGL renderer.
 *
 * Required if anything invalidates the state of the current OpenGL context projectM is rendering to.
 * This is the case on window resizes, restoring a window from minimized state or other events that
 * required recreating of OpenGL objects.
 *
 * @param instance The projectM instance handle.
 * @param width The rendering viewport width.
 * @param height The rendering viewport height.
 */
PROJECTM_EXPORT void projectm_reset_gl(projectm_handle instance, int width, int height);


/**
 * @brief Reloads all textures.
 *
 * Also resets the OpenGL renderer without changing the viewport size. Useful if preset paths were changed.
 *
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_reset_textures(projectm_handle instance);

/**
 * @brief Sets the current title text and displays it.
 * @param instance The projectM instance handle.
 * @param title The title text to display.
 */
PROJECTM_EXPORT void projectm_set_title(projectm_handle instance, const char* title);

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
 * @brief Key handler that processes user input.
 *
 * This method can be used to send user input in the host application to projectM, for example
 * to switch presets, display the help and search menus or change settings like beat sensitivity.
 *
 * All actions executed by the key handler can also be run programmatically if the host application
 * is not able to redirect keyboard input to projectM.
 *
 * @param instance The projectM instance handle.
 * @param event The key event, valid are either PROJECTM_KEYUP or PROJECTM_KEYDOWN.
 * @param keycode The key code, mapped to a value of the projectMKeycode enumeration.
 * @param modifier The key modifier as a value from the projectMModifier.
 */
PROJECTM_EXPORT void projectm_key_handler(projectm_handle instance, projectMEvent event,
                                          projectMKeycode keycode, projectMModifier modifier);

/**
 * @brief Default key handler that processes user input.
 *
 * This method can be used to send user input in the host application to projectM, for example
 * to switch presets, display the help and search menus or change settings like beat sensitivity.
 *
 * All actions executed by the key handler can also be run programmatically if the host application
 * is not able to redirect keyboard input to projectM.
 *
 * @param instance The projectM instance handle.
 * @param event The key event, valid are either PROJECTM_KEYUP or PROJECTM_KEYDOWN.
 * @param keycode The key code, mapped to a value of the projectMKeycode enumeration.
 */
PROJECTM_EXPORT void projectm_default_key_handler(projectm_handle instance, projectMEvent event,
                                                  projectMKeycode keycode);
/**
 * @brief Changes the size of the internal render texture.
 * @note This will recreate the internal renderer.
 * @param instance The projectM instance handle.
 * @param size The new size of the render texture. Must be a power of 2.
 */
PROJECTM_EXPORT void projectm_set_texture_size(projectm_handle instance, int size);

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
PROJECTM_EXPORT void projectm_set_hardcut_duration(projectm_handle instance, double seconds);

/**
 * @brief Sets the preset display duration before switching to the next using a soft cut.
 * @param instance The projectM instance handle.
 * @param seconds The number of seconds a preset will be displayed before the next is shown.
 */
PROJECTM_EXPORT void projectm_set_preset_duration(projectm_handle instance, double seconds);

/**
 * @brief returns the per-pixel equation mesh size in units.
 * @param instance The projectM instance handle.
 * @param width The width of the mesh.
 * @param height The height of the mesh.
 */
PROJECTM_EXPORT void projectm_get_mesh_size(projectm_handle instance, int* width, int* height);

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
 * @brief Sets the help menu text.
 *
 * The help menu will be toggled if the key mapped to PROJECTM_K_F1 is pressed.
 *
 * @param instance The projectM instance handle.
 * @param help_text The help text to be displayed.
 */
PROJECTM_EXPORT void projectm_set_help_text(projectm_handle instance, const char* help_text);

/**
 * @brief Displays a short message in the center of the rendering area for a few seconds.
 *
 * <p>Useful to display song titles and changed audio settings. Used internally by projectM to display setting
 * changes like preset lock.</p>
 *
 * <p>Only one toast message is shown at a time. If this method is called while another message is shown, it
 * will be replaced immediately.</p>
 *
 * @param instance The projectM instance handle.
 * @param toast_message The message to display.
 */
PROJECTM_EXPORT void projectm_set_toast_message(projectm_handle instance, const char* toast_message);

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
 * @brief Selects a preset, but does not display it.
 * @param instance The projectM instance handle.
 * @param index The preset index to select.
 */
PROJECTM_EXPORT void projectm_select_preset_position(projectm_handle instance, unsigned int index);

/**
 * @brief Selects and displays the preset.
 * @param instance The projectM instance handle.
 * @param index the preset to display.
 * @param hard_cut If true, a hard cut is made, otherwise it will be blended smoothly.
 */
PROJECTM_EXPORT void projectm_select_preset(projectm_handle instance, unsigned int index, bool hard_cut);

/**
 * @brief Populates the on-screen preset menu.
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_populate_preset_menu(projectm_handle instance);

/**
 * @brief Removes a preset from the playlist.
 * @param instance The projectM instance handle.
 * @param index The  preset index to remove from the playlist.
 */
PROJECTM_EXPORT void projectm_remove_preset(projectm_handle instance, unsigned int index);

/**
 * @brief Clears the preset playlist.
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_clear_playlist(projectm_handle instance);

/**
 * @brief Locks or unlocks the current preset.
 *
 * Locking effectively disables automatic preset transitions, both hard and soft cuts. Programmatic
 * preset switches will still be executed.
 *
 * @param instance The projectM instance handle.
 * @param lock True to lock the current preset, false to enable automatic transitions.
 */
PROJECTM_EXPORT void projectm_lock_preset(projectm_handle instance, bool lock);

/**
 * @brief Returns whether the current preset is locked or not.
 * @param instance The projectM instance handle.
 * @return True if the preset lock is enabled, false otherwise.
 */
PROJECTM_EXPORT bool projectm_is_preset_locked(projectm_handle instance);

/**
 * @brief Returns whether the search text input mode is active or not.
 * @param instance The projectM instance handle.
 * @param no_minimum_length If set to true, will return true if at least one character has been typed, otherwise
 *                          a minimum length of three characters is required.
 * @return True if text input mode is active, false otherwise.
 */
PROJECTM_EXPORT bool projectm_is_text_input_active(projectm_handle instance, bool no_minimum_length);

/**
 * @brief Returns the playlist index for the given preset name.
 *
 * If the preset name is found multiple times, the first matching index will be returned.
 *
 * @param instance The projectM instance handle.
 * @param preset_name The preset name to search for.
 * @return The first found playlist index of the requested preset, or 0 if the preset wasn't found.
 */
PROJECTM_EXPORT unsigned int projectm_get_preset_index(projectm_handle instance, const char* preset_name);

/**
 * @brief Displays the preset with the given name.
 * @param instance The projectM instance handle.
 * @param preset_name The preset name to search for.
 * @param hard_cut If true, the preset will be shown immediately, if false a soft transition will be rendered.
 */
PROJECTM_EXPORT void projectm_select_preset_by_name(projectm_handle instance, const char* preset_name, bool hard_cut);

/**
 * @brief Sets the current preset search text.
 * @param instance The projectM instance handle.
 * @param search_text The search text used to search for presets in the current playlist.
 */
PROJECTM_EXPORT void projectm_set_search_text(projectm_handle instance, const char* search_text);

/**
 * @brief Deletes one character from the preset search text.
 *
 * This is equivalent to pressing DEL in a text box.
 *
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_delete_search_text(projectm_handle instance);

/**
 * @brief Deletes the whole search text.
 *
 * This will effectively leave preset search mode.
 *
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_reset_search_text(projectm_handle instance);

/**
 * @brief Returns the currently selected preset index.
 * @param instance The projectM instance handle.
 * @param index A valid pointer to an unsigned int that will receive the preset index.
 * @return True if a preset idnex was returned, false if no preset was selected, e.g. the playlist is empty.
 */
PROJECTM_EXPORT bool projectm_selected_preset_index(projectm_handle instance, unsigned int* index);

/**
 * @brief Adds a new preset at the end of the playlist.
 *
 * The rating list is one rating per value of the projectm_preset_rating_type enumeration, with each actual enum
 * value used as the index. If the rating list has the wrong length, the preset will not be added.
 *
 * @param instance The projectM instance handle.
 * @param preset_url The full path and filename of the preset.
 * @param preset_name The display name of the preset.
 * @param rating_list A list with ratings for the preset, one rating per rating type.
 * @param rating_list_length Length of the preset rating list.
 */
PROJECTM_EXPORT void projectm_add_preset_url(projectm_handle instance, const char* preset_url,
                                             const char* preset_name, int* rating_list,
                                             unsigned int rating_list_length);

/**
 * @brief Adds a new preset at the given position in the playlist.
 *
 * The rating list is one rating per value of the projectm_preset_rating_type enumeration, with each actual enum
 * value used as the index. If the rating list has the wrong length, the preset will not be added.
 *
 * @param instance The projectM instance handle.
 * @param index The playlist index to insert the preset at. Must be less than or equal to the length of
 *              the playlist.
 * @param preset_url The full path and filename of the preset.
 * @param preset_name The display name of the preset.
 * @param rating_list A list with ratings for the preset, one rating per rating type.
 * @param rating_list_length Length of the preset rating list.
 */
PROJECTM_EXPORT void projectm_insert_preset_url(projectm_handle instance, unsigned int index, const char* preset_url,
                                                const char* preset_name, int* rating_list,
                                                unsigned int rating_list_length);

/**
 * @brief Returns whether the currently selected preset has a valid position in the playlist.
 *
 * This function is useful to check if the currently displayed preset is still inside the bounds of
 * the current playlist, for example after the playlist was changed.
 *
 * @param instance The projectM instance handle.
 * @return True if the position is valid, false if outside bounds.
 */
PROJECTM_EXPORT bool projectm_preset_position_valid(projectm_handle instance);

/**
 * @brief Returns the path and filename of the preset at the requested playlist index.
 * @note Make sure the index is inside the playlist bounds!
 * @param instance The projectM instance handle.
 * @param index The playlist index to return the filename for.
 * @return The full path and filename of the preset at the given index.
 */
PROJECTM_EXPORT const char* projectm_get_preset_url(projectm_handle instance, unsigned int index);
/**
 * @brief Returns the display name of the preset at the requested playlist index.
 * @note Make sure the index is inside the playlist bounds!
 * @param instance The projectM instance handle.
 * @param index The playlist index to return the display name for.
 * @return The display name of the preset at the given index.
 */
PROJECTM_EXPORT const char* projectm_get_preset_name(projectm_handle instance, unsigned int index);

/**
 * @brief Changes the display name of the given preset in the playlist.
 * @param instance The projectM instance handle.
 * @param index the playlist item index to change.
 * @param name The new display name.
 */
PROJECTM_EXPORT void projectm_change_preset_name(projectm_handle instance, unsigned int index, const char* name);

/**
 * @brief Returns the rating for the given index and transition type.
 * @param instance The projectM instance handle.
 * @param index The playlist item to retrieve the rating from.
 * @param rating_type The rating type to retrieve, either hard or soft cut.
 * @return The rating value of the requested item and type.
 */
PROJECTM_EXPORT int projectm_get_preset_rating(projectm_handle instance, unsigned int index,
                                               projectm_preset_rating_type rating_type);

/**
 * @brief Changes the rating or a playlist item and type.
 * @param instance The projectM instance handle.
 * @param index the playlist item to change the rating of.
 * @param rating The new rating value.
 * @param rating_type The type of the rating, either hard or soft cut.
 */
PROJECTM_EXPORT void projectm_change_preset_rating(projectm_handle instance, unsigned int index, int rating,
                                                   projectm_preset_rating_type rating_type);

/**
 * @brief Returns the number of presets in the current playlist.
 * @param instance The projectM instance handle.
 * @return The number of presets in the currently loaded playlist.
 */
PROJECTM_EXPORT unsigned int projectm_get_playlist_size(projectm_handle instance);

/**
 * @brief Enables or disables preset playlist shuffling.
 * @param instance The projectM instance handle.
 * @param shuffle_enabled True to randomly select the next preset, false to skip to the next item in line.
 */
PROJECTM_EXPORT void projectm_set_shuffle_enabled(projectm_handle instance, bool shuffle_enabled);

/**
 * @brief Returns whether playlist shuffling is currently enabled or not.
 * @param instance The projectM instance handle.
 * @return True if shuffle is enabled, false if not.
 */
PROJECTM_EXPORT bool projectm_is_shuffle_enabled(projectm_handle instance);

/**
 * @brief Gets the index of the provided preset name in the current search result list.
 * @param instance The projectM instance handle.
 * @param name The name of the preset to return the index for.
 * @return The search result list index of the given preset name.
 */
PROJECTM_EXPORT unsigned int projectm_get_search_index(projectm_handle instance, const char* name);

/**
 * @brief Switches to the previous preset in the current playlist.
 *
 * This is unaffected by the shuffle mode and will always switch to the previous item.
 *
 * @param instance The projectM instance handle.
 * @param hard_cut True to immediately perform to the previous preset, false to do a soft transition.
 */
PROJECTM_EXPORT void projectm_select_previous(projectm_handle instance, bool hard_cut);

/**
 * @brief Switches to the next preset in the current playlist.
 *
 * This is unaffected by the shuffle mode and will always switch to the next item.
 *
 * @param instance The projectM instance handle.
 * @param hard_cut True to immediately perform to the next preset, false to do a soft transition.
 */
PROJECTM_EXPORT void projectm_select_next(projectm_handle instance, bool hard_cut);

/**
 * @brief Switches to a random preset in the current playlist.
 *
 * This is unaffected by the shuffle mode and will always switch to a random item.
 *
 * @param instance The projectM instance handle.
 * @param hard_cut True to immediately perform to a random preset, false to do a soft transition.
 */
PROJECTM_EXPORT void projectm_select_random(projectm_handle instance, bool hard_cut);

/**
 * @brief Returns the current viewport width in pixels.
 * @param instance The projectM instance handle.
 * @return The viewport width in pixels.
 */
PROJECTM_EXPORT int projectm_get_window_width(projectm_handle instance);

/**
 * @brief Returns the current viewport height in pixels.
 * @param instance The projectM instance handle.
 * @return The viewport height in pixels.
 */
PROJECTM_EXPORT int projectm_get_window_height(projectm_handle instance);

/**
 * @brief Returns whether the current preset was loaded successfully or not.
 * @param instance The projectM instance handle.
 * @return True if the preset was not loaded successfully, false if it is displayed correctly.
 */
PROJECTM_EXPORT bool projectm_get_error_loading_current_preset(projectm_handle instance);


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
 * @brief Adds 1-channel 32-bit floating-point audio samples.
 *
 * <p>This function is used to add new audio data to projectM's internal audio buffer. It is internally converted
 * to 2-channel float data, duplicating the channel.</p>
 *
 * <p>If possible, always use projectm_pcm_add_float_2ch_data() for best performance and quality.</p>
 *
 * @param instance The projectM instance handle.
 * @param pcm_data An array of PCM samples.
 * @param sample_count The number of audio samples in pcm_data.
 */
PROJECTM_EXPORT void projectm_pcm_add_float_1ch_data(projectm_handle instance, const float* pcm_data,
                                                     unsigned int sample_count);

/**
 * @brief Adds 2-channel 32-bit floating-point audio samples.
 *
 * <p>This function is used to add new audio data to projectM's internal audio buffer.</p>
 *
 * <p>This function represents projectM's internal audio data format. If possible, always use this function for best
 * performance and quality.</p>
 *
 * <p>Channel order in pcm_data is LRLRLR.</p>
 *
 * @param instance The projectM instance handle.
 * @param pcm_data An array of PCM samples, two floats per sample.
 * @param sample_count The number of audio samples in pcm_data. Half the actual size of pcm_data.
 */
PROJECTM_EXPORT void projectm_pcm_add_float_2ch_data(projectm_handle instance, const float* pcm_data,
                                                     unsigned int sample_count);

/**
 * @brief Adds 512 2-channel 16-bit integer audio samples.
 *
 * <p>This function is used to add new audio data to projectM's internal audio buffer. It is internally converted
 * to 2-channel float data.</p>
 *
 * <p>If possible, always use projectm_pcm_add_float_2ch_data() for best performance and quality.</p>
 *
 * <p>Channel indices are 0 for the left channel and 1 for the right channel. Use the projectm_pcm_channel
 * enum values instead of numerical indices for readability.</p>
 *
 * @param instance The projectM instance handle.
 * @param pcm_data The audio data.
 */
PROJECTM_EXPORT void projectm_pcm_add_16bit_2ch_512(projectm_handle instance, const short pcm_data[2][512]);

/**
 * @brief Adds 2-channel 16-bit integer audio samples.</p>
 *
 * <p>This function is used to add new audio data to projectM's internal audio buffer. It is internally converted
 * to 2-channel float data.</p>
 *
 * <p>If possible, always use projectm_pcm_add_float_2ch_data() for best performance and quality.</p>
 *
 * <p>Channel order in pcm_data is LRLRLR.</p>
 *
 * @param instance The projectM instance handle.
 * @param pcm_data The audio data.
 */
PROJECTM_EXPORT void projectm_pcm_add_16bit_2ch_data(projectm_handle instance, const short* pcm_data,
                                                     unsigned int sample_count);

/**
 * @brief Adds 1024 2-channel 8-bit integer audio samples.
 *
 * <p>This function is used to add new audio data to projectM's internal audio buffer. It is internally converted
 * to 2-channel float data.</p>
 *
 * <p>If possible, always use projectm_pcm_add_float_2ch_data() for best performance and quality.</p>
 *
 * <p>Channel indices are 0 for the left channel and 1 for the right channel. Use the projectm_pcm_channel
 * enum values instead of numerical indices for readability.</p>
 *
 * @param instance The projectM instance handle.
 * @param pcm_data The audio data.
 */
PROJECTM_EXPORT void projectm_pcm_add_8bit_2ch_1024(projectm_handle instance, const unsigned char pcm_data[2][1024]);

/**
 * @brief Adds 512 2-channel 8-bit integer audio samples.
 *
 * <p>This function is used to add new audio data to projectM's internal audio buffer. It is internally converted
 * to 2-channel float data.</p>
 *
 * <p>If possible, always use projectm_pcm_add_float_2ch_data() for best performance and quality.</p>
 *
 * <p>Channel indices are 0 for the left channel and 1 for the right channel. Use the projectm_pcm_channel
 * enum values instead of numerical indices for readability.</p>
 *
 * @param instance The projectM instance handle.
 * @param pcm_data The audio data.
 */
PROJECTM_EXPORT void projectm_pcm_add_8bit_2ch_512(projectm_handle instance, const unsigned char pcm_data[2][512]);

#ifdef __cplusplus
} // extern "C"
#endif