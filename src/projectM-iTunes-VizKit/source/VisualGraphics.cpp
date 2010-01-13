/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualGraphics.cpp
 *
 */

/***************************************************************************
 
 Copyright (c) 2004-2009 Heiko Wichmann (http://www.imagomat.de/vizkit)
 
 
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

#include "VisualGraphics.h"
#include "VisualGraphicTypes.h"
#include "VisualVertex.h"
#include "VisualErrorHandling.h"
#include "VisualGraphicsCore.h"
#include "VisualConvolutionFilter.h"
#include "VisualNurbs.h"
#include "VisualDispatch.h"
#include "VisualTiming.h"
#include "VisualColorTools.h"
#include "VisualPreferences.h"
#include "VisualFile.h"
#include "VisualInterpolation.h"
#include "VisualCamera.h"

#include <iostream>

#include <algorithm> // sort on vector

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#if TARGET_OS_MAC
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif

#if TARGET_OS_WIN
#include <GL/gl.h>
#include <GL/glu.h>
#include "win/wglext.h"
#include "win/glext.h"
#include "win/resource.h"
#endif

#if TARGET_OS_WIN
#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#endif



using namespace VizKit;



VisualGraphics* VisualGraphics::theVisualGraphics = NULL;  // singleton


#if TARGET_OS_WIN
GLuint base; // Base Display List For The Font Set
#endif


VisualGraphics::VisualGraphics() {
	isSetUpOnFullscreen = false;
	
	canvasBackgroundColor.r = VisualPreferences::getValue(VisualPreferences::kCanvasBackgroundColorRed);
	canvasBackgroundColor.g = VisualPreferences::getValue(VisualPreferences::kCanvasBackgroundColorGreen);
	canvasBackgroundColor.b = VisualPreferences::getValue(VisualPreferences::kCanvasBackgroundColorBlue);
	canvasBackgroundColor.a = VisualPreferences::getValue(VisualPreferences::kCanvasBackgroundColorAlpha);
	
	surfaceBackgroundColor.r = VisualPreferences::getValue(VisualPreferences::kSurfaceBackgroundColorRed);
	surfaceBackgroundColor.g = VisualPreferences::getValue(VisualPreferences::kSurfaceBackgroundColorGreen);
	surfaceBackgroundColor.b = VisualPreferences::getValue(VisualPreferences::kSurfaceBackgroundColorBlue);
	surfaceBackgroundColor.a = VisualPreferences::getValue(VisualPreferences::kSurfaceBackgroundColorAlpha);
	
	surfaceRect.pixelRect.width = 0;
	surfaceRect.pixelRect.height = 0;
	surfaceRect.bottomLeftPixel.x = 0;
	surfaceRect.bottomLeftPixel.y = 0;
	
	surfaceRectVerticalOffsetFromBottom = 0;
	
}


VisualGraphics::~VisualGraphics() {
	
	for (NurbsMapIterator it = this->nurbsMap.begin(); it != this->nurbsMap.end(); it++) {
		delete it->second;
		it->second = NULL;
	}
	nurbsMap.clear();
	
}


VisualGraphics* VisualGraphics::getInstance() {
    if (theVisualGraphics == NULL) {
		theVisualGraphics = new VisualGraphics;
    }
	return theVisualGraphics;
}


void VisualGraphics::dispose() {
	
	VisualGraphicsCore::dispose();
	
	if (theVisualGraphics != NULL) {
		delete theVisualGraphics;
		theVisualGraphics = NULL;
	}
}


bool VisualGraphics::isInitialized() {
	if (theVisualGraphics == NULL) {
		return false;
	} else {
		return true;
	}
}


void VisualGraphics::setGraphicsDevicePort(const GRAPHICS_DEVICE thePort){
	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	theVisualGraphicsCore->setGraphicsDevicePort(thePort);
}


void VisualGraphics::setCanvasRect(const TopLeftPositionedPixelRect& aCanvasRect) {
	this->canvasRect.pixelRect.width = aCanvasRect.pixelRect.width;
	this->canvasRect.pixelRect.height = aCanvasRect.pixelRect.height;
	this->canvasRect.bottomLeftPixel.x = aCanvasRect.topLeftPixel.x;
	this->canvasRect.bottomLeftPixel.y = ((this->surfaceRect.pixelRect.height - aCanvasRect.pixelRect.height) / 2) + this->surfaceRect.bottomLeftPixel.y;
}


void VisualGraphics::setTotalVisualizerRect(TopLeftPositionedPixelRect totalVisualizerRect) {
	this->surfaceRect.pixelRect.width = totalVisualizerRect.pixelRect.width;
	this->surfaceRect.pixelRect.height = totalVisualizerRect.pixelRect.height;
	this->surfaceRect.bottomLeftPixel.x = totalVisualizerRect.topLeftPixel.x;
	this->surfaceRect.bottomLeftPixel.y = this->surfaceRectVerticalOffsetFromBottom;
}


void VisualGraphics::setTotalVisualizerRectVerticalOffsetFromBottom(int top, int height) {
	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	PixelRect canvasSurroundingRect = theVisualGraphicsCore->getCanvasSurroundingRect();
	this->surfaceRectVerticalOffsetFromBottom = canvasSurroundingRect.height - (top + height);
}


void VisualGraphics::isSetupForFullScreenMode(const bool isFullscreen) {
	this->isSetUpOnFullscreen = isFullscreen;
}


BottomLeftPositionedPixelRect VisualGraphics::getCanvasRect() {
	return this->canvasRect;
}


PixelRect VisualGraphics::getScreenRect() {
	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	return theVisualGraphicsCore->getScreenRect();
}


uint16 VisualGraphics::getBitsPerPixelOfScreen() {
	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	return theVisualGraphicsCore->getBitsPerPixelOfScreen();
}


uint16 VisualGraphics::getRefreshRateOfScreen() {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	return theVisualGraphicsCore->getRefreshRateOfScreen();
}


uint32 VisualGraphics::getCanvasPixelWidth() {
	return canvasRect.pixelRect.width;
}


uint32 VisualGraphics::getCanvasPixelHeight() {
	return canvasRect.pixelRect.height;
}


BottomLeftPositionedPixelRect VisualGraphics::getViewportRect() {
	BottomLeftPositionedPixelRect bottomLeftPositionedPixelRect;
	GLint viewportRect[4];
    glGetIntegerv(GL_VIEWPORT, viewportRect);
	bottomLeftPositionedPixelRect.bottomLeftPixel.x = viewportRect[0];
	bottomLeftPositionedPixelRect.bottomLeftPixel.y = viewportRect[1];
	bottomLeftPositionedPixelRect.pixelRect.width = viewportRect[2];
	bottomLeftPositionedPixelRect.pixelRect.height = viewportRect[3];
	return bottomLeftPositionedPixelRect;
}


Pixel VisualGraphics::getViewportBottomLeftOrigin() {
	Pixel bottomLeftPixel;
	BottomLeftPositionedPixelRect bottomLeftPositionedPixelRect = this->getViewportRect();
	bottomLeftPixel.x = bottomLeftPositionedPixelRect.bottomLeftPixel.x;
	bottomLeftPixel.y = bottomLeftPositionedPixelRect.bottomLeftPixel.y;
	return bottomLeftPixel;
}


RelationalRect VisualGraphics::getViewportOrientationAndAspectRatio() {
	RelationalRect relationalRect;
	relationalRect.orientation = kSquare;
	relationalRect.aspectRatio = 1.0;
	BottomLeftPositionedPixelRect bottomLeftPositionedPixelRect = this->getViewportRect();
	if ((bottomLeftPositionedPixelRect.pixelRect.height) > (bottomLeftPositionedPixelRect.pixelRect.width)) {
		relationalRect.orientation = kPortrait;
		relationalRect.aspectRatio = (double)bottomLeftPositionedPixelRect.pixelRect.height / (double)bottomLeftPositionedPixelRect.pixelRect.width;
	} else if ((bottomLeftPositionedPixelRect.pixelRect.width) > (bottomLeftPositionedPixelRect.pixelRect.height)) {
		relationalRect.orientation = kLandscape;
		relationalRect.aspectRatio = (double)bottomLeftPositionedPixelRect.pixelRect.width / (double)bottomLeftPositionedPixelRect.pixelRect.height;
	}
	return relationalRect;
}


#if TARGET_OS_WIN
void VisualGraphics::initGdiplus() {
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}


void VisualGraphics::terminateGdiplus() {
	Gdiplus::GdiplusShutdown(gdiplusToken);
}


bool VisualGraphics::getGdiplusEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	bool success = false;
	
	UINT  num = 0;
	UINT  size = 0;
	
	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
	
	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return success;
	
	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return success;
	
	GetImageEncoders(num, size, pImageCodecInfo);
	
	for (UINT j = 0; j < num; ++j) {
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0 ) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return true;  // success
		}
	}
	
	free(pImageCodecInfo);
	return success; // failure
}

#endif


bool VisualGraphics::setupContext() {
	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	bool success = theVisualGraphicsCore->setupContext();
	return success;
}


void VisualGraphics::disposeContext() {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
#if TARGET_OS_MAC
	theVisualGraphicsCore->cleanupAGL();
#endif
#if TARGET_OS_WIN
	theVisualGraphicsCore->cleanUpWGL();
	this->killFont();
#endif
}


void VisualGraphics::setCanvasViewport() {
	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	theVisualGraphicsCore->setViewport(this->canvasRect, this->isSetUpOnFullscreen);
}


void VisualGraphics::setSurfaceViewport() {
	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	theVisualGraphicsCore->setViewport(this->surfaceRect, this->isSetUpOnFullscreen);
}


void VisualGraphics::setOrthographicProjection(double maxLeftCoord, double maxRightCoord, double maxBottomCoord, double maxTopCoord, double maxNearPos, double maxFarPos, double zoomFactor) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	glOrtho(maxLeftCoord * zoomFactor, maxRightCoord * zoomFactor, maxBottomCoord * zoomFactor, maxTopCoord * zoomFactor, maxNearPos, maxFarPos);
	
	glMatrixMode(GL_MODELVIEW);
}


void VisualGraphics::setPerspectiveProjection(double maxLeftCoord, double maxRightCoord, double maxBottomCoord, double maxTopCoord, double maxNearPos, double maxFarPos, double zoomFactor) {
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	glFrustum(maxLeftCoord * zoomFactor, maxRightCoord * zoomFactor, maxBottomCoord * zoomFactor, maxTopCoord * zoomFactor, maxNearPos, maxFarPos);
	
	glMatrixMode(GL_MODELVIEW);
}


void VisualGraphics::loadModelViewIdentityMatrix() {
	glLoadIdentity();
}


void VisualGraphics::lookAt(Point3D eye, Point3D center, Vector up) {
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
}


void VisualGraphics::setCanvasBackgroundColor(const RGBAColor& aColorVal) {
	this->canvasBackgroundColor.r = aColorVal.r;
	this->canvasBackgroundColor.g = aColorVal.g;
	this->canvasBackgroundColor.b = aColorVal.b;
	this->canvasBackgroundColor.a = aColorVal.a;
}


RGBAColor VisualGraphics::getBackgroundColor() {
	return this->canvasBackgroundColor;
}


void VisualGraphics::clearCanvasBackground() {
	glClearColor((GLclampf)canvasBackgroundColor.r, (GLclampf)canvasBackgroundColor.g, (GLclampf)canvasBackgroundColor.b, (GLclampf)canvasBackgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void VisualGraphics::clearSurfaceBackground() {
	glClearColor((GLclampf)surfaceBackgroundColor.r, (GLclampf)surfaceBackgroundColor.g, (GLclampf)surfaceBackgroundColor.b, (GLclampf)surfaceBackgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}



void VisualGraphics::setPixelStorageParams() {
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#if TARGET_OS_MAC
	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_FALSE);
#endif
}


void VisualGraphics::resetModelViewMatrix() {
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glTranslated(0.0, 0.0, 0.0);
}


void VisualGraphics::translateMatrix(double xNum, double yNum, double zNum) {
	glTranslated(xNum, yNum, zNum);
}


void VisualGraphics::rotateMatrix(double angle, double xAmount, double yAmount, double zAmount) {
	glRotated(angle, xAmount, yAmount, zAmount);
}


void VisualGraphics::scaleMatrix(double xFactor, double yFactor, double zFactor) {
	glScaled(xFactor, yFactor, zFactor);
}


void VisualGraphics::enableTexturing(bool useRectExtension) {
	if (useRectExtension == false) {
        glEnable(GL_TEXTURE_2D);
    } else {
#if TARGET_OS_MAC
        glEnable(GL_TEXTURE_RECTANGLE_EXT);
#endif
	}
}


void VisualGraphics::disableTexturing(bool useRectExtension) {
    if (useRectExtension == false) {
        glDisable(GL_TEXTURE_2D);
	} else {
#if TARGET_OS_MAC
        glDisable(GL_TEXTURE_RECTANGLE_EXT);
#endif
    }
}


void VisualGraphics::bindTexture(uint32 textureName, bool useRectExtension) {
	if (useRectExtension == false) {
		glBindTexture(GL_TEXTURE_2D, (GLuint)textureName);
	} else {
#if TARGET_OS_MAC
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, (GLuint)textureName);
#endif
	}
}


void VisualGraphics::getPixelsOfCurrentTexture(bool useRectExtension, uint16 format, uint16 type, uint32** buffer) {
	if (useRectExtension == false) {
		glGetTexImage(GL_TEXTURE_2D, 0, format, type, *buffer);
	} else {
#if TARGET_OS_MAC
		glGetTexImage(GL_TEXTURE_RECTANGLE_EXT, 0, format, type, *buffer);
#endif
	}
}


void VisualGraphics::enableAlphaBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void VisualGraphics::disableBlending() {
	glDisable(GL_BLEND);
}


void VisualGraphics::setColor(const RGBAColor& theColor) {
	glColor4d(theColor.r, theColor.g, theColor.b, theColor.a);
}


void VisualGraphics::setLineWidth(double width) {
	glLineWidth(static_cast<GLfloat>(width));
}


void VisualGraphics::enableDepthTest() {
	glEnable(GL_DEPTH_TEST);
}


void VisualGraphics::disableDepthTest() {
	glDisable(GL_DEPTH_TEST);
}


void VisualGraphics::enableCullFace() {
	glEnable(GL_CULL_FACE);
}


void VisualGraphics::disableCullFace() {
	glDisable(GL_CULL_FACE);
}


void VisualGraphics::cullFaceFront() {
	glCullFace(GL_FRONT);
}


void VisualGraphics::cullFaceBack() {
	glCullFace(GL_BACK);
}


uint8 VisualGraphics::getOpenGLError (char* outErrorString) {
    GLenum glErrorNum;
    const GLubyte *glErrorString;  
	
    glErrorNum = glGetError();
    if (glErrorNum != GL_NO_ERROR) {
        glErrorString = gluErrorString(glErrorNum);
        strcpy(outErrorString, (char*)glErrorString);
        return 1;
    } else {
        return 0;
    }
}


uint16 VisualGraphics::setCurrentContext() {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	return theVisualGraphicsCore->setCurrentContext();
}


void VisualGraphics::finishGLDrawing() {
	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	theVisualGraphicsCore->finishGLDrawing();
}


int VisualGraphics::getCurrentColorBufferForPixelReadingOperations() {
	GLint currReadBuffer;
	glGetIntegerv(GL_READ_BUFFER, &currReadBuffer);
	return currReadBuffer;
}


int VisualGraphics::getCurrentColorBufferForPixelDrawingOperations() {
	GLint currDrawBuffer;
	glGetIntegerv(GL_DRAW_BUFFER, &currDrawBuffer);
	return currDrawBuffer;
}


void VisualGraphics::setColorBufferForPixelReadingOperations(int colorBuffer) {
	glReadBuffer(colorBuffer);
}


void VisualGraphics::setColorBufferForPixelDrawingOperations(int colorBuffer) {
	glDrawBuffer(colorBuffer);
}


void VisualGraphics::drawTexture(uint32 textureNumber, const VertexChain* const vertexChain, bool canUseRectExtension, BlendMode aBlendMode, bool debug) {
    
	if (aBlendMode == kBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
	
	if (canUseRectExtension == false) {
        glEnable(GL_TEXTURE_2D);
    } else {
#if TARGET_OS_MAC
        glEnable (GL_TEXTURE_RECTANGLE_EXT);
#endif
	}
	
	if (canUseRectExtension == false) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    } else {
#if TARGET_OS_MAC
		glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
#endif
	}
	
	if (canUseRectExtension == false) {
        glBindTexture (GL_TEXTURE_2D, (GLuint)textureNumber);
    } else {
#if TARGET_OS_MAC
        glBindTexture (GL_TEXTURE_RECTANGLE_EXT, (GLuint)textureNumber);
#endif
    }
    
	if (aBlendMode == kBlend) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	
	glBegin(GL_POLYGON);
	
	for (ConstVertexChainConstIterator it = vertexChain->begin(); it != vertexChain->end(); it++) {
		glColor4d((*it)->vertexColor.r, (*it)->vertexColor.g, (*it)->vertexColor.b, (*it)->vertexColor.a);
		if (debug == true) {
			char logStr[128];
			sprintf(logStr, "r: %f, g: %f, b: %f, a: %f\n", (*it)->vertexColor.r, (*it)->vertexColor.g, (*it)->vertexColor.b, (*it)->vertexColor.a);
			writeLog(logStr);
		}
		glTexCoord2d((*it)->texCoordPosition.coord.s, (*it)->texCoordPosition.coord.t);
		glVertex3d((*it)->vertexPosition.coord.x, (*it)->vertexPosition.coord.y, (*it)->vertexPosition.coord.z);
		if (debug == true) {
			char logStr[128];
			sprintf(logStr, "x: %f, y: %f, z: %f\n", (*it)->vertexPosition.coord.x, (*it)->vertexPosition.coord.y, (*it)->vertexPosition.coord.z);
			writeLog(logStr);
		}
	}
	
    glEnd();
	
    if (canUseRectExtension == false) {
        glDisable(GL_TEXTURE_2D);
    } else {
#if TARGET_OS_MAC
        glDisable(GL_TEXTURE_RECTANGLE_EXT);
#endif
    }
	
	if (aBlendMode == kBlend) {
		glDisable(GL_BLEND);
	}
	
}


void VisualGraphics::drawVertexChain(const VertexChain& vertexChain, int drawMode, BlendMode aBlendMode) {
	if (aBlendMode == kBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
	glBegin(drawMode);
	for (ConstVertexChainConstIterator it = vertexChain.begin(); it != vertexChain.end(); it++) {
		if ((*it)->colorHasBeenSet() == true) {
			glColor4d((*it)->vertexColor.r, (*it)->vertexColor.g, (*it)->vertexColor.b, (*it)->vertexColor.a);
		}
		glVertex3d((*it)->vertexPosition.coord.x, (*it)->vertexPosition.coord.y, (*it)->vertexPosition.coord.z);
		//char coordLogStr[128];
		//sprintf(coordLogStr, "x: %f, y: %f, z: %f\n", (*it)->vertexPosition.coord.x, (*it)->vertexPosition.coord.y, (*it)->vertexPosition.coord.z);
		//writeLog(coordLogStr);
	}
    glEnd();
	if (aBlendMode == kBlend) {
		glDisable(GL_BLEND);
	}
}


void VisualGraphics::drawDebugVertexChain(const VertexChain& vertexChain, const VisualCamera& aCamera) {
	glColor3d(1.0, 0.0, 0.0);
	glLineWidth(3.0f);
	glBegin (GL_LINE_LOOP);
	for (ConstVertexChainConstIterator it = vertexChain.begin(); it != vertexChain.end(); it++) {
		glVertex3d((*it)->vertexPosition.coord.x, (*it)->vertexPosition.coord.y, (*it)->vertexPosition.coord.z);
	}
	glEnd();
	
	Coord coord;
	glTranslated(0.0, 0.0, 0.0);
	glColor3d(1.0, 1.0, 0.0);
	char posStr[64];
	for (ConstVertexChainConstIterator it = vertexChain.begin(); it != vertexChain.end(); it++) {
		sprintf(posStr, "tex.x:%.2f, tex.y:%.2f", (*it)->texCoordPosition.coord.s, (*it)->texCoordPosition.coord.t);
		coord.x = (*it)->vertexPosition.coord.x;
		coord.y = (*it)->vertexPosition.coord.y + this->yPixelToCoord(13, aCamera);;
		coord.z = (*it)->vertexPosition.coord.z;
		showProcessInfoRow(coord, posStr);
		sprintf(posStr, "x:%.2f, y:%.2f, z:%.2f", (*it)->vertexPosition.coord.x, (*it)->vertexPosition.coord.y, (*it)->vertexPosition.coord.z);
		coord.y = (*it)->vertexPosition.coord.y;
		showProcessInfoRow(coord, posStr);
	}
}


void VisualGraphics::drawPixels(PixelColor** pixelData, double xCoord, double yCoord, uint32 width, uint32 height, uint16 format, uint16 dataType, const VisualConvolutionFilter* const aConvolutionFilter) {
	
	bool useGLConvolutionFilter = false;
	GLfloat* kernel = NULL;
	
#if TARGET_OS_WIN
	// Declare function pointers
	PFNGLCONVOLUTIONFILTER2DPROC glConvolutionFilter2D;
	PFNGLCONVOLUTIONPARAMETERIPROC glConvolutionParameteri;
	PFNGLCONVOLUTIONPARAMETERFVPROC glConvolutionParameterfv;
#endif
	
	if (aConvolutionFilter != NULL) {
#if TARGET_OS_WIN
		// Obtain the address of the extension entry points
		glConvolutionFilter2D = (PFNGLCONVOLUTIONFILTER2DPROC)wglGetProcAddress("glConvolutionFilter2D"); 
		glConvolutionParameteri = (PFNGLCONVOLUTIONPARAMETERIPROC)wglGetProcAddress("glConvolutionParameteri");
		glConvolutionParameterfv = (PFNGLCONVOLUTIONPARAMETERFVEXTPROC)wglGetProcAddress("glConvolutionParameterfv");
		if (glConvolutionFilter2D) useGLConvolutionFilter = true;
#endif
#if TARGET_OS_MAC
		useGLConvolutionFilter = this->capabilities.fImaging;
#endif
	}
	
	//useGLConvolutionFilter = false; // test
	
	if (useGLConvolutionFilter == false && aConvolutionFilter != NULL) {
		PixelColor* filteredPixels = NULL;
		aConvolutionFilter->applyToPixelData(*pixelData, width, height, format, dataType, &filteredPixels);
		free(*pixelData);
		*pixelData = filteredPixels;
	}
	
	if (useGLConvolutionFilter == true) {
		kernel = (GLfloat*)malloc(aConvolutionFilter->getNumberOfKernelValueColumns() * aConvolutionFilter->getNumberOfKernelValueRows() * sizeof(GLfloat));
		aConvolutionFilter->copyKernelValues(kernel);
		
		GLfloat borderColor[4];
		borderColor[0] = 0.0f;
		borderColor[1] = 0.0f;
		borderColor[2] = 0.0f;
		borderColor[3] = 0.0f;
		
#if TARGET_OS_WIN
		// Call the function via function pointer
		(*glConvolutionFilter2D)(GL_CONVOLUTION_2D, GL_LUMINANCE, aConvolutionFilter->getNumberOfKernelValueColumns(), aConvolutionFilter->getNumberOfKernelValueRows(), GL_LUMINANCE, GL_FLOAT, kernel);
		(*glConvolutionParameteri) (GL_CONVOLUTION_2D, GL_CONVOLUTION_BORDER_MODE, GL_CONSTANT_BORDER);
		(*glConvolutionParameterfv) (GL_CONVOLUTION_2D, GL_CONVOLUTION_BORDER_COLOR, borderColor);
#else
		glConvolutionFilter2D(GL_CONVOLUTION_2D, GL_LUMINANCE, aConvolutionFilter->getNumberOfKernelValueColumns(), aConvolutionFilter->getNumberOfKernelValueRows(), GL_LUMINANCE, GL_FLOAT, kernel);
		glConvolutionParameteri(GL_CONVOLUTION_2D, GL_CONVOLUTION_BORDER_MODE, GL_CONSTANT_BORDER);
		glConvolutionParameterfv(GL_CONVOLUTION_2D, GL_CONVOLUTION_BORDER_COLOR, borderColor);
#endif
		// glConvolutionParameterfv(GL_CONVOLUTION_2D, GL_CONVOLUTION_FILTER_SCALE, filterScale);
		
		glEnable(GL_CONVOLUTION_2D);
		
		glPixelTransferf(GL_POST_CONVOLUTION_RED_SCALE, aConvolutionFilter->getPostConvolutionScaleFactor());
		glPixelTransferf(GL_POST_CONVOLUTION_GREEN_SCALE, aConvolutionFilter->getPostConvolutionScaleFactor());
		glPixelTransferf(GL_POST_CONVOLUTION_BLUE_SCALE, aConvolutionFilter->getPostConvolutionScaleFactor());
		glPixelTransferf(GL_POST_CONVOLUTION_ALPHA_SCALE, aConvolutionFilter->getPostConvolutionScaleFactor());
		
		glPixelTransferf(GL_POST_CONVOLUTION_RED_BIAS, aConvolutionFilter->getPostConvolutionBias());
		glPixelTransferf(GL_POST_CONVOLUTION_GREEN_BIAS, aConvolutionFilter->getPostConvolutionBias());
		glPixelTransferf(GL_POST_CONVOLUTION_BLUE_BIAS, aConvolutionFilter->getPostConvolutionBias());
		glPixelTransferf(GL_POST_CONVOLUTION_ALPHA_BIAS, aConvolutionFilter->getPostConvolutionBias());
		
	}
	
	glRasterPos2d(xCoord, yCoord);
	
	/*
	 When the GL_ARB_imaging extension  is  supported,  there  are  distinct
	 raster  texture  coordinates for each texture unit. Each texture unit's
	 current raster texture coordinates are updated by glRasterPos.
	 */
	
	this->setPixelStorageParams();
	
	glDrawPixels(width, height, format, dataType, *pixelData);
	
	if (useGLConvolutionFilter == true) {
		glDisable(GL_CONVOLUTION_2D);
		free(kernel);
	}
	
}


