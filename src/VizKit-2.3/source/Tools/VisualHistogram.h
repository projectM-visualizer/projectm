/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualHistogram.h
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

#ifndef VisualHistogram_h
#define VisualHistogram_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"

#include <vector>


namespace VizKit {

	/**
	 * Creates histogram data.
	 */
	class VisualHistogram {
	
	public:

		/**
		 * Data value of a histogram of PixelColor values.
		 */
		typedef struct {
			PixelColor pixelColor; /**< The RGB pixelcolor value. */
			uint32 count; /**< The number of times the value has been counted (absolute number). */
			double percent; /**< The number of times the value has been counted (expressed as percent value). */
		} PixelColorHistogramValue;
		
		/** Vector of histogram values. */
		typedef std::vector<PixelColorHistogramValue> PixelColorHistogram;
		
		/** The PixelColorHistogramIterator is an iterator of the PixelColorHistogram. */
		typedef PixelColorHistogram::iterator PixelColorHistogramIterator; 

		/**
		 * Creates a histogram of ARGB PixelColor values.
		 * @param inputValues Array of PixelColor values.
		 * @remarks The inputValues are sorted inside this function. The histogram values are calculated with the k-means algorithm. Alpha value of PixelColor is not considered in this function (alpha has to be premultiplied before calling this function).
		 */
		static PixelColorHistogram createHistogramOfRGBPixels(std::vector<PixelColor>& inputValues);

	private:

		/** The constructor. VisualHistogram is a collection of static methods. Class does not need any instances. Constructor is private and not implemented. */
		VisualHistogram();

		/** The destructor. VisualHistogram is a collection of static methods. Class does not need any instances. Destructor is private and not implemented. */
		~VisualHistogram();

		/**
		 * Initializes the centroids of the histogram.
		 * @param inputValues The input values.
		 * @param requestedNumberOfCentroids The requested number of centroids.
		 * @param[out] centroids The centroid values. The number of centroids can be lower than the requested number of centroids.
		 */
		static void initCentroids(const std::vector<PixelColor>& inputValues, const uint32 requestedNumberOfCentroids, std::vector<PixelColor>& centroids);

		/**
		 * Normalizes PixelColor values by merging similar ones.
		 * @param inputValues The histogram input values.
		 * @return Normalized histogram.
		 */
		static VisualHistogram::PixelColorHistogram thresholdPass(PixelColorHistogram& inputValues);

	};
	
}


#endif /* VisualHistogram_h */
