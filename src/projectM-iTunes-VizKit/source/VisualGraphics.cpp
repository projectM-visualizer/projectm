/*
 * Project: VizKitrow
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualGraphics.cpp
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

#include "VisualGraphics.h"
#include "VisualErrorHandling.h"
#include "VisualGraphicsCore.h"
#include "VisualConvolutionFilter.h"
#include "VisualNurbs.h"
#include "VisualDispatch.h"
#include "VisualTiming.h"
#include "VisualColorTools.h"
#include "VisualDataStore.h"
#include "VisualFile.h"
#include "VisualConfiguration.h"

#include <stdlib.h> // RAND_MAX

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
#endif



using namespace VizKit;


#if TARGET_OS_WIN
	UInt8 fontIsInstalledBool;
	GLuint base; // Base Display List For The Font Set
#endif



#if TARGET_OS_WIN

int CALLBACK EnumFontFamExProc(
				ENUMLOGFONTEX *lpelfe,    // logical-font data
				NEWTEXTMETRICEX *lpntme,  // physical-font data
				DWORD FontType,           // type of font
				LPARAM lParam             // application-defined data
)
{
	/*
  CString *pEnumerateFont = (CString*)lParam;
  *pEnumerateFont += CString("|") + ((char *)lpelfe->elfLogFont.lfFaceName);
  
  */
	//VisualGraphics* visualGraphics;
	//writeLog((char *)lpelfe->elfLogFont.lfFaceName);


	fontIsInstalledBool = 1;
	return 1;
}


UInt8 VisualGraphics::fontIsInstalled(HDC currHdc, const char* const fontName) {

	LOGFONT lf;

	lf.lfCharSet = ANSI_CHARSET;
	lf.lfFaceName[0]='\0';
	strcpy(lf.lfFaceName, fontName);
	fontIsInstalledBool = 0;
	EnumFontFamiliesEx(currHdc, &lf, (FONTENUMPROC) EnumFontFamExProc, 0, 0);
	if (fontIsInstalledBool == 1) {
		return 1;
	} else {
		return 0;
	}

}

#endif


VisualGraphics* VisualGraphics::theVisualGraphics = NULL;  // singleton


/**
 * Compare function for sort operation.
 */
struct CompareDisplayResolutionAscending {
	bool operator()(const DisplayResolution* x, const DisplayResolution* y) {
		// Does x preceed y?
		bool doesXPreceedYBool = true;
		if (x->horizontalPixels == y->horizontalPixels) {
			if (x->verticalPixels == y->verticalPixels) {
				if (x->bitsPerPixel == y->bitsPerPixel) {
					if (x->refreshRate == y->refreshRate) {
						doesXPreceedYBool = true; // identical
					} else if (x->refreshRate > y->refreshRate) {
						doesXPreceedYBool = false;
					} else if (x->refreshRate < y->refreshRate) {
						doesXPreceedYBool = true;
					}
				} else if (x->bitsPerPixel > y->bitsPerPixel) {
					doesXPreceedYBool = false;
				} else if (x->bitsPerPixel < y->bitsPerPixel) {
					doesXPreceedYBool = true;
				}
			} else if (x->verticalPixels > y->verticalPixels) {
				doesXPreceedYBool = false;
			} else if (x->verticalPixels < y->verticalPixels) {
				doesXPreceedYBool = true;
			}
		} else if (x->horizontalPixels > y->horizontalPixels) {
			doesXPreceedYBool = false;
		} else if (x->horizontalPixels < y->horizontalPixels) {
			doesXPreceedYBool = true;
		}
		return doesXPreceedYBool;
	}
};


VisualGraphics::VisualGraphics() {
	isSetUpOnFullscreen = false;
	
	fullscreenDisplayResolution.theFullscreenDisplayResolution.horizontalPixels = 0;
	fullscreenDisplayResolution.theFullscreenDisplayResolution.verticalPixels = 0;
	fullscreenDisplayResolution.theFullscreenDisplayResolution.bitsPerPixel = 0;
	fullscreenDisplayResolution.theFullscreenDisplayResolution.refreshRate = 0;
	strcpy(fullscreenDisplayResolution.theFullscreenDisplayResolution.showStr, "unknown display resolution");

	displayResolutionIterIndex = 0;
	
	backgroundColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kBackgroundColorRed);
	backgroundColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kBackgroundColorGreen);
	backgroundColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kBackgroundColorBlue);
	backgroundColor.a = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kBackgroundColorAlpha);

}


VisualGraphics::~VisualGraphics() {

	for (DisplayResolutionVectorIterator it = theVisualGraphics->displayResolutionVector.begin(); it != theVisualGraphics->displayResolutionVector.end(); it++) {
		delete *it;
		*it = NULL;
	}
	displayResolutionVector.clear();

	for (NurbsMapIterator it = theVisualGraphics->nurbsMap.begin(); it != theVisualGraphics->nurbsMap.end(); it++) {
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
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	theVisualGraphicsCore->setGraphicsDevicePort(thePort);
}


void VisualGraphics::setCanvasRect(const ::Rect* const aCanvasRect) {
    theVisualGraphics->canvasRect.top = aCanvasRect->top;
    theVisualGraphics->canvasRect.left = aCanvasRect->left;
    theVisualGraphics->canvasRect.bottom = aCanvasRect->bottom;
    theVisualGraphics->canvasRect.right = aCanvasRect->right;
}


void VisualGraphics::isSetupForFullScreenMode(const bool isFullscreen) {
	theVisualGraphics->isSetUpOnFullscreen = isFullscreen;
}


void VisualGraphics::getCanvasRect(::Rect* aRect) {
	aRect->top = theVisualGraphics->canvasRect.top;
	aRect->left = theVisualGraphics->canvasRect.left;
	aRect->bottom = theVisualGraphics->canvasRect.bottom;
	aRect->right = theVisualGraphics->canvasRect.right;
}


UInt16 VisualGraphics::getScreenWidth() {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	return theVisualGraphicsCore->getScreenWidth();
}


UInt16 VisualGraphics::getScreenHeight() {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	return theVisualGraphicsCore->getScreenHeight();
}


UInt16 VisualGraphics::getBitsPerPixelOfScreen() {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	return theVisualGraphicsCore->getBitsPerPixelOfScreen();
}


UInt16 VisualGraphics::getRefreshRateOfScreen() {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	return theVisualGraphicsCore->getRefreshRateOfScreen();
}


UInt16 VisualGraphics::getCanvasPixelWidth() {
    return (theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left);
}


UInt16 VisualGraphics::getCanvasPixelHeight() {
    return (theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top);
}


double VisualGraphics::getCanvasCoordWidth() {
	return (theVisualGraphics->maxRightCoordOfGLView - theVisualGraphics->maxLeftCoordOfGLView);
}


double VisualGraphics::getCanvasCoordHeight() {
	return (theVisualGraphics->maxTopCoordOfGLView - theVisualGraphics->maxBottomCoordOfGLView);
}


void VisualGraphics::getCanvasSurroundingRect(::Rect* aRect) {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	theVisualGraphicsCore->getCanvasSurroundingRect(aRect);
}


void VisualGraphics::getViewportRect(::Rect* aViewport) {
	GLint viewportRect[4];
    glGetIntegerv(GL_VIEWPORT, viewportRect);
	// the x and y window coordinates of the viewport, followed by its width and height
	aViewport->left = viewportRect[0];
	aViewport->top = viewportRect[1];
	aViewport->right = viewportRect[2];
	aViewport->bottom = viewportRect[3];
}


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
	theVisualGraphics->killFont();
#endif
}


void VisualGraphics::setOrthographicProjection() {

	::Rect canvasSurroundingRect;
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	theVisualGraphicsCore->getCanvasSurroundingRect(&canvasSurroundingRect);
	theVisualGraphicsCore->setViewport(&(theVisualGraphics->canvasRect), &canvasSurroundingRect, theVisualGraphics->isSetUpOnFullscreen);

	double maxTopPos, maxLeftPos, maxBottomPos, maxRightPos, maxNearPos, maxFarPos;

    if ((theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left) <= (theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top)) {
    	maxLeftPos = -1.0;
        maxRightPos = 1.0;
        maxBottomPos = -1.0 * (GLdouble)(theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top) / (GLdouble)(theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left);
        maxTopPos = 1.0 * (GLdouble)(theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top) / (GLdouble)(theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left);
    } else {
    	maxLeftPos = -1.0 * (GLdouble)(theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left) / (GLdouble)(theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top);
        maxRightPos = 1.0 * (GLdouble)(theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left) / (GLdouble)(theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top);
        maxBottomPos = -1.0;
        maxTopPos = 1.0;
    }
	
	maxNearPos = 0.0;
	maxFarPos = 10.0;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	glOrtho(maxLeftPos, maxRightPos, maxBottomPos, maxTopPos, maxNearPos, maxFarPos);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	theVisualGraphics->setMaxTopCoordOfCanvas(maxTopPos);
	theVisualGraphics->setMaxLeftCoordOfCanvas(maxLeftPos);
	theVisualGraphics->setMaxBottomCoordOfCanvas(maxBottomPos);
	theVisualGraphics->setMaxRightCoordOfCanvas(maxRightPos);
	theVisualGraphics->setMaxNearCoordOfCanvas(maxNearPos);
	theVisualGraphics->setMaxFarCoordOfCanvas(maxFarPos);
	
}


void VisualGraphics::setPerspectiveProjection(double maxNearCoord) {

	::Rect canvasSurroundingRect;
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	theVisualGraphicsCore->getCanvasSurroundingRect(&canvasSurroundingRect);
	theVisualGraphicsCore->setViewport(&(theVisualGraphics->canvasRect), &canvasSurroundingRect, theVisualGraphics->isSetUpOnFullscreen);

	double maxTopCoord, maxLeftCoord, maxBottomCoord, maxRightCoord, maxFarCoord;

    if ((theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left) <= (theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top)) {
    	maxLeftCoord = -1.0;
        maxRightCoord = 1.0;
        maxBottomCoord = -1.0 * (GLfloat)(theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top) / (GLfloat)(theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left);
        maxTopCoord = 1.0 * (GLfloat)(theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top) / (GLfloat)(theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left);
		maxFarCoord = (maxBottomCoord * -1.0) + maxTopCoord + maxNearCoord;
	} else {
    	maxLeftCoord = -1.0 * (GLfloat)(theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left) / (GLfloat)(theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top);
        maxRightCoord = 1.0 * (GLfloat)(theVisualGraphics->canvasRect.right - theVisualGraphics->canvasRect.left) / (GLfloat)(theVisualGraphics->canvasRect.bottom - theVisualGraphics->canvasRect.top);
        maxBottomCoord = -1.0;
        maxTopCoord = 1.0;
		maxFarCoord = (maxLeftCoord * -1.0) + maxRightCoord + maxNearCoord;
    }
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	glFrustum(maxLeftCoord, maxRightCoord, maxBottomCoord, maxTopCoord, maxNearCoord, maxFarCoord);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	theVisualGraphics->setMaxTopCoordOfCanvas(maxTopCoord);
	theVisualGraphics->setMaxLeftCoordOfCanvas(maxLeftCoord);
	theVisualGraphics->setMaxBottomCoordOfCanvas(maxBottomCoord);
	theVisualGraphics->setMaxRightCoordOfCanvas(maxRightCoord);
	theVisualGraphics->setMaxNearCoordOfCanvas(maxNearCoord);
	theVisualGraphics->setMaxFarCoordOfCanvas(maxFarCoord);
	
	char cStr64[64];
	sprintf(cStr64, "top: %.2f, left: %.2f", maxTopCoord, maxLeftCoord);
	setProcessInfo("PerspCoordMaxTopLeft", cStr64);
	sprintf(cStr64, "bottom: %.2f, right: %.2f", maxBottomCoord, maxRightCoord);
	setProcessInfo("PerspCoordMaxBottomRight", cStr64);	
	sprintf(cStr64, "near: %.2f, far: %.2f", maxNearCoord, maxFarCoord);
	setProcessInfo("PerspCoordMaxNearFar", cStr64);
	
}


