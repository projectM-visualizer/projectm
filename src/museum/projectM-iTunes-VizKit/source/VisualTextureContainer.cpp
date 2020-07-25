/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualTextureContainer.cpp
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

#include "VisualTextureContainer.h"
#include "VisualGraphicTypes.h"
#include "VisualVertex.h"
#include "VisualConvolutionFilter.h"
#include "VisualStyledString.h"
#include "VisualFile.h"
#include "VisualGraphics.h"
#include "VisualColorTools.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"
#include "VisualCamera.h"
#if TARGET_OS_MAC
#include <ApplicationServices/../Frameworks/ImageIO.framework/Headers/ImageIO.h> // CGImage...
#endif

#if TARGET_OS_WIN
#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#endif

using namespace VizKit;


VisualTextureContainer::TextureRefCountMap VisualTextureContainer::textureRefCountMap;


VisualTextureContainer::VisualTextureContainer() {
	aTextureIsSet = false;
	textureName = 0;
	textureRect.width = 0;
	textureRect.height = 0;
	logicalSize.width = 0.0;
	logicalSize.height = 0.0;
	imageRect.width = 0;
	imageRect.height = 0;
	useRectExtension = false;
}


VisualTextureContainer::~VisualTextureContainer() {
	this->releaseTextureData();
}


VisualTextureContainer::VisualTextureContainer(const VisualTextureContainer& other) {
	copy(other);
}


VisualTextureContainer& VisualTextureContainer::operator=(const VisualTextureContainer& other) {
	
	if (this == &other) return *this;
	
	if ((this->aTextureIsSet) && (this->textureName != other.textureName)) {
		this->releaseTextureData();
	}
	
	this->copy(other);
	
	return *this;
}


bool VisualTextureContainer::initWithFile(VisualFile& aFile) {
	
	bool success = true;
	
	this->releaseTextureData();
	
	void* encodedImageData = NULL;
	size_t encodedImageDataSize = 0;
	success = aFile.getData(&encodedImageData, encodedImageDataSize);
	if (!success) {
		return success;
	}
	
	success = this->initWithEncodedData((const char* const)encodedImageData, encodedImageDataSize);
	
	return success;
}


bool VisualTextureContainer::initWithEncodedData(const char* const bufferData, size_t size) {
	
	bool success = true;
	bool debug = false;
	
	this->releaseTextureData();
	
	uint32* aPixelBuffer = NULL;
	
#if TARGET_OS_WIN
	
	HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, (SIZE_T)size);
	if (!hGlobal)
		return false;
	
	BYTE* pDest = (BYTE*)::GlobalLock(hGlobal);
	
	memcpy(pDest, bufferData, size);
	
	::GlobalUnlock(hGlobal);
	
	IStream* pStream = NULL;
	if (::CreateStreamOnHGlobal(hGlobal, FALSE, &pStream) != S_OK)
		return false;
	
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromStream(pStream);
	bitmap->RotateFlip(Gdiplus::RotateNoneFlipY);
	
	this->imageRect.width = bitmap->GetWidth();
	this->imageRect.height = bitmap->GetHeight();
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	this->useRectExtension = theVisualGraphics->canUseTextureRectExtension();
	if (this->useRectExtension == false) {
		this->textureRect.width = theVisualGraphics->power2Ceiling(this->imageRect.width);
		this->textureRect.height = theVisualGraphics->power2Ceiling(this->imageRect.height);
	} else {
		this->textureRect.width = this->imageRect.width;
		this->textureRect.height = this->imageRect.height;
	}
	
	aPixelBuffer = (uint32*)malloc(this->imageRect.width * this->imageRect.height * sizeof(uint32));
	Gdiplus::Rect rect(0, 0, this->imageRect.width, this->imageRect.height);
	Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;
	
	bitmapData->Width = this->imageRect.width;
	bitmapData->Height = this->imageRect.height;
	bitmapData->Stride = sizeof(uint32) * bitmapData->Width;
	bitmapData->PixelFormat = PixelFormat32bppARGB;
	bitmapData->Scan0 = (VOID*)aPixelBuffer;
	
	Gdiplus::Status status = Gdiplus::Ok;
	status = bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeUserInputBuf, PixelFormat32bppPARGB, bitmapData);
	
