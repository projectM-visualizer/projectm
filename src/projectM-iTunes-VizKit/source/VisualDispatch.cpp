/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualDispatch.cpp
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

#include "VisualDispatch.h"

#include "VisualErrorHandling.h"
#include "VisualConfiguration.h"
#include "VisualDataStore.h"
#include "VisualPlayerState.h"
#include "VisualAudioLab.h"
#include "VisualGraphics.h"
#include "VisualTiming.h"
#include "VisualString.h"

#include <iostream> // cout



using namespace VizKit;


#if TARGET_OS_MAC
//	Function pointer prototypes to the Mach-O Cocoa wrappers

typedef OSStatus (*InitCocoaForCarbonBundleProc)();
InitCocoaForCarbonBundleProc doInitCocoaForCarbonBundle;

typedef OSStatus (*GetDimensionsOfStringBitmapProc)(CFStringRef stringValue, UInt32* bitmapWidth, UInt32* bitmapHeight, const char* const fontNameStr, float* fontSize, float red, float green, float blue, UInt16 maxPixelWidth, UInt16 maxPixelHeight, const char* const alignment);
GetDimensionsOfStringBitmapProc doGetDimensionsOfStringBitmap;

typedef OSStatus (*GetStringBitmapDataProc)(CFStringRef stringValue, UInt32 bitmapWidth, UInt32 bitmapHeight, const char* const fontNameStr, float fontSize, float red, float green, float blue, const char* const alignment, UInt32** bitmapOut);
GetStringBitmapDataProc doGetStringBitmapData;

typedef void (*LogStringProc)(CFStringRef aString);
LogStringProc doLogString;


CFBundleRef bundleRef = NULL;
#endif


void setProcessInfo(const char* const labelStr, const char* const valueStr) {
    VisualDataStore::setProcessInfo(labelStr, valueStr);
}


void monitorRenderMessageProcess() {

	static float framesPerSecond = 0.0f;
	static UInt16 frameCount = 0;
	::Rect viewportRect;
	UInt32 elapsedMilliseconds, remainingMilliseconds, totalMilliseconds = 0;
	VisualAudioLab* theVisualAudioLab = NULL;
	VisualPlayerState* theVisualPlayerState = NULL;
	VisualGraphics* theVisualGraphics = NULL;
	UInt8 playState;
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
	sprintf(cStr64, "width: %d, height: %d, refreshRate: %d", theVisualGraphics->getScreenWidth(), theVisualGraphics->getScreenHeight(), theVisualGraphics->getRefreshRateOfScreen());
	setProcessInfo("Screen", cStr64);
	
	sprintf(cStr64, "%d", theVisualGraphics->getBitsPerPixelOfScreen());
	setProcessInfo("BitsPerPixel", cStr64);
	
	sprintf(cStr64, "%d", theVisualGraphics->getCanvasPixelHeight());
	setProcessInfo("CanvasPixelHeight", cStr64);
	
	sprintf(cStr64, "%d", theVisualGraphics->getCanvasPixelWidth());
	setProcessInfo("CanvasPixelWidth", cStr64);

	::Rect theCanvasRect;
	theVisualGraphics->getCanvasRect(&theCanvasRect);
	sprintf(cStr64, "top: %d, left: %d", theCanvasRect.top, theCanvasRect.left);
	setProcessInfo("CanvasRectTopLeft", cStr64);
	sprintf(cStr64, "bottom: %d, right: %d", theCanvasRect.bottom, theCanvasRect.right);
	setProcessInfo("CanvasRectBottomRight", cStr64);

	::Rect theCanvasSurroundingRect;
	theVisualGraphics->getCanvasSurroundingRect(&theCanvasSurroundingRect);
	sprintf(cStr64, "top: %d, left: %d", theCanvasSurroundingRect.top, theCanvasSurroundingRect.left);
	setProcessInfo("CanvasSurroundRectTopLeft", cStr64);
	sprintf(cStr64, "bottom: %d, right: %d", theCanvasSurroundingRect.bottom, theCanvasSurroundingRect.right);
	setProcessInfo("CanvasSurroundRectBottomRight", cStr64);
	
	sprintf(cStr64, "top: %.2f, left: %.2f", theVisualGraphics->getMaxTopCoordOfCanvas(), theVisualGraphics->getMaxLeftCoordOfCanvas());
	setProcessInfo("CoordMaxTopLeft", cStr64);
	sprintf(cStr64, "bottom: %.2f, right: %.2f", theVisualGraphics->getMaxBottomCoordOfCanvas(), theVisualGraphics->getMaxRightCoordOfCanvas());
	setProcessInfo("CoordMaxBottomRight", cStr64);
	sprintf(cStr64, "near: %.2f, far: %.2f", theVisualGraphics->getMaxNearCoordOfCanvas(), theVisualGraphics->getMaxFarCoordOfCanvas());
	setProcessInfo("CoordMaxNearFar", cStr64);

	theVisualGraphics->getViewportRect(&viewportRect);
	sprintf(cStr64, "top: %d, left: %d, bottom: %d, right: %d",  viewportRect.top, viewportRect.left, viewportRect.bottom, viewportRect.right);
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
	sprintf(cStr64, "%ld", elapsedMilliseconds);
	setProcessInfo("TrackTimeElapsed", cStr64);

	remainingMilliseconds = theVisualPlayerState->getRemainingAudioTime();
	sprintf(cStr64, "%ld", remainingMilliseconds);
	setProcessInfo("TrackTimeRemaining", cStr64);

	theVisualAudioLab = VisualAudioLab::getInstance();
	totalMilliseconds = theVisualAudioLab->getTotalTimeOfCurrentTrack();
	sprintf(cStr64, "%ld", totalMilliseconds);
	setProcessInfo("TrackTimeTotal", cStr64);

}


