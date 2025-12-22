#include "SOIL2_gl_bridge.h"

static soil_gl_resolver_t g_resolver = 0;

void SOIL_GL_SetResolver(soil_gl_resolver_t resolver) {
    g_resolver = resolver;
}

void* SOIL_GL_GetProcAddress(const char* proc) {
    if (g_resolver)
        return g_resolver(proc);
    return 0;
}
