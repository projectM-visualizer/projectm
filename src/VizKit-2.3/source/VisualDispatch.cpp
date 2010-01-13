/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualDispatch.cpp
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

#include "VisualDispatch.h"

#include "VisualErrorHandling.h"
#include "VisualConfiguration.h"
#include "VisualDataStore.h"
#include "VisualPlayerState.h"
#include "VisualGraphics.h"
#include "VisualTiming.h"
#include "VisualStyledString.h"
#include "VisualString.h"

#include <iostream> // cout



using namespace VizKit;


#if TARGET_OS_MAC
//	Function pointer prototypes to the Mach-O Cocoa wrappers

typedef bool (*GetDimensionsOfStringBitmapProc)(CFStringRef stringValue, uint32* bitmapWidth, uint32* bitmapHeight, const char* const fontNameStr, double* fontSize, double red, double green, double blue, uint16 maxPixelWidth, uint16 maxPixelHeight, const char* const alignment);
GetDimensionsOfStringBitmapProc doGetDimensionsOfStringBitmap;

typedef bool (*GetStringBitmapDataProc)(CFStringRef stringValue, uint32 bitmapWidth, uint32 bitmapHeight, const char* const fontNameStr, double fontSize, double red, double green, double blue, const char* const alignment, uint32** bitmapOut);
GetStringBitmapDataProc doGetStringBitmapData;

typedef void (*LogStringProc)(CFStringRef aString);
LogStringProc doLogString;


CFBundleRef bundleRef = NULL;
#endif


// private declaration of internally used function
bool initCocoaBundle();


void setProcessInfo(const char* const labelStr, const char* const valueStr) {
    VisualDataStore::setProcessInfo(labelStr, valueStr);
}