UInt8 graphicsDoSupportTextureRectExtension() {
	VisualGraphics* theVisualGraphics = NULL;
	theVisualGraphics = VisualGraphics::getInstance();
	return (theVisualGraphics->canUseTextureRectExtension());
}


#if TARGET_OS_MAC

void setRefsToCocoaFunctionCalls() {
	CFStringRef pluginName;
	pluginName = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingWindowsLatin1);
	bundleRef = CFBundleGetBundleWithIdentifier(pluginName);
    if (!bundleRef) {
        writeLog("no bundleRef in setRefsToCocoaFunctionCalls");
        return;
    } else {
        doInitCocoaForCarbonBundle = (InitCocoaForCarbonBundleProc)CFBundleGetFunctionPointerForName(bundleRef, CFSTR("initCocoaForCarbonBundle"));
		if (!doInitCocoaForCarbonBundle) {
			writeLog("no function pointer for doInitCocoaForCarbonBundle");
		}
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


OSStatus initCocoaTextInfo() {
    OSStatus err;
    err = doInitCocoaForCarbonBundle();
    if (err != noErr) {
        writeLog("Err in initCocoaTextInfo");
        return 1000;
    } else {
        return noErr;
    }
}


OSStatus initCocoaBundle() {
    OSStatus status;
    setRefsToCocoaFunctionCalls();
    status = initCocoaTextInfo();
	return status;
}


OSStatus getDimensionsOfCocoaStringBitmap(void* aVisualString, UInt32* imageWidth, UInt32* imageHeight, char* fontNameStr, float* fontSize, float red, float green, float blue, UInt16 maxPixelWidth, UInt16 maxPixelHeight, char* alignment) {

    OSStatus err = noErr;
	
	if (!doGetDimensionsOfStringBitmap) {
		initCocoaBundle();
	}

	VisualString* aString = reinterpret_cast<VisualString*>(aVisualString);
	const CFStringRef aCFStringRef = aString->getCharactersPointer();
    err = doGetDimensionsOfStringBitmap(aCFStringRef, imageWidth, imageHeight, fontNameStr, fontSize, red, green, blue, maxPixelWidth, maxPixelHeight, alignment);
	
    if (err != noErr) {
		char logStr[64];
		sprintf(logStr, "ERR: getDimensionsOfCocoaStringBitmap: %ld", err);
        writeLog(logStr);
        return 1;
    } else {
        return 0;
    }
	
	return err;
}


OSStatus getCocoaStringBitmapData(void* aVisualString, UInt32 bitmapWidth, UInt32 bitmapHeight, char* fontNameStr, float fontSize, float red, float green, float blue, char* alignment, UInt32** bitmapData) {

    OSStatus err = noErr;
	
	if (!doGetStringBitmapData) {
		initCocoaBundle();
	}
    
	VisualString* aString = reinterpret_cast<VisualString*>(aVisualString);
	CFStringRef aCFStringRef = aString->getCharactersPointer();
    err = doGetStringBitmapData(aCFStringRef, bitmapWidth, bitmapHeight, fontNameStr, fontSize, red, green, blue, alignment, bitmapData);
	
    if (err != noErr) {
		char logStr[64];
		sprintf(logStr, "ERR: getCocoaStringBitmapData: %ld", err);
        writeLog(logStr);
        return 1001;
    }
	
	return err;
}


OSStatus copyBitmapDataToTexture(UInt32 textureNumber, UInt32 width, UInt32 height, bool canUseRectExtension, UInt16 format, UInt16 dataType, UInt32** bitmapData) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->copyARGBBitmapDataToTexture(textureNumber, width, height, canUseRectExtension, format, dataType, const_cast<const UInt32**>(bitmapData));
}


UInt32* createCheckPixels(UInt32 width, UInt32 height) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->createARGBCheckPixels(width, height);
}


void logString(void* aVisualString) {
	if (!doLogString) {
		initCocoaBundle();
	}
	VisualString* aString = reinterpret_cast<VisualString*>(aVisualString);
	CFStringRef aCFStringRef = aString->getCharactersPointer();
	doLogString(aCFStringRef);
}


const char* getVisualPluginName() {
	return VisualConfiguration::kVisualPluginName;
}


const char* getVisualPluginShowName() {
	return VisualConfiguration::kVisualPluginShowName;
}


const char* getVisualPluginDomainIdentifier() {
	return VisualConfiguration::kVisualPluginDomainIdentifier;
}


#endif
