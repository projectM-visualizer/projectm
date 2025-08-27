

#pragma once

#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_EXPORT char* projectm_alloc_string(unsigned int length);


PROJECTM_EXPORT void projectm_free_string(const char* str);

#ifdef __cplusplus
}
#endif
