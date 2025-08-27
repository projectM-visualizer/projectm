

#pragma once

#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif


PROJECTM_EXPORT void projectm_opengl_render_frame(projectm_handle instance);


PROJECTM_EXPORT void projectm_opengl_render_frame_fbo(projectm_handle instance, uint32_t framebuffer_object_id);

#ifdef __cplusplus
}
#endif
