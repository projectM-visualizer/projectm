

#pragma once

#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_EXPORT uint32_t projectm_sprite_create(projectm_handle instance,
                                                const char* type,
                                                const char* code);


PROJECTM_EXPORT void projectm_sprite_destroy(projectm_handle instance, uint32_t sprite_id);


PROJECTM_EXPORT void projectm_sprite_destroy_all(projectm_handle instance);


PROJECTM_EXPORT uint32_t projectm_sprite_get_sprite_count(projectm_handle instance);


PROJECTM_EXPORT void projectm_sprite_get_sprite_ids(projectm_handle instance, uint32_t* sprite_ids);


PROJECTM_EXPORT void projectm_sprite_set_max_sprites(projectm_handle instance,
                                                     uint32_t max_sprites);


PROJECTM_EXPORT uint32_t projectm_sprite_get_max_sprites(projectm_handle instance);

#ifdef __cplusplus
}
#endif
