/**
 * @file playlist_items.h
 * @copyright 2003-2023 projectM Team
 * @brief Playlist item management functions.
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

#include "projectM-4/playlist_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the number of presets in the current playlist.
 * @param instance The playlist manager instance.
 * @return The number of presets in the current playlist.
 */
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_size(projectm_playlist_handle instance);

/**
 * @brief Clears the playlist.
 * @param instance The playlist manager instance to clear.
 */
PROJECTM_PLAYLIST_EXPORT void projectm_playlist_clear(projectm_playlist_handle instance);

/**
 * @brief Returns a list of preset files inside the given range of the current playlist, in order.
 *
 * This function can be used to return the whole playlist to save it to a file, or just a part of
 * it for use in virtual lists. If less elements than given in @a count are available, only the
 * remainder of items after the starting index are returned. If the starting index equals or exceeds
 * the playlist size, an empty list is returned.
 *
 * @note Call projectm_playlist_free_string_array() when you're done using the list.
 * @note Ideally, don't rely on the value provided as count to iterate over the filenames.
 *       Instead, look for the terminating null pointer to abort the loop.
 * @param instance The playlist manager instance.
 * @param start The zero-based starting index of the range to return.
 * @param count The maximum number if items to return.
 * @return A pointer to a list of char pointers, each containing a single preset. The last entry
 *         is denoted by a null pointer.
 */
PROJECTM_PLAYLIST_EXPORT char** projectm_playlist_items(projectm_playlist_handle instance, uint32_t start, uint32_t count);

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
PROJECTM_PLAYLIST_EXPORT char* projectm_playlist_item(projectm_playlist_handle instance, uint32_t index);

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
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_add_path(projectm_playlist_handle instance, const char* path,
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
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_insert_path(projectm_playlist_handle instance, const char* path,
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
PROJECTM_PLAYLIST_EXPORT bool projectm_playlist_add_preset(projectm_playlist_handle instance, const char* filename,
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
PROJECTM_PLAYLIST_EXPORT bool projectm_playlist_insert_preset(projectm_playlist_handle instance, const char* filename,
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
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_add_presets(projectm_playlist_handle instance, const char** filenames,
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
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_insert_presets(projectm_playlist_handle instance, const char** filenames,
                                                                   uint32_t count, unsigned int index, bool allow_duplicates);

/**
 * @brief Removes a single preset from the playlist at the specified position.
 *
 * @param instance The playlist manager instance.
 * @param index The preset index to remove. If it exceeds the playlist size, no preset will be
 *              removed.
 * @return True if the preset was removed from the playlist, false if the index was out of range.
 */
PROJECTM_PLAYLIST_EXPORT bool projectm_playlist_remove_preset(projectm_playlist_handle instance, uint32_t index);

/**
 * @brief Removes a number of presets from the playlist from the specified position.
 *
 * @param instance The playlist manager instance.
 * @param index The first preset index to remove. If it exceeds the playlist size, no preset will be
 *              removed.
 * @param count The number of presets to remove from the given index.
 * @return The number of presets removed from the playlist.
 */
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_remove_presets(projectm_playlist_handle instance, uint32_t index,
                                                                   uint32_t count);

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
PROJECTM_PLAYLIST_EXPORT void projectm_playlist_sort(projectm_playlist_handle instance, uint32_t start_index, uint32_t count,
                                                     projectm_playlist_sort_predicate predicate, projectm_playlist_sort_order order);

#ifdef __cplusplus
} // extern "C"
#endif

