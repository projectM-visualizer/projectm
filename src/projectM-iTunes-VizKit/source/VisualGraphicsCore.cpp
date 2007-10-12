/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualGraphicsCore.cpp
 *
 */

/***************************************************************************

Copyright (c) 2004-2007 Heiko Wichmann (http://www.imagomat.de/vizkit)


This software is provided 'as-is', without any expressed or implied warranty. 
In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; 
   you must not claim that you wrote the original software. 
   If you use this software in a product, an acknowledgment 
   in the product documentation would be appreciated 
   but is not required.

2. Altered source versions must be plainly marked as such, 
   and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

 ***************************************************************************/

#include "VisualGraphicsCore.h"
#include "VisualErrorHandling.h"

#if TARGET_OS_WIN
#include <stdio.h> // sprintf
#endif


#if TARGET_OS_WIN
#include <GL/gl.h>
#include <GL/glu.h>
#endif



using namespace VizKit;


VisualGraphicsCore* VisualGraphicsCore::theVisualGraphicsCore = NULL;  // singleton


VisualGraphicsCore::VisualGraphicsCore() {
	ctx = NULL;
#if TARGET_OS_MAC
	directDisplayId = CGMainDisplayID();
#endif
#if TARGET_OS_WIN
	windowDC = NULL;
#endif
	canvasXOriginOffset = 0;
	canvasYOriginOffset = 0;
}


VisualGraphicsCore::~VisualGraphicsCore() {
	// null
}


VisualGraphicsCore* VisualGraphicsCore::getInstance() {
    if (theVisualGraphicsCore == NULL) {
		theVisualGraphicsCore = new VisualGraphicsCore;
    }
	return theVisualGraphicsCore;
}


void VisualGraphicsCore::dispose() {
    if (theVisualGraphicsCore != NULL) {
        delete theVisualGraphicsCore;
		theVisualGraphicsCore = NULL;
    }
}


