#include "SOIL2_gl_bridge.h"

#if !defined(__STDC_NO_ATOMICS__)
#include <stdatomic.h>
static _Atomic(soil_gl_resolver_t) g_resolver = 0;
#else
static soil_gl_resolver_t g_resolver = 0;
#endif

void SOIL_GL_SetResolver(soil_gl_resolver_t resolver) {
#if !defined(__STDC_NO_ATOMICS__)
    // Publish resolver to other threads (if any) safely.
    atomic_store_explicit(&g_resolver, resolver, memory_order_release);
#else
    g_resolver = resolver;
#endif
}

void* SOIL_GL_GetProcAddress(const char* proc) {
#if !defined(__STDC_NO_ATOMICS__)
    soil_gl_resolver_t resolver_func = atomic_load_explicit(&g_resolver, memory_order_acquire);
    if (resolver_func)
    {
        return resolver_func(proc);
    }
#else
    if (g_resolver)
    {
        return g_resolver(proc);
    }
#endif
    return 0;
}