void VisualGraphics::setBackgroundColor(const RGBAColor& aColorVal) {
	theVisualGraphics->backgroundColor.r = aColorVal.r;
	theVisualGraphics->backgroundColor.g = aColorVal.g;
	theVisualGraphics->backgroundColor.b = aColorVal.b;
	theVisualGraphics->backgroundColor.a = aColorVal.a;
}


RGBAColor VisualGraphics::getBackgroundColor() {
	return theVisualGraphics->backgroundColor;
}


void VisualGraphics::clearBackground() {
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
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


void VisualGraphics::rotateMatrix(float angle, float xAmount, float yAmount, float zAmount) {
	glRotatef(angle, xAmount, yAmount, zAmount);
}


void VisualGraphics::scaleMatrix(float xFactor, float yFactor, float zFactor) {
	glScalef(xFactor, yFactor, zFactor);
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


void VisualGraphics::bindTexture(UInt32 textureName, bool useRectExtension) {
	if (useRectExtension == false) {
		glBindTexture(GL_TEXTURE_2D, (GLuint)textureName);
	} else {
#if TARGET_OS_MAC
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, (GLuint)textureName);
#endif
	}
}


void VisualGraphics::get32BitPixelsOfCurrentTexture(bool useRectExtension, UInt16 format, UInt16 type, UInt32** buffer) {
	glGetTexImage(GL_TEXTURE_2D, 0, format, type, *buffer);
}


void VisualGraphics::get8BitPixelsOfCurrentTexture(bool useRectExtension, UInt16 format, UInt16 type, UInt8** buffer) {
	glGetTexImage(GL_TEXTURE_2D, 0, format, type, *buffer);
}


void VisualGraphics::enableAlphaBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void VisualGraphics::disableBlending() {
	glDisable(GL_BLEND);
}


void VisualGraphics::setColor(const RGBAColor& theColor) {
	glColor4f(theColor.r, theColor.g, theColor.b, theColor.a);
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


UInt8 VisualGraphics::getOpenGLError (char* outErrorString) {
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


void VisualGraphics::gatherAvailableDisplayResolutions() {

	DisplayResolution* aDisplayResolution;
	bool found;
	DisplayResolutionVectorIterator it;
	UInt16 minRefreshRate = 50;
#if TARGET_OS_WIN
	UInt16 minBitsPerPixel = 32;
	UInt16 maxBitsPerPixel = 32;
#endif
#if TARGET_OS_MAC
	UInt16 minBitsPerPixel = 32;
	UInt16 maxBitsPerPixel = 32;
#endif
	SInt16 exactMatchOfRefreshrate = -1;
	bool displayResolutionIsAccepted = false;

	theVisualGraphics = VisualGraphics::getInstance();

	for (it = theVisualGraphics->displayResolutionVector.begin(); it != theVisualGraphics->displayResolutionVector.end(); it++) {
		delete *it;
		*it = NULL;
	}	
	theVisualGraphics->displayResolutionVector.clear();
	
	// Default: no resolution switch but the default one of iTunes
	aDisplayResolution = new DisplayResolution;
	aDisplayResolution->horizontalPixels = 0;
	aDisplayResolution->verticalPixels = 0;
	aDisplayResolution->bitsPerPixel = 0;
	aDisplayResolution->refreshRate = 0;
	strcpy(aDisplayResolution->showStr, "Default");
	displayResolutionVector.push_back(aDisplayResolution);
	exactMatchOfRefreshrate = theVisualGraphics->getRefreshRateOfScreen();
	// we take the refresh rate as it is set now

#if TARGET_OS_MAC
	CFArrayRef displayModeArray;
	CFDictionaryRef displayMode;
	CFNumberRef number;
	char str[32];
	UInt16 theDisplayModeCount;
	UInt16 i;
	long height, width, bitsPerPixel, refreshRate;
	
	displayModeArray = CGDisplayAvailableModes(CGMainDisplayID());
	theDisplayModeCount = CFArrayGetCount(displayModeArray);
	
	for (i = 0; i < theDisplayModeCount; i++) {
		displayMode = (CFDictionaryRef)CFArrayGetValueAtIndex(displayModeArray, i);
		
		number = (CFNumberRef)CFDictionaryGetValue(displayMode, kCGDisplayWidth);
		CFNumberGetValue(number, kCFNumberLongType, &width);
		
		number = (CFNumberRef)CFDictionaryGetValue(displayMode, kCGDisplayHeight);
		CFNumberGetValue(number, kCFNumberLongType, &height);
		
		number = (CFNumberRef)CFDictionaryGetValue(displayMode, kCGDisplayBitsPerPixel);
		CFNumberGetValue(number, kCFNumberLongType, &bitsPerPixel);
		
		number = (CFNumberRef)CFDictionaryGetValue(displayMode, kCGDisplayRefreshRate);
		CFNumberGetValue(number, kCFNumberLongType, &refreshRate);

		
		if (exactMatchOfRefreshrate > 0) {
			if (refreshRate == exactMatchOfRefreshrate) {
				displayResolutionIsAccepted = true;
			} else {
				displayResolutionIsAccepted = false;
			}
		} else {
			displayResolutionIsAccepted = true;
		}
		if (displayResolutionIsAccepted == true) {
			if (((refreshRate >= minRefreshRate) || (refreshRate == 0)) && (bitsPerPixel >= minBitsPerPixel) && (bitsPerPixel <= maxBitsPerPixel)) {
				displayResolutionIsAccepted = true;
			} else {
				displayResolutionIsAccepted = false;
			}
		}
		
		found = false;
		
		if (displayResolutionIsAccepted == true) {
			for (it = theVisualGraphics->displayResolutionVector.begin(); it != theVisualGraphics->displayResolutionVector.end(); it++) {
				if (((*it)->horizontalPixels == width) && ((*it)->verticalPixels == height)) {
					found = true;
					if ((*it)->bitsPerPixel < bitsPerPixel) {
						(*it)->bitsPerPixel = bitsPerPixel;
					}
					if ((*it)->refreshRate < refreshRate) {
						(*it)->refreshRate = refreshRate;
					}
				}
			}
			if (found == false) {
				aDisplayResolution = new DisplayResolution;
				aDisplayResolution->horizontalPixels = width;
				aDisplayResolution->verticalPixels = height;
				aDisplayResolution->bitsPerPixel = bitsPerPixel;
				aDisplayResolution->refreshRate = refreshRate;
				sprintf(str, "%ld x %ld", width, height);
				strcpy(aDisplayResolution->showStr, str);
				displayResolutionVector.push_back(aDisplayResolution);
			}
		}
	}
#endif
#if TARGET_OS_WIN
	DEVMODE devmode;
	//CString devModeStr;
	int iMode = 0;
	BOOL flag;
	char str[32];
	UInt16 zaehler=0;
	do
	{
		flag = ::EnumDisplaySettings(NULL, iMode, &devmode);
		
		if (flag) {

			if (exactMatchOfRefreshrate > 0) {
				if (devmode.dmDisplayFrequency == exactMatchOfRefreshrate) {
					displayResolutionIsAccepted = true;
				} else {
					displayResolutionIsAccepted = false;
				}
			} else {
				displayResolutionIsAccepted = true;
			}
			if (displayResolutionIsAccepted == true) {
				if ((devmode.dmDisplayFrequency >= minRefreshRate) && (devmode.dmBitsPerPel >= minBitsPerPixel) && (devmode.dmBitsPerPel <= maxBitsPerPixel)) {
					displayResolutionIsAccepted = true;
				} else {
					displayResolutionIsAccepted = false;
				}
			}

			found = false;
			if (displayResolutionIsAccepted == true) {
				for (it = theVisualGraphics->displayResolutionVector.begin(); it != theVisualGraphics->displayResolutionVector.end(); it++) {
					if (((*it)->horizontalPixels == devmode.dmPelsWidth) && ((*it)->verticalPixels == devmode.dmPelsHeight)) {
						found = true;
						if ((*it)->bitsPerPixel < (UInt16)devmode.dmBitsPerPel) {
							(*it)->bitsPerPixel = (UInt16)devmode.dmBitsPerPel;
						}
						if ((*it)->refreshRate < (UInt16)devmode.dmDisplayFrequency) {
							(*it)->refreshRate = (UInt16)devmode.dmDisplayFrequency;
						}
					}
				}
				if (found == false) {
					aDisplayResolution = new DisplayResolution;
					aDisplayResolution->horizontalPixels = (UInt16)devmode.dmPelsWidth;
					aDisplayResolution->verticalPixels = (UInt16)devmode.dmPelsHeight;
					aDisplayResolution->bitsPerPixel = (UInt16)devmode.dmBitsPerPel;
					aDisplayResolution->refreshRate = (UInt16)devmode.dmDisplayFrequency;
					sprintf(str, "%d x %d", devmode.dmPelsWidth, devmode.dmPelsHeight);
					strcpy(aDisplayResolution->showStr, str);
					theVisualGraphics->displayResolutionVector.push_back(aDisplayResolution);
				}
			}
		iMode++;
		}
	}
	while (flag);
#endif

	std::sort(theVisualGraphics->displayResolutionVector.begin(), theVisualGraphics->displayResolutionVector.end(), CompareDisplayResolutionAscending());

}


void VisualGraphics::resetDisplayResolutionIterIndex() {
    theVisualGraphics->displayResolutionIterIndex = 0;
}


bool VisualGraphics::getNextAvailableDisplayResolution(char* showStr, UInt8* isSelected) {

	if (theVisualGraphics->displayResolutionIterIndex < theVisualGraphics->displayResolutionVector.size()) {
		if ((theVisualGraphics->displayResolutionVector[theVisualGraphics->displayResolutionIterIndex]->horizontalPixels == theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.horizontalPixels)
			&& (theVisualGraphics->displayResolutionVector[theVisualGraphics->displayResolutionIterIndex]->verticalPixels == theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.verticalPixels)) {
			*isSelected = 1;
		} else {
			*isSelected = 0;
		}
		strcpy(showStr, theVisualGraphics->displayResolutionVector[theVisualGraphics->displayResolutionIterIndex]->showStr);
		theVisualGraphics->displayResolutionIterIndex++;
		return true;
	} else {
		return false;
	}
}


OSStatus VisualGraphics::matchDisplayResolutionShowStrWithPrefs(const char* const selectedShowStr, UInt16& horizontalPixels, UInt16& verticalPixels, UInt16& bitsPerPixel, UInt16& refreshRate) {
	
	OSStatus osStatus = noErr;
	bool found = false;
	
	horizontalPixels = 0;
	verticalPixels = 0;
	bitsPerPixel = 0;
	refreshRate = 0;
	
	DisplayResolutionVectorIterator it;
	for (it = theVisualGraphics->displayResolutionVector.begin(); it != theVisualGraphics->displayResolutionVector.end(); it++) {
		if (strcmp((*it)->showStr, selectedShowStr) == 0) {
			horizontalPixels = (*it)->horizontalPixels;
			verticalPixels = (*it)->verticalPixels;
			bitsPerPixel = (*it)->bitsPerPixel;
			refreshRate = (*it)->refreshRate;
			found = true;
		}
	}
	if (found == false) {
		osStatus = 1001;
	}
	
	return osStatus;
}


void VisualGraphics::evaluateFullscreenDisplayResolution() {
	bool isDefaultValue = false;
	if (theVisualGraphics->displayResolutionVector.size() == 0) {
		theVisualGraphics->gatherAvailableDisplayResolutions();
	}
	theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.horizontalPixels = VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFullscreenWidth, &isDefaultValue);
	if (isDefaultValue == true) {
		// no preferred resolution explicitly set, so we try to evaluate best available resolution
		if (theVisualGraphics->displayResolutionVector.size() > 0) {
			theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution = *(theVisualGraphics->displayResolutionVector[theVisualGraphics->displayResolutionVector.size() - 1]);
		} else {
			theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.horizontalPixels = 0;
			theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.verticalPixels = 0;
			theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.bitsPerPixel = 0;
			theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.refreshRate = 0;
			strcpy(theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.showStr, "unknown best display resolution");
		}
	} else {
		theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.verticalPixels = VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFullscreenHeight);
		theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.bitsPerPixel = VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFullscreenBitsPerPixel);
		theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution.refreshRate = VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFullscreenRefreshRate);
	}
	theVisualGraphics->resetDisplayResolutionIterIndex();
}


