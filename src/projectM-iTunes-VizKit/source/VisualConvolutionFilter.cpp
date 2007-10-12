/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualConvolutionFilter.cpp
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

#include "VisualConvolutionFilter.h"
#include "VisualErrorHandling.h"



using namespace VizKit;


VisualConvolutionFilter::VisualConvolutionFilter(const Effect effect, const UInt8 colCount, const UInt8 rowCount) {

	numberOfKernelValueColumns = colCount;
	numberOfKernelValueRows = rowCount;
	
	postConvolutionScaleFactor = 1.0f;
	postConvolutionBias = 0.0f;
	
	/* If the sum of all elements of the filter is 1, the resulting image has the same brightness as the original. */
	
	switch (effect) {
	
		case kNone:
			break;

		case kBlur:
			for (UInt8 i = 0; i < (colCount * rowCount); i++) {
				kernelValues.push_back(1.0f);
			}
			
			postConvolutionScaleFactor = 1.0f / (float)(colCount * rowCount);
			break;

		case kMotionBlur:
			numberOfKernelValueColumns = 5;
			numberOfKernelValueRows = 5;
			for (UInt8 i = 0; i < numberOfKernelValueColumns; i++) {
				for (UInt8 k = 0; k < numberOfKernelValueRows; k++) {
					if (k == i) {
						kernelValues.push_back(1.0f);
					} else {
						kernelValues.push_back(0.0f);
					}
				}
			}			
			postConvolutionScaleFactor = 1.0f / (float)numberOfKernelValueColumns;
			break;

		case kGaussianBlur:
			kernelValues.push_back(1.0f);
			kernelValues.push_back(2.0f);
			kernelValues.push_back(1.0f);
			kernelValues.push_back(2.0f);
			kernelValues.push_back(4.0f);
			kernelValues.push_back(2.0f);
			kernelValues.push_back(1.0f);
			kernelValues.push_back(2.0f);
			kernelValues.push_back(1.0f);
			postConvolutionScaleFactor = 1.0f / (float)16.0;
			break;

		case kLaplacian:
			kernelValues.push_back(-0.125f);
			kernelValues.push_back(-0.125f);
			kernelValues.push_back(-0.125f);
			kernelValues.push_back(-0.125f);
			kernelValues.push_back(1.0f);
			kernelValues.push_back(-0.125f);
			kernelValues.push_back(-0.125f);
			kernelValues.push_back(-0.125f);
			kernelValues.push_back(-0.125f);
			break;

		case kEdgeDetect:
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(8.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			break;
			
		case kSharpen:
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(9.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			break;

		case kEmboss:
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(0.0f);
			kernelValues.push_back(-1.0f);
			kernelValues.push_back(0.0f);
			kernelValues.push_back(1.0f);
			kernelValues.push_back(0.0f);
			kernelValues.push_back(1.0f);
			kernelValues.push_back(1.0f);
			postConvolutionBias = 128.0f;
			break;

		default:
			char errStr[256];
			sprintf(errStr, "unknown switch case (%d) in file: %s (line: %d) [%s])", effect, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errStr);
	}
}


VisualConvolutionFilter::~VisualConvolutionFilter() {
	kernelValues.clear();
}


VisualConvolutionFilter::VisualConvolutionFilter(const VisualConvolutionFilter& other) {
	copy(other);
}


VisualConvolutionFilter& VisualConvolutionFilter::operator=(const VisualConvolutionFilter& other) {
	if (this != &other) {
		this->copy(other);
	}
	return *this;
}


void VisualConvolutionFilter::copy(const VisualConvolutionFilter& other) {
	this->kernelValues = other.kernelValues;
	this->numberOfKernelValueColumns = other.numberOfKernelValueColumns;
	this->numberOfKernelValueRows = other.numberOfKernelValueRows;
}


UInt8 VisualConvolutionFilter::getNumberOfKernelValueRows() const {
	return this->numberOfKernelValueRows;
}


UInt8 VisualConvolutionFilter::getNumberOfKernelValueColumns() const {
	return this->numberOfKernelValueColumns;
}


float VisualConvolutionFilter::getPostConvolutionScaleFactor() const {
	return this->postConvolutionScaleFactor;
}


float VisualConvolutionFilter::getPostConvolutionBias() const {
	return this->postConvolutionBias;
}


void VisualConvolutionFilter::copyKernelValues(float* buffer) const {
	UInt16 i = 0;
	UInt16 k = 0;
	UInt16 currIdx = 0;
	for (i = 0; i < this->numberOfKernelValueColumns; i++) {
		for (k = 0; k < this->numberOfKernelValueRows; k++) {
			buffer[currIdx] = this->kernelValues[currIdx];
			currIdx++;
		}
	}
}


void VisualConvolutionFilter::applyToPixelData(const unsigned char* const pixelData, UInt32 imageWidth, UInt32 imageHeight, int pixelFormat, int pixelDataType, unsigned char** filteredPixelData) const {

	bool debug = false;
	UInt32 imageX;
	UInt32 imageY;
	SInt8 kernelX;
	SInt8 kernelY;
	
	char errStr[256];
	
	float sum;
	UInt32 kernelPos;
	float grayscale;

	UInt8 numberOfBytesPerChannel = 0;
	UInt8 numberOfChannels = 0; // channel == color resp. alpha channel
	UInt8 numberOfBytesPerPixel = 0;
	//UInt32 numberOfBytesPerRow = 0;
	if (pixelFormat == kGL_RGBA || pixelFormat == kGL_BGRA) {
		numberOfChannels = 4;
	} else if (pixelFormat == kGL_LUMINANCE_WITH_ALPHA) {
		numberOfChannels = 2;
	}

	if ((pixelDataType == kGL_UNSIGNED_INT_8_8_8_8_REV) || (pixelDataType == kGL_UNSIGNED_INT_8_8_8_8) || (pixelDataType == kGL_UNSIGNED_BYTE)) {
		numberOfBytesPerChannel = 1; // // 1 byte (== 8 bits) per color/channel
	} else {
		sprintf(errStr, "unknown type %d in file: %s (line: %d) [%s])", pixelDataType, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return;
	}
	
	numberOfBytesPerPixel = numberOfBytesPerChannel * numberOfChannels;

/*
	if ((pixelDataType == kGL_UNSIGNED_INT_8_8_8_8_REV) || (pixelDataType == kGL_UNSIGNED_INT_8_8_8_8) || (pixelDataType == kGL_UNSIGNED_BYTE)) {
		*filteredPixelData = (unsigned char*)calloc(imageWidth * imageHeight, numberOfBytesPerPixel);
	} else if (pixelDataType == kGL_FLOAT) {
		*filteredPixelData = (float*)calloc(imageWidth * imageHeight * numberOfChannels, numberOfBytesPerPixel);
	}
*/
	if (pixelFormat == kGL_RGBA || pixelFormat == kGL_BGRA) {
		//*filteredPixelData = (unsigned char*)calloc(imageWidth * imageHeight, numberOfBytesPerPixel);
	}

	if (debug == true) {
		memcpy(*filteredPixelData, pixelData, imageWidth * imageHeight * numberOfChannels);
		return;
	}

	for (imageY = (this->numberOfKernelValueRows / 2) * numberOfChannels; imageY < (imageHeight * numberOfChannels) - ((this->numberOfKernelValueRows / 2) * numberOfChannels); imageY++) {
		for (imageX = (this->numberOfKernelValueColumns / 2) * numberOfChannels; imageX < (imageWidth * numberOfChannels) - ((this->numberOfKernelValueColumns / 2) * numberOfChannels); imageX++) {
			
			sum = 0.0f;
			kernelPos = 0;
			grayscale = 0.0f;
			for (kernelY = -1 * (this->numberOfKernelValueRows / 2); kernelY <= (this->numberOfKernelValueRows / 2); kernelY++) {
				for (kernelX = -1 * (this->numberOfKernelValueColumns / 2); kernelX <= (this->numberOfKernelValueColumns / 2); kernelX++) {
					SInt8 jColorShift = kernelY * numberOfChannels;
					SInt8 kColorShift = kernelX * numberOfChannels;					
					
					if ((pixelDataType == kGL_UNSIGNED_INT_8_8_8_8_REV) || (pixelDataType == kGL_UNSIGNED_INT_8_8_8_8) || (pixelDataType == kGL_UNSIGNED_BYTE)) {
						grayscale = ((unsigned char*)pixelData)[(imageY + jColorShift) * imageWidth + (imageX + kColorShift)];
					} else if (pixelDataType == kGL_FLOAT) {
						grayscale = ((float*)pixelData)[(imageY + jColorShift) * imageWidth + (imageX + kColorShift)];
					}

					sum = sum + (this->kernelValues[kernelPos] * grayscale);
					kernelPos++;
				}
			}

			if ((pixelDataType == kGL_UNSIGNED_INT_8_8_8_8_REV) || (pixelDataType == kGL_UNSIGNED_INT_8_8_8_8) || (pixelDataType == kGL_UNSIGNED_BYTE)) {
				((unsigned char*)*filteredPixelData)[(imageY * imageWidth) + imageX] = (unsigned char)(sum * this->postConvolutionScaleFactor + this->postConvolutionBias);
			} else if (pixelDataType == kGL_FLOAT) {
				((float*)*filteredPixelData)[(imageY * imageWidth) + imageX] = sum * this->postConvolutionScaleFactor + this->postConvolutionBias;
			}

		}
	}

}