void VisualGraphics::resample(uint16 formatIn, uint32 widthIn, uint32 heightIn, uint16 dataTypeIn, PixelColor* pixelDataIn, uint32 widthOut, uint32 heightOut, uint16 dataTypeOut, PixelColor** pixelDataOut) {
	
	this->setPixelStorageParams();
	gluScaleImage((GLenum)formatIn, (GLint)widthIn, (GLint)heightIn, (GLenum)dataTypeIn, (void*)pixelDataIn, (GLint)widthOut, (GLint)heightOut, (GLenum)dataTypeOut, (void*)(*pixelDataOut));
	return;
	
	/*
	 uint32 widthBlockRatio = (uint32)((double)widthIn / (double)widthOut);
	 uint32 heightBlockRatio = (uint32)((double)heightIn / (double)heightOut);
	 
	 uint32* pixelDataoutStore = *pixelDataOut;
	 
	 uint32 outPixelIdx = 0;
	 
	 uint32 heightIdx = 0;
	 uint32 startIdx = 0;
	 uint32 lastStartIdx = 0;
	 for (uint32 y = 0; y < heightOut; y++) {
	 heightIdx = y * heightBlockRatio;
	 for (uint32 x = 0; x < widthOut; x++) {
	 startIdx = (heightIdx * widthIn) + (x * widthBlockRatio);
	 if ((startIdx + widthBlockRatio) < ((heightIdx * widthIn) + widthIn)) {
	 VisualColorTools::getMeanPixelColor(pixelDataIn + startIdx, widthBlockRatio, pixelDataoutStore[y * widthOut + x]);
	 lastStartIdx = startIdx;
	 } else {
	 pixelDataoutStore[y * widthOut + x] = 0xff0000ff;
	 //VisualColorTools::getMeanPixelColor(pixelDataIn + lastStartIdx, widthBlockRatio, pixelDataoutStore[y * widthOut + x]);
	 }
	 }
	 }
	 */
}