#endif
	
#if TARGET_OS_MAC
	
	CFDataRef dataRef = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (UInt8*)bufferData, (CFIndex)size, kCFAllocatorDefault);
	
	CFDictionaryRef options = NULL;
	CGImageSourceRef imageSourceRef = CGImageSourceCreateWithData(dataRef, options);
	
	CGImageRef imageRef = CGImageSourceCreateImageAtIndex(imageSourceRef, 0, options);
	
	this->imageRect.width = CGImageGetWidth(imageRef);
	this->imageRect.height = CGImageGetHeight(imageRef);
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	this->useRectExtension = theVisualGraphics->canUseTextureRectExtension();
	if (this->useRectExtension == false) {
		this->textureRect.width = theVisualGraphics->power2Ceiling(this->imageRect.width);
		this->textureRect.height = theVisualGraphics->power2Ceiling(this->imageRect.height);
	} else {
		this->textureRect.width = this->imageRect.width;
		this->textureRect.height = this->imageRect.height;
	}
	
	CGContextRef contextPtr = theVisualGraphics->createBitmapContext(this->imageRect.width, this->imageRect.height);
	
	CGContextTranslateCTM(contextPtr, 0, this->imageRect.height);
	CGContextScaleCTM(contextPtr, 1.0f, -1.0f);
	
	CGRect rect = CGRectMake(0, 0, this->imageRect.width, this->imageRect.height);
	CGContextDrawImage(contextPtr, rect, imageRef);
	
	aPixelBuffer = static_cast<uint32*>(CGBitmapContextGetData(contextPtr));
#endif
	
	PixelColor* interleavedARGBColorPixelBuffer = NULL;
	
	if (debug == true) {
		interleavedARGBColorPixelBuffer = VisualColorTools::createARGBCheckPixels(this->textureRect.width, this->textureRect.height);
	} else {
		interleavedARGBColorPixelBuffer = static_cast<PixelColor*>(aPixelBuffer);
	}
	success = this->initWithARGBPixelData(interleavedARGBColorPixelBuffer, this->imageRect.width, this->imageRect.height);
	
#if TARGET_OS_MAC
	CGContextRelease(contextPtr);
	CGImageRelease(imageRef);
#endif
	
#if TARGET_OS_WIN
	bitmap->UnlockBits(bitmapData);
#endif
	
	return success;
	
}


bool VisualTextureContainer::initWithARGBPixelData(PixelColor* argbPixels, size_t imageWidth, size_t imageHeight, bool debug) {
	
	bool success = true;
	
	this->imageRect.width = imageWidth;
	this->imageRect.height = imageHeight;
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	this->useRectExtension = theVisualGraphics->canUseTextureRectExtension();
	if (this->useRectExtension == false) {
		this->textureRect.width = theVisualGraphics->power2Ceiling(this->imageRect.width);
		this->textureRect.height = theVisualGraphics->power2Ceiling(this->imageRect.height);
	} else {
		this->textureRect.width = this->imageRect.width;
		this->textureRect.height = this->imageRect.height;
	}
	
	this->textureName = theVisualGraphics->getNextFreeTextureName();
	this->aTextureIsSet = true;
	VisualTextureContainer::textureRefCountMap[this->textureName] = 1;
	
	success = theVisualGraphics->copyARGBBitmapDataToTexture(this->textureName, this->imageRect.width, this->imageRect.height, this->useRectExtension, const_cast<const uint32**>(&(argbPixels)), debug);
	
	if (this->useRectExtension == false) {
		this->logicalSize.width = (double)this->imageRect.width / (double)this->textureRect.width;
		this->logicalSize.height = (double)this->imageRect.height / (double)this->textureRect.height;
	} else {
		this->logicalSize.width = (double)this->textureRect.width;
		this->logicalSize.height = (double)this->textureRect.height;
	}
	
	return success;
}


