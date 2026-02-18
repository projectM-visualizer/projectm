/**
 * @file OpenGL.h
 * @brief Includes the appropriate OpenGL headers for each platform.
 */
#pragma once

#ifdef USE_GLES
   #include <glad/gles2.h>
#else
   #include <glad/gl.h>
#endif