void monitorRenderMessageProcess() {
	
	static float framesPerSecond = 0.0f;
	static uint16 frameCount = 0;
	uint32 elapsedMilliseconds = 0;
	uint32 remainingMilliseconds = 0;
	VisualPlayerState* theVisualPlayerState = NULL;
	VisualGraphics* theVisualGraphics = NULL;
	uint8 playState;
	char cStr64[64];
	
	theVisualPlayerState = VisualPlayerState::getInstance();
	
    elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset("fps");
    
    if (elapsedMilliseconds > 1000) {
        framesPerSecond = (float)frameCount / (float)elapsedMilliseconds * 1000.0f;
        frameCount = 0;
        VisualTiming::resetTimestamp("fps");
    }
	frameCount++;
    
    sprintf(cStr64, "%.2f",  framesPerSecond);
    setProcessInfo("FramesPerSecond", cStr64);
	
	theVisualGraphics = VisualGraphics::getInstance();
	
	setProcessInfo("RendererName",theVisualGraphics->getRendererName());
	
	setProcessInfo("RendererVendor",theVisualGraphics->getRendererVendor());
	
	setProcessInfo("RendererVersion",theVisualGraphics->getRendererVersion());
	
	sprintf(cStr64, "%#x", theVisualGraphics->getGLVersion());
	setProcessInfo("GLVersion", cStr64);
	
	sprintf(cStr64, "%ld", theVisualGraphics->getMaxTextureSize());
	setProcessInfo("Maximum Texture Size", cStr64);
	
	// screen measurements
	PixelRect screenRect = theVisualGraphics->getScreenRect();
	sprintf(cStr64, "width: %d, height: %d, refreshRate: %d", screenRect.width, screenRect.height, theVisualGraphics->getRefreshRateOfScreen());
	setProcessInfo("Screen", cStr64);
	
	sprintf(cStr64, "%d", theVisualGraphics->getBitsPerPixelOfScreen());
	setProcessInfo("BitsPerPixel", cStr64);
	
	sprintf(cStr64, "%d", theVisualGraphics->getCanvasPixelHeight());
	setProcessInfo("CanvasPixelHeight", cStr64);
	
	sprintf(cStr64, "%d", theVisualGraphics->getCanvasPixelWidth());
	setProcessInfo("CanvasPixelWidth", cStr64);
	
	BottomLeftPositionedPixelRect theCanvasRect;
	theCanvasRect = theVisualGraphics->getCanvasRect();
	sprintf(cStr64, "bottom: %d, left: %d", theCanvasRect.bottomLeftPixel.y, theCanvasRect.bottomLeftPixel.x);
	setProcessInfo("CanvasRectTopLeft", cStr64);
	sprintf(cStr64, "width: %d, height: %d", theCanvasRect.pixelRect.width, theCanvasRect.pixelRect.height);
	setProcessInfo("CanvasRectWidthHeight", cStr64);
	
	/*
	 sprintf(cStr64, "top: %.2f, left: %.2f", theVisualGraphics->getMaxTopCoordOfCanvas(), theVisualGraphics->getMaxLeftCoordOfCanvas());
	 setProcessInfo("CoordMaxTopLeft", cStr64);
	 sprintf(cStr64, "bottom: %.2f, right: %.2f", theVisualGraphics->getMaxBottomCoordOfCanvas(), theVisualGraphics->getMaxRightCoordOfCanvas());
	 setProcessInfo("CoordMaxBottomRight", cStr64);
	 sprintf(cStr64, "near: %.2f, far: %.2f", theVisualGraphics->getMaxNearCoordOfCanvas(), theVisualGraphics->getMaxFarCoordOfCanvas());
	 setProcessInfo("CoordMaxNearFar", cStr64);
	 */
	
	BottomLeftPositionedPixelRect bottomLeftPositionedPixelRect = theVisualGraphics->getViewportRect();
	sprintf(cStr64, "bottom: %d, left: %d, width: %d, height: %d",  bottomLeftPositionedPixelRect.bottomLeftPixel.y, bottomLeftPositionedPixelRect.bottomLeftPixel.x, bottomLeftPositionedPixelRect.pixelRect.width, bottomLeftPositionedPixelRect.pixelRect.height);
	setProcessInfo("ViewportRect", cStr64);
	
	playState = theVisualPlayerState->getAudioPlayState();
	switch (playState) {
		case kAudioIsPlaying:
			sprintf(cStr64, "kAudioIsPlaying");
			break;
		case kAudioPlayStarted:
			sprintf(cStr64, "kAudioPlayStarted");
			break;		
		case kAudioPlayResumed:
			sprintf(cStr64, "kAudioPlayResumed");
			break;
		case kAudioPlayPaused:
			sprintf(cStr64, "kAudioPlayPaused");
			break;
		case kAudioIsNotPlaying:
			sprintf(cStr64, "kAudioIsNotPlaying");
			break;
		default:
			sprintf(cStr64, "unknown");
	}
	setProcessInfo("AudioPlayState", cStr64);
	
	elapsedMilliseconds = theVisualPlayerState->getElapsedAudioTime();
	sprintf(cStr64, "%d", elapsedMilliseconds);
	setProcessInfo("TrackTimeElapsed", cStr64);
	
	remainingMilliseconds = theVisualPlayerState->getRemainingAudioTime();
	sprintf(cStr64, "%d", remainingMilliseconds);
	setProcessInfo("TrackTimeRemaining", cStr64);
	
}


uint8 graphicsDoSupportTextureRectExtension() {
	VisualGraphics* theVisualGraphics = NULL;
	theVisualGraphics = VisualGraphics::getInstance();
	return (theVisualGraphics->canUseTextureRectExtension());
}


#if TARGET_OS_MAC

