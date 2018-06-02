/**
   Include appropriate OpenGL headers for this platform.
**/

#ifndef  __PROJECTM_OPENGL_H__
#define  __PROJECTM_OPENGL_H__

// temporary flag signalling that we are using OpenGL 3 (as a stepping stone to getting to GLES compatibility)
#define GL_TRANSITION

#ifdef __APPLE__
//# include <OpenGL/gl.h>
# include <OpenGL/gl3.h>
#elif defined(_WIN32)
# include <windows.h>
#else /* linux/unix/other */
// may need to be changed to use GLES/GL3 or whatever is modern on linux
#  include <GL/gl.h>
#endif

#endif // __PROJECTM_OPENGL_H__