DisplayResolution VisualGraphics::returnPreferredDisplayResolution() {
	theVisualGraphics->evaluateFullscreenDisplayResolution();
	return theVisualGraphics->fullscreenDisplayResolution.theFullscreenDisplayResolution;
}


double VisualGraphics::getRandXScreenPos() {
    double xPos;
    double screenKoordWidth;

    xPos = (double) rand() / (double) RAND_MAX;
    //screenKoordWidth = getMaxRightPos() - getMaxLeftPos();
	screenKoordWidth = theVisualGraphics->maxRightCoordOfGLView - theVisualGraphics->maxLeftCoordOfGLView;
    //xPos = (xPos * screenKoordWidth) + getMaxLeftPos();
	xPos = (xPos * screenKoordWidth) + theVisualGraphics->maxLeftCoordOfGLView;

    return xPos;
}


double VisualGraphics::getRandYScreenPos() {
    double yPos;
    double screenKoordHeight;

    yPos = (double) rand() / (double) RAND_MAX;
    //screenKoordHeight = getMaxTopPos() - getMaxBottomPos();
	screenKoordHeight = theVisualGraphics->maxTopCoordOfGLView - theVisualGraphics->maxBottomCoordOfGLView;
    //yPos = (yPos * screenKoordHeight) + getMaxBottomPos();
	yPos = (yPos * screenKoordHeight) + theVisualGraphics->maxBottomCoordOfGLView;

    return yPos;
}


