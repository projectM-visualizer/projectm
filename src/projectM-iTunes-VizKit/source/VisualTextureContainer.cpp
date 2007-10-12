/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualTextureContainer.cpp
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

#include "VisualTextureContainer.h"
#include "VisualConvolutionFilter.h"
#include "VisualString.h"
#include "VisualStringStyle.h"
#include "VisualFile.h"
#include "VisualQuickTime.h"
#include "VisualGraphics.h"
#include "VisualStageBox.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"



using namespace VizKit;


VisualTextureContainer::TextureRefCountMap VisualTextureContainer::textureRefCountMap;


VisualTextureContainer::VisualTextureContainer() {
	textureIsSet = false;
	textureName = 0;
	textureWidth = 0;
	textureHeight = 0;
	textureLogicalWidth = 0.0;
	textureLogicalHeight = 0.0;
	imageWidth = 0;
	imageHeight = 0;
	useRectExtension = false;
	pixelFormat = kGL_BGRA;
#if __BIG_ENDIAN__
	dataType = kGL_UNSIGNED_INT_8_8_8_8_REV;
#else
#if TARGET_OS_WIN
	dataType = kGL_UNSIGNED_BYTE;
#else
	dataType = kGL_UNSIGNED_INT_8_8_8_8;
#endif
#endif
	pixelBuffer = NULL;
}


VisualTextureContainer::VisualTextureContainer(UInt32 anImageWidth, UInt32 anImageHeight, bool useRectExtensionBool) {
	textureIsSet = false;
	textureName = 0;
	imageWidth = anImageWidth;
	imageHeight = anImageHeight;
	useRectExtension = useRectExtensionBool;
	if (useRectExtensionBool == false) {
		VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
		textureWidth = theVisualGraphics->power2Ceiling(imageWidth);
		textureHeight = theVisualGraphics->power2Ceiling(imageHeight);
		textureLogicalWidth = (double)imageWidth / (double)textureWidth;
		textureLogicalHeight = (double)imageHeight / (double)textureHeight;
	} else {
		textureWidth = imageWidth;
		textureHeight = imageHeight;
		textureLogicalWidth = (double)textureWidth;
		textureLogicalHeight = (double)textureHeight;
	}
	pixelFormat = kGL_BGRA;
#if __BIG_ENDIAN__
	dataType = kGL_UNSIGNED_INT_8_8_8_8_REV;
#else
#if TARGET_OS_WIN
	dataType = kGL_UNSIGNED_BYTE;
#else
	dataType = kGL_UNSIGNED_INT_8_8_8_8;
#endif
#endif
	pixelBuffer = NULL;
}


VisualTextureContainer::~VisualTextureContainer() {
	this->releaseTextureData();
	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
	}
}


VisualTextureContainer::VisualTextureContainer(const VisualTextureContainer& other) {
	copy(other);
}


VisualTextureContainer& VisualTextureContainer::operator=(const VisualTextureContainer& other) {
	if (this != &other) {
		if ((this->textureIsSet) && (this->textureName != other.textureName)) {
			this->releaseTextureData();
		}
		if (this->pixelBuffer != NULL) {
			free(this->pixelBuffer);
			this->pixelBuffer = NULL;
		}
		this->copy(other);
	}
	return *this;
}


OSStatus VisualTextureContainer::initWithDataHandle(Handle dataHandle, OSType fileType) {
	
	this->releaseTextureData();
	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}

	OSStatus status = noErr;
	char errLog[64];
	GraphicsImportComponent gi = 0;

	status = VisualQuickTime::getGraphicsImporterForHandle(dataHandle, gi, fileType);
	if (status != noErr) {
		sprintf(errLog, "err %ld: getGraphicsImporterForHandle", status);
		writeLog(errLog);
		return status;
	}
	
	status = VisualQuickTime::setMaxQuality(gi);
	if (status != noErr) {
		sprintf(errLog, "err %ld: setMaxQuality", status);
		writeLog(errLog);
		return status;
	}

	VisualQuickTime::getImageDimensions(gi, this->imageWidth, this->imageHeight);

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	this->useRectExtension = theVisualGraphics->canUseTextureRectExtension();
	if (this->useRectExtension == false) {
		this->textureWidth = theVisualGraphics->power2Ceiling(this->imageWidth);
		this->textureHeight = theVisualGraphics->power2Ceiling(this->imageHeight);
	} else {
		this->textureWidth = this->imageWidth;
		this->textureHeight = this->imageHeight;
	}

	status = VisualQuickTime::flipImportMatrix(gi, this->imageWidth, this->imageHeight);
	if (status != noErr) {
		sprintf(errLog, "err %ld: flipImportMatrix", status);
		writeLog(errLog);
		return status;
	}

