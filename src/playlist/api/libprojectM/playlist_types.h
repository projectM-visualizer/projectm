/**
 * @file playlist_types.h
 * @copyright 2003-2023 projectM Team
 * @brief Types and enumerations used in the playlist API headers.
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

#include "projectM-4/projectM_playlist_export.h"

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

#ifdef __cplusplus
} // extern "C"
#endif