bool VisualGraphicsCore::setupContext() {

	bool success = true;
	UInt8 errNum = 0;
	char errLog[256];
	
#if TARGET_OS_MAC
	errNum = theVisualGraphicsCore->setupAGL();
	if (errNum != 0) {
		sprintf(errLog, "error %d on setupAGL in file: %s (line: %d) [%s])", errNum, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		success = false;
	}
#endif

#if TARGET_OS_WIN
	errNum = theVisualGraphicsCore->setupWGL();
	if (errNum != 0) {
		sprintf(errLog, "error %d on setupWGL in file: %s (line: %d) [%s])", errNum, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		success = false;
	}
#endif
    
	return success;
}


#if TARGET_OS_MAC
UInt8 VisualGraphicsCore::setupAGL() {
    GLboolean ok;
    AGLPixelFormat fmt;
    
	GLint attrib[] = {AGL_RGBA, AGL_RED_SIZE, 8, AGL_GREEN_SIZE, 8, AGL_BLUE_SIZE, 8, AGL_ALPHA_SIZE, 8, AGL_ACCELERATED, AGL_DOUBLEBUFFER, AGL_DEPTH_SIZE, 24, AGL_CLOSEST_POLICY, AGL_NONE};
    
    fmt = aglChoosePixelFormat(NULL, 0, attrib);

    if (fmt == NULL) {
        return 1;
    }

    theVisualGraphicsCore->ctx = aglCreateContext(fmt, NULL);
    if (theVisualGraphicsCore->ctx == NULL) {
        return 2;
    }

	ok = aglSetDrawable(theVisualGraphicsCore->ctx, theVisualGraphicsCore->port);
    if (!ok) {
        return 3;
    }
        
    ok = aglSetCurrentContext(theVisualGraphicsCore->ctx);
    if (!ok) {
        return 4;
    }

	ok = aglUpdateContext(theVisualGraphicsCore->ctx);
	if (!ok) {
		return 5;
	}

	GLint swapinterval = 1;
    ok = aglSetInteger(theVisualGraphicsCore->ctx, AGL_SWAP_INTERVAL, &swapinterval);
    if (!ok) {
        return 6;
    }

    aglDestroyPixelFormat(fmt);
	
    return 0;
}


void VisualGraphicsCore::cleanupAGL() {
    aglSetCurrentContext(NULL);
    if (theVisualGraphicsCore->ctx != NULL) {
		aglSetDrawable(theVisualGraphicsCore->ctx, NULL);
		aglDestroyContext(theVisualGraphicsCore->ctx);
    }
}

#endif


UInt16 VisualGraphicsCore::setCurrentContext() {
#if TARGET_OS_MAC
	GLboolean ok;
    ok = aglSetCurrentContext(theVisualGraphicsCore->ctx);
    if(!ok) {
        return 1;
    }
#endif
#if TARGET_OS_WIN
	if (!wglMakeCurrent(theVisualGraphicsCore->windowDC, theVisualGraphicsCore->ctx)) {
		writeLastErrorLog("setCurrentContext");
        return 1;
	}
#endif
	return 0;
}


#if TARGET_OS_WIN

UInt8 VisualGraphicsCore::setupWGL() {

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
    pfd.cDepthBits = 24;

	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(theVisualGraphicsCore->windowDC, &pfd);
    if (pixelFormat == 0) {
		writeLastErrorLog("setupWGL1");
        return 1;
	}
	
	if (SetPixelFormat(theVisualGraphicsCore->windowDC, pixelFormat, &pfd) == 0) {
		writeLastErrorLog("setupWGL2");
        return 2;
	}

	theVisualGraphicsCore->ctx = wglCreateContext(theVisualGraphicsCore->windowDC);
    if (theVisualGraphicsCore->ctx == NULL) {
		writeLastErrorLog("setupWGL3");
        return 3;
	}

	if (!wglMakeCurrent(theVisualGraphicsCore->windowDC, theVisualGraphicsCore->ctx)) {
		writeLastErrorLog("setupWGL4");
        return 4;
	}

    // enable v-sync if WGL_EXT_swap_control is supported
	/*
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT != NULL)
        wglSwapIntervalEXT(1);
	*/

	return 0;
}


void VisualGraphicsCore::cleanUpWGL() {

    wglMakeCurrent(NULL, NULL);

    if (theVisualGraphicsCore->ctx != NULL) {
        wglDeleteContext(theVisualGraphicsCore->ctx);
        theVisualGraphicsCore->ctx = NULL;
    }

    if (theVisualGraphicsCore->windowDC) {
		ReleaseDC(theVisualGraphicsCore->port, theVisualGraphicsCore->windowDC);
        theVisualGraphicsCore->windowDC = NULL;
    }
}


HDC VisualGraphicsCore::getWindowDC() {
	return theVisualGraphicsCore->windowDC;
}

#endif


void VisualGraphicsCore::setGraphicsDevicePort(const GRAPHICS_DEVICE thePort) {
    theVisualGraphicsCore->port = thePort;
#if TARGET_OS_WIN
	theVisualGraphicsCore->windowDC = GetDC(thePort);
#endif
}


GRAPHICS_DEVICE VisualGraphicsCore::getGraphicsDevicePort() {
    return theVisualGraphicsCore->port;
}


UInt16 VisualGraphicsCore::getScreenWidth() {
	UInt16 screenWidth;
#if TARGET_OS_WIN
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
#endif
#if TARGET_OS_MAC
	screenWidth = (UInt16)CGDisplayPixelsWide(theVisualGraphicsCore->directDisplayId);
#endif
	return screenWidth;
}


UInt16 VisualGraphicsCore::getScreenHeight() {
	UInt16 screenHeight;
#if TARGET_OS_WIN
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
#endif
#if TARGET_OS_MAC
	screenHeight = (UInt16)CGDisplayPixelsHigh(theVisualGraphicsCore->directDisplayId);
#endif
	return screenHeight;
}


UInt16 VisualGraphicsCore::getBitsPerPixelOfScreen() {
	UInt16 bitsPerPixel;
#if TARGET_OS_WIN
	bitsPerPixel = GetDeviceCaps(theVisualGraphicsCore->windowDC, BITSPIXEL);
#endif
#if TARGET_OS_MAC
	bitsPerPixel = (UInt16)CGDisplayBitsPerPixel(theVisualGraphicsCore->directDisplayId);
#endif
	return bitsPerPixel;
}


UInt16 VisualGraphicsCore::getRefreshRateOfScreen() {
	UInt16 refreshRate = 0;
#if TARGET_OS_MAC
	long refreshRateLong;
	CFDictionaryRef displayMode;
	CFNumberRef number;
	displayMode = CGDisplayCurrentMode(theVisualGraphicsCore->directDisplayId);
	number = (CFNumberRef)CFDictionaryGetValue(displayMode, kCGDisplayRefreshRate);
	CFNumberGetValue(number, kCFNumberLongType, &refreshRateLong);
	refreshRate = (UInt16)refreshRateLong;
#endif
#if TARGET_OS_WIN
	refreshRate = GetDeviceCaps(theVisualGraphicsCore->windowDC, VREFRESH);
#endif
	return refreshRate;
}


void VisualGraphicsCore::getCanvasSurroundingRect(::Rect* aRect) {
#if TARGET_OS_MAC
	GetPortBounds(theVisualGraphicsCore->port, aRect);
#endif
#if TARGET_OS_WIN
	HWND windowHandle;
	RECT windowRect;
	POINT aPoint;
	aPoint.x = 0;
	aPoint.y = 0;
	
	windowHandle = theVisualGraphicsCore->port;
	ClientToScreen(windowHandle, &aPoint);
	GetClientRect(windowHandle, &windowRect);
	aRect->top = (short)aPoint.y;
	aRect->left = (short)aPoint.x;
	
	windowRect.bottom += aPoint.y;
	windowRect.right += aPoint.x;
	aRect->bottom = (short)windowRect.bottom;
	aRect->right = (short)windowRect.right;
#endif
}


void VisualGraphicsCore::finishGLDrawing() {
    glFinish();
    glFlush();
#if TARGET_OS_MAC
	aglSwapBuffers(theVisualGraphicsCore->ctx);
#endif
#if TARGET_OS_WIN
	SwapBuffers(theVisualGraphicsCore->windowDC);
#endif
}


void VisualGraphicsCore::setViewport(const ::Rect* const canvasRect, const ::Rect* const canvasSurroundingRect, const bool isFullscreen) {

	theVisualGraphicsCore->canvasXOriginOffset = 0;
	theVisualGraphicsCore->canvasYOriginOffset = 0;


#if TARGET_OS_MAC
	GLboolean ok;
	GLint bufferRect[4];
	// bufferRect:
	// the x and y window coordinates 
	// (bottom left origin) of the buffer rectangle, 
	// followed by its width and height.
	bufferRect[0] = (GLint)canvasRect->left;
	bufferRect[1] = (GLint)(canvasSurroundingRect->bottom - canvasSurroundingRect->top - canvasRect->bottom);
	bufferRect[2] = (GLint)(canvasRect->right - canvasRect->left);
	bufferRect[3] = (GLint)(canvasRect->bottom - canvasRect->top);

	ok = GL_FALSE;
	ok = aglEnable(theVisualGraphicsCore->ctx, AGL_BUFFER_RECT);
	if(!ok) {
		writeLog("unable to enable AGL_BUFFER_RECT");
	}
	// adjust drawing rectange
	ok = aglSetInteger(theVisualGraphicsCore->ctx, AGL_BUFFER_RECT, bufferRect);
	if(!ok) {
		writeLog("setViewport: aglSetInteger AGL_BUFFER_RECT != ok");
	}
	ok = aglUpdateContext(theVisualGraphicsCore->ctx);
	if(!ok) {
		writeLog("reshapeAGL: aglUpdateContext != ok");
	}
#endif

GLsizei viewportWidth = canvasRect->right - canvasRect->left;
GLsizei viewportHeight = canvasRect->bottom - canvasRect->top;
	
#if TARGET_OS_WIN
if (isFullscreen == true) {
	theVisualGraphicsCore->canvasXOriginOffset = canvasRect->left;
	theVisualGraphicsCore->canvasYOriginOffset = (canvasSurroundingRect->bottom - canvasSurroundingRect->top - canvasRect->bottom);
	
	glEnable(GL_SCISSOR_TEST);
	glScissor((GLint)theVisualGraphicsCore->canvasXOriginOffset, (GLint)theVisualGraphicsCore->canvasYOriginOffset, (GLsizei)viewportWidth, (GLsizei)viewportHeight);
	//glViewport((GLint)canvasRect->left, (GLint)(canvasSurroundingRect->bottom - canvasSurroundingRect->top - canvasRect->bottom), (GLsizei)(canvasRect->right - canvasRect->left), (GLsizei)(canvasRect->bottom - canvasRect->top));

}
#endif

	GLint leftPixelPos = theVisualGraphicsCore->canvasXOriginOffset;
	GLint bottomPixelPos = theVisualGraphicsCore->canvasYOriginOffset;
	glViewport(leftPixelPos, bottomPixelPos, viewportWidth, viewportHeight);

}


int VisualGraphicsCore::getCanvasXOriginOffset() {
	return this->canvasXOriginOffset;
}


int VisualGraphicsCore::getCanvasYOriginOffset() {
	return this->canvasYOriginOffset;
}