bool VisualTextureContainer::initWithBGRAPixelData(PixelColor* bgraPixels, size_t imageWidth, size_t imageHeight, bool debug) {

	uint32 numberOfPixels = imageWidth * imageHeight;
	VisualColorTools::convertInterleavedPixels1234To4321(bgraPixels, numberOfPixels);
	
	return this->initWithARGBPixelData(bgraPixels, imageWidth, imageHeight, debug);
}


bool VisualTextureContainer::initWithStyledString(VisualStyledString& styledString) {
	
	this->releaseTextureData();
	
    bool success = true;
	
	const VisualString* aVisualString = &(styledString);
	VisualStringStyle& stringStyle = styledString.getStyle();
	if (aVisualString->getNumberOfCharacters() == 0) {
		return false;
	}
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	uint32 maxPixelWidth = theVisualGraphics->getCanvasPixelWidth();
	uint32 maxPixelHeight = theVisualGraphics->getCanvasPixelHeight();
	
	this->textureName = theVisualGraphics->getNextFreeTextureName();
	this->aTextureIsSet = true;
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
	
	success = getDimensionsOfCocoaStringBitmap((void*)aVisualString, (void*)&stringStyle, &(this->imageRect.width), &(this->imageRect.height), maxPixelWidth, maxPixelHeight, alignmentStr);
	if (!success) {
		return success;
	}
	
	if (this->useRectExtension == false) {
		this->textureRect.width = theVisualGraphics->power2Ceiling(this->imageRect.width);
		this->textureRect.height = theVisualGraphics->power2Ceiling(this->imageRect.height);
	} else {
		this->textureRect.width = this->imageRect.width;
		this->textureRect.height = this->imageRect.height;
	}
	
	PixelColor* pixelBuffer = (uint32*)calloc(this->textureRect.width * this->textureRect.height, sizeof(uint32));
	
	success = getCocoaStringBitmapData((void*)&styledString, this->textureRect.width, this->textureRect.height, alignmentStr, &(pixelBuffer));
	
	success = theVisualGraphics->copyARGBBitmapDataToTexture(this->textureName, this->textureRect.width, this->textureRect.height, this->useRectExtension, const_cast<const uint32**>(&(pixelBuffer)));
	
#endif
	
#if TARGET_OS_WIN
	
	wchar_t* stringValueRef = (wchar_t*)(styledString.getCharactersPointer());
	uint8 red = (uint8)(stringStyle.fontColor.r * 255.0f);
	uint8 green = (uint8)(stringStyle.fontColor.g * 255.0f);
	uint8 blue = (uint8)(stringStyle.fontColor.b * 255.0f);

	if (!stringValueRef) {
		success = false;
		return success;
	}
	
	success = theVisualGraphics->makeTextureOfStringWin(stringValueRef, styledString.getNumberOfCharacters(), this->textureName, this->textureRect.width, this->textureRect.height, this->imageRect.width, this->imageRect.height, stringStyle.fontNameStr, (uint16)stringStyle.fontSize, red, green, blue, stringStyle.horizontalAlignment, maxPixelWidth, maxPixelHeight);
	
#endif
	
	if (this->useRectExtension == false) {
		this->logicalSize.width = (double)this->imageRect.width / (double)this->textureRect.width;
		this->logicalSize.height = (double)this->imageRect.height / (double)this->textureRect.height;
	} else {
		this->logicalSize.width = (double)this->textureRect.width;
		this->logicalSize.height = (double)this->textureRect.height;
	}
	
	return success;
}


void VisualTextureContainer::clean() {
	this->releaseTextureData();
}