UInt16 VisualGraphics::setCurrentContext() {
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


void VisualGraphics::drawTexture(UInt32 textureNumber, ConstVertexChainRef const vertexChain, bool canUseRectExtension, BlendMode aBlendMode) {

	ConstVertexChainConstIterator it;
	bool debug = false;
    
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
        glBindTexture (GL_TEXTURE_2D, (GLuint)textureNumber);
    } else {
#if TARGET_OS_MAC
        glBindTexture (GL_TEXTURE_RECTANGLE_EXT, (GLuint)textureNumber);
#endif
    }
    
	if (aBlendMode == kBlend) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	
	glBegin (GL_POLYGON);

	for (it = vertexChain->begin(); it != vertexChain->end(); it++) {
		glColor4f((*it)->vertexColor.r, (*it)->vertexColor.g, (*it)->vertexColor.b, (*it)->vertexColor.a);
		glTexCoord2d((*it)->texCoordPosition.sPos, (*it)->texCoordPosition.tPos);
		glVertex3d((*it)->vertexPosition.xPos, (*it)->vertexPosition.yPos, (*it)->vertexPosition.zPos);
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
	
	if (debug == true) {
		glTranslated(0.0, 0.0, 0.0);
		glColor3f(1.0f, 0.0f, 0.0f);
		glLineWidth(3.0f);
		glBegin (GL_LINE_LOOP);
		for (it = vertexChain->begin(); it != vertexChain->end(); it++) {
			glVertex3d((*it)->vertexPosition.xPos, (*it)->vertexPosition.yPos, (*it)->vertexPosition.zPos);
		}
		glEnd();
		
		glTranslated(0.0, 0.0, 0.0);
		glColor3f(1.0f, 1.0f, 0.0f);
		char posStr[64];
		for (it = vertexChain->begin(); it != vertexChain->end(); it++) {
			glVertex3d((*it)->vertexPosition.xPos, (*it)->vertexPosition.yPos, (*it)->vertexPosition.zPos);
			sprintf(posStr, "x: %.2f, y: %.2f", (*it)->texCoordPosition.sPos, (*it)->texCoordPosition.tPos);
			showProcessInfoRow((*it)->vertexPosition.xPos, (*it)->vertexPosition.yPos, posStr);
		}
	}
	
}


void VisualGraphics::drawPixels(UInt32** pixelData, double xCoord, double yCoord, UInt32 width, UInt32 height, UInt16 format, UInt16 dataType, const VisualConvolutionFilter* const aConvolutionFilter) {

	bool useGLConvolutionFilter = false;
	GLfloat* kernel = NULL;

#if TARGET_OS_WIN
	/* Declare function pointers */
	PFNGLCONVOLUTIONFILTER2DPROC glConvolutionFilter2D;
	PFNGLCONVOLUTIONPARAMETERIPROC glConvolutionParameteri;
	PFNGLCONVOLUTIONPARAMETERFVPROC glConvolutionParameterfv;
#endif

	if (aConvolutionFilter != NULL) {
#if TARGET_OS_WIN
		/* Obtain the address of the extension entry points. */
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
		UInt32* filteredPixelData = NULL;
		filteredPixelData = (UInt32*)calloc(width * height, sizeof(UInt32));
		aConvolutionFilter->applyToPixelData((const unsigned char* const)*pixelData, width, height, format, dataType, (unsigned char**)&filteredPixelData);

		free((UInt32*)*pixelData);
		*pixelData = NULL;
		*pixelData = (UInt32*)calloc(width * height, sizeof(UInt32));
		memcpy((UInt32*)*pixelData, (UInt32*)filteredPixelData, width * height * sizeof(UInt32));
		free(filteredPixelData);
		filteredPixelData = NULL;

		//*pixelData = filteredPixelData;
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
		/* Call the function via function pointer */
		(*glConvolutionFilter2D)(GL_CONVOLUTION_2D, GL_LUMINANCE, aConvolutionFilter->getNumberOfKernelValueColumns(), aConvolutionFilter->getNumberOfKernelValueRows(), GL_LUMINANCE, GL_FLOAT, kernel);
		(*glConvolutionParameteri) (GL_CONVOLUTION_2D, GL_CONVOLUTION_BORDER_MODE, GL_CONSTANT_BORDER);
		(*glConvolutionParameterfv) (GL_CONVOLUTION_2D, GL_CONVOLUTION_BORDER_COLOR, borderColor);
#else
		glConvolutionFilter2D(GL_CONVOLUTION_2D, GL_LUMINANCE, aConvolutionFilter->getNumberOfKernelValueColumns(), aConvolutionFilter->getNumberOfKernelValueRows(), GL_LUMINANCE, GL_FLOAT, kernel);
		glConvolutionParameteri(GL_CONVOLUTION_2D, GL_CONVOLUTION_BORDER_MODE, GL_CONSTANT_BORDER);
		glConvolutionParameterfv(GL_CONVOLUTION_2D, GL_CONVOLUTION_BORDER_COLOR, borderColor);
#endif
		
		glEnable(GL_CONVOLUTION_2D);

		glPixelTransferf(GL_POST_CONVOLUTION_RED_SCALE, aConvolutionFilter->getPostConvolutionScaleFactor());
		glPixelTransferf(GL_POST_CONVOLUTION_GREEN_SCALE, aConvolutionFilter->getPostConvolutionScaleFactor());
		glPixelTransferf(GL_POST_CONVOLUTION_BLUE_SCALE, aConvolutionFilter->getPostConvolutionScaleFactor());
		glPixelTransferf(GL_POST_CONVOLUTION_ALPHA_SCALE, aConvolutionFilter->getPostConvolutionScaleFactor());

		/*
		glPixelTransferf(GL_POST_CONVOLUTION_RED_BIAS, aConvolutionFilter->getPostConvolutionBias());
		glPixelTransferf(GL_POST_CONVOLUTION_GREEN_BIAS, aConvolutionFilter->getPostConvolutionBias());
		glPixelTransferf(GL_POST_CONVOLUTION_BLUE_BIAS, aConvolutionFilter->getPostConvolutionBias());
		glPixelTransferf(GL_POST_CONVOLUTION_ALPHA_BIAS, aConvolutionFilter->getPostConvolutionBias());
		*/
		
		/*
		GLfloat filterScale[4];
		filterScale[0] = 1.0f / 9.0f;
		filterScale[1] = 1.0f / 9.0f;
		filterScale[2] = 1.0f / 9.0f;
		filterScale[3] = 1.0f / 9.0f;
		glConvolutionParameterfv(GL_CONVOLUTION_2D, GL_CONVOLUTION_FILTER_SCALE, filterScale);
		*/

	}
	
	//printf("xCoord: %f, yCoord: %f\n", xCoord, yCoord);
	
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


void VisualGraphics::readPixels(double xCoord, double yCoord, UInt32 width, UInt32 height, UInt32** pixelData, UInt16 format, UInt16 dataType) {
	UInt16 xPos, yPos;
	xPos = xCoordToPixel(xCoord);
	yPos = yCoordToPixel(yCoord);
	this->setPixelStorageParams();
	glReadPixels(xPos, yPos, width, height, format, dataType, *pixelData);
}


#if TARGET_OS_WIN
OSStatus VisualGraphics::makeTextureOfStringWin(wchar_t* stringValue, 
														int stringValueLength, 
														UInt32 textureNumber, 
														UInt32 &textureWidth, 
														UInt32 &textureHeight, 
														UInt32 &imageWidth, 
														UInt32 &imageHeight, 
														const char* const fontName, 
														UInt16 fontSize, 
														UInt8 red, 
														UInt8 green, 
														UInt8 blue,
														HorizontalAlignment alignment,
														UInt16 maxPixelWidth,
														UInt16 maxPixelHeight) {
//#define useGetDIBits

	OSStatus osStatus = noErr;

	LOGFONT lFont;
	HDC hdcTemp;
#ifndef useGetDIBits
	BITMAPV5HEADER bi;
#endif
	BYTE* bitmapBits = NULL;
	HBITMAP hbmpTemp;
	unsigned long lWidthPixels;
	unsigned long lHeightPixels;
	HFONT trackTitleFont;
	UInt32 stringSizeWidth = 0;
	UInt32 stringSizeHeight = 0;
	UInt8 fontIsInstalled = 0;
	static UInt8 resourceFontIsPresent = 0;
	char errStr[256];

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();

	if (resourceFontIsPresent == 0) {
		VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
		fontIsInstalled = theVisualGraphics->fontIsInstalled(theVisualGraphicsCore->getWindowDC(), fontName);
		if (fontIsInstalled == 0) {
			void* fontData = NULL;
			UInt32 sizeOfFontResource = 0;
			OSStatus status = VisualFile::getDataOfResource(IDR_DEVROYEFONT, "FONTRESOURCE", &fontData, sizeOfFontResource);
			if (status == noErr) {
				HANDLE fontResHandle = NULL;
				DWORD numberOfFonts = 0;
				fontResHandle = AddFontMemResourceEx(fontData, sizeOfFontResource, 0, &numberOfFonts);
				if (!fontResHandle || numberOfFonts == 0) {
					writeLog("unsuccessful AddFontMemResourceEx");
				} else {
					// system made its own copy
					DeleteObject(fontData);
				}
			}
			resourceFontIsPresent = 1;
		} else {
			// font was already present on system
			resourceFontIsPresent = 1;
		}
	}

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
	if (strlen(fontName) > 31) {
		sprintf(errStr, "length of fontName must not exceed 32 characters but is %d in file: %s (line: %d) [%s])", strlen(fontName), __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		strcpy(lFont.lfFaceName, "Arial"); // fallback font name
	} else {
		strcpy(lFont.lfFaceName, fontName); // length of fontName must not exceed 32 characters (incl. null terminator)
	}

	trackTitleFont = CreateFontIndirect(&lFont);

	if (!trackTitleFont) {
		sprintf(errStr, "err after CreateFontIndirect() in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
	}

	SelectObject(hdcTemp, trackTitleFont);
	SetTextColor(hdcTemp, RGB((BYTE)red, (BYTE)green, (BYTE)blue));

	RECT textDrawRect;
	textDrawRect.top = 0;
	textDrawRect.left = 0;
	textDrawRect.right = theVisualGraphics->getCanvasPixelWidth();
	textDrawRect.bottom = 0;
	DrawTextW(hdcTemp, stringValue, stringValueLength, &textDrawRect, DT_CALCRECT|textFormat);

	if (textDrawRect.bottom == 0) {
		writeLastErrorLog("makeTextureOfStringWin1");
	}

	stringSizeWidth = textDrawRect.right - textDrawRect.left;
	stringSizeHeight = textDrawRect.bottom - textDrawRect.top;

	UInt16 aFontSize = fontSize;
	if ((maxPixelWidth > 0) && (maxPixelHeight > 0)) {
		while ((stringSizeWidth > maxPixelWidth) || (stringSizeHeight > maxPixelHeight)) {
			lFont.lfHeight = MulDiv(aFontSize, GetDeviceCaps(hdcTemp, LOGPIXELSY), 72);
			DeleteObject(trackTitleFont);
			trackTitleFont = CreateFontIndirect(&lFont);
			SelectObject(hdcTemp, trackTitleFont);
			//success = GetTextExtentPoint32W(hdcTemp, stringValue, stringValueLength, &stringSize);
			DrawTextW(hdcTemp, stringValue, stringValueLength, &textDrawRect, DT_CALCRECT|textFormat);
			stringSizeWidth = textDrawRect.right - textDrawRect.left;
			stringSizeHeight = textDrawRect.bottom - textDrawRect.top;
			aFontSize = aFontSize - 1;
		}
	} else if (maxPixelWidth > 0) {
		while (stringSizeWidth > maxPixelWidth) {
			lFont.lfHeight = MulDiv(aFontSize, GetDeviceCaps(hdcTemp, LOGPIXELSY), 72);
			DeleteObject(trackTitleFont);
			trackTitleFont = CreateFontIndirect(&lFont);
			SelectObject(hdcTemp, trackTitleFont);
			//success = GetTextExtentPoint32W(hdcTemp, stringValue, stringValueLength, &stringSize);
			DrawTextW(hdcTemp, stringValue, stringValueLength, &textDrawRect, DT_CALCRECT|textFormat);
			stringSizeWidth = textDrawRect.right - textDrawRect.left;
			stringSizeHeight = textDrawRect.bottom - textDrawRect.top;
			aFontSize = aFontSize - 1;
		}
	} else if (maxPixelHeight > 0) {
		while (stringSizeHeight > maxPixelHeight) {
			lFont.lfHeight = MulDiv(aFontSize, GetDeviceCaps(hdcTemp, LOGPIXELSY), 72);
			DeleteObject(trackTitleFont);
			trackTitleFont = CreateFontIndirect(&lFont);
			SelectObject(hdcTemp, trackTitleFont);
			//success = GetTextExtentPoint32W(hdcTemp, stringValue, stringValueLength, &stringSize);
			DrawTextW(hdcTemp, stringValue, stringValueLength, &textDrawRect, DT_CALCRECT|textFormat);
			stringSizeWidth = textDrawRect.right - textDrawRect.left;
			stringSizeHeight = textDrawRect.bottom - textDrawRect.top;
			aFontSize = aFontSize - 1;
		}
	}

	lWidthPixels = theVisualGraphics->power2Ceiling((UInt16)textDrawRect.right);
	lHeightPixels = theVisualGraphics->power2Ceiling((UInt16)textDrawRect.bottom);

#ifndef useGetDIBits
	
	ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
	bi.bV5Size = sizeof(BITMAPV5HEADER);
	bi.bV5Width = lWidthPixels;
	bi.bV5Height = lHeightPixels;
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;

	// supported 32 BPP alpha format for Windows XP (ARGB)
	bi.bV5RedMask = 0x00FF0000;
	bi.bV5GreenMask = 0x0000FF00;
	bi.bV5BlueMask = 0x000000FF;
	bi.bV5AlphaMask = 0xFF000000;

	hbmpTemp = CreateDIBSection(hdcTemp, (BITMAPINFO *)&bi, DIB_RGB_COLORS, (void**)&bitmapBits, NULL, (DWORD)0);

	if (!hbmpTemp) {
		DeleteDC(hdcTemp);
		ReleaseDC(theVisualGraphicsCore->getGraphicsDevicePort(), hdcTemp);
		sprintf(errStr, "err after CreateDIBSection() in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return 1001;
	}

#endif

#ifdef useGetDIBits

	hbmpTemp = CreateCompatibleBitmap(theVisualGraphicsCore->getWindowDC(), lWidthPixels, lHeightPixels);
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

	UInt16 moveVertical = (UInt16)(lHeightPixels - stringSizeHeight);
	textDrawRect.top += moveVertical;
	textDrawRect.bottom += moveVertical;

	DrawTextW(hdcTemp, stringValue, stringValueLength, &textDrawRect, textFormat);

	GdiFlush();

#ifdef useGetDIBits

	bitmapBits = (BYTE*)malloc(lWidthPixels * lHeightPixels * 4);

	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = lWidthPixels;
	bmi.bmiHeader.biHeight = lHeightPixels;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	GetDIBits(hdcTemp, hbmpTemp, 0, lHeightPixels, bitmapBits, &bmi, DIB_RGB_COLORS);

#endif

	// set alpha bits
	// alpha = 1 where not empty (where text has been rendered)
	// alpha = 0 where no text
	/*
	DWORD x,y;
	DWORD* lpdwPixels;
	lpdwPixels = (DWORD*)bitmapBits;
	for (x = 0; x < lWidthPixels; x++) {
		for (y = 0; y < lHeightPixels; y++) {
			if ((*lpdwPixels & 0x00FFFFFF) != 0x00000000) {
				*lpdwPixels |= 0xFF000000;
			} else {
				*lpdwPixels &= 0x00FFFFFF;
			}
			lpdwPixels++;
		}
	}
	*/

	glBindTexture(GL_TEXTURE_2D, textureNumber);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lWidthPixels, lHeightPixels, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLubyte*)bitmapBits);

	DeleteObject(trackTitleFont);

#ifdef useGetDIBits
	free(bitmapBits);
#endif

	DeleteObject(hbmpTemp);

	DeleteDC(hdcTemp);

	textureWidth = (UInt32)lWidthPixels;
	textureHeight = (UInt32)lHeightPixels;
	imageWidth = (UInt32)stringSizeWidth;
	imageHeight = (UInt32)stringSizeHeight;

	return osStatus;
}
#endif


void VisualGraphics::drawSpot(const float xNum, const float yNum, const float r, const float g, const float b, UInt16 waveformIntensityVal, float intensity, const UInt8 tailSize) {

    float maxRadius = 0.3f;
    int slices = 40;
    int i;
    float scaleRatio;
    //float xNum, yNum, r, g, b;
    //UInt8 waveformIntensityVal;
    //float intensity;
    float rotationVal=3.0;
    UInt8 numberOfIterations;
    
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
        rotationVal -=4.0;
		glLoadIdentity();
		
        glScalef (1.0, 1.0, 1.0);
        glRotatef(rotationVal, 0.0, 0.0, 1.0);
        glTranslatef(xNum, yNum, 0.0);

        glScalef (scaleRatio, scaleRatio, scaleRatio);

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
            glColor4f(r,g,b, intensity/m);
        
            glVertex2d(0.0, 0.0);
        
            glColor4f(r, g, b, 0.0); //glColor4f(r,g,b, intensity*(intensity/2.0)); // aufblenden (test)
        
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
                glColor4f (1.0, 1.0, 1.0, (intensity/3.0f/m)); // weiss
                glVertex2d(0.0, 0.0);
                
                glColor4f(r, g, b, 0.0);
                
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

	static GLfloat rotateAngle = 0.0;

	red = red + 0.003f;
    
    green = red;
    blue = 1.0f - red;
    
    rotateAngle = rotateAngle + 0.3f;
    if (rotateAngle > 360.0) {
        rotateAngle = 0.0;
    }
	
    glRotatef(rotateAngle,0.0,1.0,0.0);

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
	glPointSize(5.0);
	glBegin(GL_POINTS);
		glColor3d(1.0, 1.0, 0.0);
		glVertex3d( 0.0, 0.0, zPlane);
	glEnd();
	glDisable(GL_POINT_SMOOTH);
}


void VisualGraphics::drawPerspectiveRect(UInt32 textureNumber) {
	glColor4f(1.0f, 0.3f, 0.0f, 1.0f); // orange
	if (textureNumber) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureNumber);
	}
	glBegin(GL_QUADS);
	
	if (textureNumber) {
		glTexCoord2d(0.0, 0.0);
	}
	glVertex3d(maxLeftCoordOfGLView, maxBottomCoordOfGLView, 0.0);
	if (textureNumber) {
		glTexCoord2d(1.0, 0.0);
	}
	glVertex3d(maxRightCoordOfGLView, maxBottomCoordOfGLView, 0.0);
	if (textureNumber) {
		glTexCoord2d(1.0, 1.0);
	}
	glVertex3d(maxRightCoordOfGLView, maxBottomCoordOfGLView, -7.0);
	if (textureNumber) {
		glTexCoord2d(0.0, 1.0);
	}
	glVertex3d(maxLeftCoordOfGLView, maxBottomCoordOfGLView, -7.0);
	glEnd();
	if (textureNumber) {
		glDisable(GL_TEXTURE_2D);
	}
	// some numbers along the z-axis
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	char str[4];
	for (int i = 0; i < 10; i++) {
		glRasterPos3d(maxLeftCoordOfGLView, maxBottomCoordOfGLView, (double)i * -1.0);
		sprintf(str, "-%d-", i);
		theVisualGraphics->drawCStringWithGL(str, strlen(str));
	}
}


void VisualGraphics::drawBeatHistogram(const UInt32* const beatHistogram) {
    GLdouble xPos;
    GLdouble xPosIncVal;
	GLdouble height;
	UInt32 maxval=0;
	UInt8 i;
	for (i = 0; i < 100; i++) {
		if (beatHistogram[i] > maxval) {
			maxval = beatHistogram[i];
		}
	}
	xPos = theVisualGraphics->maxLeftCoordOfGLView + 0.2;
	xPosIncVal = ((theVisualGraphics->maxRightCoordOfGLView - theVisualGraphics->maxLeftCoordOfGLView) / 2.0 / 100.0);
	height = theVisualGraphics->maxTopCoordOfGLView - theVisualGraphics->maxBottomCoordOfGLView;
	height /= 3.5;
	//glLoadIdentity();
	glLineWidth(1.0);
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


void VisualGraphics::drawColorlessGLPrimitive(UInt16 primitiveType, ConstVertexChainRef const vertexChain) {
	ConstVertexChainConstIterator it;
	glBegin(primitiveType);
	for (it = vertexChain->begin(); it != vertexChain->end(); it++) {
		glVertex3d((*it)->vertexPosition.xPos, (*it)->vertexPosition.yPos, (*it)->vertexPosition.zPos);
	}
	glEnd();
}


void VisualGraphics::drawGLPrimitive(UInt16 primitiveType, ConstVertexChainRef const vertexChain) {
	ConstVertexChainConstIterator it;
	glBegin(primitiveType);
	for (it = vertexChain->begin(); it != vertexChain->end(); it++) {
		glColor4f((*it)->vertexColor.r, (*it)->vertexColor.g, (*it)->vertexColor.b, (*it)->vertexColor.a);
		glVertex3d((*it)->vertexPosition.xPos, (*it)->vertexPosition.yPos, (*it)->vertexPosition.zPos);
	}
	glEnd();
}


bool VisualGraphics::canUseTextureRectExtension() {
#if TARGET_OS_MAC
/*
	printf(theVisualGraphics->capabilities.strRendererName);
	printf("\n");
	printf(theVisualGraphics->capabilities.strRendererVendor);
	printf("\n");
	printf(theVisualGraphics->capabilities.strRendererVersion);
	printf("\n");
	if (theVisualGraphics->capabilities.glVersion >= 0x0140) {
		printf("Open GL 1.4\n");
	} else if (theVisualGraphics->capabilities.glVersion >= 0x0130) {
		printf("Open GL 1.3\n");
	} else if (theVisualGraphics->capabilities.glVersion >= 0x0120) {
		printf("Open GL 1.2\n");
	} else if (theVisualGraphics->capabilities.glVersion >= 0x0110) {
		printf("Open GL 1.1\n");
	} else if (theVisualGraphics->capabilities.glVersion >= 0x0100) {
		printf("Open GL 1.0\n");
	} else {
		printf("Open GL ?.?");
	}
*/	
	//return false; // test
	
	if (theVisualGraphics->capabilities.fTexRect) {
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


void VisualGraphics::drawWaveform(const SInt16 historyNum, const UInt16 maxNumberOfHistories, const UInt32 numberOfWaveformEntries, SInt16** waveformDataMonoArray) {
    
	if (historyNum == -1) return;

	GLdouble heightOfWaveform;
	GLdouble widthOfWaveform;
	GLdouble bottomCoord;
	GLdouble rightCoord;
    SInt16 historyIdx;
	UInt32 i, k;
	
    GLdouble xPos;
    GLdouble xPosIncVal;

	widthOfWaveform = theVisualGraphics->getCanvasCoordWidth() - theVisualGraphics->xPixelToCoord(300) - theVisualGraphics->xPixelToCoord(30);
	heightOfWaveform = theVisualGraphics->getCanvasCoordHeight() / 7.0;
	bottomCoord = theVisualGraphics->maxBottomCoordOfGLView + theVisualGraphics->yPixelToCoord(10);
	rightCoord = theVisualGraphics->maxLeftCoordOfGLView + theVisualGraphics->xPixelToCoord(10) + widthOfWaveform;
	xPosIncVal = widthOfWaveform / ((float)numberOfWaveformEntries * (float)maxNumberOfHistories);

    glLineWidth(1.0);
    glColor3d(0.7, 0.7, 0.7);
    
	glLoadIdentity();
	glTranslated(0.0, theVisualGraphics->maxBottomCoordOfGLView + theVisualGraphics->xPixelToCoord(60) + heightOfWaveform, 0.0f);

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
	glTranslated(0.0, theVisualGraphics->maxBottomCoordOfGLView + theVisualGraphics->xPixelToCoord(10) + (heightOfWaveform / 2.0), 0.0);

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


void VisualGraphics::drawHistoryDiagram(std::vector<double>& aVector, size_t baseIdx, double minVal, double maxVal) {

	bool drawBorder = true;

	theVisualGraphics = VisualGraphics::getInstance();

	GLdouble diagramCoordWidth = theVisualGraphics->getCanvasCoordWidth() / 3.0;
	GLdouble diagramCoordHeight = theVisualGraphics->getCanvasCoordHeight() / 7.0;
	GLdouble topCoord = theVisualGraphics->maxTopCoordOfGLView - theVisualGraphics->xPixelToCoord(10);;
	GLdouble leftCoord = theVisualGraphics->maxLeftCoordOfGLView + theVisualGraphics->xPixelToCoord(10);
	GLdouble xCoordStepWidth = diagramCoordWidth / static_cast<GLdouble>(aVector.size());

    glLineWidth(1.0);
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
		glLineWidth(1.0);
		glColor4d(0.0, 0.0, 1.0, 0.7);
		glBegin (GL_LINE_LOOP);
			glVertex2d(leftCoord, topCoord - diagramCoordHeight);
			glVertex2d(leftCoord, topCoord);
			glVertex2d(leftCoord + diagramCoordWidth, topCoord);
			glVertex2d(leftCoord + diagramCoordWidth, topCoord - diagramCoordHeight);
		glEnd();
	}
	
}


void VisualGraphics::drawWaveformSpiral(const UInt16 currHistoryNum, const UInt16 numberOfWaveformEntries, const SInt16* const waveformDataMonoArray) {
    // eine Spriale der waveform-History

     
     // neue Version der drawWaveformSpiral
     //20030129 (HW)
     //
     
    UInt16 index;
    UInt16 historyIndex;
    GLfloat waveformVal;
    static GLfloat cosTwoTimesPi = 0.0;
    static GLfloat sinTwoTimesPi = 0.0;
    // float radius = 0.25;
    float radius = 0.02f;
    float waveRadius;
    //UInt8 waveformHistory[20][kVisualNumWaveformEntries];
    //UInt16 timeStoreIndex;
    //static UInt16 accuElapsedMilliSecs;
    UInt32 accuElapsedMilliSecs;
    //UInt8 oneSecondIsOverBool = 0;
    static SInt8 redHistoryNum = 0;
	UInt16 theCurrHistoryNum;
	
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
	//pMusicStatsStruct->getMonoWaveformData((UInt8*)waveformHistory, 20);
    
	glColor4f(0.0, 0.0, 0.0, 1.0);
    glLineWidth(1.0);
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
                glColor4f(1.0, 0.0, 0.0, 1.0); // rot
            } else {
                glColor4f(0.0, 0.0, 0.0, 1.0);
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


void VisualGraphics::drawSpectrumAnalyzer(const SInt16 currHistoryNum, const UInt16 numberOfHistories, const UInt32 numberOfSpectrumEntries, const UInt16 numberOfAudioChannels, const UInt8*** const spectrumDataArray) {
    
	if (currHistoryNum == -1) return;
	
    GLdouble xPos;
	GLdouble yPos;
	GLdouble coordWidth;
	GLdouble coordHeight;
    UInt16 index, i, k;
    GLdouble spectrumVal;
	UInt16 numberOfSubBands = 15;
	float* spectrumDataSubBands;
	float maxSpectrumSum = 0;
	UInt32 numberOfEntriesPerBarGraph;
	UInt16 numberOfBlocksPerBarGraph = 10;
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
	coordWidth = theVisualGraphics->xPixelToCoord(300);
	coordHeight = theVisualGraphics->yPixelToCoord(70);
	heightOfCell = coordHeight / (float)numberOfBlocksPerBarGraph;
	heightOfBlock = heightOfCell * 0.8;
	heightOfGapBetweenBlocks = heightOfCell * 0.2;
	widthOfCell = coordWidth / (float)numberOfSubBands;
	widthOfBlock = widthOfCell * 0.9;
	
	xPos = theVisualGraphics->maxRightCoordOfGLView - coordWidth;
	xPos -= theVisualGraphics->xPixelToCoord(10);
    
    glLineWidth(1.0);
    glColor3d(0.7,0.7,0.7);
    
	glLoadIdentity();
	
	glPointSize(1.0);
    
	glTranslated(0.0, theVisualGraphics->maxBottomCoordOfGLView + theVisualGraphics->xPixelToCoord(10), 0.0);
	
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
				glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
			} else {
				glColor4f(0.8f, 0.8f, 0.8f, 0.1f);
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


void VisualGraphics::drawSpectrogram(const SInt16 currHistoryNum, const UInt16 numberOfHistories, const UInt32 numberOfSpectrumEntries, const UInt16 numberOfAudioChannels, const UInt8*** const spectrumDataArray) {
    
	if (currHistoryNum == -1) return;

	GLdouble heightOfSpectrogram;
	GLdouble widthOfSpectrogram;
	GLdouble bottomCoord;
	GLdouble leftCoord;
	GLdouble rowCoordHeight;
	GLdouble xCoordStepSize;
	GLdouble xCoordPos;
	GLdouble yCoordPos;
	UInt16 i;
	UInt16 k;
	SInt16 historyIdx;
	char strInC[32];
	
	widthOfSpectrogram = theVisualGraphics->xPixelToCoord(300);
	heightOfSpectrogram = theVisualGraphics->getCanvasCoordHeight() - theVisualGraphics->yPixelToCoord(95);
	bottomCoord = theVisualGraphics->getMaxBottomCoordOfCanvas() + theVisualGraphics->yPixelToCoord(85);
	leftCoord = theVisualGraphics->getMaxRightCoordOfCanvas() - widthOfSpectrogram - theVisualGraphics->yPixelToCoord(10);
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
			glColor4f((float)spectrumDataArray[historyIdx][0][k] / 255.0f, 0.0f, 0.0f, 0.8f);
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

	glRotatef(rotVal, 0.0, 0.0, 1.0);
	glTranslatef(xNum, yNum, 0.0);

	// glCallList(startList);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	qobj = gluNewQuadric();

	gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);

	gluDisk (qobj, 0.0, radius, 10, 1);

}


void VisualGraphics::drawWaveformShape(const SInt8* const waveformShapeArray, const UInt16 maxNumberOfWaveformShapeHistory) {
    UInt16 index;
    GLdouble xPoint;
    
	xPoint = theVisualGraphics->maxLeftCoordOfGLView;
    
    glLineWidth(0.6f);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 0.3f); // white
    glBegin (GL_LINES);
    for (index = 0; index < maxNumberOfWaveformShapeHistory; index++) {
        xPoint += 0.004f;
		glVertex2d(xPoint, (GLfloat)waveformShapeArray[index * 2 + 0]/256.0);
        glVertex2d(xPoint, (GLfloat)waveformShapeArray[index * 2 + 1]/256.0);
    }
    glEnd();
    glDisable (GL_BLEND);
}


void VisualGraphics::drawCStringWithGL (const char* const cString, const UInt16 stringLength) {
#if TARGET_OS_MAC
    for (UInt16 i = 0; i < stringLength; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, cString[i]);
    }
#endif
#if TARGET_OS_WIN
	theVisualGraphics->glPrint(cString);
#endif
}


void VisualGraphics::doFallbackActorShow(const char* const visualActorName) {
	GLdouble xCoordPos, yCoordPos;
    int len;
	char fallbackActorString[128];
	strcpy(fallbackActorString, visualActorName);
	strcat(fallbackActorString, ": unimplemented show() method in VisualActor implementation");
	len = (int)strlen(fallbackActorString);

	xCoordPos = theVisualGraphics->maxLeftCoordOfGLView + theVisualGraphics->xPixelToCoord(10);
	yCoordPos = theVisualGraphics->maxBottomCoordOfGLView + theVisualGraphics->yPixelToCoord(62);
	glColor3d(1.0, 1.0, 0.0);
	glRasterPos3d(xCoordPos, yCoordPos, 0.0);
    theVisualGraphics->drawCStringWithGL(fallbackActorString, len);
}


float VisualGraphics::getCosTwoTimesPi() {
    return (float)(cos (2.0f * (float)M_PI));
}


float VisualGraphics::getSinTwoTimesPi() {
    return (float)(sin (2.0f * (float)M_PI));
}


UInt32 VisualGraphics::getNextFreeTextureName() {
	GLuint aTextureName;
	glGenTextures(1, &aTextureName);
	return (UInt32)aTextureName;
}


void VisualGraphics::deleteTextures(const UInt16 numberOfTextures, const UInt32* const textureNames) {
	glDeleteTextures(numberOfTextures, (GLuint*)textureNames);
}


void VisualGraphics::showProcessInfoRow(double xNum, double yNum, const char* const textRowStr) {
    int len;
	glRasterPos3d(xNum, yNum, 0.0);
    len = (int)strlen(textRowStr);
    theVisualGraphics->drawCStringWithGL(textRowStr, len);
}


void VisualGraphics::showProcessInfoNote() {
	char noteStr[128];
	GLdouble xCoordPos, yCoordPos;
	glColor3f(1.0f, 1.0f, 1.0f);
	strcpy(noteStr, "Press [s] on the keyboard to hide Process Monitor Info");
	xCoordPos = theVisualGraphics->maxLeftCoordOfGLView + theVisualGraphics->xPixelToCoord(10);
	yCoordPos = theVisualGraphics->maxBottomCoordOfGLView + theVisualGraphics->yPixelToCoord(36);
	glRasterPos3d(xCoordPos, yCoordPos, 0.0);
	theVisualGraphics->drawCStringWithGL(noteStr, strlen(noteStr));
	strcpy(noteStr, "Press [a] on the keyboard to hide Process Monitor Audio Info");
	xCoordPos = theVisualGraphics->maxLeftCoordOfGLView + theVisualGraphics->xPixelToCoord(10);
	yCoordPos = theVisualGraphics->maxBottomCoordOfGLView + theVisualGraphics->yPixelToCoord(18);
	glRasterPos3d(xCoordPos, yCoordPos, 0.0);
	theVisualGraphics->drawCStringWithGL(noteStr, strlen(noteStr));
}


#if TARGET_OS_MAC
CGContextRef VisualGraphics::createBitmapContext(size_t pixelWidth, size_t pixelHeight) {
 
	size_t bitsPerComponent = 8;
	
    UInt16 bitmapBytesPerRow = pixelWidth * 4;
    UInt32 bitmapByteCount = bitmapBytesPerRow * pixelHeight;
 
    //CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB(); // In Mac OS X v10.4 and later, this color space is no longer device-dependent and is replaced by the generic counterpart—kCGColorSpaceGenericRGB 
	
    UInt8* bitmapData = (UInt8*)malloc(bitmapByteCount);
    if (bitmapData == NULL) {
        writeLog("bitmapData is NULL in createBitmapContext");
        return NULL;
    }
	memset(bitmapData, 0, bitmapByteCount);

	CGContextRef context = CGBitmapContextCreate(bitmapData, pixelWidth, pixelHeight, bitsPerComponent, bitmapBytesPerRow, colorSpace, kCGImageAlphaPremultipliedFirst);
    if (context == NULL) {
		free (bitmapData);
        writeLog("context is NULL in createBitmapContext");
        return NULL;
    }
    CGColorSpaceRelease(colorSpace);
 
    return context;
}
#endif


UInt32* VisualGraphics::createARGBCheckPixels(UInt32 width, UInt32 height) {

	const char* color = "red"; // red, green, blue, white
	
	UInt8 numberOfChannels = 4; // ARGB
	char* argbPixels = (char*)malloc(width * height * numberOfChannels * sizeof(char));
	if (argbPixels == NULL) {
		char errStr[256];
		sprintf(errStr, "argbPixels == NULL in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return (UInt32*)argbPixels;
	}

	UInt32 cnt = 0;
	UInt32 i;
	UInt32 j;
	UInt32 c;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			if (c == 255) {
				argbPixels[cnt + 0] = (char)255; // alpha
			} else {
				argbPixels[cnt + 0] = (char)0; // alpha
			}
			if (strcmp(color, "red") == 0 || strcmp(color, "white") == 0) {
				argbPixels[cnt + 1] = (char)c;
			} else {
				argbPixels[cnt + 1] = (char)0;
			}
			if (strcmp(color, "green") == 0 || strcmp(color, "white") == 0) {
				argbPixels[cnt + 2] = (char)c;
			} else {
				argbPixels[cnt + 2] = (char)0;
			}
			if (strcmp(color, "blue") == 0 || strcmp(color, "white") == 0) {
				argbPixels[cnt + 3] = (char)c;
			} else {
				argbPixels[cnt + 3] = (char)0;
			}
			cnt += 4;
		}
	}
	
	return (UInt32*)argbPixels;
}


UInt32* VisualGraphics::createBGRACheckPixels(UInt32 width, UInt32 height) {

	const char* color = "red"; // red, green, blue, white
	
	UInt8 numberOfChannels = 4; // BGRA
	char* bgraPixels = (char*)malloc(width * height * numberOfChannels * sizeof(char));
	if (bgraPixels == NULL) {
		char errStr[256];
		sprintf(errStr, "bgraPixels == NULL in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return (UInt32*)bgraPixels;
	}

	UInt32 cnt = 0;
	UInt32 i;
	UInt32 j;
	UInt32 c;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			if (strcmp(color, "blue") == 0 || strcmp(color, "white") == 0) {
				bgraPixels[cnt + 0] = (char)c;
			} else {
				bgraPixels[cnt + 0] = (char)0;
			}
			if (strcmp(color, "green") == 0 || strcmp(color, "white") == 0) {
				bgraPixels[cnt + 1] = (char)c;
			} else {
				bgraPixels[cnt + 1] = (char)0;
			}
			if (strcmp(color, "red") == 0 || strcmp(color, "white") == 0) {
				bgraPixels[cnt + 2] = (char)c;
			} else {
				bgraPixels[cnt + 2] = (char)0;
			}
			if (c == 255) {
				bgraPixels[cnt + 3] = (char)255; // alpha
			} else {
				bgraPixels[cnt + 3] = (char)0; // alpha
			}
			cnt += 4;
		}
	}
	
	return (UInt32*)bgraPixels;
}


UInt32* VisualGraphics::createRGBACheckPixels(UInt32 width, UInt32 height) {

	const char* color = "red"; // red, green, blue, white
	
	UInt8 numberOfChannels = 4; // RGBA
	char* rgbaPixels = (char*)malloc(width * height * numberOfChannels * sizeof(char));
	if (rgbaPixels == NULL) {
		char errStr[256];
		sprintf(errStr, "rgbaPixels == NULL in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return (UInt32*)rgbaPixels;
	}

	UInt32 cnt = 0;
	UInt32 i;
	UInt32 j;
	UInt32 c;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			if (strcmp(color, "red") == 0 || strcmp(color, "white") == 0) {
				rgbaPixels[cnt + 0] = (char)c;
			} else {
				rgbaPixels[cnt + 0] = (char)0;
			}
			if (strcmp(color, "green") == 0 || strcmp(color, "white") == 0) {
				rgbaPixels[cnt + 1] = (char)c;
			} else {
				rgbaPixels[cnt + 1] = (char)0;
			}
			if (strcmp(color, "blue") == 0 || strcmp(color, "white") == 0) {
				rgbaPixels[cnt + 2] = (char)c;
			} else {
				rgbaPixels[cnt + 2] = (char)0;
			}
			if (c == 255) {
				rgbaPixels[cnt + 3] = (char)255; // alpha
			} else {
				rgbaPixels[cnt + 3] = (char)0; // alpha
			}
			cnt += 4;
		}
	}
	
	return (UInt32*)rgbaPixels;
}