#if TARGET_OS_MAC	
	CGImageRef imageRef = VisualQuickTime::createCGImageRef(gi);
	if (imageRef == NULL) {
		char errStr[256];
		sprintf(errStr, "imageRef is NULL in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return NULL;
	}
	
	CGContextRef contextPtr = theVisualGraphics->createBitmapContext(this->textureWidth, this->textureHeight);

	CGRect rect = CGRectMake(0, (this->textureHeight - this->imageHeight), this->imageWidth, this->imageHeight);
	CGContextDrawImage(contextPtr, rect, imageRef);

	this->pixelBuffer = static_cast<UInt32*>(CGBitmapContextGetData(contextPtr));
#endif

#if TARGET_OS_WIN
	this->pixelBuffer = VisualQuickTime::getBitmapOfDrawing(gi, this->textureWidth, this->textureHeight);
#endif

	CloseComponent(gi);

	this->textureName = theVisualGraphics->getNextFreeTextureName();
	this->textureIsSet = true;
	VisualTextureContainer::textureRefCountMap[this->textureName] = 1;
#if TARGET_OS_WIN
	theVisualGraphics->copyARGBBitmapDataToTexture(this->textureName, this->textureWidth, this->textureHeight, this->useRectExtension, this->pixelFormat, kGL_UNSIGNED_BYTE, const_cast<const UInt32**>(&(this->pixelBuffer)));
#endif
#if TARGET_OS_MAC
	theVisualGraphics->copyARGBBitmapDataToTexture(this->textureName, this->textureWidth, this->textureHeight, this->useRectExtension, this->pixelFormat, this->dataType, const_cast<const UInt32**>(&(this->pixelBuffer)));
#endif

#if TARGET_OS_MAC
	CGContextRelease(contextPtr);
	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}
	CGImageRelease(imageRef);
#endif

#if TARGET_OS_WIN
	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}
#endif

	if (this->useRectExtension == false) {
		this->textureLogicalWidth = (double)this->imageWidth / (double)this->textureWidth;
		this->textureLogicalHeight = (double)this->imageHeight / (double)this->textureHeight;
	} else {
		this->textureLogicalWidth = (double)this->textureWidth;
		this->textureLogicalHeight = (double)this->textureHeight;
	}

	return status;
}


OSStatus VisualTextureContainer::initWithDataPointerToPointer(const unsigned char** const aPointerToPointerToBuffer, UInt32 size, OSType aFileFormatType) {

	this->releaseTextureData();
	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}
	
	OSStatus status = noErr;
	
	Handle handle;
	PtrToHand(*aPointerToPointerToBuffer, &handle, size);
	status = this->initWithDataHandle(handle, aFileFormatType);
	
	return status;
}


OSStatus VisualTextureContainer::initWithFile(const VisualFile& aFile, OSType fileType) {
	
	this->releaseTextureData();
	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}

	OSStatus status = noErr;
	char errLog[128];
	GraphicsImportComponent gi = 0;
	
	status = VisualQuickTime::getGraphicsImporterForFile(aFile, gi, fileType);
	if (status != noErr) {
		sprintf(errLog, "err %ld: after getGraphicsImporterForFile() in file: %s (line: %d) [%s])", status, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return status;
	}
	
	status = VisualQuickTime::setMaxQuality(gi);
	if (status != noErr) {
		sprintf(errLog, "err %ld: after setMaxQuality() in file: %s (line: %d) [%s])", status, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return status;
	}

	VisualQuickTime::getImageDimensions(gi, this->imageWidth, this->imageHeight);

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	this->useRectExtension = theVisualGraphics->canUseTextureRectExtension();
	if (this->useRectExtension == false) {
		this->textureWidth = theVisualGraphics->power2Ceiling(this->imageWidth);
		this->textureHeight = theVisualGraphics->power2Ceiling(this->imageHeight);
	} else {
		this->textureWidth = this->imageWidth;
		this->textureHeight = this->imageHeight;
	}

	status = VisualQuickTime::flipImportMatrix(gi, this->imageWidth, this->imageHeight);
	if (status != noErr) {
		sprintf(errLog, "err %ld: flipImportMatrix", status);
		writeLog(errLog);
		return status;
	}

#if TARGET_OS_MAC	
	CGImageRef imageRef = VisualQuickTime::createCGImageRef(gi);
	if (imageRef == NULL) {
		char errStr[256];
		sprintf(errStr, "imageRef is NULL in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return NULL;
	}
	
	CGContextRef contextPtr = theVisualGraphics->createBitmapContext(this->textureWidth, this->textureHeight);

	CGRect rect = CGRectMake(0, (this->textureHeight - this->imageHeight), this->imageWidth, this->imageHeight);
	CGContextDrawImage(contextPtr, rect, imageRef);

	this->pixelBuffer = static_cast<UInt32*>(CGBitmapContextGetData(contextPtr));
#endif

#if TARGET_OS_WIN
	this->pixelBuffer = VisualQuickTime::getBitmapOfDrawing(gi, this->textureWidth, this->textureHeight);
#endif

	CloseComponent(gi);

	this->textureName = theVisualGraphics->getNextFreeTextureName();
	this->textureIsSet = true;
	VisualTextureContainer::textureRefCountMap[this->textureName] = 1;
#if TARGET_OS_WIN
	theVisualGraphics->copyARGBBitmapDataToTexture(this->textureName, this->textureWidth, this->textureHeight, this->useRectExtension, this->pixelFormat, kGL_UNSIGNED_BYTE, const_cast<const UInt32**>(&(this->pixelBuffer)));
#endif
#if TARGET_OS_MAC
	theVisualGraphics->copyARGBBitmapDataToTexture(this->textureName, this->textureWidth, this->textureHeight, this->useRectExtension, this->pixelFormat, this->dataType, const_cast<const UInt32**>(&(this->pixelBuffer)));
#endif

#if TARGET_OS_WIN
	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}
#endif

#if TARGET_OS_MAC
	CGContextRelease(contextPtr);
	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}
	CGImageRelease(imageRef);	
#endif

	if (this->useRectExtension == false) {
		this->textureLogicalWidth = (double)this->imageWidth / (double)this->textureWidth;
		this->textureLogicalHeight = (double)this->imageHeight / (double)this->textureHeight;
	} else {
		this->textureLogicalWidth = (double)this->textureWidth;
		this->textureLogicalHeight = (double)this->textureHeight;
	}

	return status;
}


