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
# ifdef USE_GLES1
#  include <GLES/gl.h>
# else
#  include <GL/gl.h>
# endif
#endif

#endif // __PROJECTM_OPENGL_H__