UInt32* VisualGraphics::createABGRCheckPixels(UInt32 width, UInt32 height) {

	const char* color = "red"; // red, green, blue, white
	
	UInt8 numberOfChannels = 4; // ABGR
	char* abgrPixels = (char*)malloc(width * height * numberOfChannels * sizeof(char));
	if (abgrPixels == NULL) {
		char errStr[256];
		sprintf(errStr, "abgrPixels == NULL in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return (UInt32*)abgrPixels;
	}

	UInt32 cnt = 0;
	UInt32 i;
	UInt32 j;
	UInt32 c;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			if (c == 255) {
				abgrPixels[cnt + 0] = (char)255; // alpha
			} else {
				abgrPixels[cnt + 0] = (char)0; // alpha
			}
			if (strcmp(color, "blue") == 0 || strcmp(color, "white") == 0) {
				abgrPixels[cnt + 1] = (char)c;
			} else {
				abgrPixels[cnt + 1] = (char)0;
			}
			if (strcmp(color, "green") == 0 || strcmp(color, "white") == 0) {
				abgrPixels[cnt + 2] = (char)c;
			} else {
				abgrPixels[cnt + 2] = (char)0;
			}
			if (strcmp(color, "red") == 0 || strcmp(color, "white") == 0) {
				abgrPixels[cnt + 3] = (char)c;
			} else {
				abgrPixels[cnt + 3] = (char)0;
			}
			cnt += 4;
		}
	}
	
	return (UInt32*)abgrPixels;
}


