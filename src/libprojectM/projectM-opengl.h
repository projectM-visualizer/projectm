

#pragma once


#define OGL_DEBUG 0


#define UNLOCK_FPS 0

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#elif defined(EYETUNE_WINRT)
#define GL_GLEXT_PROTOTYPES
#define GLM_FORCE_CXX03
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#elif defined(_WIN32)
#define GLM_FORCE_CXX03
#include <GL/glew.h>
#include <GL/wglew.h>
#include <windows.h>
#else
#ifdef USE_GLES
#include <GLES3/gl3.h>
#else
#if !defined(GL_GLEXT_PROTOTYPES)
#define GL_GLEXT_PROTOTYPES
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif
