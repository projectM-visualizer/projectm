/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualImage.cpp
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

#include "VisualImage.h"
#include "VisualTextureContainer.h"
#include "VisualStyledString.h"
#include "VisualFile.h"
#include "VisualGraphics.h"
#include "VisualErrorHandling.h"
#include "VisualNotification.h"
#include "VisualNetwork.h"
#include "VisualDispatch.h"
#include "VisualColorTools.h"

#if TARGET_OS_WIN
#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#endif


using namespace VizKit;



VisualImage::VisualImage() {
	visualTextureContainer = new VisualTextureContainer;
	isSet = false;
	blendMode = kBlend;
	histogram = NULL;
}


VisualImage::~VisualImage() {
	delete visualTextureContainer;
	if (histogram != NULL) {
		delete histogram;
	}
}


VisualImage::VisualImage(const VisualImage& other) : VisualObject(other) {
	copy(other);
}


VisualImage& VisualImage::operator=(const VisualImage& other) {
	
	if (this == &other) return *this;
	
	VisualObject::operator=(other);
	
	delete this->visualTextureContainer;
	
	if (this->histogram != NULL) {
		delete this->histogram;
	}
	
	this->copy(other);
	
	return *this;
}


VisualImage* VisualImage::clone(void) const {
	return new VisualImage(*this);
}


VisualImage* VisualImage::cloneIndependently() const {
	VisualTextureContainer* textureContainer = this->getTextureContainer();
	PixelColor* pixels = textureContainer->createARGBImagePixels();
	VisualImage* imageCopy = VisualImage::createWithARGBPixelData(pixels, textureContainer->getImageWidth(), textureContainer->getImageHeight());
	free(pixels);
	return imageCopy;
}


bool VisualImage::initWithFile(VisualFile& aFile) {
	bool success = this->visualTextureContainer->initWithFile(aFile);
	if (success) {
		this->isSet = true;
	}
	return success;
}


bool VisualImage::initWithARGBPixelData(PixelColor* argbPixels, uint32 width, uint32 height) {
	bool success = true;
	success = this->visualTextureContainer->initWithARGBPixelData(argbPixels, width, height);
	if (success) {
		this->isSet = true;
	}
	return success;
}


bool VisualImage::initWithEncodedData(const char* const bufferData, size_t size) {
	bool success = this->visualTextureContainer->initWithEncodedData(bufferData, size);
	if (success) {
		this->isSet = true;
	}
	return success;
}


bool VisualImage::initWithStyledString(VisualStyledString& styledString) {
	bool success = this->visualTextureContainer->initWithStyledString(styledString);
	if (success) {
		this->isSet = true;
	}
	return success;
}


