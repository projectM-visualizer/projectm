/**
   Include appropriate OpenGL headers for this platform.
**/

#ifndef  __PROJECTM_OPENGL_H__
#define  __PROJECTM_OPENGL_H__

<<<<<<< HEAD
//#define GL_TRANSITION

//#include <GL/glew.h>

#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/gl3.h>
=======
// stuff that needs to be ported to newer GL calls
#define GL_TRANSITION

#ifdef __APPLE__
# include <OpenGL/gl3.h>
# include <OpenGL/gl3ext.h>
>>>>>>> master
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
