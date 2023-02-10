/**
 * @file audio.h
 * @copyright 2003-2023 projectM Team
 * @brief Functions to pass in audio data to libprojectM.
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
 * @brief Returns the maximum number of audio samples that can be stored.
 *
 * Each PCM data UpdateMeshSize should not exceed this number of samples. If more samples
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

#ifdef __cplusplus
} // extern "C"
#endif

