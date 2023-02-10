/**
 * @file memory.h
 * @copyright 2003-2023 projectM Team
 * @brief Memory allocation/deallocation helpers.
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
 * @param str A pointer returned by projectm_alloc_string().
 */
PROJECTM_EXPORT void projectm_free_string(const char* str);

#ifdef __cplusplus
} // extern "C"
#endif
