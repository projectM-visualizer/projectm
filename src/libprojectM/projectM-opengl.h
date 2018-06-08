/**
   Include appropriate OpenGL headers for this platform.
**/

#ifndef  __PROJECTM_OPENGL_H__
#define  __PROJECTM_OPENGL_H__

#ifdef __APPLE__
# include <OpenGL/gl.h>
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

#endif // __PROJECTM_OPENGL_H__
