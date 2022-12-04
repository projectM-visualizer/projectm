#pragma once

#include "libprojectM/projectM.h"

#ifdef __cplusplus
extern "C" {
#endif

struct projectm_playlist;                                   //!< Opaque projectM playlist instance type.
typedef struct projectm_playlist* projectm_playlist_handle; //!< A pointer to the opaque projectM playlist instance.

/**
 * Sort predicate for playlist sorting.
 */
typedef enum
{
    SORT_PREDICATE_FULL_PATH,    //!< Sort by full path name
    SORT_PREDICATE_FILENAME_ONLY //!< Sort only by preset filename
} projectm_playlist_sort_predicate;


/**
 * Sort order for playlist sorting.
 */
typedef enum
{
    SORT_ORDER_ASCENDING, //!< Sort in alphabetically ascending order.
    SORT_ORDER_DESCENDING //!< Sort in alphabetically descending order.
} projectm_playlist_sort_order;


/**
 * @brief Frees a char pointer returned by any of the playlist API functions.
 *
 * Please only use this function with char pointers returned by the playlist library, and don't use
 * other projectM memory management functions with pointers returned by the playlist library.
 *
 * @param string A pointer to a string that should be freed.
 */
void projectm_playlist_free_string(char* string);

/**
 * @brief Frees a string array returned by any of the playlist API functions.
 *
 * Please only use this function with pointers returned by the playlist library, and don't use
 * other projectM memory management functions with pointers returned by the playlist library.
 *
 * @param array The pointer to the array of strings that should be freed.
 */
void projectm_playlist_free_string_array(char** array);

/**
 * @brief Callback function that is executed if a preset change failed too often.
 *
 * Similar to the projectM API function, but will only be called if the preset switch failed
 * multiple times in a row, e.g. due to missing files or broken presets. The application should
 * decide what action to take.
 *
 * @note Do NOT call any functions that switch presets inside the callback, at it might
 *       lead to infinite recursion and thus a stack overflow!
 * @note The message pointer is only valid inside the callback. Make a copy if it need to be
 *       retained for later use.
 * @param preset_filename The filename of the failed preset.
 * @param message The last error message.
 * @param user_data A user-defined data pointer that was provided when registering the callback,
 *                  e.g. context information.
 */
typedef void (*projectm_playlist_preset_switch_failed_event)(const char* preset_filename,
                                                             const char* message, void* user_data);


/**
 * @brief Creates a playlist manager for the given projectM instance
 *
 * Only one active playlist manager is supported per projectM instance. If multiple playlists use
 * the same projectM instance, only the last created playlist manager will receive preset change
 * callbacks from the projectM instance.
 *
 * To switch to another playlist, use the projectm_playlist_connect() method.
 *
 * @param instance The projectM instance to connect to. Can be a null pointer to leave the newly
 *                 created playlist instance unconnected.
 * @return An opaque pointer to the newly created playlist manager instance. Null if creation failed.
 */
projectm_playlist_handle projectm_playlist_create(projectm_handle projectm_instance);

/**
 * @brief Destroys a previously created playlist manager.
 *
 * If the playlist manager is currently connected to a projectM instance, it will be disconnected.
 *
 * @param instance The playlist manager instance to destroy.
 */
void projectm_playlist_destroy(projectm_playlist_handle instance);

/**
 * @brief Sets a callback function that will be called when a preset change failed.
 *
 * Only one callback can be registered per projectM instance. To remove the callback, use NULL.
 *
 * If the application want to receive projectM's analogous callback directly, use the
 * projectm_set_preset_switch_failed_event_callback() function after calling
 * projectm_playlist_create() or projectm_playlist_connect(), respectively, as these will both
 * override the callback set in projectM.
 *
 * @param instance The playlist manager instance.
 * @param callback A pointer to the callback function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 */
void projectm_playlist_set_preset_switch_failed_event_callback(projectm_playlist_handle instance,
                                                               projectm_playlist_preset_switch_failed_event callback,
                                                               void* user_data);

/**
 * @brief Connects the playlist manager to a projectM instance.
 *
 * Sets or removes the preset switch callbacks and stores the projectM instance handle for use with
 * manual preset switches via the playlist API.
 *
 * When switching between multiple playlist managers, first call this method on the last used
 * playlist manager with a null pointer for the projectM instance, then call this method with the
 * actual projectM instance on the playlist manager that should be activated. It is also safe to
 * call projectm_playlist_connect() with a null projectM handle on all playlist manager instances
 * before activating a single one with a valid, non-null projectM handle.
 *
 * @param instance The playlist manager instance.
 * @param projectm_instance The projectM instance to connect to. Can be a null pointer to remove
 *                          an existing binding and clear the projectM preset switch callback.
 */
void projectm_playlist_connect(projectm_playlist_handle instance, projectm_handle projectm_instance);

/**
 * @brief Returns the number of presets in the current playlist.
 * @param instance The playlist manager instance.
 * @return The number of presets in the current playlist.
 */
uint32_t projectm_playlist_size(projectm_playlist_handle instance);

/**
 * @brief Clears the playlist.
 * @param instance The playlist manager instance to clear.
 */
void projectm_playlist_clear(projectm_playlist_handle instance);

/**
 * @brief Returns a list of all preset files in the current playlist, in order.
 * @note Call projectm_playlist_free_string_array() when you're done using the list.
 * @note Ideally, don't rely on the playlist size to iterate over the filenames. Instead, look for
 *       the terminating null pointer to abort the loop.
 * @param instance The playlist manager instance.
 * @return A pointer to a list of char pointers, each containing a single preset. The last entry
 *         is denoted by a null pointer.
 */
char** projectm_playlist_items(projectm_playlist_handle instance);

/**
 * @brief Returns the name of a preset at the given index in the current playlist.
 * @note Call projectm_playlist_free_string() when you're done using the return value.
 * @note If you need to retrieve a major part of playlist filenames, use projectm_playlist_items()
 *       instead.
 * @param instance The playlist manager instance.
 * @param index The index to retrieve the filename for.
 * @return The filename of the requested preset, or NULL if the index was out of bounds or the
 *         playlist is empty.
 */
char* projectm_playlist_item(projectm_playlist_handle instance, uint32_t index);

/**
 * @brief Appends presets from the given path to the end of the current playlist.
 *
 * This method will scan the given path for files with a ".milk" extension and add these to the
 * playlist.
 *
 * Symbolic links are not followed.
 *
 * @param instance The playlist manager instance.
 * @param path A local filesystem path to scan for presets.
 * @param recurse_subdirs If true, subdirectories of the given path will also be scanned. If false,
 *                        only the exact path given is searched for presets.
 * @param allow_duplicates If true, files found will always be added. If false, only files are
 *                         added that do not already exist in the current playlist.
 * @return The number of files added. 0 may indicate issues scanning the path.
 */
uint32_t projectm_playlist_add_path(projectm_playlist_handle instance, const char* path,
                                    bool recurse_subdirs, bool allow_duplicates);


/**
 * @brief Inserts presets from the given path to the end of the current playlist.
 *
 * This method will scan the given path for files with a ".milk" extension and add these to the
 * playlist.
 *
 * Symbolic links are not followed.
 *
 * @param instance The playlist manager instance.
 * @param path A local filesystem path to scan for presets.
 * @param index The index to insert the presets at. If it exceeds the playlist size, the presets are
*              added at the end of the playlist.
 * @param recurse_subdirs If true, subdirectories of the given path will also be scanned. If false,
 *                        only the exact path given is searched for presets.
 * @param allow_duplicates If true, files found will always be added. If false, only files are
 *                         added that do not already exist in the current playlist.
 * @return The number of files added. 0 may indicate issues scanning the path.
 */
uint32_t projectm_playlist_insert_path(projectm_playlist_handle instance, const char* path,
                                       uint32_t index, bool recurse_subdirs, bool allow_duplicates);

/**
 * @brief Adds a single preset to the end of the playlist.
 *
 * @note The file is not checked for existence or for being readable.
 *
 * @param instance The playlist manager instance.
 * @param filename A local preset filename.
 * @param allow_duplicates If true, the preset filename will always be added. If false, the preset
 *                         is only added to the playlist if the exact filename doesn't exist in the
 *                         current playlist.
 * @return True if the file was added to the playlist, false if the file was a duplicate and
 *         allow_duplicates was set to false.
 */
bool projectm_playlist_add_preset(projectm_playlist_handle instance, const char* filename,
                                  bool allow_duplicates);

/**
 * @brief Adds a single preset to the playlist at the specified position.
 *
 * To always add a file at the end of the playlist, use projectm_playlist_add_preset() as it is
 * performs better.
 *
 * @note The file is not checked for existence or for being readable.
 *
 * @param instance The playlist manager instance.
 * @param filename A local preset filename.
 * @param index The index to insert the preset at. If it exceeds the playlist size, the preset is
 *              added at the end of the playlist.
 * @param allow_duplicates If true, the preset filename will always be added. If false, the preset
 *                         is only added to the playlist if the exact filename doesn't exist in the
 *                         current playlist.
 * @return True if the file was added to the playlist, false if the file was a duplicate and
 *         allow_duplicates was set to false.
 */
bool projectm_playlist_insert_preset(projectm_playlist_handle instance, const char* filename,
                                     uint32_t index, bool allow_duplicates);

/**
 * @brief Adds a list of presets to the end of the playlist.
 *
 * @note The files are not checked for existence or for being readable.
 *
 * @param instance The playlist manager instance.
 * @param filenames A list of local preset filenames.
 * @param count The number of files in the list.
 * @param allow_duplicates If true, the preset filenames will always be added. If false, a preset
 *                         is only added to the playlist if the exact filename doesn't exist in the
 *                         current playlist.
 * @return The number of files added to the playlist. Ranges between 0 and count.
 */
uint32_t projectm_playlist_add_presets(projectm_playlist_handle instance, const char** filenames,
                                       uint32_t count, bool allow_duplicates);

/**
 * @brief Adds a single preset to the playlist at the specified position.
 *
 * To always add a file at the end of the playlist, use projectm_playlist_add_preset() as it is
 * performs better.
 *
 * @note The files are not checked for existence or for being readable.
 *
 * @param instance The playlist manager instance.
 * @param filenames A list of local preset filenames.
 * @param count The number of files in the list.
 * @param index The index to insert the presets at. If it exceeds the playlist size, the presets are
 *              added at the end of the playlist.
 * @param allow_duplicates If true, the preset filenames will always be added. If false, a preset
 *                         is only added to the playlist if the exact filename doesn't exist in the
 *                         current playlist.
 * @return The number of files added to the playlist. Ranges between 0 and count.
 */
uint32_t projectm_playlist_insert_presets(projectm_playlist_handle instance, const char** filenames,
                                          uint32_t count, unsigned int index, bool allow_duplicates);

/**
 * @brief Removes a single preset from the playlist at the specified position.
 *
 * @param instance The playlist manager instance.
 * @param index The preset index to remove. If it exceeds the playlist size, no preset will be
 *              removed.
 * @return True if the preset was removed from the playlist, false if the index was out of range.
 */
bool projectm_playlist_remove_preset(projectm_playlist_handle instance, uint32_t index);

/**
 * @brief Removes a number of presets from the playlist from the specified position.
 *
 * @param instance The playlist manager instance.
 * @param index The first preset index to remove. If it exceeds the playlist size, no preset will be
 *              removed.
 * @param count The number of presets to remove from the given index.
 * @return The number of presets removed from the playlist.
 */
uint32_t projectm_playlist_remove_presets(projectm_playlist_handle instance, uint32_t index,
                                          uint32_t count);

/**
 * @brief Enable or disable shuffle mode.
 * @param instance The playlist manager instance.
 * @param shuffle True to enable random shuffling, false to play presets in playlist order.
 */
void projectm_playlist_set_shuffle(projectm_playlist_handle instance, bool shuffle);

/**
 * @brief Sorts part or the whole playlist according to the given predicate and order.
 *
 * It is safe to provide values in start_index and count that will exceed the number of items
 * in the playlist. Only items that fall into an existing index range are sorted. If start_index
 * is equal to or larger than the playlist size, no items are sorted. If start_index is inside the
 * playlist, but adding count results in an index outside the playlist, items until the end are
 * sorted.
 *
 * Sort order is lexicographical for both predicates and case-sensitive.
 *
 * If invalid values are provides as predicate or order, the defaults as mentioned in the parameter
 * description are used.
 *
 * @param instance The playlist manager instance.
 * @param start_index The starting index to begin sorting at.
 * @param count The number of items, beginning at start_index, to sort.
 * @param predicate The predicate to use for sorting. Default is SORT_PREDICATE_FULL_PATH.
 * @param order The sort order. Default is SORT_ORDER_ASCENDING.
 */
void projectm_playlist_sort(projectm_playlist_handle instance, uint32_t start_index, uint32_t count,
                            projectm_playlist_sort_predicate predicate, projectm_playlist_sort_order order);

/**
 * @brief Returns the number of retries after failed preset switches.
 * @param instance The playlist manager instance.
 * @return The number of retries after failed preset switches.
 */
uint32_t projectm_playlist_get_retry_count(projectm_playlist_handle instance);

/**
 * @brief Sets the number of retries after failed preset switches.
 * @note Don't set this value too high, as each retry is done recursively.
 * @param instance The playlist manager instance.
 * @param retry_count The number of retries after failed preset switches. Default is 5. Set to 0
 *                    to simply forward the failure event from projectM.
 */
void projectm_playlist_set_retry_count(projectm_playlist_handle instance, uint32_t retry_count);

/**
 * @brief Returns the current playlist position.
 * @param instance The playlist manager instance.
 * @return The current playlist position. If the playlist is empty, 0 will be returned.
 */
uint32_t projectm_playlist_get_position(projectm_playlist_handle instance);

/**
 * @brief Plays the preset at the requested playlist position and returns the actual playlist index.
 *
 * If the requested position is out of bounds, the returned position will wrap to 0, effectively
 * repeating the playlist as if shuffle was disabled. It has no effect if the playlist is empty.
 *
 * This method ignores the shuffle setting and will always jump to the requested index, given it is
 * withing playlist bounds.
 *
 * @param instance The playlist manager instance.
 * @param new_position The new position to jump to.
 * @param hard_cut If true, the preset transition is instant. If true, a smooth transition is played.
 * @return The new playlist position. If the playlist is empty, 0 will be returned.
 */
uint32_t projectm_playlist_set_position(projectm_playlist_handle instance, uint32_t new_position,
                                        bool hard_cut);

/**
 * @brief Plays the next playlist item and returns the index of the new preset.
 *
 * If shuffle is on, it will select a random preset, otherwise the next in the playlist. If the
 * end of the playlist is reached in continuous mode, it will wrap back to 0.
 *
 * The old playlist item is added to the history.
 *
 * @param instance The playlist manager instance.
 * @param hard_cut If true, the preset transition is instant. If true, a smooth transition is played.
 * @return The new playlist position. If the playlist is empty, 0 will be returned.
 */
uint32_t projectm_playlist_play_next(projectm_playlist_handle instance, bool hard_cut);

/**
 * @brief Plays the previous playlist item and returns the index of the new preset.
 *
 * If shuffle is on, it will select a random preset, otherwise the next in the playlist. If the
 * end of the playlist is reached in continuous mode, it will wrap back to 0.
 *
 * The old playlist item is added to the history.
 *
 * @param instance The playlist manager instance.
 * @param hard_cut If true, the preset transition is instant. If true, a smooth transition is played.
 * @return The new playlist position. If the playlist is empty, 0 will be returned.
 */
uint32_t projectm_playlist_play_previous(projectm_playlist_handle instance, bool hard_cut);

/**
 * @brief Plays the last preset played in the history and returns the index of the preset.
 *
 * The history keeps track of the last 1000 presets and will go back in the history. The
 * playback history will be cleared whenever the playlist items are changed.
 *
 * If the history is empty, this call behaves identical to projectm_playlist_play_previous(),
 * but the item is not added to the history.
 *
 * Presets which failed to load are not recorded in the history and thus will be skipped when
 * calling this method.
 *
 * @param instance The playlist manager instance.
 * @param hard_cut If true, the preset transition is instant. If true, a smooth transition is played.
 * @return The new playlist position. If the playlist is empty, 0 will be returned.
 */
uint32_t projectm_playlist_play_last(projectm_playlist_handle instance, bool hard_cut);

/**
 * @brief Sets a new filter list.
 *
 * <p>Does not immediately apply the new list to an existing playlist, only newly added files
 * will be affected. If you need to filter the existing playlist after calling this method,
 * additionally call projectm_playlist_apply_filter() afterwards.</p>
 *
 * <p>The filter list consists of simple globbing expressions similar to the .gitignore syntax:</p>
 *
 * <ul>
 * <li><strong>?</strong>: Matches any single character except /.</li>
 * <li><strong>*</strong>: Matches 0 or more characters except /.</li>
 * <li><strong>/</strong>: When used at the begin of a glob, matches if
 * pathname has no path separator.</li>
 * <li><strong>**&zwj;/</strong>: Matches 0 or more directories.</li>
 * <li><strong>/&zwj;**</strong>: When at the end of the glob, matches everything after the /.</li>
 * </ul>
 *
 * <p>In globbing expressions, \\ can be used as path separator instead of /. The backslash can't
 * be used to escape globbing patterns, so matching literal * and ? in filenames is not possible.
 * This is not a huge issue as Windows doesn't allow those characters in filenames and Milkdrop
 * files originate from the Windows world. Character classes like "[0-9]" are also not supported to
 * keep the syntax simple.</p>
 *
 * <p>Each line can be prefixed with either + or - to either include files matching the pattern
 * or excluding them. Any other character is not interpreted as a prefix and the filter line is
 * matching as an exclude filter. To match a literal + or - at the beginning, add the appropriate
 * prefix in front. Empty filters never match anything, even if the filename is empty.</p>
 *
 * <p>The filter list is checked in order. The first pattern that matches the filename determines
 * the filter result (include or exclude). If no pattern matches, the file is included. In the case
 * that a default exclude action is required, add a "-/&zwj;**" filter at the end of the list.</p>
 *
 * @param instance The playlist manager instance.
 * @param filter_list An array with filter strings.
 * @param count The size of the filter array.
 */
void projectm_playlist_set_filter(projectm_playlist_handle instance, const char** filter_list,
                                  size_t count);

/**
 * @brief Returns the current filter list.
 *
 * Always call projectm_playlist_free_string_array() on the returned pointer if the data is
 * no longer needed.
 *
 * @param instance The playlist manager instance.
 * @param[out] count The size of the filter array.
 * @return An array with filter strings.
 */
char** projectm_playlist_get_filter(projectm_playlist_handle instance, size_t* count);

/**
 * @brief Applies the current filter list to the existing playlist.
 *
 * Note this function only removes items. Previously filtered items are not added again. If items
 * were removed, the playback history is cleared.
 *
 * @param instance The playlist manager instance.
 * @return The number of removed items.
 */
size_t projectm_playlist_apply_filter(projectm_playlist_handle instance);

#ifdef __cplusplus
}
// extern "C"
#endif
