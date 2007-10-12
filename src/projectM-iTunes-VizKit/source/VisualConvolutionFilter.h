/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualConvolutionFilter.h
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

#ifndef VisualConvolutionFilter_h
#define VisualConvolutionFilter_h

#include "VisualGraphicTypes.h"

#include <vector>

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	/**
	 * Convolution filter for bitmap image effects.\ A.k.a.\ kernel filter or matrix.\ A.k.a.\ correlation, impulse response, or point spread function.
	 */
	class VisualConvolutionFilter {
	
	public:

		/** Constants for various convolution filters. */
		typedef enum {
			kNone = 0,
			kBlur, /** < Blur filter (lowpass filter).\ Eliminates changes that are only a pixel wide.\ Rapid change from pixel to pixel is blocked while slow change is passed. */
			kMotionBlur, /** < Motion blur filter. */
			kGaussianBlur, /** < Gaussian blur filter. */
			kLaplacian, /** < Laplacian filter. */
			kEdgeDetect, /** < Filter for edge detection. */
			kSharpen, /** < Sharpens image (highpass filter).\ Rapid change from pixel to pixel is passed while slow change is blocked. */
			kEmboss /** < Emboss filter that gives the image a relief look (useful when bumpmapping). */
		} Effect;

		/**
		 * The constructor.
		 * @param effect The name of the convolution filter.
		 * @param colCount The number of columns of the kernel matrix.\ Default = 3.
		 * @param rowCount The number of rows of the kernel matrix.\ Default = 3.
		 * @remarks The size of the filter has to be uneven, so that it has a center (e.g.\ 3x3, 5x5, or 7x7).
		 */
		VisualConvolutionFilter(const Effect effect, const UInt8 colCount = 3, const UInt8 rowCount = 3);
		
		/**
		 * The destructor.
		 */
		~VisualConvolutionFilter();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualConvolutionFilter.
		 */
		VisualConvolutionFilter(const VisualConvolutionFilter& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualConvolutionFilter.
		 */
		VisualConvolutionFilter& operator=(const VisualConvolutionFilter& other);

		/**
		 * Returns the number of kernel value columns.
		 * @return The number of kernel value columns.
		 */
		UInt8 getNumberOfKernelValueColumns(void) const;

		/**
		 * Returns the number of kernel value rows.
		 * @return The number of kernel value rows.
		 */
		UInt8 getNumberOfKernelValueRows(void) const;

		/**
		 * Returns the post convolution scale factor.
		 * @return The post convolution scale factor.
		 * @remarks The scale factor is the factor with which each pixel is multiplied.
		 */
		float getPostConvolutionScaleFactor(void) const;

		/**
		 * Returns the post convolution bias.
		 * @return The post convolution bias.
		 * @remarks The bias is a coefficient that is added to each pixel value.
		 */
		float getPostConvolutionBias(void) const;

		/**
		 * Copies the kernel values of the convolution filter into the provided buffer.
		 * @param[out] buffer The buffer into which the kernel values are written.
		 * @remarks The caller has to make sure that the provided buffer is big enough to store columns * rows * sizeof(float) values.
		 */
		void copyKernelValues(float* buffer) const;

		/**
		 * Applies the convolution filter to the pixel data.
		 * @param pixelData The pixel data on which the convolution filter is about to be applied.\ The pixel data must be provided in RGBA format with 8 bits per color channel.
		 * @param imageWidth The width of the image.
		 * @param imageHeight The height of the image.
		 * @param pixelFormat The format of the pixel data (rgba, luminance, etc.).
		 * @param pixelDataType The data type of the pixel data (8-bit byte, float, etc.).
		 * @param[out] filteredPixelData The filtered pixel data.\ The filtered pixel data is of the same size as the input pixel data.\ The caller has to free the allocated memory of filteredPixelData.
		 */
		void applyToPixelData(const unsigned char* const pixelData, UInt32 imageWidth, UInt32 imageHeight, int pixelFormat, int pixelDataType, unsigned char** filteredPixelData) const;

	private:

		/** Kernel values of convolution filters are stored in vectors of floats. */
		typedef std::vector<float> KernelValues;

		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualConvolutionFilter.
		 */
		void copy(const VisualConvolutionFilter& other);
		
		/** Kernel values. */
		KernelValues kernelValues;

		/** The number of columns of the kernel values. */
		UInt8 numberOfKernelValueColumns;

		/** The number of rows of the kernel values. */
		UInt8 numberOfKernelValueRows;
		
		/** The scaling of values applied after the convolution. */
		float postConvolutionScaleFactor;
		
		/** The bias of values applied after the convolution. */
		float postConvolutionBias;

	};

}

#endif /* VisualConvolutionFilter_h */