OSStatus VisualTextureContainer::initWithString(const VisualString& stringValue, VisualStringStyle& stringStyle) {
	
	this->releaseTextureData();
	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}

    OSStatus status = noErr;

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	UInt16 maxPixelWidth = theVisualGraphics->getCanvasPixelWidth();
	UInt16 maxPixelHeight = theVisualGraphics->getCanvasPixelHeight();
	
	this->textureName = theVisualGraphics->getNextFreeTextureName();
	this->textureIsSet = true;
	VisualTextureContainer::textureRefCountMap[this->textureName] = 1;
	this->useRectExtension = theVisualGraphics->canUseTextureRectExtension();

#if TARGET_OS_MAC
	char alignmentStr[32];	
	switch (stringStyle.horizontalAlignment) {
		case (kLeftAligned):
			strcpy(alignmentStr, "left");
			break;
		case(kCenterAligned):
			strcpy(alignmentStr, "center");
			break;
		case(kRightAligned):
			strcpy(alignmentStr, "right");
			break;
		default:
			break;
	}
	
	status = getDimensionsOfCocoaStringBitmap((void*)&stringValue, &(this->imageWidth), &(this->imageHeight), const_cast<char*>(stringStyle.fontNameStr), &(stringStyle.fontSize), stringStyle.fontColor.r, stringStyle.fontColor.g, stringStyle.fontColor.b, maxPixelWidth, maxPixelHeight, alignmentStr);
	
	if (this->useRectExtension == false) {
		this->textureWidth = theVisualGraphics->power2Ceiling(this->imageWidth);
		this->textureHeight = theVisualGraphics->power2Ceiling(this->imageHeight);
	} else {
		this->textureWidth = this->imageWidth;
		this->textureHeight = this->imageHeight;
	}
	
	this->pixelBuffer = (UInt32*)calloc(this->textureWidth * this->textureHeight, sizeof(UInt32));

	status = getCocoaStringBitmapData((void*)&stringValue, this->textureWidth, this->textureHeight, const_cast<char*>(stringStyle.fontNameStr), stringStyle.fontSize, stringStyle.fontColor.r, stringStyle.fontColor.g, stringStyle.fontColor.b, alignmentStr, &(this->pixelBuffer));
	
	theVisualGraphics->copyARGBBitmapDataToTexture(this->textureName, this->textureWidth, this->textureHeight, this->useRectExtension, this->pixelFormat, this->dataType, const_cast<const UInt32**>(&(this->pixelBuffer)));

#endif

#if TARGET_OS_WIN

	wchar_t* stringValueRef = (wchar_t*)stringValue.getCharactersPointer();
	UInt8 red = (UInt8)(stringStyle.fontColor.r * 255.0f);
	UInt8 green = (UInt8)(stringStyle.fontColor.g * 255.0f);
	UInt8 blue = (UInt8)(stringStyle.fontColor.b * 255.0f);
	
	status = theVisualGraphics->makeTextureOfStringWin(stringValueRef, stringValue.getNumberOfCharacters(), this->textureName, this->textureWidth, this->textureHeight, this->imageWidth, this->imageHeight, stringStyle.fontNameStr, (UInt16)stringStyle.fontSize, red, green, blue, stringStyle.horizontalAlignment, maxPixelWidth, maxPixelHeight);

