#ifndef SOIL2_GL_BRIDGE_H
#define SOIL2_GL_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Function used by SOIL2 to resolve GL function pointers.
     */
    typedef void* (*soil_gl_resolver_t)(const char* name);

    /**
     * Sets the GL function resolver for SOIL2.
     *
     * Contract: call once during initialization before SOIL2 performs any GL entrypoint lookups.
     * If atomics are available, this is thread-safe; otherwise concurrent Set/Get is undefined.
     *
     * @param resolver The resolver function to use.
     */
    void SOIL_GL_SetResolver(soil_gl_resolver_t resolver);

    /**
     * Resolver function to look up gl functions for SOIL2.
     *
     * @param proc GL Function name.
     * @return Resolved function pointer for the given function name, or 0 if the function could not be resolved.
     */
    void* SOIL_GL_GetProcAddress(const char* proc);

#ifdef __cplusplus
}
#endif

#endif