bool VisualTextureContainer::initWithFramebuffer(const BottomLeftPositionedPixelRect& clipRect) {
	
	bool success = true;

	uint16 dataType;
	uint16 pixelFormat = kGL_BGRA;
	// BGRA on intel machines and ARGB on ppc
#if TARGET_OS_WIN
	dataType = kGL_UNSIGNED_BYTE;
#else
#if __BIG_ENDIAN__
	dataType = kGL_UNSIGNED_INT_8_8_8_8_REV;
#else
	dataType = kGL_UNSIGNED_INT_8_8_8_8;
#endif
#endif
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	this->releaseTextureData();
	
	this->textureName = theVisualGraphics->getNextFreeTextureName();
	this->aTextureIsSet = true;
	VisualTextureContainer::textureRefCountMap[this->textureName] = 1;
	this->useRectExtension = theVisualGraphics->canUseTextureRectExtension();
	
	int prevReadBuffer = theVisualGraphics->getCurrentColorBufferForPixelReadingOperations();
	theVisualGraphics->setColorBufferForPixelReadingOperations(kGL_BACK_COLOR_BUFFER);
	
	int prevDrawBuffer = theVisualGraphics->getCurrentColorBufferForPixelDrawingOperations();
	theVisualGraphics->setColorBufferForPixelDrawingOperations(kGL_BACK_COLOR_BUFFER);
	
	theVisualGraphics->enableTexturing(this->useRectExtension);
	theVisualGraphics->copyFramebufferToTexture(this->textureName, this->useRectExtension, clipRect, pixelFormat, dataType);
	theVisualGraphics->disableTexturing(this->useRectExtension);
	
	theVisualGraphics->setColorBufferForPixelDrawingOperations(prevDrawBuffer);
	theVisualGraphics->setColorBufferForPixelReadingOperations(prevReadBuffer);
	
	this->imageRect.width = clipRect.pixelRect.width;
	this->imageRect.height = clipRect.pixelRect.height;
	
	this->useRectExtension = theVisualGraphics->canUseTextureRectExtension();
	if (this->useRectExtension == false) {
		this->textureRect.width = theVisualGraphics->power2Ceiling(this->imageRect.width);
		this->textureRect.height = theVisualGraphics->power2Ceiling(this->imageRect.height);
	} else {
		this->textureRect.width = this->imageRect.width;
		this->textureRect.height = this->imageRect.height;
	}
	
	return success;
}


PixelColor* VisualTextureContainer::createARGBImagePixels(bool debug) {
	
	PixelColor* pixelBuffer = NULL;
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	if (debug == true) {
		pixelBuffer = VisualColorTools::createARGBCheckPixels(this->textureRect.width, this->textureRect.height);
		return pixelBuffer;
	}
	
	theVisualGraphics->enableTexturing(this->useRectExtension);
	theVisualGraphics->bindTexture(this->textureName, this->useRectExtension);
	theVisualGraphics->setPixelStorageParams();
	
	uint16 format = kGL_BGRA;
	uint16 type;
	
	// GL_BGRA/GL_UNSIGNED_BYTE and GL_BGRA/GL_UNSIGNED_INT_8_8_8_8_REV are equivalent on little endian machines
	
	if (this->useRectExtension == false) {
		
		pixelBuffer = (PixelColor*)malloc(this->textureRect.width * this->textureRect.height * sizeof(PixelColor));
		
		type = kGL_UNSIGNED_BYTE;
		theVisualGraphics->getPixelsOfCurrentTexture(this->useRectExtension, format, type, &(pixelBuffer));
		
#if __BIG_ENDIAN__
		VisualColorTools::convertInterleavedPixels1234To4321(pixelBuffer, this->textureRect.width * this->textureRect.height);
#endif

		if ((this->textureRect.width != this->imageRect.width) || (this->textureRect.height != this->imageRect.height)) {
			// copy actual image pixel data out of texture pixel data
			Pixel topLeftPosition;
			topLeftPosition.x = 0;
			topLeftPosition.y = 0;
			TopLeftPositionedPixelRect clipRect;
			clipRect.topLeftPixel = topLeftPosition;
			clipRect.pixelRect = this->imageRect;
			PixelColor* clippedPixelData = theVisualGraphics->clipPixelData(pixelBuffer, this->textureRect, clipRect);
			free(pixelBuffer);
			pixelBuffer = clippedPixelData;
		}

	} else {
		
		pixelBuffer = (PixelColor*)malloc(this->imageRect.width * this->imageRect.height * sizeof(PixelColor));
		
		type = kGL_UNSIGNED_INT_8_8_8_8_REV;
		theVisualGraphics->getPixelsOfCurrentTexture(this->useRectExtension, format, type, &pixelBuffer);
	}
	
	theVisualGraphics->disableTexturing(this->useRectExtension);
	
	return pixelBuffer;
}


