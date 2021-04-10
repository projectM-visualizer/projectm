/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualConvolutionFilter.cpp
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

#include "VisualConvolutionFilter.h"
#include "VisualErrorHandling.h"



using namespace VizKit;


VisualConvolutionFilter::VisualConvolutionFilter(const Effect effect, const uint8 colCount, const uint8 rowCount) {
	
	numberOfKernelValueColumns = colCount;
	numberOfKernelValueRows = rowCount;
	
	postConvolutionScaleFactor = 1.0f;
	postConvolutionBias = 0.0f;
	
	edgeAction = kZeroEdges;
	
	// If the sum of all elements of the filter is 1, the resulting image has the same brightness as the original.
	
	switch (effect) {
			
		case kNone:
			break;
			
		case kBlur:
		{
			float count = (float)(colCount * rowCount);
			for (uint8 i = 0; i < (colCount * rowCount); i++) {
				//kernelValues.push_back(1.0f / count);
				kernelValues.push_back(1.0f);
			}
			this->postConvolutionScaleFactor = 1.0f / count;
		}
			break;
			
		case kMotionBlur:
			numberOfKernelValueColumns = 5;
			numberOfKernelValueRows = 5;
			for (uint8 i = 0; i < numberOfKernelValueColumns; i++) {
				for (uint8 k = 0; k < numberOfKernelValueRows; k++) {
					if (k == i) {
						kernelValues.push_back(1.0f / (float)numberOfKernelValueColumns);
					} else {
						kernelValues.push_back(0.0f);
					}
				}
			}
			break;
			
		case kGaussianBlur:
		{
			float count = 16.0f;
			kernelValues.push_back(1.0f / count);
			kernelValues.push_back(2.0f / count);
			kernelValues.push_back(1.0f / count);
			kernelValues.push_back(2.0f / count);
			kernelValues.push_back(4.0f / count);
			kernelValues.push_back(2.0f / count);
			kernelValues.push_back(1.0f / count);
			kernelValues.push_back(2.0f / count);
			kernelValues.push_back(1.0f / count);
		}
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


uint8 VisualConvolutionFilter::getNumberOfKernelValueRows() const {
	return this->numberOfKernelValueRows;
}


uint8 VisualConvolutionFilter::getNumberOfKernelValueColumns() const {
	return this->numberOfKernelValueColumns;
}


float VisualConvolutionFilter::getPostConvolutionScaleFactor() const {
	return this->postConvolutionScaleFactor;
}


float VisualConvolutionFilter::getPostConvolutionBias() const {
	return this->postConvolutionBias;
}


void VisualConvolutionFilter::copyKernelValues(float* buffer) const {
	uint16 i = 0;
	uint16 k = 0;
	uint16 currIdx = 0;
	for (i = 0; i < this->numberOfKernelValueColumns; i++) {
		for (k = 0; k < this->numberOfKernelValueRows; k++) {
			buffer[currIdx] = this->kernelValues[currIdx];
			currIdx++;
		}
	}
}


void VisualConvolutionFilter::applyToPixelData(const PixelColor* inPixelData, uint32 imageWidth, uint32 imageHeight, int pixelFormat, int pixelDataType, PixelColor** outPixelData, uint16 numberOfInterations) const {
	
	bool alpha = true;
	
	float* matrix = (float*)malloc(this->numberOfKernelValueRows * this->numberOfKernelValueColumns * sizeof(float));
	this->copyKernelValues(matrix);
	uint8 rows2 = this->numberOfKernelValueRows / 2;
	uint8 cols2 = this->numberOfKernelValueColumns / 2;
	
	uint32 pixelDataSizeInBytes = imageWidth * imageHeight * sizeof(PixelColor);
	
	uint32* filteredPixelData = (uint32*)calloc(pixelDataSizeInBytes, sizeof(char));
	
	if (numberOfInterations > 1) {
		memcpy(filteredPixelData, inPixelData, pixelDataSizeInBytes);
	}
	
	for (uint16 i = 0; i < numberOfInterations; i++) {
		
		uint32 index = 0;
		
		if (numberOfInterations > 1) {
			memcpy(*outPixelData, filteredPixelData, pixelDataSizeInBytes);
		}
		
		for (uint32 y = 0; y < imageHeight; y++) {
			
			for (uint32 x = 0; x < imageWidth; x++) {
				
				float r = 0;
				float g = 0;
				float b = 0;
				float a = 0;
				
				for (sint8 row = -rows2; row <= rows2; row++) {
					sint32 iy = y + row;
					sint32 ioffset;
					if (0 <= iy && iy < static_cast<sint32>(imageHeight))
						ioffset = iy * imageWidth;
					else if (this->edgeAction == kClampEdges)
						ioffset = y * imageWidth;
					else if (this->edgeAction == kWrapEdges)
						ioffset = ((iy + imageHeight) % imageHeight) * imageWidth;
					else
						continue;
					uint32 moffset = this->numberOfKernelValueColumns * (row + rows2) + cols2;
					for (sint8 col = -cols2; col <= cols2; col++) {
						float f = matrix[moffset + col];
						
						if (f != 0) {
							sint32 ix = x + col;
							if (!(0 <= ix && ix < static_cast<sint32>(imageWidth))) {
								if (this->edgeAction == kClampEdges)
									ix = x;
								else if (this->edgeAction == kWrapEdges)
									ix = (x + imageWidth) % imageWidth;
								else
									continue;
							}
							uint32 rgb = (inPixelData)[ioffset + ix];
							a += f * ((rgb >> 24) & 0xff);
							r += f * ((rgb >> 16) & 0xff);
							g += f * ((rgb >> 8) & 0xff);
							b += f * (rgb & 0xff);
						}
					}
				}
				
				if (alpha) {
					a *= this->postConvolutionScaleFactor + this->postConvolutionBias;
				}
				r *= this->postConvolutionScaleFactor + this->postConvolutionBias;
				g *= this->postConvolutionScaleFactor + this->postConvolutionBias;
				b *= this->postConvolutionScaleFactor + this->postConvolutionBias;
				
				uint32 ia = alpha ? this->clamp((uint32)(a + 0.5)) : 0xff;
				uint32 ir = this->clamp((uint32)(r + 0.5));
				uint32 ig = this->clamp((uint32)(g + 0.5));
				uint32 ib = this->clamp((uint32)(b + 0.5));
				
				filteredPixelData[index++] = (ia << 24) | (ir << 16) | (ig << 8) | ib;
			}
		}
		
	}
	
	free(matrix);
	
	*outPixelData = filteredPixelData;
	
}

uint32 VisualConvolutionFilter::clamp(uint32 inVal) const {
	if (inVal > 255) return 255;
	if (inVal < 0) return 0;
	return inVal;
}