void setRefsToCocoaFunctionCalls() {
	CFStringRef pluginName;
	pluginName = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingASCII);
	bundleRef = CFBundleGetBundleWithIdentifier(pluginName);
    if (!bundleRef) {
        writeLog("no bundleRef in setRefsToCocoaFunctionCalls");
        return;
    } else {
		doGetDimensionsOfStringBitmap = (GetDimensionsOfStringBitmapProc)CFBundleGetFunctionPointerForName(bundleRef, CFSTR("getDimensionsOfStringBitmap"));
		if (!doGetDimensionsOfStringBitmap) {
			writeLog("no function pointer for doGetDimensionsOfStringBitmap");
		}
		doGetStringBitmapData = (GetStringBitmapDataProc)CFBundleGetFunctionPointerForName(bundleRef, CFSTR("getStringBitmapData"));
		if (!doGetStringBitmapData) {
			writeLog("no function pointer for doGetStringBitmapData");
		}
        doLogString = (LogStringProc)CFBundleGetFunctionPointerForName(bundleRef, CFSTR("logCFString"));
		if (!doLogString) {
			writeLog("no function pointer for doLogString");
		}
    }
	CFRelease(pluginName);
}


bool initCocoaBundle() {
    bool success = true;
    setRefsToCocoaFunctionCalls();
	return success;
}


bool getDimensionsOfCocoaStringBitmap(void* aVisualString, void* aVisualStringStyle, uint32* imageWidth, uint32* imageHeight, uint16 maxPixelWidth, uint16 maxPixelHeight, char* alignment) {
	
	bool success = true;
	
	if (!doGetDimensionsOfStringBitmap) {
		initCocoaBundle();
	}
	
	VisualString* aString = reinterpret_cast<VisualString*>(aVisualString);
	VisualStringStyle* aStringStyle = reinterpret_cast<VisualStringStyle*>(aVisualStringStyle);
	const CFStringRef aCFStringRef = aString->getCharactersPointer();
	success = doGetDimensionsOfStringBitmap(aCFStringRef, imageWidth, imageHeight, aStringStyle->fontNameStr, &(aStringStyle->fontSize), aStringStyle->fontColor.red, aStringStyle->fontColor.green, aStringStyle->fontColor.blue, maxPixelWidth, maxPixelHeight, alignment);
	
    if (!success) {
        writeLog("ERR: getDimensionsOfCocoaStringBitmap");
        return false;
    }
	
	return success;
}


bool getCocoaStringBitmapData(void* aVisualStyledString, uint32 bitmapWidth, uint32 bitmapHeight, char* alignment, uint32** bitmapData) {
	
	bool success = true;
	
	if (!doGetStringBitmapData) {
		initCocoaBundle();
	}
    
	VisualStyledString* aStyledString = reinterpret_cast<VisualStyledString*>(aVisualStyledString);
	
	CFStringRef aCFStringRef = aStyledString->getCharactersPointer();
	VisualStringStyle aStringStyle = aStyledString->getStyle();
    success = doGetStringBitmapData(aCFStringRef, bitmapWidth, bitmapHeight, aStringStyle.fontNameStr, aStringStyle.fontSize, aStringStyle.fontColor.red, aStringStyle.fontColor.green, aStringStyle.fontColor.blue, alignment, bitmapData);
	
    if (!success) {
        writeLog("ERR: getCocoaStringBitmapData");
    }
	
	return success;
}


bool copyARGBBitmapDataToTexture(uint32 textureNumber, uint32 imageWidth, uint32 imageHeight, bool canUseRectExtension, uint32** bitmapData) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->copyARGBBitmapDataToTexture(textureNumber, imageWidth, imageHeight, canUseRectExtension, const_cast<const uint32**>(bitmapData));
}


void logString(const void* aVisualString) {
	if (!doLogString) {
		initCocoaBundle();
	}
	const VisualString* aString = reinterpret_cast<const VisualString*>(aVisualString);
	CFStringRef aCFStringRef = aString->getCharactersPointer();
	doLogString(aCFStringRef);
}


const char* getVisualPluginDomainIdentifier() {
	return VisualConfiguration::kVisualPluginDomainIdentifier;
}

#endif