PixelColor* VisualTextureContainer::createReadPixels(const uint16 format, const uint16 type) {
	
	PixelColor* pixels = NULL;
	PixelColor* prevPixels = NULL;
	
	char errStr[512];
	
	double scaleFactor = 1.0;
	
	uint8 numberOfBytesPerChannel = 0;
	uint8 numberOfChannels = 0; // channel == color resp. alpha channel
	uint8 numberOfBytesPerPixel = 0;
	uint32 numberOfBytesPerRow = 0;
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
	numberOfBytesPerRow = numberOfBytesPerPixel * this->textureRect.width;
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	Coord coord;
	VisualCamera aCamera;
	aCamera.setOrthographicProjection();
	coord.x = aCamera.getMaxLeftCoord() * scaleFactor;
	coord.y = (aCamera.getMaxBottomCoord() + theVisualGraphics->yPixelToCoord(this->textureRect.height, aCamera)) * scaleFactor;
	coord.z = aCamera.getMaxNearPos();
	VertexChain aVertexChain;
	TexCoord texCoord;
	texCoord.s = 0.0;
	texCoord.t = this->getTextureLogicalHeight();
	VisualVertex* topLeftFrontVertex = new VisualVertex(coord, texCoord, white);
	aVertexChain.push_back(topLeftFrontVertex);
	
	coord.x = aCamera.getMaxLeftCoord() * scaleFactor;
	coord.y = aCamera.getMaxBottomCoord() * scaleFactor;
	coord.z = aCamera.getMaxNearPos();
	texCoord.s = 0.0;
	texCoord.t = 0.0;
	VisualVertex* bottomLeftFrontVertex = new VisualVertex(coord, texCoord, white);
	aVertexChain.push_back(bottomLeftFrontVertex);
	
	coord.x = (aCamera.getMaxLeftCoord() + theVisualGraphics->xPixelToCoord(this->textureRect.width, aCamera)) * scaleFactor;
	coord.y = aCamera.getMaxBottomCoord() * scaleFactor;
	coord.z = aCamera.getMaxNearPos();
	texCoord.s = this->getTextureLogicalWidth();
	texCoord.t = 0.0;
	VisualVertex* bottomRightFrontVertex = new VisualVertex(coord, texCoord, white);
	aVertexChain.push_back(bottomRightFrontVertex);
	
	coord.x = (aCamera.getMaxLeftCoord() + theVisualGraphics->xPixelToCoord(this->textureRect.width, aCamera)) * scaleFactor;
	coord.y = (aCamera.getMaxBottomCoord() + theVisualGraphics->yPixelToCoord(this->textureRect.height, aCamera)) * scaleFactor;
	coord.z = aCamera.getMaxNearPos();
	texCoord.s = this->getTextureLogicalWidth();
	texCoord.t = this->getTextureLogicalHeight();
	VisualVertex* topRightFrontVertex = new VisualVertex(coord, texCoord, white);
	aVertexChain.push_back(topRightFrontVertex);
	
	// read previous pixels
	if ((format == kGL_RGBA) || (format == kGL_BGRA)) {
		prevPixels = (uint32*)calloc((numberOfBytesPerRow / numberOfBytesPerPixel) * this->textureRect.height, numberOfBytesPerPixel);
	}
	theVisualGraphics->readPixels(aCamera.getMaxLeftCoord(), aCamera.getMaxBottomCoord(), this->textureRect.width, this->textureRect.height, &prevPixels, format, type, aCamera);
	
	// draw current texture
	theVisualGraphics->drawTexture(this->textureName, &aVertexChain, this->getUseRectExtension(), kReplace);
	
	// read pixels of current texture
	if ((format == kGL_RGBA) || (format == kGL_BGRA)) {
		pixels = (uint32*)calloc((numberOfBytesPerRow / numberOfBytesPerPixel) * this->textureRect.height, numberOfBytesPerPixel);
	}
	theVisualGraphics->readPixels(aCamera.getMaxLeftCoord(), aCamera.getMaxBottomCoord(), this->textureRect.width, this->textureRect.height, &pixels, format, type, aCamera);
	
	// restore previous pixels
	theVisualGraphics->drawPixels(&prevPixels, aCamera.getMaxLeftCoord(), aCamera.getMaxBottomCoord(), this->textureRect.width, this->textureRect.height, format, type);
	
	for (VertexChainIterator chainIt = aVertexChain.begin(); chainIt != aVertexChain.end(); chainIt++) {
		delete *chainIt;
		*chainIt = NULL;
	}
	aVertexChain.clear();
	
	free(prevPixels);
	
	return pixels;
}