void VisualGraphics::createCheckTexture(UInt32& textureNumber, UInt32& textureWidth, UInt32& textureHeight, UInt32& imageWidth, UInt32& imageHeight) {

	GLuint texid;
	textureWidth = 64;
	textureHeight = 64;
	imageWidth = 64;
	imageHeight = 64;
	
	UInt32* rgbaPixels = this->createARGBCheckPixels(textureWidth, textureHeight);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texid);

	glBindTexture(GL_TEXTURE_2D, texid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	this->setPixelStorageParams();

#if TARGET_OS_MAC
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, (GLuint*)rgbaPixels);
#endif
#if TARGET_OS_WIN
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLubyte*)rgbaPixels);
#endif

	glDisable(GL_TEXTURE_2D);

	free(rgbaPixels);

	textureNumber = texid;

}


OSStatus VisualGraphics::copyARGBBitmapDataToTexture(UInt32 textureNumber, UInt32 width, UInt32 height, bool canUseRectExtension, UInt16 format, UInt16 dataType, const UInt32** bitmapData) {

	bool debug = false;

	OSStatus status = noErr;

	GLuint* argbBitmapData;
	if (debug == true) {
		argbBitmapData = (GLuint*)this->createARGBCheckPixels(width, height);
	} else {
		argbBitmapData = (GLuint*)*bitmapData;
	}

	if (canUseRectExtension == false) {
		glEnable (GL_TEXTURE_2D);
	} else {
#if TARGET_OS_MAC
		glEnable (GL_TEXTURE_RECTANGLE_EXT);
#endif
	}

	if (canUseRectExtension == false) {
		glBindTexture (GL_TEXTURE_2D, (GLuint)textureNumber);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	} else {
#if TARGET_OS_MAC
		glBindTexture (GL_TEXTURE_RECTANGLE_EXT, (GLuint)textureNumber);
#endif
	}
	
	this->setPixelStorageParams();

	if (canUseRectExtension == false) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, dataType, argbBitmapData);
	} else {
#if TARGET_OS_MAC
		glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, width, height, 0, format, dataType, argbBitmapData);
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
		free(argbBitmapData);
	}
	
	return status;

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


void VisualGraphics::setMaxTopCoordOfCanvas(double theMaxTopCoordOfGLView) {
	theVisualGraphics->maxTopCoordOfGLView = theMaxTopCoordOfGLView;
}


void VisualGraphics::setMaxLeftCoordOfCanvas(double theMaxLeftCoordOfGLView) {
	theVisualGraphics->maxLeftCoordOfGLView = theMaxLeftCoordOfGLView;
}


void VisualGraphics::setMaxBottomCoordOfCanvas(double theMaxBottomCoordOfGLView) {
	theVisualGraphics->maxBottomCoordOfGLView = theMaxBottomCoordOfGLView;
}


void VisualGraphics::setMaxRightCoordOfCanvas(double theMaxRightCoordOfGLView) {
	theVisualGraphics->maxRightCoordOfGLView = theMaxRightCoordOfGLView;
}


