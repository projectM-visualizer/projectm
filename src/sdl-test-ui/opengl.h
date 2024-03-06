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

#ifdef __APPLE__
# include <OpenGL/gl3.h>
# include <OpenGL/gl3ext.h>
#elif defined(EYETUNE_WINRT)
#define GL_GLEXT_PROTOTYPES
#define GLM_FORCE_CXX03
#include <GLES3/gl31.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#elif defined(_WIN32)
#define GLM_FORCE_CXX03
# include <windows.h>
#include "GL/glew.h"
#include "GL/wglew.h"
#else /* linux/unix/other */
# ifdef USE_GLES
#  include <GLES3/gl3.h>
# else
#  if !defined(GL_GLEXT_PROTOTYPES)
#     define GL_GLEXT_PROTOTYPES
#  endif
#  include <GL/gl.h>
#  include <GL/glext.h>
# endif
#endif


#endif // __PROJECTM_OPENGL_H__
