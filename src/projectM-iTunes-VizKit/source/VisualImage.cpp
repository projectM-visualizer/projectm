/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualImage.cpp
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

#include "VisualImage.h"
#include "VisualTextureContainer.h"
#include "VisualQuickTime.h"
#include "VisualString.h"
#include "VisualStringStyle.h"
#include "VisualFile.h"
#include "VisualGraphics.h"
#include "VisualErrorHandling.h"



using namespace VizKit;


VisualImage::VisualImage() {
	visualTextureContainer = new VisualTextureContainer;
	isSet = false;
}


VisualImage::~VisualImage() {
	delete visualTextureContainer;
}


VisualImage::VisualImage(const VisualImage& other) {
	visualTextureContainer = new VisualTextureContainer;
	copy(other);
}


VisualImage& VisualImage::operator=(const VisualImage& other) {
	if (this != &other) {
		this->copy(other);
	}
	return *this;
}


OSStatus VisualImage::initWithFile(const VisualFile& aFile) {
	OSStatus osStatus = this->visualTextureContainer->initWithFile(aFile);
	if (osStatus == noErr) {
		this->isSet = true;
	}
	return osStatus;
}


OSStatus VisualImage::initWithDataHandle(Handle aDataHandle, OSType aFileFormatType) {
	OSStatus osStatus = this->visualTextureContainer->initWithDataHandle(aDataHandle, aFileFormatType);
	if (osStatus == noErr) {
		this->isSet = true;
	}
	return osStatus;
}


OSStatus VisualImage::initWithDataPointerToPointer(const unsigned char** const aPointerToPointerToBuffer, UInt32 size, OSType aFileFormatType) {
	OSStatus osStatus = this->visualTextureContainer->initWithDataPointerToPointer(aPointerToPointerToBuffer, size, aFileFormatType);
	if (osStatus == noErr) {
		this->isSet = true;
	}
	return osStatus;
}


OSStatus VisualImage::initWithString(const VisualString& stringValue, VisualStringStyle& stringStyle) {
	OSStatus osStatus = this->visualTextureContainer->initWithString(stringValue, stringStyle);
	if (osStatus == noErr) {
		this->isSet = true;
	}
	return osStatus;
}


#if TARGET_OS_WIN
OSStatus VisualImage::initWithResource(int nameId) {
	char* type = "PNG";
	char* pngImageData = NULL;
	UInt32 sizeOfImageResource = 0;
	OSStatus osStatus = VisualFile::getDataOfResource(nameId, type, (void**)&pngImageData, sizeOfImageResource);
	if (osStatus == noErr) {
		Handle pngImageDataHandle;
		PtrToHand(pngImageData, &pngImageDataHandle, sizeOfImageResource);
		osStatus = this->visualTextureContainer->initWithDataHandle(pngImageDataHandle, 'PNGf');
		DeleteObject(pngImageData);
	}
	if (osStatus == noErr) {
		this->isSet = true;
	}
	return osStatus;
}
#endif
#if TARGET_OS_MAC
OSStatus VisualImage::initWithResource(char* name) {
	VisualFile* resourceFile = VisualFile::createWithResourcesDirectory();
	VisualString resourceFileName = VisualString(name);
	resourceFile->appendFileName(resourceFileName);
	OSStatus osStatus = this->initWithFile(*resourceFile);
	delete(resourceFile);
	if (osStatus == noErr) {
		this->isSet = true;
	}
	return osStatus;
}
#endif


#if TARGET_OS_MAC
OSStatus VisualImage::initWithURL(VisualString& anURL) {
	OSStatus osStatus = this->visualTextureContainer->initWithURL(anURL);
	if (osStatus == noErr) {
		this->isSet = true;
	}
	return osStatus;
}
#endif


OSStatus VisualImage::writeToFile(const VisualFile& aVisualFile, OSType aFileFormatType) {

	OSStatus osStatus = noErr;
	
	UInt32* texturePixels = NULL;

#if __BIG_ENDIAN__
	texturePixels = this->visualTextureContainer->getTexturePixels(kGL_BGRA, kGL_UNSIGNED_INT_8_8_8_8_REV);
#else
#if TARGET_OS_WIN
	texturePixels = this->visualTextureContainer->getTexturePixels(kGL_BGRA, kGL_UNSIGNED_BYTE);
#else
	texturePixels = this->visualTextureContainer->getTexturePixels(kGL_BGRA, kGL_UNSIGNED_INT_8_8_8_8);
#endif
#endif

	UInt32* flippedImagePixels = (UInt32*)malloc(this->visualTextureContainer->getImageWidth() * this->visualTextureContainer->getImageHeight() * 4);
	for (UInt32 i = 0; i < this->visualTextureContainer->getImageHeight(); i++) {
		memcpy((flippedImagePixels + this->visualTextureContainer->getImageWidth() * (this->visualTextureContainer->getImageHeight() - i - 1)), (texturePixels + this->visualTextureContainer->getTextureWidth() * i), this->visualTextureContainer->getImageWidth() * 4);
	}
	osStatus = VisualQuickTime::writeARGBBitmapToFile(flippedImagePixels, aVisualFile, this->visualTextureContainer->getImageWidth(), this->visualTextureContainer->getImageHeight(), aFileFormatType);

	free(flippedImagePixels);

	return osStatus;

}