void VisualGraphics::setMaxNearCoordOfCanvas(double theMaxNearCoordOfGLView) {
	theVisualGraphics->maxNearCoordOfGLView = theMaxNearCoordOfGLView;
}


void VisualGraphics::setMaxFarCoordOfCanvas(double theMaxFarCoordOfGLView) {
	theVisualGraphics->maxFarCoordOfGLView = theMaxFarCoordOfGLView;
}


double VisualGraphics::getMaxTopCoordOfCanvas() {
	return theVisualGraphics->maxTopCoordOfGLView;
}


double VisualGraphics::getMaxLeftCoordOfCanvas() {
	return theVisualGraphics->maxLeftCoordOfGLView;
}


double VisualGraphics::getMaxBottomCoordOfCanvas() {
	return theVisualGraphics->maxBottomCoordOfGLView;
}


double VisualGraphics::getMaxRightCoordOfCanvas() {
	return theVisualGraphics->maxRightCoordOfGLView;
}


double VisualGraphics::getMaxNearCoordOfCanvas() {
	return theVisualGraphics->maxNearCoordOfGLView;
}


double VisualGraphics::getMaxFarCoordOfCanvas() {
	return theVisualGraphics->maxFarCoordOfGLView;
}


int VisualGraphics::getCanvasXOriginOffset() {
	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	return theVisualGraphicsCore->getCanvasXOriginOffset();
}


int VisualGraphics::getCanvasYOriginOffset() {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	return theVisualGraphicsCore->getCanvasYOriginOffset();
}


const char* const VisualGraphics::getRendererName() {
	return theVisualGraphics->capabilities.strRendererName;
}


const char* const VisualGraphics::getRendererVendor() {
	return theVisualGraphics->capabilities.strRendererVendor;
}


const char* const VisualGraphics::getRendererVersion() {
	return theVisualGraphics->capabilities.strRendererVersion;
}


unsigned short VisualGraphics::getGLVersion() {
	return theVisualGraphics->capabilities.glVersion;
}


long VisualGraphics::getMaxTextureSize() {
	return (long)theVisualGraphics->capabilities.maxTextureSize;
}


double VisualGraphics::xPixelToCoord(UInt32 pixelPos) {
	return ((double)pixelPos * (theVisualGraphics->getCanvasCoordWidth() / (double)theVisualGraphics->getCanvasPixelWidth()));
}


double VisualGraphics::yPixelToCoord(UInt32 pixelPos) {
	return ((double)pixelPos * (theVisualGraphics->getCanvasCoordHeight() / (double)theVisualGraphics->getCanvasPixelHeight()));
}


UInt16 VisualGraphics::xCoordToPixel(double coordPos) {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	double tempCoord = coordPos + (theVisualGraphics->getCanvasCoordWidth() / 2.0);
	
	return (UInt16)(tempCoord * ((double)theVisualGraphics->getCanvasPixelWidth() / theVisualGraphics->getCanvasCoordWidth()));
}