void VisualTextureContainer::copy(const VisualTextureContainer& other) {
	if (other.aTextureIsSet) {
		VisualTextureContainer::textureRefCountMap[other.textureName]++;
	}
	this->aTextureIsSet = other.aTextureIsSet;
	this->textureName = other.textureName;
	this->textureRect = other.textureRect;
	this->logicalSize = other.logicalSize;
	this->imageRect = other.imageRect;
	this->useRectExtension = other.useRectExtension;
}


uint32 VisualTextureContainer::getTextureName() const {
	return this->textureName;
}


size_t VisualTextureContainer::getTextureWidth() const {
	return this->textureRect.width;
}


size_t VisualTextureContainer::getTextureHeight() const {
	return this->textureRect.height;
}


double VisualTextureContainer::getTextureLogicalWidth() const {
	return this->logicalSize.width;
}


double VisualTextureContainer::getTextureLogicalHeight() const {
	return this->logicalSize.height;
}


size_t VisualTextureContainer::getImageWidth() const {
	return this->imageRect.width;
}


size_t VisualTextureContainer::getImageHeight() const {
	return this->imageRect.height;
}


bool VisualTextureContainer::getUseRectExtension() const {
	return this->useRectExtension;
}


void VisualTextureContainer::releaseTextureData() {
	if (this->aTextureIsSet) {
		VisualTextureContainer::textureRefCountMap[this->textureName]--;
		if (VisualTextureContainer::textureRefCountMap[this->textureName] == 0) {
			VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
			theVisualGraphics->deleteTextures(1, &(this->textureName));
			this->textureName = 0;
			this->aTextureIsSet = false;
		}
	}
}


bool VisualTextureContainer::textureIsSet() {
	return this->aTextureIsSet;
}


