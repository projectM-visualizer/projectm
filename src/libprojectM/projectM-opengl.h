/**
   Include appropriate OpenGL headers for this platform.
**/

#ifndef  __PROJECTM_OPENGL_H__
#define  __PROJECTM_OPENGL_H__

// stuff that needs to be ported to newer GL calls
#define GL_TRANSITION

#define DUMP_SHADERS_ON_ERROR

#ifdef __APPLE__
# include <OpenGL/gl3.h>
# include <OpenGL/gl3ext.h>
#elif defined(_WIN32)
# include <windows.h>
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

void _check_gl_error(const char *file, int line);
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

#endif // __PROJECTM_OPENGL_H__
