/**
 * @file playlist_playback.h
 * @copyright 2003-2023 projectM Team
 * @brief Playback control functions.
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
 * @brief Enable or disable shuffle mode.
 * @param instance The playlist manager instance.
 * @param shuffle True to enable random shuffling, false to play presets in playlist order.
 */
PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_shuffle(projectm_playlist_handle instance, bool shuffle);

/**
 * @brief Retrieves the current state of shuffle mode.
 * @param instance The playlist manager instance.
 * @return True if shuffle mode is enabled, false otherwise.
 */
PROJECTM_PLAYLIST_EXPORT bool projectm_playlist_get_shuffle(projectm_playlist_handle instance);

/**
 * @brief Sets the number of retries after failed preset switches.
 * @note Don't set this value too high, as each retry is done recursively.
 * @param instance The playlist manager instance.
 * @param retry_count The number of retries after failed preset switches. Default is 5. Set to 0
 *                    to simply forward the failure event from projectM.
 */
PROJECTM_PLAYLIST_EXPORT void projectm_playlist_set_retry_count(projectm_playlist_handle instance, uint32_t retry_count);

/**
 * @brief Returns the number of retries after failed preset switches.
 * @param instance The playlist manager instance.
 * @return The number of retries after failed preset switches.
 */
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_get_retry_count(projectm_playlist_handle instance);

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
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_set_position(projectm_playlist_handle instance, uint32_t new_position,
                                                                 bool hard_cut);

/**
 * @brief Returns the current playlist position.
 * @param instance The playlist manager instance.
 * @return The current playlist position. If the playlist is empty, 0 will be returned.
 */
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_get_position(projectm_playlist_handle instance);

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
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_play_next(projectm_playlist_handle instance, bool hard_cut);

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
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_play_previous(projectm_playlist_handle instance, bool hard_cut);

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
PROJECTM_PLAYLIST_EXPORT uint32_t projectm_playlist_play_last(projectm_playlist_handle instance, bool hard_cut);

#ifdef __cplusplus
} // extern "C"
#endif
