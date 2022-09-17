#pragma once

#include "libprojectM/projectM.h"

#ifdef __cplusplus
extern "C" {
#endif

struct projectm_playlist;                                   //!< Opaque projectM playlist instance type.
typedef struct projectm_playlist* projectm_playlist_handle; //!< A pointer to the opaque projectM playlist instance.

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
 */
void projectm_playlist_create(projectm_handle projectm_instance);

/**
 * @brief Destroys a previously created playlist manager.
 *
 * If the playlist manager is currently connected to a projectM instance, it will be disconnected.
 *
 * @param instance The playlist manager instance to destroy.
 */
void projectm_playlist_destroy(projectm_playlist instance);

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
void projectm_playlist_connect(projectm_playlist instance, projectm_handle projectm_instance);

/**
 * @brief Returns the current playlist size.
 * @return The number of items in the current playlist.
 */
unsigned int projectm_playlist_size(projectm_playlist instance);

/**
 * @brief Clears the playlist.
 * @param instance The playlist manager instance to clear.
 */
void projectm_playlist_clear(projectm_playlist instance);

/**
 * @brief Adds presets from the given path to the current playlist.
 *
 * This method will scan the given path for files with a ".milk" extension and add these to the
 * playlist.
 *
 * @param instance The playlist manager instance.
 * @param path A local filesystem path to scan for presets.
 * @param recurse_subdirs If true, subdirectories of the given path will also be scanned. If false,
 *                        only the exact path given is searched for presets.
 * @param allow_duplicates If true, files found will always be added. If false, only files are
 *                         added that do not already exist in the current playlist.
 * @return The number of files added or -1 if the path could not be scanned.
 */
int projectm_playlist_add_path(projectm_playlist instance, const char* path, bool recurse_subdirs, bool allow_duplicates);

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
bool projectm_playlist_add_preset(projectm_playlist instance, const char* filename, bool allow_duplicates);

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
bool projectm_playlist_insert_preset(projectm_playlist instance, const char* filename,
                                     unsigned int index, bool allow_duplicates);

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
int projectm_playlist_add_presets(projectm_playlist instance, const char** filenames,
                                  size_t count, bool allow_duplicates);

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
int projectm_playlist_insert_presets(projectm_playlist instance, const char* filenames,
                                     size_t count, unsigned int index, bool allow_duplicates);


#ifdef __cplusplus
} // extern "C"
#endif