void VisualGraphics::readPixels(double xCoord, double yCoord, uint32 width, uint32 height, uint32** pixelData, uint16 format, uint16 dataType, const VisualCamera& aCamera) {
	uint16 xPos, yPos;
	xPos = xCoordToPixel(xCoord, aCamera);
	yPos = yCoordToPixel(yCoord, aCamera);
	this->setPixelStorageParams();
	glReadPixels(xPos, yPos, width, height, format, dataType, *pixelData);
}


#if TARGET_OS_WIN
bool VisualGraphics::makeTextureOfStringWin(wchar_t* stringValue, 
											int stringValueLength, 
											uint32 textureNumber, 
											uint32 &textureWidth, 
											uint32 &textureHeight, 
											uint32 &imageWidth, 
											uint32 &imageHeight, 
											const char* const fontName, 
											uint16 fontSize, 
											uint8 red, 
											uint8 green, 
											uint8 blue,
											HorizontalAlignment alignment,
											uint32 maxPixelWidth,
											uint32 maxPixelHeight) {
	//#define useGetDIBits
	
	bool success = true;
	
	LOGFONT lFont;
	HDC hdcTemp;
#ifndef useGetDIBits
	BITMAPV5HEADER bi;
#endif
	BYTE* bitmapBits = NULL;
	HBITMAP hbmpTemp;
	HFONT trackTitleFont;
	uint32 stringSizeWidth = 0;
	uint32 stringSizeHeight = 0;
	
	char errStr[256];
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	UINT textFormat = (DT_EXPANDTABS|DT_NOPREFIX);
	switch (alignment) {
		case (kLeftAligned):
			textFormat |= DT_LEFT;
			break;
		case (kCenterAligned):
			textFormat |= DT_CENTER;
			break;
		case (kRightAligned):
			textFormat |= DT_RIGHT;
			break;
		default:
			sprintf(errStr, "unknown switch case (%d) in file: %s (line: %d) [%s])", alignment, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errStr);
	}
	
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	
	hdcTemp = CreateCompatibleDC(theVisualGraphicsCore->getWindowDC());
	
	ZeroMemory(&lFont,sizeof(LOGFONT));
	lFont.lfHeight = MulDiv(fontSize, GetDeviceCaps(hdcTemp, LOGPIXELSY), 72);
	lFont.lfWidth = 0;
	lFont.lfEscapement = 0;
	lFont.lfOrientation = 0;
	lFont.lfWeight = FW_DONTCARE;
	lFont.lfItalic = FALSE;
	lFont.lfUnderline = FALSE;
	lFont.lfStrikeOut = FALSE;
	lFont.lfCharSet = DEFAULT_CHARSET;
	lFont.lfOutPrecision = OUT_OUTLINE_PRECIS;
	lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lFont.lfQuality = ANTIALIASED_QUALITY;
	lFont.lfPitchAndFamily = DEFAULT_PITCH;
	if (strlen(fontName) > 32) {
		sprintf(errStr, "length of fontName must not exceed 32 characters but is %d in file: %s (line: %d) [%s])", strlen(fontName), __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		strcpy(lFont.lfFaceName, "Arial"); // fallback font name
	} else {
		strcpy(lFont.lfFaceName, fontName); // length of fontName must not exceed 32 characters (incl. null terminator)
	}
	
	trackTitleFont = CreateFontIndirect(&lFont);
	
	if (!trackTitleFont) {
		sprintf(errStr, "Err after CreateFontIndirect() in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
	}
	
	SelectObject(hdcTemp, trackTitleFont);
	SetTextColor(hdcTemp, RGB((BYTE)red, (BYTE)green, (BYTE)blue));
	
	RECT textDrawRect;
	textDrawRect.top = 0;
	textDrawRect.left = 0;
	textDrawRect.right = this->getCanvasPixelWidth();
	textDrawRect.bottom = 0;
	int drawTextResult = 0;
	drawTextResult = DrawTextW(hdcTemp, stringValue, stringValueLength, &textDrawRect, DT_CALCRECT|textFormat);
	
	if (textDrawRect.bottom == 0) {
		sprintf(errStr, "Err (textDrawRect.bottom == 0) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
	}
	if (drawTextResult == 0) {
		sprintf(errStr, "Err (drawTextResult == 0) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return false;
	}

	stringSizeWidth = textDrawRect.right - textDrawRect.left;
	stringSizeHeight = textDrawRect.bottom - textDrawRect.top;
	
	//SIZE stringSize;
	uint16 aFontSize = fontSize;
	if ((maxPixelWidth > 0) && (maxPixelHeight > 0)) {
		while ((stringSizeWidth > maxPixelWidth) || (stringSizeHeight > maxPixelHeight)) {
			lFont.lfHeight = MulDiv(aFontSize, GetDeviceCaps(hdcTemp, LOGPIXELSY), 72);
			DeleteObject(trackTitleFont);
			trackTitleFont = CreateFontIndirect(&lFont);
			SelectObject(hdcTemp, trackTitleFont);
			/*
			BOOL textExtendSuccess = GetTextExtentPoint32W(hdcTemp, stringValue, stringValueLength, &stringSize);
			if (!textExtendSuccess) {
				sprintf(errStr, "Err (!textExtendSuccess) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errStr);
				return false;
			}
			*/
			//stringSizeWidth = stringSize.cx;
			//stringSizeHeight = stringSize.cy;
			drawTextResult = DrawTextW(hdcTemp, stringValue, stringValueLength, &textDrawRect, DT_CALCRECT|textFormat);
			if (drawTextResult == 0) {
				sprintf(errStr, "Err (drawTextResult == 0) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errStr);
				return false;
			}
			stringSizeWidth = textDrawRect.right - textDrawRect.left;
			stringSizeHeight = textDrawRect.bottom - textDrawRect.top;
			aFontSize = aFontSize - 1;
			if (aFontSize == 0) {
				sprintf(errStr, "Err (aFontSize == 0) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errStr);
				return false;
			}
		}
	} else if (maxPixelWidth > 0) {
		while (stringSizeWidth > maxPixelWidth) {
			lFont.lfHeight = MulDiv(aFontSize, GetDeviceCaps(hdcTemp, LOGPIXELSY), 72);
			DeleteObject(trackTitleFont);
			trackTitleFont = CreateFontIndirect(&lFont);
			SelectObject(hdcTemp, trackTitleFont);
			/*
			BOOL textExtendSuccess = GetTextExtentPoint32W(hdcTemp, stringValue, stringValueLength, &stringSize);
			if (!textExtendSuccess) {
				sprintf(errStr, "Err (!textExtendSuccess) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errStr);
				return false;
			}
			*/
			//stringSizeWidth = stringSize.cx;
			//stringSizeHeight = stringSize.cy;
			drawTextResult = DrawTextW(hdcTemp, stringValue, stringValueLength, &textDrawRect, DT_CALCRECT|textFormat);
			if (drawTextResult == 0) {
				sprintf(errStr, "Err (drawTextResult == 0) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errStr);
				return false;
			}
			stringSizeWidth = textDrawRect.right - textDrawRect.left;
			stringSizeHeight = textDrawRect.bottom - textDrawRect.top;
			aFontSize = aFontSize - 1;
			if (aFontSize == 0) {
				sprintf(errStr, "Err (aFontSize == 0) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errStr);
				return false;
			}
		}
	} else if (maxPixelHeight > 0) {
		while (stringSizeHeight > maxPixelHeight) {
			lFont.lfHeight = MulDiv(aFontSize, GetDeviceCaps(hdcTemp, LOGPIXELSY), 72);
			DeleteObject(trackTitleFont);
			trackTitleFont = CreateFontIndirect(&lFont);
			SelectObject(hdcTemp, trackTitleFont);
			/*
			BOOL textExtendSuccess = GetTextExtentPoint32W(hdcTemp, stringValue, stringValueLength, &stringSize);
			if (!textExtendSuccess) {
				sprintf(errStr, "Err (!textExtendSuccess) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errStr);
				return false;
			}
			*/
			//stringSizeWidth = stringSize.cx;
			//stringSizeHeight = stringSize.cy;
			drawTextResult = DrawTextW(hdcTemp, stringValue, stringValueLength, &textDrawRect, DT_CALCRECT|textFormat);
			if (drawTextResult == 0) {
				sprintf(errStr, "Err (drawTextResult == 0) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errStr);
				return false;
			}
			stringSizeWidth = textDrawRect.right - textDrawRect.left;
			stringSizeHeight = textDrawRect.bottom - textDrawRect.top;
			aFontSize = aFontSize - 1;
			if (aFontSize == 0) {
				sprintf(errStr, "Err (aFontSize == 0) in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errStr);
				return false;
			}
		}
	}

#ifndef useGetDIBits
	
	ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
	bi.bV5Size = sizeof(BITMAPV5HEADER);
	bi.bV5Width = stringSizeWidth;
	bi.bV5Height = stringSizeHeight;
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;
	
	// supported 32 BPP alpha format for Windows XP (ARGB)
	bi.bV5AlphaMask = 0xFF000000;
	bi.bV5RedMask = 0x00FF0000;
	bi.bV5GreenMask = 0x0000FF00;
	bi.bV5BlueMask = 0x000000FF;
	
	hbmpTemp = CreateDIBSection(hdcTemp, (BITMAPINFO *)&bi, DIB_RGB_COLORS, (void**)&bitmapBits, NULL, (DWORD)0);
	
	if (!hbmpTemp) {
		DeleteDC(hdcTemp);
		ReleaseDC(theVisualGraphicsCore->getGraphicsDevicePort(), hdcTemp);
		sprintf(errStr, "err after CreateDIBSection() in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return false;
	}
	
#endif
	
#ifdef useGetDIBits
	
	hbmpTemp = CreateCompatibleBitmap(theVisualGraphicsCore->getWindowDC(), stringSizeWidth, stringSizeHeight);
	if (!hbmpTemp) {
		sprintf(errStr, "err after CreateCompatibleBitmap() in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
	}
	
#endif
	
	SelectObject(hdcTemp, hbmpTemp);
	SetBkMode(hdcTemp, TRANSPARENT);
	
	SelectObject(hdcTemp, trackTitleFont);
	SetBkMode(hdcTemp, TRANSPARENT);
	
	SetTextColor(hdcTemp, RGB((BYTE)red, (BYTE)green, (BYTE)blue));
	SetTextAlign(hdcTemp, TA_TOP | TA_LEFT);
	
	DrawTextW(hdcTemp, stringValue, stringValueLength, &textDrawRect, textFormat);
	
	GdiFlush();
	
#ifdef useGetDIBits
	
	bitmapBits = (BYTE*)malloc(stringSizeWidth * stringSizeHeight * sizeof(PixelColor));
	
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = stringSizeWidth;
	bmi.bmiHeader.biHeight = stringSizeHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	
	GetDIBits(hdcTemp, hbmpTemp, 0, stringSizeHeight, bitmapBits, &bmi, DIB_RGB_COLORS);
	
#endif
	
	// set alpha bits
	// alpha = 1 where not empty (where text has been rendered)
	// alpha = 0 where no text

	 DWORD x,y;
	 DWORD* lpdwPixels;
	 lpdwPixels = (DWORD*)bitmapBits;
	 for (x = 0; x < stringSizeWidth; x++) {
		for (y = 0; y < stringSizeHeight; y++) {
			if ((*lpdwPixels & 0x00FFFFFF) != 0x00000000) {
				*lpdwPixels |= 0xFF000000;
			} else {
				*lpdwPixels &= 0x00FFFFFF;
			}
			lpdwPixels++;
		}
	 }

	const PixelColor* pixels = (PixelColor*)bitmapBits;
	this->copyARGBBitmapDataToTexture(textureNumber, stringSizeWidth, stringSizeHeight, this->canUseTextureRectExtension(), &pixels);

	DeleteObject(trackTitleFont);
	
#ifdef useGetDIBits
	free(bitmapBits);
#endif
	
	DeleteObject(hbmpTemp);
	
	DeleteDC(hdcTemp);

	if (this->canUseTextureRectExtension() == false) {
		textureWidth = this->power2Ceiling((uint32)stringSizeWidth);
		textureHeight = this->power2Ceiling((uint32)stringSizeHeight);
	} else {
		textureWidth = (uint32)stringSizeWidth;
		textureHeight = (uint32)stringSizeHeight;
	}
	imageWidth = (uint32)stringSizeWidth;
	imageHeight = (uint32)stringSizeHeight;

	return success;
}
#endif


void VisualGraphics::drawSpot(const double xNum, const double yNum, const double r, const double g, const double b, uint16 waveformIntensityVal, double intensity, const uint8 tailSize) {
	
    float maxRadius = 0.3f;
    int slices = 40;
    int i;
    float scaleRatio;
    //float xNum, yNum, r, g, b;
    //uint8 waveformIntensityVal;
    //float intensity;
    float rotationVal=3.0f;
    uint8 numberOfIterations;
    
    /* zentrale Methode zum Malen eines Lichtpunktes */
    /* xNum und yNum : Position */
    /* r, g, b : Farbe */
    /* waveformIntensityVal : Umfang (0-100) */
    /* intensity : alphaVal (0.0 - 1.0) */
    
    //xNum = pLichtpunkt->xPos;
    //yNum = pLichtpunkt->yPos;
    //r = pLichtpunkt->redVal;
    //g = pLichtpunkt->greenVal;
    //b = pLichtpunkt->blueVal;
    //waveformIntensityVal = pLichtpunkt->waveformIntensityVal;
    //intensity = pLichtpunkt->lifetimeIntensity;
	
	
    if (waveformIntensityVal>100) {
    	waveformIntensityVal = 100; // max
    }
	
    if (intensity > 1.0) {
        intensity = 1.0; //max
    }
    
    intensity = intensity/2.2f; // max alphaVal ist < 1.0
    
    slices = (int)((float)slices * ((float)waveformIntensityVal/100.0));
    
    scaleRatio = (float)waveformIntensityVal/100.0f;
    
    
    if (tailSize == 0) {
        numberOfIterations = 1;
    } else {
        numberOfIterations = tailSize;
    }
    
    for (int m = 1; m <= numberOfIterations; m++) {
        rotationVal -=4.0f;
		glLoadIdentity();
		
        glScaled(1.0, 1.0, 1.0);
        glRotated(rotationVal, 0.0, 0.0, 1.0);
        glTranslated(xNum, yNum, 0.0);
		
        glScaled(scaleRatio, scaleRatio, scaleRatio);
		
        // Aussenschein
        glBegin (GL_TRIANGLE_FAN);
		
		
		//    if (intensity>0.8) {
		//    	// das Licht scheint langsam auf
		//    	// (und startet nicht mit full power)
		//    	glColor4f(r,g,b, (1.0 - intensity));
		//    } else {
		//    	glColor4f(r,g,b, intensity*(1.0/0.8));
		//    }
        
        
		//glColor4f(r,g,b, intensity);
		glColor4d(r, g, b, intensity/m);
        
		glVertex2d(0.0, 0.0);
        
		glColor4d(r, g, b, 0.0); //glColor4f(r,g,b, intensity*(intensity/2.0)); // aufblenden (test)
        
		for (i = 0; i <= slices; i++) {
			glVertex2f (maxRadius * (float)(cos ((2.0f * (float)M_PI) * (float) i/(float) slices)),
						maxRadius * (float)(sin ((2.0f * (float)M_PI) * (float) i/(float) slices)));
		}
		
        glEnd();
		
        if (m==1) {
            // (nur der Kopf hat einen Kern)
            // Kern
            glBegin (GL_TRIANGLE_FAN);
			
			//glColor4f (1.0, 1.0, 1.0, (intensity/3.0)); // weiss
			glColor4d(1.0, 1.0, 1.0, (intensity/3.0/m)); // weiss
			glVertex2d(0.0, 0.0);
			
			glColor4d(r, g, b, 0.0);
			
			for (i = 0; i <= slices; i++) {
				glVertex2f ((maxRadius/4.0f) * (float)(cos ((2.0 * (float)M_PI) * (float) i/(float) slices)),
                            (maxRadius/4.0f) * (float)(sin ((2.0f * (float)M_PI) * (float) i/(float) slices)));
			}
			
            glEnd();
        }
    }
	
}


void VisualGraphics::drawTriangle() {
	
	GLfloat topPos = 1.0f;
	static GLfloat red;
	GLfloat green;
	GLfloat blue;
	
	static GLfloat rotateAngle = 0.0f;
	
	red = red + 0.003f;
    
    green = red;
    blue = 1.0f - red;
    
    rotateAngle = rotateAngle + 0.3f;
    if (rotateAngle > 360.0) {
        rotateAngle = 0.0f;
    }
	
    glRotatef(rotateAngle,0.0f,1.0f,0.0f);
	
    glBegin(GL_TRIANGLES);
	glColor3d(red,0.0,0.0);
	glVertex2d(0.0, topPos);
	glColor3d(0.0,green,0.0);
	glVertex2d(-1.0,-1.0);
	glColor3d(0.0,0.0,blue);
	glVertex2d( 1.0,-1.0);
	
    glEnd();
	
}


void VisualGraphics::spotGL(double zPlane) {
	glEnable(GL_POINT_SMOOTH);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3d(1.0, 1.0, 0.0);
	glVertex3d( 0.0, 0.0, zPlane);
	glEnd();
	glDisable(GL_POINT_SMOOTH);
}


void VisualGraphics::drawProjectionMetrics(const VisualCamera& aCamera) {
	// some grey numbers along the axes of the projection space
	uint32 numberOfMarks = 10;
	glColor4d(0.4, 0.4, 0.4, 1.0);
	char str[32];
	double distance = aCamera.getDepth();
	double nearPos = aCamera.getMaxNearPos() * -1.0;
	double farPos = aCamera.getMaxFarPos() * -1.0;
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxLeftCoord(), aCamera.getMaxBottomCoord(), nearPos - ((distance / numberOfMarks) * i));
		sprintf(str, "%.2f", nearPos - ((distance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxRightCoord(), aCamera.getMaxBottomCoord(), nearPos - ((distance / numberOfMarks) * i));
		sprintf(str, "%.2f", nearPos - ((distance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxLeftCoord(), aCamera.getMaxTopCoord(), nearPos - ((distance / numberOfMarks) * i));
		sprintf(str, "%.2f", nearPos - ((distance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxRightCoord(), aCamera.getMaxTopCoord(), nearPos - ((distance / numberOfMarks) * i));
		sprintf(str, "%.2f", nearPos - ((distance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	double verticalDistance = aCamera.getCoordHeight();
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxLeftCoord(), aCamera.getMaxBottomCoord() + ((verticalDistance / numberOfMarks) * i), nearPos);
		sprintf(str, "%.2f", aCamera.getMaxBottomCoord() + ((verticalDistance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxRightCoord() - 0.2, aCamera.getMaxBottomCoord() + ((verticalDistance / numberOfMarks) * i), nearPos);
		sprintf(str, "%.2f", aCamera.getMaxBottomCoord() + ((verticalDistance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	double horizontalDistance = aCamera.getCoordWidth();
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxLeftCoord() + ((horizontalDistance / numberOfMarks) * i), aCamera.getMaxBottomCoord(), nearPos);
		sprintf(str, "%.2f", aCamera.getMaxLeftCoord() + ((horizontalDistance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxLeftCoord() + ((horizontalDistance / numberOfMarks) * i), aCamera.getMaxTopCoord() - 0.1, nearPos);
		sprintf(str, "%.2f", aCamera.getMaxLeftCoord() + ((horizontalDistance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxLeftCoord(), aCamera.getMaxBottomCoord() + ((verticalDistance / numberOfMarks) * i), farPos);
		sprintf(str, "%.2f", aCamera.getMaxBottomCoord() + ((verticalDistance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxRightCoord() - 0.2, aCamera.getMaxBottomCoord() + ((verticalDistance / numberOfMarks) * i), farPos);
		sprintf(str, "%.2f", aCamera.getMaxBottomCoord() + ((verticalDistance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxLeftCoord() + ((horizontalDistance / numberOfMarks) * i), aCamera.getMaxBottomCoord(), farPos);
		sprintf(str, "%.2f", aCamera.getMaxLeftCoord() + ((horizontalDistance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
	for (uint32 i = 0; i < numberOfMarks; i++) {
		glRasterPos3d(aCamera.getMaxLeftCoord() + ((horizontalDistance / numberOfMarks) * i), aCamera.getMaxTopCoord(), farPos);
		sprintf(str, "%.2f", aCamera.getMaxLeftCoord() + ((horizontalDistance / numberOfMarks) * i));
		this->drawCStringWithGL(str, strlen(str));
	}
}


Coord VisualGraphics::getCirclePoint(uint32 sliceIdx, uint32 slicesCount, double radius, Coord circleCenter) {
	Coord coordPoint;
	coordPoint.x = cos((2.0 * M_PI) * static_cast<double>(sliceIdx)/static_cast<double>(slicesCount)) * radius;
	coordPoint.x += circleCenter.x;
	coordPoint.y = sin((2.0 * M_PI) * static_cast<double>(sliceIdx)/static_cast<double>(slicesCount)) * radius;
	coordPoint.y += circleCenter.y;
	return coordPoint;
}


void VisualGraphics::drawBeatHistogram(const uint32* const beatHistogram, const VisualCamera& aCamera) {
    GLdouble xPos;
    GLdouble xPosIncVal;
	GLdouble height;
	uint32 maxval=0;
	uint8 i;
	for (i = 0; i < 100; i++) {
		if (beatHistogram[i] > maxval) {
			maxval = beatHistogram[i];
		}
	}
	xPos = aCamera.getMaxLeftCoord() + 0.2;
	xPosIncVal = ((aCamera.getMaxRightCoord() - aCamera.getMaxLeftCoord()) / 2.0 / 100.0);
	height = aCamera.getMaxTopCoord() - aCamera.getMaxBottomCoord();
	height /= 3.5;
	//glLoadIdentity();
	glLineWidth(1.0f);
	glColor3d(1.0, 1.0, 0.0);
	//glTranslatef(0.5f, 0.0f, 0.0f);
	glBegin (GL_LINE_STRIP);
	xPos = xPos + xPosIncVal;
	for (i = 0; i < 100; i++) {
		glVertex2d(xPos, (height * ((GLdouble)beatHistogram[i] / (GLdouble)maxval)));
		xPos = xPos + xPosIncVal;
		glVertex2d(xPos, (height * ((GLdouble)beatHistogram[i] / (GLdouble)maxval)));
	}
	glEnd();
}


bool VisualGraphics::canUseTextureRectExtension() {
#if TARGET_OS_MAC
	/*
	 printf(this->capabilities.strRendererName);
	 printf("\n");
	 printf(this->capabilities.strRendererVendor);
	 printf("\n");
	 printf(this->capabilities.strRendererVersion);
	 printf("\n");
	 if (this->capabilities.glVersion >= 0x0140) {
	 printf("Open GL 1.4\n");
	 } else if (this->capabilities.glVersion >= 0x0130) {
	 printf("Open GL 1.3\n");
	 } else if (this->capabilities.glVersion >= 0x0120) {
	 printf("Open GL 1.2\n");
	 } else if (this->capabilities.glVersion >= 0x0110) {
	 printf("Open GL 1.1\n");
	 } else if (this->capabilities.glVersion >= 0x0100) {
	 printf("Open GL 1.0\n");
	 } else {
	 printf("Open GL ?.?");
	 }
	 */	
	//return false; // test
	
	if (this->capabilities.fTexRect) {
		//printf("fTexRect true\n");
		return true;
	} else {
		//printf("fTexRect false\n");
		return false;
	}
	
	//const GLubyte* strExt;
	//strExt = glGetString (GL_EXTENSIONS);
	//return false;
	//return (gluCheckExtension ("GL_EXT_texture_rectangle", strExt));
#endif
#if TARGET_OS_WIN
	return false;
#endif
}


bool VisualGraphics::doesSupportGLConvolutionFilter() {
	bool useGLConvolutionFilter = false;
#if TARGET_OS_WIN
	// Declare function pointers
	PFNGLCONVOLUTIONFILTER2DPROC glConvolutionFilter2D;
	glConvolutionFilter2D = (PFNGLCONVOLUTIONFILTER2DPROC)wglGetProcAddress("glConvolutionFilter2D");
	//if (glConvolutionFilter2D) useGLConvolutionFilter = true;
	// Because we want to use the software path on Windows for convolution filter, 
	// we generally return false on Windows for now
#endif
#if TARGET_OS_MAC
	useGLConvolutionFilter = true;
#endif
	return useGLConvolutionFilter;
}


void VisualGraphics::drawWaveform(const sint16 historyNum, const uint16 maxNumberOfHistories, const uint32 numberOfWaveformEntries, sint16** waveformDataMonoArray, const VisualCamera& aCamera) {
    
	if (historyNum == -1) return;
	
	GLdouble heightOfWaveform;
	GLdouble widthOfWaveform;
	GLdouble bottomCoord;
	GLdouble rightCoord;
    sint16 historyIdx;
	uint32 i, k;
	
    GLdouble xPos;
    GLdouble xPosIncVal;
	
	CoordSize3D size = aCamera.getSize();
	widthOfWaveform = size.width - this->xPixelToCoord(300, aCamera) - this->xPixelToCoord(30, aCamera);
	heightOfWaveform = size.height / 7.0;
	bottomCoord = aCamera.getMaxBottomCoord() + this->yPixelToCoord(10, aCamera);
	rightCoord = aCamera.getMaxLeftCoord() + this->xPixelToCoord(10, aCamera) + widthOfWaveform;
	xPosIncVal = widthOfWaveform / ((float)numberOfWaveformEntries * (float)maxNumberOfHistories);
	
    glLineWidth(1.0f);
    glColor3d(0.7, 0.7, 0.7);
    
	glLoadIdentity();
	glTranslated(0.0, aCamera.getMaxBottomCoord() + this->xPixelToCoord(60, aCamera) + heightOfWaveform, 0.0f);
	
	// waveform history
	historyIdx = historyNum;
	xPos = rightCoord;
	for (i = 0; i < maxNumberOfHistories; i++) {
		glBegin(GL_LINE_STRIP);
		for (k = 0; k < numberOfWaveformEntries; k++) {
			//glVertex2f (xPos, static_cast<GLfloat>(waveformDataMonoArray[historyIdx * numberOfWaveformEntries + k]) / 128.0f * heightOfWaveform);
			//waveformDataMonoArray[historyIdx][k] = 128;
			glVertex2d(xPos, static_cast<GLdouble>(waveformDataMonoArray[historyIdx][k]) / 128.0 * heightOfWaveform);
			//glVertex2f (xPos, heightOfWaveform);
			xPos -= xPosIncVal;
		}
		glEnd();
		if ((historyIdx - 1) < 0) {
			historyIdx = maxNumberOfHistories - 1;
		} else {
			historyIdx--;
		}
	}
	
	// waveform of current waveform data chunk
	glLoadIdentity();
	glTranslated(0.0, aCamera.getMaxBottomCoord() + this->xPixelToCoord(10, aCamera) + (heightOfWaveform / 2.0), 0.0);
	
	xPos = rightCoord;
	xPosIncVal = widthOfWaveform / (float)numberOfWaveformEntries;
	glBegin (GL_LINE_STRIP);
	for (k = 0; k < numberOfWaveformEntries; k++) {
		//glVertex2f(xPos, static_cast<GLfloat>(waveformDataMonoArray[historyIdx * numberOfWaveformEntries + k]) / 128.0f * heightOfWaveform);
		//glVertex2f(xPos, static_cast<GLfloat>(waveformDataMonoArray[historyIdx * numberOfWaveformEntries + k]) / 128.0f * heightOfWaveform);
		glVertex2d(xPos, static_cast<GLdouble>(waveformDataMonoArray[historyNum][k]) / 128.0 * heightOfWaveform);
		xPos -= xPosIncVal;
	}
	glEnd();
    
}


void VisualGraphics::drawHistoryDiagram(std::vector<double>& aVector, size_t baseIdx, double minVal, double maxVal, const VisualCamera& aCamera) {
	
	bool drawBorder = true;
	
	theVisualGraphics = VisualGraphics::getInstance();
	
	CoordSize3D size = aCamera.getSize();
	GLdouble diagramCoordWidth = size.width / 3.0;
	GLdouble diagramCoordHeight = size.height / 7.0;
	GLdouble topCoord = aCamera.getMaxTopCoord() - theVisualGraphics->xPixelToCoord(10, aCamera);;
	GLdouble leftCoord = aCamera.getMaxLeftCoord() + theVisualGraphics->xPixelToCoord(10, aCamera);
	GLdouble xCoordStepWidth = diagramCoordWidth / static_cast<GLdouble>(aVector.size());
	
    glLineWidth(1.0f);
    glColor3d(0.0, 0.0, 1.0);
	
	glBegin (GL_LINE_STRIP);
	
	GLdouble xCoord = leftCoord + diagramCoordWidth;
	size_t count = 0;
	size_t currIdx = baseIdx;
	double valueRange = maxVal - minVal;
	while (count < aVector.size()) {
		
		glVertex2d(xCoord, topCoord - diagramCoordHeight + (static_cast<GLdouble>(aVector[currIdx] / valueRange * diagramCoordHeight)));
		xCoord -= xCoordStepWidth;
		
		if (currIdx == 0) {
			currIdx = (aVector.size() - 1);
		} else {
			currIdx--;
		}
		count++;
	}
	
	glEnd();
	
	if (drawBorder == true) {
		glLineWidth(1.0f);
		glColor4d(0.0, 0.0, 1.0, 0.7);
		glBegin (GL_LINE_LOOP);
		glVertex2d(leftCoord, topCoord - diagramCoordHeight);
		glVertex2d(leftCoord, topCoord);
		glVertex2d(leftCoord + diagramCoordWidth, topCoord);
		glVertex2d(leftCoord + diagramCoordWidth, topCoord - diagramCoordHeight);
		glEnd();
	}
	
}


void VisualGraphics::drawWaveformSpiral(const uint16 currHistoryNum, const uint16 numberOfWaveformEntries, const sint16* const waveformDataMonoArray) {
    // eine Spriale der waveform-History
	
	
	// neue Version der drawWaveformSpiral
	//20030129 (HW)
	//
	
    uint16 index;
    uint16 historyIndex;
    GLfloat waveformVal;
    static GLfloat cosTwoTimesPi = 0.0f;
    static GLfloat sinTwoTimesPi = 0.0f;
    // float radius = 0.25;
    float radius = 0.02f;
    float waveRadius;
    //uint8 waveformHistory[20][kVisualNumWaveformEntries];
    //uint16 timeStoreIndex;
    //static uint16 accuElapsedMilliSecs;
    uint32 accuElapsedMilliSecs;
    //uint8 oneSecondIsOverBool = 0;
    static sint8 redHistoryNum = 0;
	uint16 theCurrHistoryNum;
	
	theCurrHistoryNum = currHistoryNum;
	
    if (cosTwoTimesPi == 0.0) {
        cosTwoTimesPi = (float)(cos(2.0f * M_PI));
    }
	
    if (sinTwoTimesPi == 0.0) {
        sinTwoTimesPi = (float)(sin(2.0f * M_PI));
    }
    
    //currMusicDataHistoryNum = pMusicStatsStruct->getCurrMusicDataHistory();
	
    //timeStoreIndex = pTiming->storeMyTime("waveformSpiral");
    //accuElapsedMilliSecs += pTiming->getMyElapsedMilliseconds(timeStoreIndex);
    accuElapsedMilliSecs = VisualTiming::getElapsedMilliSecsSinceReset("waveformSpiral");
	
	/*
	 if (accuElapsedMilliSecs > 1000) {
	 oneSecondIsOverBool = 1;
	 //accuElapsedMilliSecs = 0;
	 //pTiming->clearMyTimeStore(timeStoreIndex);
	 pTiming->resetTimestamp("waveformSpiral");
	 redHistoryNum = theCurrHistoryNum;
	 } else {
	 redHistoryNum--;
	 if (redHistoryNum == -1) {
	 redHistoryNum = 20;
	 }
	 }
	 */	
	redHistoryNum--;
	if (redHistoryNum == -1) {
		redHistoryNum = 20;
	}
	
    //pMusicStatsStruct->getMonoWaveformData(waveformHistory);
	//pMusicStatsStruct->getMonoWaveformData((uint8*)waveformHistory, 20);
    
	glColor4d(0.0, 0.0, 0.0, 1.0);
    glLineWidth(1.0f);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin (GL_LINE_STRIP);
    
    for (historyIndex = 0; historyIndex < 20; historyIndex++) {
		
        theCurrHistoryNum ++;
        if (theCurrHistoryNum == 20) {
            theCurrHistoryNum = 0;
        }
		
        for (index = 0; index < numberOfWaveformEntries; index++) {
			
			waveformVal = waveformDataMonoArray[theCurrHistoryNum * numberOfWaveformEntries + index];
            
            radius = radius + 0.00009f;
            // radius = radius + (0.0004 * ((float)index2/18.0));
            // waveRadius = radius + (waveformVal/1024.0);
            waveRadius = radius + ((waveformVal/255.0f) * 0.1f);
            //waveRadius = radius + ((waveformVal/255.0)*0.6);
            if (historyIndex == redHistoryNum) {
                glColor4d(1.0, 0.0, 0.0, 1.0); // red
            } else {
                glColor4d(0.0, 0.0, 0.0, 1.0);
            }
            // nur jeder 10te Punkt wird gemalt
            if (index%10 == 0) {
				glVertex2f (waveRadius * (float)(cos ((2.0f * M_PI) * (float) index/(float) numberOfWaveformEntries)),
							waveRadius * (float)(sin ((2.0f * M_PI) * (float) index/(float) numberOfWaveformEntries)));
            }
			
        }
    }
    
    glEnd();
    glDisable (GL_BLEND);
	
}


void VisualGraphics::drawSpectrumAnalyzer(const sint16 currHistoryNum, const uint16 numberOfHistories, const uint32 numberOfSpectrumEntries, const uint16 numberOfAudioChannels, const uint8*** const spectrumDataArray, const VisualCamera& aCamera) {
    
	if (currHistoryNum == -1) return;
	
    GLdouble xPos;
	GLdouble yPos;
	GLdouble coordWidth;
	GLdouble coordHeight;
    uint16 index, i, k;
    GLdouble spectrumVal;
	uint16 numberOfSubBands = 15;
	float* spectrumDataSubBands;
	float maxSpectrumSum = 0;
	uint32 numberOfEntriesPerBarGraph;
	uint16 numberOfBlocksPerBarGraph = 10;
	GLdouble heightOfCell; // heightOfCell = heightOfBlock + heightOfGapBetweenBlocks
	GLdouble heightOfBlock;
	GLdouble heightOfGapBetweenBlocks;
	GLdouble widthOfCell;
	GLdouble widthOfBlock;
	
	spectrumDataSubBands = (float*)malloc(numberOfSubBands * sizeof(float));
	for (i = 0; i < numberOfSubBands; i++) {
		spectrumDataSubBands[i] = 0.0f;
	}
	numberOfEntriesPerBarGraph = numberOfSpectrumEntries / 2 / numberOfSubBands;
	
	// position on screen
	coordWidth = this->xPixelToCoord(300, aCamera);
	coordHeight = this->yPixelToCoord(70, aCamera);
	heightOfCell = coordHeight / (float)numberOfBlocksPerBarGraph;
	heightOfBlock = heightOfCell * 0.8;
	heightOfGapBetweenBlocks = heightOfCell * 0.2;
	widthOfCell = coordWidth / (float)numberOfSubBands;
	widthOfBlock = widthOfCell * 0.9;
	
	xPos = aCamera.getMaxRightCoord() - coordWidth;
	xPos -= this->xPixelToCoord(10, aCamera);
    
    glLineWidth(1.0f);
    glColor3d(0.7,0.7,0.7);
    
	glLoadIdentity();
	
	glPointSize(1.0f);
    
	glTranslated(0.0, aCamera.getMaxBottomCoord() + this->xPixelToCoord(10, aCamera), 0.0);
	
	// collect values for each subBand
	index = 0;
	for (i = 0; i < (numberOfSpectrumEntries / 2); i++) {
		if ((i > 0) && (i%numberOfEntriesPerBarGraph == 0)) index++;
		if (index < (numberOfSubBands - 1)) {
			//spectrumDataSubBands[index] += (float)spectrumDataArray[currHistoryNum * numberOfAudioChannels * numberOfSpectrumEntries + i + 0]; // channel 0
			spectrumDataSubBands[index] += (float)spectrumDataArray[currHistoryNum][0][i]; // channel 0
			//spectrumDataSubBands[index] += (float)10;
		}
	}
	
	for (i = 0; i < numberOfSubBands; i++) {
		if (spectrumDataSubBands[i] > maxSpectrumSum) {
			maxSpectrumSum = spectrumDataSubBands[i];
		}
	}
	for (i = 0; i < numberOfSubBands; i++) {
		spectrumDataSubBands[i] =  spectrumDataSubBands[i] / maxSpectrumSum; // maximum value 1.0
	}
    
	
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glBegin (GL_QUADS);
	for (i = 0; i < numberOfSubBands; i++) {
		spectrumVal = spectrumDataSubBands[i];
		for (k = 0; k < numberOfBlocksPerBarGraph; k++) {
			if ((spectrumVal * coordHeight) > (k * heightOfCell)) {
				glColor4d(0.0, 0.0, 0.0, 0.6);
			} else {
				glColor4d(0.8, 0.8, 0.8, 0.1);
			}
			yPos = (float)k * heightOfCell + (float)heightOfGapBetweenBlocks;
			glVertex2d(xPos, 0.0 + yPos);
			glVertex2d(xPos + widthOfBlock, 0.0 + yPos);
			glVertex2d(xPos + widthOfBlock, 0.0 + yPos + heightOfBlock);
			glVertex2d(xPos, 0.0 + yPos + heightOfBlock);
		}
		xPos += widthOfCell;
	}
	glEnd();
    glDisable (GL_BLEND);
	
	free(spectrumDataSubBands);
    
}


void VisualGraphics::drawSpectrogram(const sint16 currHistoryNum, const uint16 numberOfHistories, const uint32 numberOfSpectrumEntries, const uint16 numberOfAudioChannels, const uint8*** const spectrumDataArray, const VisualCamera& aCamera) {
    
	if (currHistoryNum == -1) return;
	
	GLdouble heightOfSpectrogram;
	GLdouble widthOfSpectrogram;
	GLdouble bottomCoord;
	GLdouble leftCoord;
	GLdouble rowCoordHeight;
	GLdouble xCoordStepSize;
	GLdouble xCoordPos;
	GLdouble yCoordPos;
	uint16 i;
	uint16 k;
	sint16 historyIdx;
	char strInC[32];
	
	CoordSize3D size = aCamera.getSize();
	widthOfSpectrogram = this->xPixelToCoord(300, aCamera);
	heightOfSpectrogram = size.height - this->yPixelToCoord(95, aCamera);
	bottomCoord = aCamera.getMaxBottomCoord() + this->yPixelToCoord(85, aCamera);
	leftCoord = aCamera.getMaxRightCoord() - widthOfSpectrogram - this->yPixelToCoord(10, aCamera);
	rowCoordHeight = heightOfSpectrogram / (float)numberOfHistories;
	xCoordStepSize = widthOfSpectrogram / (numberOfSpectrumEntries / 2);
	
    
	//glEnable (GL_BLEND);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
	sprintf(strInC, "%d", currHistoryNum);
	setProcessInfo ("CurrHistoryNum", strInC);
	
	yCoordPos = bottomCoord;
	historyIdx = currHistoryNum;
	
	for (i = 0; i < numberOfHistories; i++) {
		xCoordPos = leftCoord;
		glBegin(GL_TRIANGLE_STRIP);
		for (k = 0; k < (numberOfSpectrumEntries / 2); k++) {
			//glColor4f((float)spectrumDataArray[historyIdx * numberOfAudioChannels * numberOfSpectrumEntries + k + 0] / 255.0f, 0.0f, 0.0f, 0.8f);
			glColor4d((float)spectrumDataArray[historyIdx][0][k] / 255.0, 0.0, 0.0, 0.8);
			glVertex2d(xCoordPos, yCoordPos + rowCoordHeight);
			glVertex2d(xCoordPos, yCoordPos);
			xCoordPos+= xCoordStepSize;
		}
		glEnd();
		yCoordPos+= rowCoordHeight;
		if ((historyIdx - 1) < 0) {
			historyIdx = numberOfHistories - 1;
		} else {
			historyIdx--;
		}
	}
	
	//glDisable (GL_BLEND);
	
}


void VisualGraphics::drawWaveformCircle (const float xNum, const float yNum, const float rotVal, const float radius) {
	GLUquadricObj *qobj;
	
	// glLoadIdentity();
	
	glRotatef(rotVal, 0.0f, 0.0f, 1.0f);
	glTranslatef(xNum, yNum, 0.0f);
	
	// glCallList(startList);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	qobj = gluNewQuadric();
	
	gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
	
	gluDisk (qobj, 0.0, radius, 10, 1);
	
}


void VisualGraphics::drawCStringWithGL (const char* const cString, const uint16 stringLength) {
#if TARGET_OS_MAC
    for (uint16 i = 0; i < stringLength; i++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, cString[i]);
    }
#endif
#if TARGET_OS_WIN
	this->glPrint(cString);
#endif
}


void VisualGraphics::doFallbackActorShow(const char* const visualActorName) {
	GLdouble xCoordPos, yCoordPos;
    int len;
	char fallbackActorString[128];
	strcpy(fallbackActorString, visualActorName);
	strcat(fallbackActorString, ": unimplemented show() method in VisualActor implementation");
	len = (int)strlen(fallbackActorString);
	
	VisualCamera* aCamera = VisualCamera::createDefaultCamera();
	xCoordPos = aCamera->getMaxLeftCoord() + this->xPixelToCoord(10, *aCamera);
	yCoordPos = aCamera->getMaxBottomCoord() + this->yPixelToCoord(62, *aCamera);
	delete aCamera;
	glColor3d(1.0, 1.0, 0.0);
	glRasterPos3d(xCoordPos, yCoordPos, 0.0);
    this->drawCStringWithGL(fallbackActorString, len);
}


float VisualGraphics::getCosTwoTimesPi() {
    return (float)(cos (2.0f * (float)M_PI));
}


float VisualGraphics::getSinTwoTimesPi() {
    return (float)(sin (2.0f * (float)M_PI));
}


uint32 VisualGraphics::getNextFreeTextureName() {
	GLuint aTextureName;
	glGenTextures(1, &aTextureName);
	return (uint32)aTextureName;
}


void VisualGraphics::deleteTextures(const uint16 numberOfTextures, const uint32* const textureNames) {
	glDeleteTextures(numberOfTextures, (GLuint*)textureNames);
}


void VisualGraphics::showProcessInfoRow(Coord coord, const char* const textRowStr) {
    int len;
	glRasterPos3d(coord.x, coord.y, coord.z);
    len = (int)strlen(textRowStr);
    this->drawCStringWithGL(textRowStr, len);
}


void VisualGraphics::showProcessInfoNote() {
	char noteStr[128];
	GLdouble xCoordPos, yCoordPos;
	glColor3d(1.0, 1.0, 1.0);
	strcpy(noteStr, "Press [s] on the keyboard to hide Process Monitor Info");
	VisualCamera* aCamera = VisualCamera::createDefaultCamera();
	xCoordPos = aCamera->getMaxLeftCoord() + this->xPixelToCoord(10, *aCamera);
	yCoordPos = aCamera->getMaxBottomCoord() + this->yPixelToCoord(36, *aCamera);
	glRasterPos3d(xCoordPos, yCoordPos, 0.0);
	this->drawCStringWithGL(noteStr, strlen(noteStr));
	strcpy(noteStr, "Press [a] on the keyboard to hide Process Monitor Audio Info");
	xCoordPos = aCamera->getMaxLeftCoord() + this->xPixelToCoord(10, *aCamera);
	yCoordPos = aCamera->getMaxBottomCoord() + this->yPixelToCoord(18, *aCamera);
	delete aCamera;
	glRasterPos3d(xCoordPos, yCoordPos, 0.0);
	this->drawCStringWithGL(noteStr, strlen(noteStr));
}


PixelColor* VisualGraphics::clipPixelData(const PixelColor* inPixelData, const PixelRect& inPixelRect, const TopLeftPositionedPixelRect& clipRect) {
	uint8 numberOfBytesPerPixel = 4;
	PixelColor* clippedPixelData = (uint32*)malloc(clipRect.pixelRect.width * clipRect.pixelRect.height * numberOfBytesPerPixel);
	for (uint32 y = clipRect.topLeftPixel.y; y < clipRect.pixelRect.height; y++) {
		for (uint32 x = clipRect.topLeftPixel.x; x < clipRect.pixelRect.width; x++) {
			clippedPixelData[y * clipRect.pixelRect.width + x] = inPixelData[y * inPixelRect.width + x];
		}
	}
	return clippedPixelData;
}


#if TARGET_OS_MAC
CGContextRef VisualGraphics::createBitmapContext(size_t pixelWidth, size_t pixelHeight) {
	
	size_t bitsPerComponent = 8;
	
    uint16 bitmapBytesPerRow = pixelWidth * 4;
    uint32 bitmapByteCount = bitmapBytesPerRow * pixelHeight;
	
    //CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB(); // In Mac OS X v10.4 and later, this color space is no longer device-dependent and is replaced by the generic counterpart — kCGColorSpaceGenericRGB 
	
    uint8* bitmapData = (uint8*)malloc(bitmapByteCount);
    if (bitmapData == NULL) {
        writeLog("bitmapData is NULL in createBitmapContext");
        return NULL;
    }
	memset(bitmapData, 0, bitmapByteCount);
	
	CGBitmapInfo bitmapInfo = (kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host);
	CGContextRef context = CGBitmapContextCreate(bitmapData, pixelWidth, pixelHeight, bitsPerComponent, bitmapBytesPerRow, colorSpace, bitmapInfo);
    if (context == NULL) {
		free (bitmapData);
        writeLog("context is NULL in createBitmapContext");
        return NULL;
    }
    CGColorSpaceRelease(colorSpace);
	
    return context;
}
#endif


bool VisualGraphics::copyARGBBitmapDataToTexture(uint32 textureNumber, uint32 imageWidth, uint32 imageHeight, bool canUseRectExtension, const PixelColor** bitmapData, bool debug) {
	
	bool success = true;
	
	GLvoid* argbPixels = NULL;
	
	if (debug == true) {
		argbPixels = (GLvoid*)VisualColorTools::createARGBCheckPixels(imageWidth, imageHeight);
	} else {
		argbPixels = (GLvoid*)*bitmapData;
	}
	
	if (canUseRectExtension == false) {
		glEnable(GL_TEXTURE_2D);
	} else {
#if TARGET_OS_MAC
		glEnable(GL_TEXTURE_RECTANGLE_EXT);
#endif
	}
	
	if (canUseRectExtension == false) {
		glBindTexture(GL_TEXTURE_2D, (GLuint)textureNumber);
		
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		
	} else {
#if TARGET_OS_MAC
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, (GLuint)textureNumber);
#endif
	}
	
	this->setPixelStorageParams();
	
	// GL_BGRA/GL_UNSIGNED_BYTE and GL_BGRA/GL_UNSIGNED_INT_8_8_8_8_REV are equivalent on little endian machines
	
	if (canUseRectExtension == false) {
		
		uint32 potWidth = this->power2Ceiling(imageWidth);
		uint32 potHeight = this->power2Ceiling(imageHeight);
		PixelColor* powerOfTwoPixelData = (PixelColor*)calloc(potWidth * potHeight, sizeof(PixelColor));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, potWidth, potHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, powerOfTwoPixelData);
		free(powerOfTwoPixelData);
		
#if __BIG_ENDIAN__
		VisualColorTools::convertInterleavedPixels1234To4321((PixelColor*)argbPixels, imageWidth * imageHeight);
#endif
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, GL_BGRA, GL_UNSIGNED_BYTE, argbPixels);
		
	} else {
#if TARGET_OS_MAC
		glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, argbPixels);
#endif
	}
	
	if (canUseRectExtension == false) {
		glDisable (GL_TEXTURE_2D);
	} else {
#if TARGET_OS_MAC
		glDisable (GL_TEXTURE_RECTANGLE_EXT);
#endif
	}
	
	if (debug == true) {
		free(argbPixels);
	}
	
	return success;
	
}


#if TARGET_OS_WIN


void VisualGraphics::buildFont()								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping
	
	base = glGenLists(96);								// Storage For 96 Characters
	
	font = CreateFont(	-15,							// Height Of Font
					  0,								// Width Of Font
					  0,								// Angle Of Escapement
					  0,								// Orientation Angle
					  FW_NORMAL,						// Font Weight
					  FALSE,							// Italic
					  FALSE,							// Underline
					  FALSE,							// Strikeout
					  ANSI_CHARSET,					// Character Set Identifier
					  OUT_TT_PRECIS,					// Output Precision
					  CLIP_DEFAULT_PRECIS,			// Clipping Precision
					  DEFAULT_QUALITY,			// Output Quality
					  FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
					  "Arial");					// Font Name
	
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	
	oldfont = (HFONT)SelectObject(theVisualGraphicsCore->getWindowDC(), font);           // Selects The Font We Want
	wglUseFontBitmaps(theVisualGraphicsCore->getWindowDC(), 32, 96, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(theVisualGraphicsCore->getWindowDC(), oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}


void VisualGraphics::killFont(void) {
	// Delete All 96 Characters
	glDeleteLists(base, 96);
}


void VisualGraphics::glPrint(const char *fmt, ...) {
	// Custom GL "Print" Routine
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments
	
	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing
	
	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text
	
	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
	
}

#endif


const char* const VisualGraphics::getRendererName() {
	return this->capabilities.strRendererName;
}


const char* const VisualGraphics::getRendererVendor() {
	return this->capabilities.strRendererVendor;
}


const char* const VisualGraphics::getRendererVersion() {
	return this->capabilities.strRendererVersion;
}


unsigned short VisualGraphics::getGLVersion() {
	return this->capabilities.glVersion;
}


long VisualGraphics::getMaxTextureSize() {
	return (long)this->capabilities.maxTextureSize;
}


double VisualGraphics::xPixelToCoord(uint32 pixelPos, const VisualCamera& aCamera) {
	CoordSize3D size = aCamera.getSize();
	return ((double)pixelPos * (size.width / (double)this->getCanvasPixelWidth()));
}


double VisualGraphics::yPixelToCoord(uint32 pixelPos, const VisualCamera& aCamera) {
	CoordSize3D size = aCamera.getSize();
	return ((double)pixelPos * (size.height / (double)this->getCanvasPixelHeight()));
}


uint16 VisualGraphics::xCoordToPixel(double coordPos, const VisualCamera& aCamera) {
	CoordSize3D size = aCamera.getSize();
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	double tempCoord = coordPos + (size.width / 2.0);
	
	return (uint16)(tempCoord * ((double)this->getCanvasPixelWidth() / size.width));
}


uint16 VisualGraphics::yCoordToPixel(double coordPos, const VisualCamera& aCamera) {
	CoordSize3D size = aCamera.getSize();
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	double tempCoord = coordPos + (size.height / 2.0);
	
	return (uint16)(tempCoord * ((double)this->getCanvasPixelHeight() / size.height));
}


bool VisualGraphics::getCoordsOfPixelPosition(
											  double xPixelPos, 
											  double yPixelPos, 
											  double zAxisPos, 
											  double* xCoordPos,
											  double* yCoordPos,
											  double* zCoordPos) {
	bool success = false;
	
	GLdouble modelviewMatrix[16];
	GLdouble projectionMatrix[16];
	GLint viewport[4];
	GLint retVal;
	glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);
	retVal = gluUnProject(xPixelPos,
						  yPixelPos,
						  zAxisPos,
						  modelviewMatrix,
						  projectionMatrix,
						  viewport,
						  xCoordPos,
						  yCoordPos,
						  zCoordPos);
	if (retVal == GL_TRUE) {
		success = true;
	}
	return success;
}


bool VisualGraphics::getPixelsOfCoordPosition(
											  double xCoordPos,
											  double yCoordPos,
											  double zCoordPos,
											  double* xPixelPos, 
											  double* yPixelPos, 
											  double* zAxisPos) {
	bool success = false;
	
	GLdouble modelviewMatrix[16];
	GLdouble projectionMatrix[16];
	GLint viewport[4];
	GLint retVal;
	glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);
	/*	
	 for (int i = 0; i < 16; i++) {
	 printf("modelviewMatrix: %.2f\n", modelviewMatrix[i]);
	 }
	 
	 for (int i = 0; i < 16; i++) {
	 printf("projectionMatrix: %.2f\n", projectionMatrix[i]);
	 }
	 
	 for (int i = 0; i < 4; i++) {
	 printf("viewport: %d\n", viewport[i]);
	 }
	 */	
	retVal = gluProject(xCoordPos,
						yCoordPos,
						zCoordPos,
						modelviewMatrix,
						projectionMatrix,
						viewport,
						xPixelPos,
						yPixelPos,
						zAxisPos);
	if (retVal == GL_TRUE) {
		success = true;
	}
	return success;
}


void VisualGraphics::gatherOpenGLCapabilities() {
	
	const GLubyte* strExt;
	
	const GLubyte* strRend;
	const GLubyte* strVers;
	const GLubyte* strVend;
	
	GLint maxTextureSize;
	
	this->capabilities.glVersion = 0;
	
	// get renderer strings
	strRend = glGetString (GL_RENDERER);
	strncpy (this->capabilities.strRendererName, (char*)strRend, 255);
	strVend = glGetString (GL_VENDOR);
	strncpy (this->capabilities.strRendererVendor, (char*)strVend, 255);
	strVers = glGetString (GL_VERSION);
	strncpy (this->capabilities.strRendererVersion, (char*)strVers, 255);
	{ // get BCD version
		short j = 0;
		short shiftVal = 8;
		while (((strVers[j] <= '9') && (strVers[j] >= '0')) || (strVers[j] == '.')) { 
			// get only basic version info (until first non-digit or non-.)
			if ((strVers[j] <= '9') && (strVers[j] >= '0')) {
				this->capabilities.glVersion += (strVers[j] - '0') << shiftVal;
				shiftVal -= 4;
			}
			j++;
		}
	}
	
	strExt = glGetString (GL_EXTENSIONS);
	//writeLog((char*)strExt);
	
	/*
	 uint16 cnt = 0;
	 char* token = NULL;
	 char* text=(char *)malloc(strlen((char *)glGetString(GL_EXTENSIONS))+1);	// Allocate Memory For Our Extension String
	 strcpy (text,(char *)glGetString(GL_EXTENSIONS));		// Grab The Extension List, Store In Text
	 
	 token=strtok(text," ");									// Parse 'text' For Words, Seperated By " " (spaces)
	 uint16 maxtokens = 80;
	 while(token!=NULL)										// While The Token Isn't NULL
	 {
	 
	 //glColor3f(0.5f,1.0f,0.5f);							// Set Color To Bright Green
	 //glPrint(0,96+(cnt*32)-scroll,0,"%i",cnt);			// Print Current Extension Number
	 //glColor3f(1.0f,1.0f,0.5f);							// Set Color To Yellow
	 //glPrint(50,96+(cnt*32)-scroll,0,token);				// Print The Current Token (Parsed Extension Name)
	 writeLog(token);
	 token=strtok(NULL," ");								// Search For The Next Token
	 cnt++;												// Increase The Counter
	 if (cnt>maxtokens)									// Is 'maxtokens' Less Than 'cnt'
	 {
	 token = NULL;									// If So, Set 'maxtokens' Equal To 'cnt'
	 }
	 }
	 */
	
	
	
	glGetIntegerv (GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	this->capabilities.maxTextureSize = (long)maxTextureSize;
	
	// get caps
#if TARGET_OS_MAC
	glGetIntegerv (GL_MAX_TEXTURE_UNITS, 
				   (GLint*)&this->capabilities.textureUnits);
	glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE, 
				   (GLint*)&this->capabilities.max3DTextureSize);
	glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE, 
				   (GLint*)&this->capabilities.maxCubeMapTextureSize);
	
	// get functionality info
	this->capabilities.fSpecularVector = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_specular_vector", strExt);
	this->capabilities.fTransformHint = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_transform_hint", strExt);
	this->capabilities.fPackedPixels = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_packed_pixels", strExt) || 
	gluCheckExtension ((GLubyte*)"GL_APPLE_packed_pixel", strExt)  || 
	(this->capabilities.glVersion >= 0x0120);
	this->capabilities.fClientStorage = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_client_storage", strExt);
	this->capabilities.fYCbCr = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_ycbcr_422", strExt);
	this->capabilities.fTextureRange = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_texture_range", strExt);
	this->capabilities.fFence = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_fence", strExt);
	this->capabilities.fVAR = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_vertex_array_range", strExt);
	this->capabilities.fVAO = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_vertex_array_object", strExt);
	this->capabilities.fElementArray = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_element_array", strExt);
	this->capabilities.fVPEvals = 
	gluCheckExtension((GLubyte*)"GL_APPLE_vertex_program_evaluators",strExt);
	this->capabilities.fFloatPixels = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_float_pixels", strExt);
	this->capabilities.fFlushRenderer = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_flush_render", strExt);
	this->capabilities.fPixelBuffer = 
	gluCheckExtension ((GLubyte*)"GL_APPLE_pixel_buffer", strExt);
	this->capabilities.fImaging = 
	gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	this->capabilities.fTransposeMatrix = 
	gluCheckExtension ((GLubyte*)"GL_ARB_transpose_matrix", strExt) ||
	(this->capabilities.glVersion >= 0x0130);
	this->capabilities.fMultitexture = 
	gluCheckExtension ((GLubyte*)"GL_ARB_multitexture", strExt) ||
	(this->capabilities.glVersion >= 0x0130);
	this->capabilities.fTexEnvAdd = 
	gluCheckExtension ((GLubyte*)"GL_ARB_texture_env_add", strExt) ||
	gluCheckExtension ((GLubyte*)"GL_EXT_texture_env_add", strExt) ||
	(this->capabilities.glVersion >= 0x0130);
	this->capabilities.fTexEnvCombine = 
	gluCheckExtension ((GLubyte*)"GL_ARB_texture_env_combine", strExt) ||
	(this->capabilities.glVersion >= 0x0130);
	this->capabilities.fTexEnvDot3 = 
	gluCheckExtension ((GLubyte*)"GL_ARB_texture_env_dot3", strExt) ||
	(this->capabilities.glVersion >= 0x0130);
	this->capabilities.fTexEnvCrossbar = 
	gluCheckExtension ((GLubyte*)"GL_ARB_texture_env_crossbar", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fTexCubeMap = 
	gluCheckExtension ((GLubyte*)"GL_ARB_texture_cube_map", strExt) ||
	(this->capabilities.glVersion >= 0x0130);
	this->capabilities.fTexCompress = 
	gluCheckExtension ((GLubyte*)"GL_ARB_texture_compression", strExt) ||
	(this->capabilities.glVersion >= 0x0130);
	this->capabilities.fMultisample = 
	gluCheckExtension ((GLubyte*)"GL_ARB_multisample", strExt) ||
	(this->capabilities.glVersion >= 0x0130);
	this->capabilities.fTexBorderClamp = 
	gluCheckExtension ((GLubyte*)"GL_ARB_texture_border_clamp", strExt) ||
	(this->capabilities.glVersion >= 0x0130);
	this->capabilities.fPointParam = 
	gluCheckExtension ((GLubyte*)"GL_ARB_point_parameters", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fVertexProg = 
	gluCheckExtension ((GLubyte*)"GL_ARB_vertex_program", strExt);
	this->capabilities.fFragmentProg = 
	gluCheckExtension ((GLubyte*)"GL_ARB_fragment_program", strExt);
	this->capabilities.fTexMirrorRepeat = 
	gluCheckExtension ((GLubyte*)"GL_ARB_texture_mirrored_repeat", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fDepthTex = 
	gluCheckExtension ((GLubyte*)"GL_ARB_depth_texture", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fShadow = 
	gluCheckExtension ((GLubyte*)"GL_ARB_shadow", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fShadowAmbient = 
	gluCheckExtension ((GLubyte*)"GL_ARB_shadow_ambient", strExt);
	this->capabilities.fVertexBlend = 
	gluCheckExtension ((GLubyte*)"GL_ARB_vertex_blend", strExt);
	this->capabilities.fWindowPos = 
	gluCheckExtension ((GLubyte*)"GL_ARB_window_pos", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fTex3D = 
	gluCheckExtension ((GLubyte*)"GL_EXT_texture3D", strExt) ||
	(this->capabilities.glVersion >= 0x0120);
	this->capabilities.fClipVolHint = 
	gluCheckExtension ((GLubyte*)"GL_EXT_clip_volume_hint", strExt);
	this->capabilities.fRescaleNorm = 
	gluCheckExtension ((GLubyte*)"GL_EXT_rescale_normal", strExt) ||
	(this->capabilities.glVersion >= 0x0120);
	this->capabilities.fBlendColor = 
	gluCheckExtension ((GLubyte*)"GL_EXT_blend_color", strExt) ||
	gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	this->capabilities.fBlendMinMax = 
	gluCheckExtension ((GLubyte*)"GL_EXT_blend_minmax", strExt) ||
	gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	this->capabilities.fBlendSub = 
	gluCheckExtension ((GLubyte*)"GL_EXT_blend_subtract", strExt) ||
	gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	this->capabilities.fCVA = 
	gluCheckExtension ((GLubyte*)"GL_EXT_compiled_vertex_array", strExt);
	this->capabilities.fTexLODBias = 
	gluCheckExtension ((GLubyte*)"GL_EXT_texture_lod_bias", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fABGR = 
	gluCheckExtension ((GLubyte*)"GL_EXT_abgr", strExt);
	this->capabilities.fBGRA = 
	gluCheckExtension ((GLubyte*)"GL_EXT_bgra", strExt) ||
	(this->capabilities.glVersion >= 0x0120);
	this->capabilities.fTexFilterAniso = 
	gluCheckExtension ((GLubyte*)"GL_EXT_texture_filter_anisotropic",strExt);
	this->capabilities.fPaletteTex = 
	gluCheckExtension ((GLubyte*)"GL_EXT_paletted_texture", strExt);
	this->capabilities.fShareTexPalette = 
	gluCheckExtension ((GLubyte*)"GL_EXT_shared_texture_palette", strExt);
	this->capabilities.fSecColor = 
	gluCheckExtension ((GLubyte*)"GL_EXT_secondary_color", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fTexCompressS3TC = 
	gluCheckExtension ((GLubyte*)"GL_EXT_texture_compression_s3tc", strExt);
	this->capabilities.fTexRect = 
	gluCheckExtension ((GLubyte*)"GL_EXT_texture_rectangle", strExt);
	this->capabilities.fFogCoord = 
	gluCheckExtension ((GLubyte*)"GL_EXT_fog_coord", strExt);
	this->capabilities.fDrawRangeElements = 
	gluCheckExtension ((GLubyte*)"GL_EXT_draw_range_elements", strExt);
	this->capabilities.fStencilWrap = 
	gluCheckExtension ((GLubyte*)"GL_EXT_stencil_wrap", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fBlendFuncSep = 
	gluCheckExtension ((GLubyte*)"GL_EXT_blend_func_separate", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fMultiDrawArrays = 
	gluCheckExtension ((GLubyte*)"GL_EXT_multi_draw_arrays", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fShadowFunc = 
	gluCheckExtension ((GLubyte*)"GL_EXT_shadow_funcs", strExt);
	this->capabilities.fStencil2Side = 
	gluCheckExtension ((GLubyte*)"GL_EXT_stencil_two_side", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fColorSubtable = 
	gluCheckExtension ((GLubyte*)"GL_EXT_color_subtable", strExt) || 
	gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	this->capabilities.fConvolution = 
	gluCheckExtension ((GLubyte*)"GL_EXT_convolution", strExt) || 
	gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	this->capabilities.fHistogram = 
	gluCheckExtension ((GLubyte*)"GL_EXT_histogram", strExt) || 
	gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	this->capabilities.fColorTable = 
	gluCheckExtension ((GLubyte*)"GL_SGI_color_table", strExt) || 
	gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	this->capabilities.fColorMatrix = 
	gluCheckExtension ((GLubyte*)"GL_SGI_color_matrix", strExt) || 
	gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	this->capabilities.fTexEdgeClamp = 
	gluCheckExtension ((GLubyte*)"GL_SGIS_texture_edge_clamp", strExt) ||
	(this->capabilities.glVersion >= 0x0120);
	this->capabilities.fGenMipmap = 
	gluCheckExtension ((GLubyte*)"GL_SGIS_generate_mipmap", strExt);
	this->capabilities.fTexLOD = 
	gluCheckExtension ((GLubyte*)"GL_SGIS_texture_lod", strExt) ||
	(this->capabilities.glVersion >= 0x0120);
	this->capabilities.fPointCull = 
	gluCheckExtension ((GLubyte*)"GL_ATI_point_cull_mode", strExt);
	this->capabilities.fTexMirrorOnce = 
	gluCheckExtension ((GLubyte*)"GL_ATI_texture_mirror_once", strExt);
	this->capabilities.fPNtriangles = 
	gluCheckExtension ((GLubyte*)"GL_ATI_pn_triangles", strExt) ||
	gluCheckExtension ((GLubyte*)"GL_ATIX_pn_triangles", strExt);
	this->capabilities.fTextFragShader = 
	gluCheckExtension ((GLubyte*)"GL_ATI_text_fragment_shader", strExt);
	this->capabilities.fBlendEqSep = 
	gluCheckExtension ((GLubyte*)"GL_ATI_blend_equation_separate", strExt);
	this->capabilities.fBlendWeightMinMax = 
	gluCheckExtension ((GLubyte*)"GL_ATI_blend_weighted_minmax", strExt);
	this->capabilities.fCombine3 = 
	gluCheckExtension ((GLubyte*)"GL_ATI_texture_env_combine3", strExt);
	this->capabilities.fSepStencil = 
	gluCheckExtension ((GLubyte*)"GL_ATI_separate_stencil", strExt);
	this->capabilities.fArrayRevComps4Byte = 
	gluCheckExtension ((GLubyte*)"GL_ATI_array_rev_comps_in_4_bytes",strExt);
	this->capabilities.fPointSprite = 
	gluCheckExtension ((GLubyte*)"GL_NV_point_sprite", strExt);
	this->capabilities.fRegCombiners = 
	gluCheckExtension ((GLubyte*)"GL_NV_register_combiners", strExt);
	this->capabilities.fRegCombiners2 = 
	gluCheckExtension ((GLubyte*)"GL_NV_register_combiners2", strExt);
	this->capabilities.fTexEnvCombine4 = 
	gluCheckExtension ((GLubyte*)"GL_NV_texture_env_combine4", strExt);
	this->capabilities.fBlendSquare = 
	gluCheckExtension ((GLubyte*)"GL_NV_blend_square", strExt) ||
	(this->capabilities.glVersion >= 0x0140);
	this->capabilities.fFogDist = 
	gluCheckExtension ((GLubyte*)"GL_NV_fog_distance", strExt);
	this->capabilities.fMultisampleFilterHint = 
	gluCheckExtension ((GLubyte*)"GL_NV_multisample_filter_hint", strExt);
	this->capabilities.fTexGenReflect = 
	gluCheckExtension ((GLubyte*)"GL_NV_texgen_reflection", strExt);
	this->capabilities.fTexShader = 
	gluCheckExtension ((GLubyte*)"GL_NV_texture_shader", strExt);
	this->capabilities.fTexShader2 = 
	gluCheckExtension ((GLubyte*)"GL_NV_texture_shader2", strExt);
	this->capabilities.fTexShader3 = 
	gluCheckExtension ((GLubyte*)"GL_NV_texture_shader3", strExt);
	this->capabilities.fDepthClamp = 
	gluCheckExtension ((GLubyte*)"GL_NV_depth_clamp", strExt);
	this->capabilities.fLightMaxExp = 
	gluCheckExtension ((GLubyte*)"GL_NV_light_max_exponent", strExt);
	this->capabilities.fRasterPosClip = 
	gluCheckExtension ((GLubyte*)"GL_IBM_rasterpos_clip", strExt);
	this->capabilities.fConvBorderModes = 
	gluCheckExtension ((GLubyte*)"GL_HP_convolution_border_modes", strExt) ||
	gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	
	if (this->capabilities.fTexRect) {
		// only check if extension supported
		glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, (GLint*)&this->capabilities.maxRectTextureSize);
	} else {
		this->capabilities.maxRectTextureSize = 0;
	}
#endif
}


uint32 VisualGraphics::power2Ceiling(uint32 n) {
    uint32 i = 1;
    while (i < n) i <<= 1;
    return i;
}


VisualNurbs* VisualGraphics::createNurbsObject(const VisualItemIdentifier& identifier, uint8 sNumKnots, float* sKnots, uint8 tNumKnots, float* tKnots, uint16 sStride, uint16 tStride, uint16 sOrder, uint16 tOrder) {
	
	VisualNurbs* aVisualNurbs = new VisualNurbs(sNumKnots, sKnots, tNumKnots, tKnots, sStride, tStride, sOrder, tOrder);
	
	NurbsMapIterator it;
	it = this->nurbsMap.find(identifier);
	if (it == this->nurbsMap.end()) {
		this->nurbsMap[identifier] = aVisualNurbs;
	} else {
		if (it->second) {
			delete it->second;
		}
		it->second = aVisualNurbs;
	}
	return aVisualNurbs;
}


VisualNurbs* VisualGraphics::getNurbsObject(const VisualItemIdentifier& identifier) {
	
	NurbsMapIterator it = this->nurbsMap.find(identifier);
	if (it != this->nurbsMap.end()) {
		return it->second;
	}
	return NULL;
}


void VisualGraphics::deleteNurbsObject(const VisualItemIdentifier& identifier) {
	NurbsMapIterator it = this->nurbsMap.find(identifier);
	if (it != this->nurbsMap.end()) {
		delete it->second;
		this->nurbsMap.erase(it);
	}
}


void VisualGraphics::copyFramebufferToTexture(uint32 textureNumber, bool canUseRectExtension, const BottomLeftPositionedPixelRect& clipRect, uint16 pixelFormat, uint16 dataType) {
	
	bool debug = false;
	
	theVisualGraphics = VisualGraphics::getInstance();
	Pixel bottomLeftViewportOrigin = this->getViewportBottomLeftOrigin();
	
	if (canUseRectExtension == false) {
        glEnable(GL_TEXTURE_2D);
    } else {
#if TARGET_OS_MAC
        glEnable (GL_TEXTURE_RECTANGLE_EXT);
#endif
	}
	
	if (canUseRectExtension == false) {
        glBindTexture(GL_TEXTURE_2D, (GLuint)textureNumber);
    } else {
#if TARGET_OS_MAC
        glBindTexture(GL_TEXTURE_RECTANGLE_EXT, (GLuint)textureNumber);
#endif
    }
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	if (debug == false) {
		if (canUseRectExtension == false) {
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (bottomLeftViewportOrigin.x + clipRect.bottomLeftPixel.x), (bottomLeftViewportOrigin.y + clipRect.bottomLeftPixel.y), clipRect.pixelRect.width, clipRect.pixelRect.height, 0);
		} else {
#if TARGET_OS_MAC
			glCopyTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, (bottomLeftViewportOrigin.x + clipRect.bottomLeftPixel.x), (bottomLeftViewportOrigin.y + clipRect.bottomLeftPixel.y), clipRect.pixelRect.width, clipRect.pixelRect.height, 0);
#endif
		}
	} else {
		uint32* image = NULL;
#if TARGET_OS_WIN
		image = VisualColorTools::createBGRACheckPixels(clipRect.pixelRect.width, clipRect.pixelRect.height, blue);
#endif
#if TARGET_OS_MAC
		image = VisualColorTools::createARGBCheckPixels(clipRect.pixelRect.width, clipRect.pixelRect.height, blue);
#endif
		this->setPixelStorageParams();
		if (canUseRectExtension == false) {
#if TARGET_OS_WIN
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, clipRect.pixelRect.width, clipRect.pixelRect.height, 0, pixelFormat, dataType, (GLubyte*)image);
#endif
#if TARGET_OS_MAC
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, clipRect.pixelRect.width, clipRect.pixelRect.height, 0, pixelFormat, dataType, (GLuint*)image);
#endif
		} else {
#if TARGET_OS_MAC
			glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, clipRect.pixelRect.width, clipRect.pixelRect.height, 0, pixelFormat, dataType, (GLuint*)image);
#endif
		}
		free(image);
	}
	
    if (canUseRectExtension == false) {
        glDisable(GL_TEXTURE_2D);
	} else {
#if TARGET_OS_MAC
        glDisable(GL_TEXTURE_RECTANGLE_EXT);
#endif
    }
	
}