#endif

	if (this->useRectExtension == false) {
		this->textureLogicalWidth = (double)this->imageWidth / (double)this->textureWidth;
		this->textureLogicalHeight = (double)this->imageHeight / (double)this->textureHeight;
	} else {
		this->textureLogicalWidth = (double)this->textureWidth;
		this->textureLogicalHeight = (double)this->textureHeight;
	}

	return status;
}


#if TARGET_OS_MAC
OSStatus VisualTextureContainer::initWithURL(VisualString& anURL) {

	this->releaseTextureData();
	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}

	OSStatus osStatus = noErr;
	
	CFURLRef imageURL = CFURLCreateWithString(kCFAllocatorDefault, anURL.getCharactersPointer(), NULL);
	CGImageSourceRef imageSource = CGImageSourceCreateWithURL(imageURL, NULL);
	CGImageRef imageRef = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
	this->imageWidth = CGImageGetWidth(imageRef);
	this->imageHeight = CGImageGetHeight(imageRef);
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	this->useRectExtension = theVisualGraphics->canUseTextureRectExtension();
	if (this->useRectExtension == false) {
		this->textureWidth = theVisualGraphics->power2Ceiling(this->imageWidth);
		this->textureHeight = theVisualGraphics->power2Ceiling(this->imageHeight);
	} else {
		this->textureWidth = this->imageWidth;
		this->textureHeight = this->imageHeight;
	}
	CGContextRef context = theVisualGraphics->createBitmapContext(this->textureWidth, this->textureHeight);

	CGContextTranslateCTM(context, 0.0, (float)this->textureHeight + (float)(this->textureHeight - this->imageHeight));
	CGContextScaleCTM(context, 1.0, -1.0);

	CGRect rect = CGRectMake(0, (this->textureHeight - this->imageHeight), this->imageWidth, this->imageHeight);
	CGContextDrawImage(context, rect, imageRef);

	this->pixelBuffer = static_cast<UInt32*>(CGBitmapContextGetData(context));

	this->textureName = theVisualGraphics->getNextFreeTextureName();
	this->textureIsSet = true;
	VisualTextureContainer::textureRefCountMap[this->textureName] = 1;
	theVisualGraphics->copyARGBBitmapDataToTexture(this->textureName, this->textureWidth, this->textureHeight, this->useRectExtension, this->pixelFormat, this->dataType, const_cast<const UInt32**>(&(this->pixelBuffer)));

	CGContextRelease(context);
	if (this->pixelBuffer) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}
	CGImageRelease(imageRef);

	if (this->useRectExtension == false) {
		this->textureLogicalWidth = (double)this->imageWidth / (double)this->textureWidth;
		this->textureLogicalHeight = (double)this->imageHeight / (double)this->textureHeight;
	} else {
		this->textureLogicalWidth = (double)this->textureWidth;
		this->textureLogicalHeight = (double)this->textureHeight;
	}

	return osStatus;

}
#endif


OSStatus VisualTextureContainer::initWithFramebuffer(UInt32 xPos, UInt32 yPos, UInt32 width, UInt32 height) {
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();

	this->releaseTextureData();

	this->textureName = theVisualGraphics->getNextFreeTextureName();
	this->textureIsSet = true;
	VisualTextureContainer::textureRefCountMap[this->textureName] = 1;
	this->useRectExtension = theVisualGraphics->canUseTextureRectExtension();

	int prevReadBuffer = theVisualGraphics->getCurrentColorBufferForPixelReadingOperations();
	theVisualGraphics->setColorBufferForPixelReadingOperations(kGL_BACK_COLOR_BUFFER);

	int prevDrawBuffer = theVisualGraphics->getCurrentColorBufferForPixelDrawingOperations();
	theVisualGraphics->setColorBufferForPixelDrawingOperations(kGL_BACK_COLOR_BUFFER);

	theVisualGraphics->enableTexturing(this->useRectExtension);
	theVisualGraphics->copyFramebufferToTexture(this->textureName, this->useRectExtension, xPos, yPos, width, height, this->pixelFormat, this->dataType);
	theVisualGraphics->disableTexturing(this->useRectExtension);

	theVisualGraphics->setColorBufferForPixelDrawingOperations(prevDrawBuffer);
	theVisualGraphics->setColorBufferForPixelReadingOperations(prevReadBuffer);


	return noErr;
}