UInt16 VisualGraphics::yCoordToPixel(double coordPos) {
	VisualGraphicsCore* theVisualGraphicsCore;
	theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	double tempCoord = coordPos + (theVisualGraphics->getCanvasCoordHeight() / 2.0);
	
	return (UInt16)(tempCoord * ((double)theVisualGraphics->getCanvasPixelHeight() / theVisualGraphics->getCanvasCoordHeight()));
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
	
	theVisualGraphics->capabilities.glVersion = 0;

	// get renderer strings
	strRend = glGetString (GL_RENDERER);
	strncpy (theVisualGraphics->capabilities.strRendererName, (char*)strRend, 255);
	strVend = glGetString (GL_VENDOR);
	strncpy (theVisualGraphics->capabilities.strRendererVendor, (char*)strVend, 255);
	strVers = glGetString (GL_VERSION);
	strncpy (theVisualGraphics->capabilities.strRendererVersion, (char*)strVers, 255);
	{ // get BCD version
		short j = 0;
		short shiftVal = 8;
		while (((strVers[j] <= '9') && (strVers[j] >= '0')) || (strVers[j] == '.')) { 
			// get only basic version info (until first non-digit or non-.)
			if ((strVers[j] <= '9') && (strVers[j] >= '0')) {
				theVisualGraphics->capabilities.glVersion += (strVers[j] - '0') << shiftVal;
				shiftVal -= 4;
			}
			j++;
		}
	}

	strExt = glGetString (GL_EXTENSIONS);
	//writeLog((char*)strExt);

/*
	UInt16 cnt = 0;
	char* token = NULL;
	char* text=(char *)malloc(strlen((char *)glGetString(GL_EXTENSIONS))+1);	// Allocate Memory For Our Extension String
	strcpy (text,(char *)glGetString(GL_EXTENSIONS));		// Grab The Extension List, Store In Text

	token=strtok(text," ");									// Parse 'text' For Words, Seperated By " " (spaces)
	UInt16 maxtokens = 80;
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
	theVisualGraphics->capabilities.maxTextureSize = (long)maxTextureSize;

	// get caps
#if TARGET_OS_MAC
	glGetIntegerv (GL_MAX_TEXTURE_UNITS, 
		&theVisualGraphics->capabilities.textureUnits);
	glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE, 
		&theVisualGraphics->capabilities.max3DTextureSize);
	glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE, 
		&theVisualGraphics->capabilities.maxCubeMapTextureSize);

	// get functionality info
	theVisualGraphics->capabilities.fSpecularVector = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_specular_vector", strExt);
	theVisualGraphics->capabilities.fTransformHint = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_transform_hint", strExt);
	theVisualGraphics->capabilities.fPackedPixels = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_packed_pixels", strExt) || 
		gluCheckExtension ((GLubyte*)"GL_APPLE_packed_pixel", strExt)  || 
		(theVisualGraphics->capabilities.glVersion >= 0x0120);
	theVisualGraphics->capabilities.fClientStorage = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_client_storage", strExt);
	theVisualGraphics->capabilities.fYCbCr = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_ycbcr_422", strExt);
	theVisualGraphics->capabilities.fTextureRange = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_texture_range", strExt);
	theVisualGraphics->capabilities.fFence = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_fence", strExt);
	theVisualGraphics->capabilities.fVAR = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_vertex_array_range", strExt);
	theVisualGraphics->capabilities.fVAO = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_vertex_array_object", strExt);
	theVisualGraphics->capabilities.fElementArray = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_element_array", strExt);
	theVisualGraphics->capabilities.fVPEvals = 
		gluCheckExtension((GLubyte*)"GL_APPLE_vertex_program_evaluators",strExt);
	theVisualGraphics->capabilities.fFloatPixels = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_float_pixels", strExt);
	theVisualGraphics->capabilities.fFlushRenderer = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_flush_render", strExt);
	theVisualGraphics->capabilities.fPixelBuffer = 
		gluCheckExtension ((GLubyte*)"GL_APPLE_pixel_buffer", strExt);
	theVisualGraphics->capabilities.fImaging = 
		gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	theVisualGraphics->capabilities.fTransposeMatrix = 
		gluCheckExtension ((GLubyte*)"GL_ARB_transpose_matrix", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0130);
	theVisualGraphics->capabilities.fMultitexture = 
		gluCheckExtension ((GLubyte*)"GL_ARB_multitexture", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0130);
	theVisualGraphics->capabilities.fTexEnvAdd = 
		gluCheckExtension ((GLubyte*)"GL_ARB_texture_env_add", strExt) ||
		gluCheckExtension ((GLubyte*)"GL_EXT_texture_env_add", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0130);
	theVisualGraphics->capabilities.fTexEnvCombine = 
		gluCheckExtension ((GLubyte*)"GL_ARB_texture_env_combine", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0130);
	theVisualGraphics->capabilities.fTexEnvDot3 = 
		gluCheckExtension ((GLubyte*)"GL_ARB_texture_env_dot3", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0130);
	theVisualGraphics->capabilities.fTexEnvCrossbar = 
		gluCheckExtension ((GLubyte*)"GL_ARB_texture_env_crossbar", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fTexCubeMap = 
		gluCheckExtension ((GLubyte*)"GL_ARB_texture_cube_map", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0130);
	theVisualGraphics->capabilities.fTexCompress = 
		gluCheckExtension ((GLubyte*)"GL_ARB_texture_compression", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0130);
	theVisualGraphics->capabilities.fMultisample = 
		gluCheckExtension ((GLubyte*)"GL_ARB_multisample", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0130);
	theVisualGraphics->capabilities.fTexBorderClamp = 
		gluCheckExtension ((GLubyte*)"GL_ARB_texture_border_clamp", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0130);
	theVisualGraphics->capabilities.fPointParam = 
		gluCheckExtension ((GLubyte*)"GL_ARB_point_parameters", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fVertexProg = 
		gluCheckExtension ((GLubyte*)"GL_ARB_vertex_program", strExt);
	theVisualGraphics->capabilities.fFragmentProg = 
		gluCheckExtension ((GLubyte*)"GL_ARB_fragment_program", strExt);
	theVisualGraphics->capabilities.fTexMirrorRepeat = 
		gluCheckExtension ((GLubyte*)"GL_ARB_texture_mirrored_repeat", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fDepthTex = 
		gluCheckExtension ((GLubyte*)"GL_ARB_depth_texture", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fShadow = 
		gluCheckExtension ((GLubyte*)"GL_ARB_shadow", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fShadowAmbient = 
		gluCheckExtension ((GLubyte*)"GL_ARB_shadow_ambient", strExt);
	theVisualGraphics->capabilities.fVertexBlend = 
		gluCheckExtension ((GLubyte*)"GL_ARB_vertex_blend", strExt);
	theVisualGraphics->capabilities.fWindowPos = 
		gluCheckExtension ((GLubyte*)"GL_ARB_window_pos", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fTex3D = 
		gluCheckExtension ((GLubyte*)"GL_EXT_texture3D", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0120);
	theVisualGraphics->capabilities.fClipVolHint = 
		gluCheckExtension ((GLubyte*)"GL_EXT_clip_volume_hint", strExt);
	theVisualGraphics->capabilities.fRescaleNorm = 
		gluCheckExtension ((GLubyte*)"GL_EXT_rescale_normal", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0120);
	theVisualGraphics->capabilities.fBlendColor = 
		gluCheckExtension ((GLubyte*)"GL_EXT_blend_color", strExt) ||
		gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	theVisualGraphics->capabilities.fBlendMinMax = 
		gluCheckExtension ((GLubyte*)"GL_EXT_blend_minmax", strExt) ||
		gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	theVisualGraphics->capabilities.fBlendSub = 
		gluCheckExtension ((GLubyte*)"GL_EXT_blend_subtract", strExt) ||
		gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	theVisualGraphics->capabilities.fCVA = 
		gluCheckExtension ((GLubyte*)"GL_EXT_compiled_vertex_array", strExt);
	theVisualGraphics->capabilities.fTexLODBias = 
		gluCheckExtension ((GLubyte*)"GL_EXT_texture_lod_bias", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fABGR = 
		gluCheckExtension ((GLubyte*)"GL_EXT_abgr", strExt);
	theVisualGraphics->capabilities.fBGRA = 
		gluCheckExtension ((GLubyte*)"GL_EXT_bgra", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0120);
	theVisualGraphics->capabilities.fTexFilterAniso = 
		gluCheckExtension ((GLubyte*)"GL_EXT_texture_filter_anisotropic",strExt);
	theVisualGraphics->capabilities.fPaletteTex = 
		gluCheckExtension ((GLubyte*)"GL_EXT_paletted_texture", strExt);
	theVisualGraphics->capabilities.fShareTexPalette = 
		gluCheckExtension ((GLubyte*)"GL_EXT_shared_texture_palette", strExt);
	theVisualGraphics->capabilities.fSecColor = 
		gluCheckExtension ((GLubyte*)"GL_EXT_secondary_color", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fTexCompressS3TC = 
		gluCheckExtension ((GLubyte*)"GL_EXT_texture_compression_s3tc", strExt);
	theVisualGraphics->capabilities.fTexRect = 
		gluCheckExtension ((GLubyte*)"GL_EXT_texture_rectangle", strExt);
	theVisualGraphics->capabilities.fFogCoord = 
		gluCheckExtension ((GLubyte*)"GL_EXT_fog_coord", strExt);
	theVisualGraphics->capabilities.fDrawRangeElements = 
		gluCheckExtension ((GLubyte*)"GL_EXT_draw_range_elements", strExt);
	theVisualGraphics->capabilities.fStencilWrap = 
		gluCheckExtension ((GLubyte*)"GL_EXT_stencil_wrap", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fBlendFuncSep = 
		gluCheckExtension ((GLubyte*)"GL_EXT_blend_func_separate", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fMultiDrawArrays = 
		gluCheckExtension ((GLubyte*)"GL_EXT_multi_draw_arrays", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fShadowFunc = 
		gluCheckExtension ((GLubyte*)"GL_EXT_shadow_funcs", strExt);
	theVisualGraphics->capabilities.fStencil2Side = 
		gluCheckExtension ((GLubyte*)"GL_EXT_stencil_two_side", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fColorSubtable = 
		gluCheckExtension ((GLubyte*)"GL_EXT_color_subtable", strExt) || 
		gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	theVisualGraphics->capabilities.fConvolution = 
		gluCheckExtension ((GLubyte*)"GL_EXT_convolution", strExt) || 
		gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	theVisualGraphics->capabilities.fHistogram = 
		gluCheckExtension ((GLubyte*)"GL_EXT_histogram", strExt) || 
		gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	theVisualGraphics->capabilities.fColorTable = 
		gluCheckExtension ((GLubyte*)"GL_SGI_color_table", strExt) || 
		gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	theVisualGraphics->capabilities.fColorMatrix = 
		gluCheckExtension ((GLubyte*)"GL_SGI_color_matrix", strExt) || 
		gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);
	theVisualGraphics->capabilities.fTexEdgeClamp = 
		gluCheckExtension ((GLubyte*)"GL_SGIS_texture_edge_clamp", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0120);
	theVisualGraphics->capabilities.fGenMipmap = 
		gluCheckExtension ((GLubyte*)"GL_SGIS_generate_mipmap", strExt);
	theVisualGraphics->capabilities.fTexLOD = 
		gluCheckExtension ((GLubyte*)"GL_SGIS_texture_lod", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0120);
	theVisualGraphics->capabilities.fPointCull = 
		gluCheckExtension ((GLubyte*)"GL_ATI_point_cull_mode", strExt);
	theVisualGraphics->capabilities.fTexMirrorOnce = 
		gluCheckExtension ((GLubyte*)"GL_ATI_texture_mirror_once", strExt);
	theVisualGraphics->capabilities.fPNtriangles = 
		gluCheckExtension ((GLubyte*)"GL_ATI_pn_triangles", strExt) ||
		gluCheckExtension ((GLubyte*)"GL_ATIX_pn_triangles", strExt);
	theVisualGraphics->capabilities.fTextFragShader = 
		gluCheckExtension ((GLubyte*)"GL_ATI_text_fragment_shader", strExt);
	theVisualGraphics->capabilities.fBlendEqSep = 
		gluCheckExtension ((GLubyte*)"GL_ATI_blend_equation_separate", strExt);
	theVisualGraphics->capabilities.fBlendWeightMinMax = 
		gluCheckExtension ((GLubyte*)"GL_ATI_blend_weighted_minmax", strExt);
	theVisualGraphics->capabilities.fCombine3 = 
		gluCheckExtension ((GLubyte*)"GL_ATI_texture_env_combine3", strExt);
	theVisualGraphics->capabilities.fSepStencil = 
		gluCheckExtension ((GLubyte*)"GL_ATI_separate_stencil", strExt);
	theVisualGraphics->capabilities.fArrayRevComps4Byte = 
		gluCheckExtension ((GLubyte*)"GL_ATI_array_rev_comps_in_4_bytes",strExt);
	theVisualGraphics->capabilities.fPointSprite = 
		gluCheckExtension ((GLubyte*)"GL_NV_point_sprite", strExt);
	theVisualGraphics->capabilities.fRegCombiners = 
		gluCheckExtension ((GLubyte*)"GL_NV_register_combiners", strExt);
	theVisualGraphics->capabilities.fRegCombiners2 = 
		gluCheckExtension ((GLubyte*)"GL_NV_register_combiners2", strExt);
	theVisualGraphics->capabilities.fTexEnvCombine4 = 
		gluCheckExtension ((GLubyte*)"GL_NV_texture_env_combine4", strExt);
	theVisualGraphics->capabilities.fBlendSquare = 
		gluCheckExtension ((GLubyte*)"GL_NV_blend_square", strExt) ||
		(theVisualGraphics->capabilities.glVersion >= 0x0140);
	theVisualGraphics->capabilities.fFogDist = 
		gluCheckExtension ((GLubyte*)"GL_NV_fog_distance", strExt);
	theVisualGraphics->capabilities.fMultisampleFilterHint = 
		gluCheckExtension ((GLubyte*)"GL_NV_multisample_filter_hint", strExt);
	theVisualGraphics->capabilities.fTexGenReflect = 
		gluCheckExtension ((GLubyte*)"GL_NV_texgen_reflection", strExt);
	theVisualGraphics->capabilities.fTexShader = 
		gluCheckExtension ((GLubyte*)"GL_NV_texture_shader", strExt);
	theVisualGraphics->capabilities.fTexShader2 = 
		gluCheckExtension ((GLubyte*)"GL_NV_texture_shader2", strExt);
	theVisualGraphics->capabilities.fTexShader3 = 
		gluCheckExtension ((GLubyte*)"GL_NV_texture_shader3", strExt);
	theVisualGraphics->capabilities.fDepthClamp = 
		gluCheckExtension ((GLubyte*)"GL_NV_depth_clamp", strExt);
	theVisualGraphics->capabilities.fLightMaxExp = 
		gluCheckExtension ((GLubyte*)"GL_NV_light_max_exponent", strExt);
	theVisualGraphics->capabilities.fRasterPosClip = 
		gluCheckExtension ((GLubyte*)"GL_IBM_rasterpos_clip", strExt);
	theVisualGraphics->capabilities.fConvBorderModes = 
		gluCheckExtension ((GLubyte*)"GL_HP_convolution_border_modes", strExt) ||
		gluCheckExtension ((GLubyte*)"GL_ARB_imaging", strExt);

	if (theVisualGraphics->capabilities.fTexRect) {
		// only check if extension supported
		glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, &theVisualGraphics->capabilities.maxRectTextureSize);
	} else {
		theVisualGraphics->capabilities.maxRectTextureSize = 0;
	}
#endif
}


UInt32 VisualGraphics::power2Ceiling(UInt32 n) {
    UInt32 i = 1;
    while (i < n) i <<= 1;
    return i;
}


#if TARGET_OS_WIN
DWORD VisualGraphics::getBestModeForDisplayResolution(const DisplayResolution* const aDisplayResolution) {
	DWORD iModeNum = 0;
	DWORD iModeNumFound = 0;
	DWORD bestRefreshRate = 0;
	DEVMODE devMode;
	bool found = false;
	
	ZeroMemory(&devMode, sizeof (DEVMODE));
	devMode.dmSize = sizeof(DEVMODE);

	while (EnumDisplaySettings(NULL, iModeNum, &devMode)) {
		if ((devMode.dmPelsWidth == aDisplayResolution->horizontalPixels) &&
			(devMode.dmPelsHeight == aDisplayResolution->verticalPixels) &&
			(devMode.dmBitsPerPel == aDisplayResolution->bitsPerPixel) &&
			(devMode.dmDisplayFrequency == aDisplayResolution->refreshRate)) {
			iModeNumFound = iModeNum;
			found = true;
		}
		iModeNum++;
	}

	if (found == false) {
		// find a resolution with best refresh rate
		iModeNum = 0;
		ZeroMemory(&devMode, sizeof (DEVMODE));
		devMode.dmSize = sizeof(DEVMODE);
		while (EnumDisplaySettings(NULL, iModeNum, &devMode)) {
			if ((devMode.dmPelsWidth == aDisplayResolution->horizontalPixels) &&
				(devMode.dmPelsHeight == aDisplayResolution->verticalPixels) &&
				(devMode.dmBitsPerPel == aDisplayResolution->bitsPerPixel)) {
				if (devMode.dmDisplayFrequency > bestRefreshRate) {
					bestRefreshRate = devMode.dmDisplayFrequency;
					iModeNumFound = iModeNum;
					found = true;
				}
			}
			iModeNum++;
		}
	}
	return iModeNumFound;
}
#endif


VisualNurbs* VisualGraphics::createNurbsObject(const VisualItemIdentifier& identifier, UInt8 sNumKnots, float* sKnots, UInt8 tNumKnots, float* tKnots, UInt16 sStride, UInt16 tStride, UInt16 sOrder, UInt16 tOrder) {

	VisualNurbs* aVisualNurbs = new VisualNurbs(sNumKnots, sKnots, tNumKnots, tKnots, sStride, tStride, sOrder, tOrder);

	NurbsMapIterator it;
	it = theVisualGraphics->nurbsMap.find(identifier);
	if (it == theVisualGraphics->nurbsMap.end()) {
		theVisualGraphics->nurbsMap[identifier] = aVisualNurbs;
	} else {
		if (it->second) {
			delete it->second;
		}
		it->second = aVisualNurbs;
	}
	return aVisualNurbs;
}


VisualNurbs* VisualGraphics::getNurbsObject(const VisualItemIdentifier& identifier) {
	NurbsMapIterator it = theVisualGraphics->nurbsMap.find(identifier);
	if (it != theVisualGraphics->nurbsMap.end()) {
		return it->second;
	}
	return NULL;
}


void VisualGraphics::deleteNurbsObject(const VisualItemIdentifier& identifier) {
	NurbsMapIterator it = theVisualGraphics->nurbsMap.find(identifier);
	if (it != theVisualGraphics->nurbsMap.end()) {
		delete it->second;
		theVisualGraphics->nurbsMap.erase(it);
	}
}


void VisualGraphics::copyFramebufferToTexture(UInt32 textureNumber, bool canUseRectExtension, UInt32 xPos, UInt32 yPos, UInt32 width, UInt32 height, UInt16 pixelFormat, UInt16 dataType) {

	bool debug = false;

	theVisualGraphics = VisualGraphics::getInstance();
	int drawRectOriginXPos = theVisualGraphics->getCanvasXOriginOffset() + xPos;
	int drawRectOriginYPos = theVisualGraphics->getCanvasYOriginOffset() + yPos;

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
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, drawRectOriginXPos, drawRectOriginYPos, width, height, 0);
		} else {
#if TARGET_OS_MAC
			glCopyTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, drawRectOriginXPos, drawRectOriginYPos, width, height, 0);
#endif
		}
	} else {
		UInt32* image = NULL;
#if TARGET_OS_WIN
		image = this->createBGRACheckPixels(width, height);
#endif
#if TARGET_OS_MAC
		image = this->createARGBCheckPixels(width, height);
#endif
		this->setPixelStorageParams();
		if (canUseRectExtension == false) {
#if TARGET_OS_WIN
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, pixelFormat, dataType, (GLubyte*)image);
#endif
#if TARGET_OS_MAC
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, pixelFormat, dataType, (GLuint*)image);
#endif
		} else {
#if TARGET_OS_MAC
			glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, width, height, 0, pixelFormat, dataType, (GLuint*)image);
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