void VisualTextureContainer::applyConvolutionFilter(const VisualConvolutionFilter& aConvolutionFilter) {
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();

	uint16 dataType;
	uint16 pixelFormat = kGL_BGRA;
	// BGRA on intel machines and ARGB on ppc
#if TARGET_OS_WIN
	dataType = kGL_UNSIGNED_BYTE;
#else
#if __BIG_ENDIAN__
	dataType = kGL_UNSIGNED_INT_8_8_8_8_REV;
#else
	dataType = kGL_UNSIGNED_INT_8_8_8_8;
#endif
#endif
	
	if (theVisualGraphics->doesSupportGLConvolutionFilter()) {
		
		int prevReadBuffer = theVisualGraphics->getCurrentColorBufferForPixelReadingOperations();
		theVisualGraphics->setColorBufferForPixelReadingOperations(kGL_BACK_COLOR_BUFFER);
		
		int prevDrawBuffer = theVisualGraphics->getCurrentColorBufferForPixelDrawingOperations();
		theVisualGraphics->setColorBufferForPixelDrawingOperations(kGL_BACK_COLOR_BUFFER);
		
		uint32* prevPixels = NULL;
		
		char errStr[512];
		
		uint8 numberOfBytesPerChannel = 0;
		uint8 numberOfChannels = 0; // channel == color resp. alpha channel
		uint8 numberOfBytesPerPixel = 0;
		uint32 numberOfBytesPerRow = 0;
		if ((pixelFormat == kGL_RGBA) || (pixelFormat == kGL_BGRA)) {
			numberOfChannels = 4;
		} else {
			sprintf(errStr, "unknown format %d in file: %s (line: %d) [%s])", pixelFormat, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errStr);
			return;
		}
		
		if ((dataType == kGL_UNSIGNED_INT_8_8_8_8_REV) || (dataType == kGL_UNSIGNED_INT_8_8_8_8) || (dataType == kGL_UNSIGNED_BYTE)) {
			numberOfBytesPerChannel = 1; // // 1 byte (== 8 bits) per color/channel
		} else {
			sprintf(errStr, "unknown type %d in file: %s (line: %d) [%s])", dataType, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errStr);
			return;
		}
		
		numberOfBytesPerPixel = numberOfBytesPerChannel * numberOfChannels;
		numberOfBytesPerRow = numberOfBytesPerPixel * this->textureRect.width;
		
		// read previous pixels
		if ((pixelFormat == kGL_RGBA) || (pixelFormat == kGL_BGRA)) {
			prevPixels = (uint32*)calloc((numberOfBytesPerRow / numberOfBytesPerPixel) * this->textureRect.height, numberOfBytesPerPixel);
		}
		VisualCamera aCamera;
		aCamera.setOrthographicProjection();
		theVisualGraphics->readPixels(aCamera.getMaxLeftCoord(), aCamera.getMaxBottomCoord(), this->textureRect.width, this->textureRect.height, &prevPixels, pixelFormat, dataType, aCamera);
		
		PixelColor* pixels = NULL;
		pixels = this->createARGBImagePixels();
		if (pixels == NULL) {
			sprintf(errStr, "pixels == NULL in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
			writeLog(errStr);
			return;
		}

		theVisualGraphics->drawPixels(&pixels, aCamera.getMaxLeftCoord(), aCamera.getMaxBottomCoord(), this->imageRect.width, this->imageRect.height, pixelFormat, dataType, &aConvolutionFilter);
		
		free(pixels);
		pixels = NULL;
		
		BottomLeftPositionedPixelRect clipRect;
		clipRect.bottomLeftPixel.x = 0;
		clipRect.bottomLeftPixel.y = 0;
		clipRect.pixelRect.width = this->textureRect.width;
		clipRect.pixelRect.height = this->textureRect.height;
		
		theVisualGraphics->enableTexturing(this->useRectExtension);
		theVisualGraphics->copyFramebufferToTexture(this->textureName, this->useRectExtension, clipRect, pixelFormat, dataType);
		theVisualGraphics->disableTexturing(this->useRectExtension);
		
		// restore previous pixels
		theVisualGraphics->drawPixels(&prevPixels, aCamera.getMaxLeftCoord(), aCamera.getMaxBottomCoord(), this->textureRect.width, this->textureRect.height, pixelFormat, dataType);
		
		free(prevPixels);
		
		theVisualGraphics->setColorBufferForPixelDrawingOperations(prevDrawBuffer);
		theVisualGraphics->setColorBufferForPixelReadingOperations(prevReadBuffer);
		
	} else {
		
		PixelColor* pixels = this->createARGBImagePixels();		
		PixelColor* filteredPixels = NULL;
		aConvolutionFilter.applyToPixelData(pixels, this->imageRect.width, this->imageRect.height, pixelFormat, dataType, &filteredPixels);
		free(pixels);
		pixels = NULL;
		bool success = false;
		const PixelColor* constFilteredPixels = const_cast<const PixelColor*>(filteredPixels);
		success = theVisualGraphics->copyARGBBitmapDataToTexture(this->textureName, this->imageRect.width, this->imageRect.height, this->useRectExtension, &constFilteredPixels);
		if (filteredPixels != NULL) {
			free(filteredPixels);
		}
	}
	
}


void VisualTextureContainer::resample(const PixelRect& pixelRect) {
	
	PixelColor* pixels = this->createARGBImagePixels();
	PixelColor* pixelsOut = (PixelColor*)malloc(pixelRect.width * pixelRect.height * sizeof(PixelColor));
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	uint16 aPixelFormat = kGL_RGBA;
	uint16 dataTypeIn = kGL_UNSIGNED_BYTE;
	uint16 dataTypeOut = dataTypeIn;
	theVisualGraphics->resample(aPixelFormat, this->imageRect.width, this->imageRect.height, dataTypeIn, pixels, pixelRect.width, pixelRect.height, dataTypeOut, &pixelsOut);
	free(pixels);

	this->initWithARGBPixelData(pixelsOut, pixelRect.width, pixelRect.height);
	free(pixelsOut);

}