UInt32* VisualTextureContainer::getTexturePixels(const UInt16 format, const UInt16 type) {

	bool debug = false;

	UInt8* pixelBuffer8Bit = NULL;
		
	char errStr[256];

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	if (debug == true) {
		if (this->pixelBuffer != NULL) {
			free(this->pixelBuffer);
			this->pixelBuffer = NULL;
		}
		this->pixelBuffer = theVisualGraphics->createARGBCheckPixels(this->textureWidth, this->textureHeight);
		return this->pixelBuffer;
	}
	
	if ((debug == false) && (this->pixelBuffer != NULL)) {
		//return this->pixelBuffer;
	}
	
	UInt8 numberOfBytesPerChannel = 0;
	UInt8 numberOfChannels = 0; // channel == color resp. alpha channel
	UInt8 numberOfBytesPerPixel = 0;
	UInt32 numberOfBytesPerRow = 0;
	if ((format == kGL_RGBA) || (format == kGL_BGRA)) {
		numberOfChannels = 4;
	} else {
		sprintf(errStr, "unknown format %d in file: %s (line: %d) [%s])", format, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return this->pixelBuffer;
	}
	
	if ((type == kGL_UNSIGNED_INT_8_8_8_8_REV) || (type == kGL_UNSIGNED_INT_8_8_8_8) || (type == kGL_UNSIGNED_BYTE)) {
		numberOfBytesPerChannel = 1; // 1 byte (== 8 bits) per color/channel
	} else {
		sprintf(errStr, "unknown type %d in file: %s (line: %d) [%s])", type, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return this->pixelBuffer;
	}

	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}

	if (this->useRectExtension == false) {
		numberOfBytesPerPixel = numberOfBytesPerChannel * numberOfChannels;
		numberOfBytesPerRow = numberOfBytesPerPixel * this->textureWidth;
		if ((format == kGL_RGBA) || (format == kGL_BGRA)) {
			if ((type == kGL_UNSIGNED_INT_8_8_8_8_REV) || (type == kGL_UNSIGNED_INT_8_8_8_8)) {
				this->pixelBuffer = (UInt32*)calloc((numberOfBytesPerRow / numberOfBytesPerPixel) * this->textureHeight, numberOfBytesPerPixel);
			} else if (type == kGL_UNSIGNED_BYTE) {
				this->pixelBuffer = (UInt32*)calloc((numberOfBytesPerRow / numberOfBytesPerPixel) * this->textureHeight, numberOfBytesPerPixel);
				pixelBuffer8Bit = (UInt8*)malloc(this->textureWidth * this->textureHeight * 4);
			}
		}
		
		theVisualGraphics->enableTexturing(this->useRectExtension);
		theVisualGraphics->bindTexture(this->textureName, this->useRectExtension);
		theVisualGraphics->setPixelStorageParams();
		if ((type == kGL_UNSIGNED_INT_8_8_8_8_REV) || (type == kGL_UNSIGNED_INT_8_8_8_8)) {
			theVisualGraphics->get32BitPixelsOfCurrentTexture(this->useRectExtension, format, type, &(this->pixelBuffer));
		} else if (type == kGL_UNSIGNED_BYTE) {
			theVisualGraphics->get8BitPixelsOfCurrentTexture(this->useRectExtension, format, type, &pixelBuffer8Bit);
		}
		theVisualGraphics->disableTexturing(this->useRectExtension);

	} else {
#if TARGET_OS_MAC
		// glGetTexImage() does not always reliably return the pixelBuffer
		// of npot (non-power-of-two, GL_TEXTURE_RECTANGLE_EXT) textures
		// because of inconsistencies with Nvidia's GeForce4 MX card (1.4.18) [only with some not all textures the pixel data was returned]
		// we grab the pixels with glReadPixels()
		// (HW, 20070208)
		this->pixelBuffer = this->getRectPixels(format, type);
#endif
	}

	if (type == kGL_UNSIGNED_BYTE) {
		UInt32 b, g, r, a, color32bit;
		UInt32 pixelBufferIdx = 0;
		UInt32 pixelBuffer8BitIdx = 0;
		for (UInt32 i = 0; i < this->textureHeight; i++) {
			for (UInt32 k = 0; k < this->textureWidth; k++) {
				b = pixelBuffer8Bit[pixelBuffer8BitIdx + 0] << 24;
				g = pixelBuffer8Bit[pixelBuffer8BitIdx + 1] << 16;
				r = pixelBuffer8Bit[pixelBuffer8BitIdx + 2] << 8;
				a = pixelBuffer8Bit[pixelBuffer8BitIdx + 3];
				color32bit = b | g | r | a;
				this->pixelBuffer[pixelBufferIdx] = color32bit;
				pixelBufferIdx++;
				pixelBuffer8BitIdx += 4;
			}
		}
		free(pixelBuffer8Bit);
	}

	return this->pixelBuffer;
}


UInt32* VisualTextureContainer::getRectPixels(const UInt16 format, const UInt16 type) {

	UInt32* pixels = NULL;
	UInt32* prevPixels = NULL;

	char errStr[512];
	
	double scaleFactor = 1.0;

	UInt8 numberOfBytesPerChannel = 0;
	UInt8 numberOfChannels = 0; // channel == color resp. alpha channel
	UInt8 numberOfBytesPerPixel = 0;
	UInt32 numberOfBytesPerRow = 0;
	if ((format == kGL_RGBA) || (format == kGL_BGRA)) {
		numberOfChannels = 4;
	} else {
		sprintf(errStr, "unknown format %d in file: %s (line: %d) [%s])", format, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return pixels;
	}
	
	if ((type == kGL_UNSIGNED_INT_8_8_8_8_REV) || (type == kGL_UNSIGNED_INT_8_8_8_8) || (type == kGL_UNSIGNED_BYTE)) {
		numberOfBytesPerChannel = 1; // // 1 byte (== 8 bits) per color/channel
	} else {
		sprintf(errStr, "unknown type %d in file: %s (line: %d) [%s])", type, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return pixels;
	}
	
	numberOfBytesPerPixel = numberOfBytesPerChannel * numberOfChannels;
	numberOfBytesPerRow = numberOfBytesPerPixel * this->textureWidth;

	VisualStageBox stageBox;

	VisualStagePosition position;
	position.horizontalAlignment = kLeftAligned;
	position.verticalAlignment = kBottomAligned;
	stageBox.setVisualStagePosition(position);

	stageBox.setContentPixelWidth(this->getImageWidth());
	stageBox.setContentPixelHeight(this->getImageHeight());

	VertexChain aVertexChain;
	Vertex* topLeftFrontVertex = new Vertex;
	topLeftFrontVertex->vertexPosition.xPos = stageBox.getLeftCoord() * scaleFactor;
	topLeftFrontVertex->vertexPosition.yPos = stageBox.getTopCoord() * scaleFactor;
	topLeftFrontVertex->vertexPosition.zPos = stageBox.getFrontPosition();
	topLeftFrontVertex->vertexColor.r = 1.0f;
	topLeftFrontVertex->vertexColor.g = 1.0f;
	topLeftFrontVertex->vertexColor.b = 1.0f;
	topLeftFrontVertex->vertexColor.a = 1.0f;
	topLeftFrontVertex->texCoordPosition.sPos = 0.0f;
	topLeftFrontVertex->texCoordPosition.tPos = this->getTextureLogicalHeight();
	aVertexChain.push_back(topLeftFrontVertex);

	Vertex* bottomLeftFrontVertex = new Vertex;
	bottomLeftFrontVertex->vertexPosition.xPos = stageBox.getLeftCoord() * scaleFactor;
	bottomLeftFrontVertex->vertexPosition.yPos = stageBox.getBottomCoord() * scaleFactor;
	bottomLeftFrontVertex->vertexPosition.zPos = stageBox.getFrontPosition();
	bottomLeftFrontVertex->vertexColor.r = 1.0f;
	bottomLeftFrontVertex->vertexColor.g = 1.0f;
	bottomLeftFrontVertex->vertexColor.b = 1.0f;
	bottomLeftFrontVertex->vertexColor.a = 1.0f;
	bottomLeftFrontVertex->texCoordPosition.sPos = 0.0f;
	bottomLeftFrontVertex->texCoordPosition.tPos = 0.0f;
	aVertexChain.push_back(bottomLeftFrontVertex);

	Vertex* bottomRightFrontVertex = new Vertex;
	bottomRightFrontVertex->vertexPosition.xPos = stageBox.getRightCoord() * scaleFactor;
	bottomRightFrontVertex->vertexPosition.yPos = stageBox.getBottomCoord() * scaleFactor;
	bottomRightFrontVertex->vertexPosition.zPos = stageBox.getFrontPosition();
	bottomRightFrontVertex->vertexColor.r = 1.0f;
	bottomRightFrontVertex->vertexColor.g = 1.0f;
	bottomRightFrontVertex->vertexColor.b = 1.0f;
	bottomRightFrontVertex->vertexColor.a = 1.0f;
	bottomRightFrontVertex->texCoordPosition.sPos = this->getTextureLogicalWidth();
	bottomRightFrontVertex->texCoordPosition.tPos = 0.0f;
	aVertexChain.push_back(bottomRightFrontVertex);

	Vertex* topRightFrontVertex = new Vertex;
	topRightFrontVertex->vertexPosition.xPos = stageBox.getRightCoord() * scaleFactor;
	topRightFrontVertex->vertexPosition.yPos = stageBox.getTopCoord() * scaleFactor;
	topRightFrontVertex->vertexPosition.zPos = stageBox.getFrontPosition();
	topRightFrontVertex->vertexColor.r = 1.0f;
	topRightFrontVertex->vertexColor.g = 1.0f;
	topRightFrontVertex->vertexColor.b = 1.0f;
	topRightFrontVertex->vertexColor.a = 1.0f;
	topRightFrontVertex->texCoordPosition.sPos = this->getTextureLogicalWidth();
	topRightFrontVertex->texCoordPosition.tPos = this->getTextureLogicalHeight();
	aVertexChain.push_back(topRightFrontVertex);

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	// read previous pixels
	if ((format == kGL_RGBA) || (format == kGL_BGRA)) {
		prevPixels = (UInt32*)calloc((numberOfBytesPerRow / numberOfBytesPerPixel) * this->textureHeight, numberOfBytesPerPixel);
	}
	theVisualGraphics->readPixels(stageBox.getLeftCoord(), stageBox.getBottomCoord(), this->textureWidth, this->textureHeight, &prevPixels, format, type);

	// draw current texture
	theVisualGraphics->drawTexture(this->textureName, &aVertexChain, this->getUseRectExtension(), kReplace);

	// read pixels of current texture
	if ((format == kGL_RGBA) || (format == kGL_BGRA)) {
		pixels = (UInt32*)calloc((numberOfBytesPerRow / numberOfBytesPerPixel) * this->textureHeight, numberOfBytesPerPixel);
	}
	theVisualGraphics->readPixels(stageBox.getLeftCoord(), stageBox.getBottomCoord(), this->textureWidth, this->textureHeight, &pixels, format, type);

	// restore previous pixels
	theVisualGraphics->drawPixels(&prevPixels, stageBox.getLeftCoord(), stageBox.getBottomCoord(), this->textureWidth, this->textureHeight, format, type);

	for (VertexChainIterator chainIt = aVertexChain.begin(); chainIt != aVertexChain.end(); chainIt++) {
		delete *chainIt;
		*chainIt = NULL;
	}
	aVertexChain.clear();

	free(prevPixels);
	
	return pixels;
}


void VisualTextureContainer::copy(const VisualTextureContainer& other) {
	if (other.textureIsSet) {
		VisualTextureContainer::textureRefCountMap[other.textureName]++;
	}
	this->textureIsSet = other.textureIsSet;
	this->textureName = other.textureName;
	this->textureWidth = other.textureWidth;
	this->textureHeight = other.textureHeight;
	this->textureLogicalWidth = other.textureLogicalWidth;
	this->textureLogicalHeight = other.textureLogicalHeight;
	this->imageWidth = other.imageWidth;
	this->imageHeight = other.imageHeight;
	this->useRectExtension = other.useRectExtension;
	this->pixelFormat = other.pixelFormat;
	this->dataType = other.dataType;
	if (other.pixelBuffer != NULL) {
		this->pixelBuffer = (UInt32*)malloc(this->textureWidth * this->textureHeight * sizeof(UInt32));
		memcpy(this->pixelBuffer, other.pixelBuffer, this->textureWidth * this->textureHeight * sizeof(UInt32));
	} else {
		this->pixelBuffer = NULL;
	}
}


void VisualTextureContainer::setTextureWidth(UInt32 aTextureWidth) {
	this->textureWidth = aTextureWidth;
}


void VisualTextureContainer::setTextureHeight(UInt32 aTextureHeight) {
	this->textureHeight = aTextureHeight;
}


void VisualTextureContainer::setTextureLogicalWidth(double aTextureLogicalWidth) {
	this->textureLogicalWidth = aTextureLogicalWidth;
}


void VisualTextureContainer::setTextureLogicalHeight(double aTextureLogicalHeight) {
	this->textureLogicalHeight = aTextureLogicalHeight;
}


void VisualTextureContainer::setImageWidth(UInt32 anImageWidth) {
	this->imageWidth = anImageWidth;
}


void VisualTextureContainer::setImageHeight(UInt32 anImageHeight) {
	this->imageHeight = anImageHeight;
}


UInt32 VisualTextureContainer::getTextureName() const {
	return this->textureName;
}


UInt32 VisualTextureContainer::getTextureWidth() const {
	return this->textureWidth;
}


UInt32 VisualTextureContainer::getTextureHeight() const {
	return this->textureHeight;
}


double VisualTextureContainer::getTextureLogicalWidth() const {
	return this->textureLogicalWidth;
}


double VisualTextureContainer::getTextureLogicalHeight() const {
	return this->textureLogicalHeight;
}


UInt32 VisualTextureContainer::getImageWidth() const {
	return this->imageWidth;
}


UInt32 VisualTextureContainer::getImageHeight() const {
	return this->imageHeight;
}


bool VisualTextureContainer::getUseRectExtension() const {
	return this->useRectExtension;
}


void VisualTextureContainer::releaseTextureData() {
	if (this->textureIsSet) {
		VisualTextureContainer::textureRefCountMap[this->textureName]--;
		if (VisualTextureContainer::textureRefCountMap[this->textureName] == 0) {
			VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
			theVisualGraphics->deleteTextures(1, &(this->textureName));
			this->textureName = 0;
			this->textureIsSet = false;
		}
	}
}


void VisualTextureContainer::applyConvolutionFilter(const VisualConvolutionFilter& aConvolutionFilter) {
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();

	int prevReadBuffer = theVisualGraphics->getCurrentColorBufferForPixelReadingOperations();
	theVisualGraphics->setColorBufferForPixelReadingOperations(kGL_BACK_COLOR_BUFFER);

	int prevDrawBuffer = theVisualGraphics->getCurrentColorBufferForPixelDrawingOperations();
	theVisualGraphics->setColorBufferForPixelDrawingOperations(kGL_BACK_COLOR_BUFFER);
	
	UInt32* prevPixels = NULL;

	char errStr[512];

	UInt8 numberOfBytesPerChannel = 0;
	UInt8 numberOfChannels = 0; // channel == color resp. alpha channel
	UInt8 numberOfBytesPerPixel = 0;
	UInt32 numberOfBytesPerRow = 0;
	if ((this->pixelFormat == kGL_RGBA) || (this->pixelFormat == kGL_BGRA)) {
		numberOfChannels = 4;
	} else {
		sprintf(errStr, "unknown format %d in file: %s (line: %d) [%s])", this->pixelFormat, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return;
	}
	
	if ((this->dataType == kGL_UNSIGNED_INT_8_8_8_8_REV) || (this->dataType == kGL_UNSIGNED_INT_8_8_8_8) || (this->dataType == kGL_UNSIGNED_BYTE)) {
		numberOfBytesPerChannel = 1; // // 1 byte (== 8 bits) per color/channel
	} else {
		sprintf(errStr, "unknown type %d in file: %s (line: %d) [%s])", this->dataType, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return;
	}
	
	numberOfBytesPerPixel = numberOfBytesPerChannel * numberOfChannels;
	numberOfBytesPerRow = numberOfBytesPerPixel * this->textureWidth;

	// read previous pixels
	if ((this->pixelFormat == kGL_RGBA) || (this->pixelFormat == kGL_BGRA)) {
		prevPixels = (UInt32*)calloc((numberOfBytesPerRow / numberOfBytesPerPixel) * this->textureHeight, numberOfBytesPerPixel);
	}
	theVisualGraphics->readPixels(theVisualGraphics->getMaxLeftCoordOfCanvas(), theVisualGraphics->getMaxBottomCoordOfCanvas(), this->textureWidth, this->textureHeight, &prevPixels, this->pixelFormat, this->dataType);

	UInt32* pixels = this->getTexturePixels(this->pixelFormat, this->dataType);
	UInt32* passedPixels = (UInt32*)malloc(this->textureWidth * this->textureHeight * 4);
	memcpy(passedPixels, pixels, this->textureWidth * this->textureHeight * 4); // VisualStudio does not like when we pass a non-const pointer to instance variable (crashes in runtime)
	theVisualGraphics->drawPixels((UInt32**)&(passedPixels), theVisualGraphics->getMaxLeftCoordOfCanvas(), theVisualGraphics->getMaxBottomCoordOfCanvas(), this->textureWidth, this->textureHeight, this->pixelFormat, this->dataType, &aConvolutionFilter);
	if (passedPixels != NULL) {
		free(passedPixels);
		passedPixels = NULL;
	}

	theVisualGraphics->copyFramebufferToTexture(this->textureName, this->useRectExtension, 0, 0, this->textureWidth, this->textureHeight, this->pixelFormat, this->dataType);

	// restore previous pixels
	theVisualGraphics->drawPixels(&prevPixels, theVisualGraphics->getMaxLeftCoordOfCanvas(), theVisualGraphics->getMaxBottomCoordOfCanvas(), this->textureWidth, this->textureHeight, this->pixelFormat, this->dataType);
	
	free(prevPixels);

	theVisualGraphics->setColorBufferForPixelDrawingOperations(prevDrawBuffer);
	theVisualGraphics->setColorBufferForPixelReadingOperations(prevReadBuffer);

	if (this->pixelBuffer != NULL) {
		free(this->pixelBuffer);
		this->pixelBuffer = NULL;
	}
	
}