#if TARGET_OS_WIN
bool VisualImage::initWithResource(int nameId) {
	bool success = true;
	char* type = "PNG";
	char* pngImageData = NULL;
	uint32 sizeOfImageResource = 0;
	success = VisualFile::getDataOfResource(nameId, type, (void**)&pngImageData, sizeOfImageResource);
	if (success) {
		success = this->visualTextureContainer->initWithEncodedData((const char* const)pngImageData, sizeOfImageResource);
		BOOL deleteSuccess = DeleteObject(pngImageData);
	} else {
		char errLog[256];
		sprintf(errLog, "Err: Unable to get resource data %s in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	}
	if (success) {
		this->isSet = true;
	}
	return success;
}
#endif
#if TARGET_OS_MAC
bool VisualImage::initWithResource(const char* name) {
	bool success = true;
	VisualFile* resourceFile = VisualFile::createWithResourcesDirectory();
	VisualString resourceFileName = VisualString(name);
	success = resourceFile->appendFileName(resourceFileName);
	if (success) {
		success = this->initWithFile(*resourceFile);
		delete(resourceFile);
		if (success) {
			this->isSet = true;
		} else {
			char errLog[256];
			sprintf(errLog, "Err: Unable to load resource %s in file: %s (line: %d) [%s])", name, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
		}
	}
	return success;
}
#endif


bool VisualImage::initWithURL(VisualString& anURL, VisualItemIdentifier& anId) {
	this->visualTextureContainer->clean();
	bool success = VisualNetwork::addToDownloadQueue(anURL, this, anId);
	return success;
}


bool VisualImage::initWithLoadedEncodedData() {
	bool success = false;
	if (this->hasData()) {
		success = this->visualTextureContainer->initWithEncodedData((const char*)this->getData(), this->getDataSize());
	}
	this->freeData();
	return success;
}


bool VisualImage::initWithFramebuffer(const BottomLeftPositionedPixelRect& clipRect) {
	this->visualTextureContainer->clean();
	bool success = this->visualTextureContainer->initWithFramebuffer(clipRect);
	return success;
}


void VisualImage::writeToPNGFileAsync(VisualFile& aVisualFile) {
	
	// Only in the main thread we can transfer the memory of the graphics card to the CPU memory
	VisualNotification aNotification;
	aNotification.setPointer(this);
	aNotification.setObject(aVisualFile);
	aNotification.setKey(kImageWriteToPNGFileMsg);
	aNotification.post();
	
}


void VisualImage::writeToPNGFileAsyncAndDelete(VisualFile& aVisualFile) {
	
	// Only in the main thread we can transfer the memory of the graphics card to the CPU memory
	VisualNotification aNotification;
	aNotification.setPointer(this);
	aNotification.setObject(aVisualFile);
	aNotification.setKey(kImageWriteToPNGFileAndDeleteMsg);
	aNotification.post();
	
}


bool VisualImage::writeToPNGFile(VisualFile& aVisualFile) const {
	
	bool success = true;
	
	PixelColor* texturePixels = this->visualTextureContainer->createARGBImagePixels();
	
	uint32 imageSize = this->visualTextureContainer->getImageWidth() * this->visualTextureContainer->getImageHeight() * sizeof(PixelColor);
	PixelColor* flippedImagePixels = (PixelColor*)malloc(imageSize);
	for (uint32 i = 0; i < this->visualTextureContainer->getImageHeight(); i++) {
		memcpy((flippedImagePixels + this->visualTextureContainer->getImageWidth() * (this->visualTextureContainer->getImageHeight() - i - 1)), (texturePixels + this->visualTextureContainer->getImageWidth() * i), this->visualTextureContainer->getImageWidth() * 4);
	}
	free(texturePixels);
	texturePixels = NULL;
	
#if TARGET_OS_MAC
	
	CFDictionaryRef options = NULL;
	CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, flippedImagePixels, imageSize, NULL);
	CGColorSpaceRef colorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	CGBitmapInfo bitmapInfo = kCGImageAlphaFirst | kCGBitmapByteOrder32Host;
	CGImageRef imageRef = CGImageCreate(this->visualTextureContainer->getImageWidth(), this->visualTextureContainer->getImageHeight(), 8, 32, this->visualTextureContainer->getImageWidth() * 4, colorspace, bitmapInfo, dataProvider, NULL, false, kCGRenderingIntentDefault);
	
	CFMutableDataRef destinationImageData = CFDataCreateMutable(kCFAllocatorDefault, 0);
	size_t numberOfImages = 1;
	CGImageDestinationRef destination = CGImageDestinationCreateWithData((CFMutableDataRef)destinationImageData, CFSTR("public.png"), numberOfImages, NULL);
	
	CGImageDestinationAddImage(destination, imageRef, options);
	
	CGImageDestinationFinalize(destination);
	
	CGColorSpaceRelease(colorspace);
	
	const UInt8* dataPointer = CFDataGetBytePtr(destinationImageData);
	void** dataHandle = (void**)&dataPointer;
	VisualFile::writeDataToFile(dataHandle, CFDataGetLength(destinationImageData), aVisualFile);
	
#endif

#if TARGET_OS_WIN

	INT stride = sizeof(PixelColor) * this->visualTextureContainer->getImageWidth();

	Gdiplus::Bitmap image(this->visualTextureContainer->getImageWidth(), 
			this->visualTextureContainer->getImageHeight(), 
			stride, 
			PixelFormat32bppARGB,
			(BYTE*)flippedImagePixels);	

	IStream* pIStream = NULL;
	HRESULT result = CreateStreamOnHGlobal(NULL, TRUE, (LPSTREAM*)&pIStream);
	if (result != S_OK)
		return false;

	CLSID pngClsid;
	VisualGraphics::getGdiplusEncoderClsid(L"image/png", &pngClsid);

	Gdiplus::EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
	encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;

	// setup compression level

	ULONG quality = 50;
	encoderParameters.Parameter[0].Value = &quality;

	Gdiplus::Status saveStatus = image.Save(pIStream, &pngClsid, &encoderParameters);
	if (saveStatus != Gdiplus::Ok) {
		pIStream->Release();
		return false;
	}

	// get the size of the stream
	ULARGE_INTEGER ulnSize;
	LARGE_INTEGER lnOffset;
	lnOffset.QuadPart = 0;
	if (pIStream->Seek(lnOffset, STREAM_SEEK_END, &ulnSize) != S_OK) {
		pIStream->Release();
		return false;
	}

	// now move the pointer to the beginning of the file
	if (pIStream->Seek(lnOffset, STREAM_SEEK_SET, NULL) != S_OK) {
		pIStream->Release();
		return false;
	}

	HGLOBAL hg;
	if (GetHGlobalFromStream(pIStream, &hg) == S_OK) {

		char* dataPointer = new char[(unsigned int)ulnSize.QuadPart];

        // Read the stream directly into the buffer
        ULONG ulBytesRead;
        if (pIStream->Read(dataPointer, (ULONG)ulnSize.QuadPart, &ulBytesRead) != S_OK) {
            pIStream->Release();
            return false;
        }

		void** dataHandle = (void**)&dataPointer;
		VisualFile::writeDataToFile(dataHandle, ulBytesRead, aVisualFile);

		delete[] dataPointer;
	}

#endif

	free(flippedImagePixels);
	
	return success;
	
}


void VisualImage::applyConvolutionFilter(const VisualConvolutionFilter& aConvolutionFilter) {
	this->visualTextureContainer->applyConvolutionFilter(aConvolutionFilter);
}


uint32 VisualImage::getWidth() const {
	return this->visualTextureContainer->getImageWidth();
}


uint32 VisualImage::getHeight() const {
	return this->visualTextureContainer->getImageHeight();
}


double VisualImage::getLogicalWidth() const {
	return this->visualTextureContainer->getTextureLogicalWidth();
}


double VisualImage::getLogicalHeight() const {
	return this->visualTextureContainer->getTextureLogicalHeight();
}


void VisualImage::draw(const VertexChain* const aVertexChain, bool debug) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawTexture(this->visualTextureContainer->getTextureName(), aVertexChain, this->visualTextureContainer->getUseRectExtension(), this->blendMode, debug);
}


bool VisualImage::isEmpty() const {
	return !(this->isSet);
}


BlendMode VisualImage::getBlendMode(void) const {
	return this->blendMode;
}


void VisualImage::setBlendMode(BlendMode aBlendMode) {
	this->blendMode = aBlendMode;
}


VisualImage* VisualImage::createWithFile(VisualFile& aFile) {
	VisualImage* anImage = new VisualImage;
	bool success = anImage->initWithFile(aFile);
	if (!success) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


VisualImage* VisualImage::createWithARGBPixelData(PixelColor* argbPixels, uint32 width, uint32 height) {
	VisualImage* anImage = new VisualImage;
	bool success = anImage->initWithARGBPixelData(argbPixels, width, height);
	if (!success) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


VisualImage* VisualImage::createWithEncodedData(const char* const bufferData, uint32 size) {
	VisualImage* anImage = new VisualImage;
	bool success = anImage->initWithEncodedData(bufferData, size);
	if (!success) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


VisualImage* VisualImage::createWithStyledString(VisualStyledString& styledString) {
	VisualImage* anImage = new VisualImage;
	bool success = anImage->initWithStyledString(styledString);
	if (!success) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


#if TARGET_OS_MAC
VisualImage* VisualImage::createWithResource(const char* name) {
#endif
#if TARGET_OS_WIN
VisualImage* VisualImage::createWithResource(int nameId) {
#endif
	VisualImage* anImage = new VisualImage;
#if TARGET_OS_MAC
	bool success = anImage->initWithResource(name);
#endif
#if TARGET_OS_WIN
	bool success = anImage->initWithResource(nameId);
#endif
	if (!success) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


VisualImage* VisualImage::createWithURL(VisualString& anURL, VisualItemIdentifier& anId) {
	VisualImage* anImage = new VisualImage;
	bool success = anImage->initWithURL(anURL, anId);
	if (!success) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


VisualImage* VisualImage::createWithFramebuffer(const BottomLeftPositionedPixelRect& clipRect) {
	VisualImage* anImage = new VisualImage;
	bool success = anImage->initWithFramebuffer(clipRect);
	if (!success) {
		delete anImage;
		anImage = NULL;
	}
	return anImage;
}


void VisualImage::resample(const PixelRect& pixelRect) {
	this->visualTextureContainer->resample(pixelRect);
}


VisualTextureContainer* VisualImage::getTextureContainer() const {
	return this->visualTextureContainer;
}


void VisualImage::dataLoadDidEnd(const VisualItemIdentifier& identifier) {
	// Only in the main thread we can transfer the fetched encoded image to the memory of the graphics card
	// (OpenGL calls must be executed in the main thread)
	VisualNotification aNotification(identifier);
	aNotification.setPointer(this);
	aNotification.setKey(kLoadingEncodedImageDataCompletedMsg);
	aNotification.post();
}
	

void VisualImage::createHistogram() {
	
	VisualImageHistogramPixelColors* visualImageHistogramPixelColors = new VisualImageHistogramPixelColors;
	visualImageHistogramPixelColors->visualImage = this;
	
	this->getRGBHistogramInputPixels(visualImageHistogramPixelColors->pixelColorValuesVector);
	
	bool success = false;
	success = VisualThreading::createThread((ThreadingFuncPtr)VisualImage::createHistogramOfRGBPixelsThread, (void*)visualImageHistogramPixelColors);

}
	

VisualHistogram::PixelColorHistogram* VisualImage::getHistogram() {
	return this->histogram;
}


#if TARGET_OS_MAC
OSStatus VisualImage::createHistogramOfRGBPixelsThread(void* visualImageHistogramPixelColors) {
	OSStatus retVal = noErr;
#endif
#if TARGET_OS_WIN
DWORD VisualImage::createHistogramOfRGBPixelsThread(LPVOID visualImageHistogramPixelColors) {
	DWORD retVal = 0;
#endif
	
	VisualHistogram::PixelColorHistogram aHistogram = VisualHistogram::createHistogramOfRGBPixels(((VisualImageHistogramPixelColors*)visualImageHistogramPixelColors)->pixelColorValuesVector);
	
	VisualImage* image = ((VisualImageHistogramPixelColors*)visualImageHistogramPixelColors)->visualImage;
	if (image->histogram != NULL) {
		delete image->histogram;
		image->histogram = NULL;
	}
	if (aHistogram.size() > 0) {
		image->histogram = new VisualHistogram::PixelColorHistogram(aHistogram);
	}
	
	delete (VisualImageHistogramPixelColors*)visualImageHistogramPixelColors;
	
	if (image->histogram != NULL) {
		VisualNotification aNotification;
		aNotification.setPointer(image);
		aNotification.setKey(kImageHistogramCompletedMsg);
		aNotification.post();
	}
	
	return retVal;
}
	
	
void VisualImage::getRGBHistogramInputPixels(std::vector<PixelColor>& inputValues) const {
	
	VisualImage* histogramImage = this->cloneIndependently();
	
	uint32 maxEdgeLength = 32;
	PixelRect pixelRect;
	if (this->getWidth() > this->getHeight()) {
		pixelRect.height = maxEdgeLength;
		pixelRect.width = (uint32)((double)pixelRect.height * ((double)this->getWidth() / (double)this->getHeight()));
	} else if (this->getWidth() < this->getHeight()) {
		pixelRect.width = maxEdgeLength;
		pixelRect.height = (uint32)((double)pixelRect.width * ((double)this->getHeight() / (double)this->getWidth()));
	} else {
		pixelRect.width = maxEdgeLength;
		pixelRect.height = maxEdgeLength;
	}
	
	histogramImage->resample(pixelRect);
	
	VisualTextureContainer* histogramTextureContainer = histogramImage->getTextureContainer();
	
	PixelColor* pixels = histogramTextureContainer->createARGBImagePixels();
	
	uint32 numberOfValues = histogramTextureContainer->getImageWidth() * histogramTextureContainer->getImageHeight();
	
	uint8 r, g, b, a;
	PixelColor posterizedPixelColor;
	for (uint32 i = 0; i < numberOfValues; i++) {
		VisualColorTools::convertARGBPixelToRGB(pixels[i]);
		VisualColorTools::getColorComponentValues(pixels[i], a, r, g, b);
		r = r & 0xfc;
		g = g & 0xfc;
		b = b & 0xfc;
		posterizedPixelColor = VisualColorTools::getPixelColor(a, r, g, b);
		inputValues.push_back(posterizedPixelColor);
	}
	free(pixels);
	
	delete histogramImage;
	
}


void VisualImage::copy(const VisualImage& other) {
	this->visualTextureContainer = new VisualTextureContainer(*(other.visualTextureContainer));
	this->isSet = other.isSet;
	this->blendMode = other.blendMode;
	if (other.histogram != NULL) {
		this->histogram = new VisualHistogram::PixelColorHistogram(*(other.histogram));
	} else {
		this->histogram = NULL;
	}
}