/*
//
- (BOOL) writeToURL:(NSURL *)absURL ofType:(NSString *)typeName forSaveOperation:(NSSaveOperationType)saveOp 
         originalContentsURL:(NSURL *)absOrigURL error:(NSError **)outError
{
    BOOL status = NO;
    
    CGImageRef image = [self currentCGImage];
    if (image==nil)
        goto bail;

    // Create an image destination writing to `url'
    CGImageDestinationRef dest = CGImageDestinationCreateWithURL((CFURLRef)absURL, (CFStringRef)typeName, 1, nil);
    if (dest==nil)
        goto bail;

    // Set the image in the image destination to be `image' with
    // optional properties specified in saved properties dict.
    CGImageDestinationAddImage(dest, image, (CFDictionaryRef)[self saveMetaAndOpts]);

    status = CGImageDestinationFinalize(dest);

    CGImageRelease(image);

bail:

    if (status==NO && outError)
        *outError = [NSError errorWithDomain:NSCocoaErrorDomain code:NSFileWriteUnknownError userInfo:nil];
    
    return status;
}
*/



void VisualImage::applyConvolutionFilter(const VisualConvolutionFilter& aConvolutionFilter) {
	this->visualTextureContainer->applyConvolutionFilter(aConvolutionFilter);
}


UInt32 VisualImage::getWidth() {
	return this->visualTextureContainer->getImageWidth();
}


UInt32 VisualImage::getHeight() {
	return this->visualTextureContainer->getImageHeight();
}


double VisualImage::getLogicalWidth() {
	return this->visualTextureContainer->getTextureLogicalWidth();
}


double VisualImage::getLogicalHeight() {
	return this->visualTextureContainer->getTextureLogicalHeight();
}


void VisualImage::draw(VertexChain* aVertexChain) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawTexture(this->visualTextureContainer->getTextureName(), aVertexChain, this->visualTextureContainer->getUseRectExtension());
}


bool VisualImage::isEmpty() {
	return !(this->isSet);
}


void VisualImage::copy(const VisualImage& other) {
	*(this->visualTextureContainer) = *(other.visualTextureContainer);
	this->isSet = other.isSet;
}


VisualImage* VisualImage::createWithFile(const VisualFile& aFile) {
	VisualImage* anImage = new VisualImage;
	OSStatus status = anImage->initWithFile(aFile);
	if (status != noErr) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


VisualImage* VisualImage::createWithDataHandle(Handle aDataHandle, OSType aFileFormatType) {
	VisualImage* anImage = new VisualImage;
	OSStatus status = anImage->initWithDataHandle(aDataHandle, aFileFormatType);
	if (status != noErr) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


VisualImage* VisualImage::createWithDataPointerToPointer(const unsigned char** const aPointerToPointerToBuffer, UInt32 size, OSType aFileFormatType) {
	VisualImage* anImage = new VisualImage;
	OSStatus status = anImage->initWithDataPointerToPointer(aPointerToPointerToBuffer, size, aFileFormatType);
	if (status != noErr) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


VisualImage* VisualImage::createWithString(const VisualString& stringValue, VisualStringStyle& stringStyle) {
	VisualImage* anImage = new VisualImage;
	OSStatus status = anImage->initWithString(stringValue, stringStyle);
	if (status != noErr) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


#if TARGET_OS_MAC
VisualImage* VisualImage::createWithResource(char* name) {
#endif
#if TARGET_OS_WIN
VisualImage* VisualImage::createWithResource(int nameId) {
#endif
	VisualImage* anImage = new VisualImage;
#if TARGET_OS_MAC
	OSStatus status = anImage->initWithResource(name);
#endif
#if TARGET_OS_WIN
	OSStatus status = anImage->initWithResource(nameId);
#endif
	if (status != noErr) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


#if TARGET_OS_MAC
VisualImage* VisualImage::createWithURL(VisualString& anURL) {
	VisualImage* anImage = new VisualImage;
	OSStatus status = anImage->initWithURL(anURL);
	if (status != noErr) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}
#endif
