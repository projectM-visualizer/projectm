

#pragma once

#include "projectM-4/projectM_playlist_export.h"

#ifdef __cplusplus
extern "C" {
#endif


struct projectm_playlist;


typedef struct projectm_playlist* projectm_playlist_handle;


typedef enum
{
    SORT_PREDICATE_FULL_PATH,
    SORT_PREDICATE_FILENAME_ONLY
} projectm_playlist_sort_predicate;



typedef enum
{
    SORT_ORDER_ASCENDING,
    SORT_ORDER_DESCENDING
} projectm_playlist_sort_order;

#ifdef __cplusplus
}
#endif
