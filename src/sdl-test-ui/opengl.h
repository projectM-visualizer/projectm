/**
   Include appropriate OpenGL headers for this platform.
**/

#ifndef  __PROJECTM_OPENGL_H__
#define  __PROJECTM_OPENGL_H__

// stuff that needs to be ported to newer GL calls
#define GL_TRANSITION

// Enable openGL extra checks, better not be enabled in release build
#define OGL_DEBUG                   0

// Unlock FPS for rendering benchmarks, it disables Vblank/Vsync and prints drawn frame count within a 5s test run
#define UNLOCK_FPS                  0

// If a shader compilation failure occurs, it dumps shader source into /tmp instead of stderr
#define DUMP_SHADERS_ON_ERROR       0


#ifdef USE_GLES
#  include <glad/gles2.h>
#else
#  include <glad/gl.h>
#endif


#endif // __PROJECTM_OPENGL_H__
