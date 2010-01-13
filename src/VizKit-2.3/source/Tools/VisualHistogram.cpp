/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualHistogram.cpp
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

#include "VisualHistogram.h"

#include "VisualErrorHandling.h"
#include "VisualColorTools.h"

#include <algorithm> // sort on vector


using namespace VizKit;


struct CompareInputValuesDescending {
	bool operator()(const PixelColor& x, const PixelColor& y) {
		if (x > y) {
			return true;
		} else {
			return false;
		}
	}
};


struct CompareHistogramValuesByWeightedLuminosityDescending {
	bool operator()(const VisualHistogram::PixelColorHistogramValue& x, const VisualHistogram::PixelColorHistogramValue& y) {
		uint8 x_a, x_r, x_g, x_b;
		uint8 y_a, y_r, y_g, y_b;
		VisualColorTools::getColorComponentValues(x.pixelColor, x_a, x_r, x_g, x_b);
		VisualColorTools::getColorComponentValues(y.pixelColor, y_a, y_r, y_g, y_b);
		double luminosity_x = (double)x_r * 0.3 + (double)x_g * 0.59 + x_b * 0.11;
		double luminosity_y = (double)y_r * 0.3 + (double)y_g * 0.59 + y_b * 0.11;
		if (luminosity_x > luminosity_y) {
			return true;
		} else {
			return false;
		}
	}
};


struct CompareHistogramValuesByRedComponentDescending {
	bool operator()(const VisualHistogram::PixelColorHistogramValue& x, const VisualHistogram::PixelColorHistogramValue& y) {
		uint8 x_a, x_r, x_g, x_b;
		uint8 y_a, y_r, y_g, y_b;
		VisualColorTools::getColorComponentValues(x.pixelColor, x_a, x_r, x_g, x_b);
		VisualColorTools::getColorComponentValues(y.pixelColor, y_a, y_r, y_g, y_b);
		if (x_r > y_r) {
			return true;
		} else {
			return false;
		}
	}
};


struct CompareHistogramValuesByGreenComponentDescending {
	bool operator()(const VisualHistogram::PixelColorHistogramValue& x, const VisualHistogram::PixelColorHistogramValue& y) {
		uint8 x_a, x_r, x_g, x_b;
		uint8 y_a, y_r, y_g, y_b;
		VisualColorTools::getColorComponentValues(x.pixelColor, x_a, x_r, x_g, x_b);
		VisualColorTools::getColorComponentValues(y.pixelColor, y_a, y_r, y_g, y_b);
		if (x_g > y_g) {
			return true;
		} else {
			return false;
		}
	}
};


struct CompareHistogramValuesByBlueComponentDescending {
	bool operator()(const VisualHistogram::PixelColorHistogramValue& x, const VisualHistogram::PixelColorHistogramValue& y) {
		uint8 x_a, x_r, x_g, x_b;
		uint8 y_a, y_r, y_g, y_b;
		VisualColorTools::getColorComponentValues(x.pixelColor, x_a, x_r, x_g, x_b);
		VisualColorTools::getColorComponentValues(y.pixelColor, y_a, y_r, y_g, y_b);
		if (x_b > y_b) {
			return true;
		} else {
			return false;
		}
	}
};


struct CompareHistogramValuesByHSLDescending {
	bool operator()(const VisualHistogram::PixelColorHistogramValue& x, const VisualHistogram::PixelColorHistogramValue& y) {
		uint8 x_a, x_r, x_g, x_b;
		uint8 y_a, y_r, y_g, y_b;
		double x_h, x_s, x_l;
		double y_h, y_s, y_l;
		VisualColorTools::getColorComponentValues(x.pixelColor, x_a, x_r, x_g, x_b);
		VisualColorTools::getColorComponentValues(y.pixelColor, y_a, y_r, y_g, y_b);
		VisualColorTools::RGBToHSL(((double)x_r / (double)255.0), (double)x_g / (double)255.0, (double)x_b / (double)255.0, &x_h, &x_s, &x_l);
		VisualColorTools::RGBToHSL(((double)y_r / (double)255.0), (double)y_g / (double)255.0, (double)y_b / (double)255.0, &y_h, &y_s, &y_l);
		if ((x_h + x_s + x_l) > (y_h + y_s + y_l)) {
			return true;
		} else {
			return false;
		}
	}
};


struct ComparePixelColorsByHSLDescending {
	bool operator()(const PixelColor& x, const PixelColor& y) {
		uint8 x_a, x_r, x_g, x_b;
		uint8 y_a, y_r, y_g, y_b;
		double x_h, x_s, x_l;
		double y_h, y_s, y_l;
		VisualColorTools::getColorComponentValues(x, x_a, x_r, x_g, x_b);
		VisualColorTools::getColorComponentValues(y, y_a, y_r, y_g, y_b);
		VisualColorTools::RGBToHSL(((double)x_r / (double)255.0), (double)x_g / (double)255.0, (double)x_b / (double)255.0, &x_h, &x_s, &x_l);
		VisualColorTools::RGBToHSL(((double)y_r / (double)255.0), (double)y_g / (double)255.0, (double)y_b / (double)255.0, &y_h, &y_s, &y_l);
		if ((x_h + x_s + x_l) > (y_h + y_s + y_l)) {
			return true;
		} else {
			return false;
		}
	}
};


struct CompareHistogramValuesByCountDescending {
	bool operator()(const VisualHistogram::PixelColorHistogramValue& x, const VisualHistogram::PixelColorHistogramValue& y) {		
		if (x.count > y.count) {
			return true;
		} else {
			if (x.count == y.count) {
				if (x.pixelColor > y.pixelColor) {
					return true;
				}
			}
			return false;
		}
	}
};


void VisualHistogram::initCentroids(const std::vector<PixelColor>& inputValues, const uint32 requestedNumberOfCentroids, std::vector<PixelColor>& centroids) {
	
	uint32 actualNumberOfCentroids = 0;
	
	if (inputValues.size() == 0) {
		return;
	}
	
	std::vector<PixelColor> distinctInputValues;
	const PixelColor* lastColorPixel = NULL;
	for (uint32 i = 0; i < inputValues.size(); i++) {
		if (lastColorPixel != NULL && (*lastColorPixel != inputValues[i])) {
			distinctInputValues.push_back(inputValues[i]);
			lastColorPixel = &inputValues[i];
		}
		if (lastColorPixel == NULL) {
			distinctInputValues.push_back(inputValues[i]);
			lastColorPixel = &inputValues[i];
		}
	}
	
	uint32 blockSize = 0;
	if (distinctInputValues.size() < requestedNumberOfCentroids) {
		actualNumberOfCentroids = distinctInputValues.size();
	} else {
		actualNumberOfCentroids = requestedNumberOfCentroids;
	}

	blockSize = distinctInputValues.size() / actualNumberOfCentroids;
	
	uint32 index = 0;
	for (uint32 i = 0; i < actualNumberOfCentroids; i++) {
		index = blockSize * i;
		centroids.push_back(distinctInputValues[index]);
	}

}


VisualHistogram::PixelColorHistogram VisualHistogram::createHistogramOfRGBPixels(std::vector<PixelColor>& inputValues) {
	
	PixelColorHistogram histogram;
	
	const uint16 maxNumberOfIterations = 40; // maximum number of iterations
	
	//std::sort(inputValues.begin(), inputValues.end(), CompareInputValuesDescending());
	std::sort(inputValues.begin(), inputValues.end(), ComparePixelColorsByHSLDescending());
	
	std::vector<PixelColor> centroids;
	uint32 numberOfCentroids = 128;
	VisualHistogram::initCentroids(inputValues, numberOfCentroids, centroids);
	
	uint32 numberOfInputPixels = inputValues.size();
	
	const uint16 numberOfClusters = centroids.size();
	
	// initialize histogram
	for (uint32 i = 0; i < numberOfClusters; i++) {
		PixelColorHistogramValue centroidColor;
		centroidColor.pixelColor = centroids[i];
		centroidColor.count = 0;
		centroidColor.percent = 0;
		histogram.push_back(centroidColor);
	}
	
	// initialize clusterMembership
	std::vector<uint32> clusterMembership;
	for (uint32 i = 0; i < numberOfInputPixels; i++) {
		clusterMembership.push_back(0);
	}
	
	bool stop = false;
	uint32 count = 0;
	
	while (stop == false) {
		
		stop = true;
		
		// (re)assign cluster membership of input values
		for (uint32 i = 0; i < numberOfInputPixels; i++) {
			uint32 nearestCentroidIndex = clusterMembership[i];
			for (uint32 k = 0; k < numberOfClusters; k++) {
				if (VisualColorTools::getDistance(inputValues[i], centroids[k]) < VisualColorTools::getDistance(inputValues[i], centroids[nearestCentroidIndex])) {
					nearestCentroidIndex = k;
				}
			}
			if (nearestCentroidIndex != clusterMembership[i]) {
				stop = false;
				clusterMembership[i] = nearestCentroidIndex;
			}
		}
		
		// reset histogram
		for (uint32 i = 0; i < numberOfClusters; i++) {
			histogram[i].count = 0;
			histogram[i].percent = 0.0;
			histogram[i].pixelColor = 0x00000000;
		}
		
		// recalc average color values of clusters
		for (uint32 i = 0; i < clusterMembership.size(); i++) {
			uint32 currHistogramIndex = clusterMembership[i];
			histogram[currHistogramIndex].count++;
			histogram[currHistogramIndex].percent = (double)histogram[currHistogramIndex].count / (double)numberOfInputPixels * 100.0;
			if (histogram[currHistogramIndex].count > 1) {
				histogram[currHistogramIndex].pixelColor = VisualColorTools::getMeanRGBPixelColor(histogram[currHistogramIndex].pixelColor, inputValues[i], histogram[currHistogramIndex].count);
			} else {
				histogram[currHistogramIndex].pixelColor = inputValues[i];
			}
		}
		
		// reassign centroids
		for (uint32 i = 0; i < numberOfClusters; i++) {
			centroids[i] = histogram[i].pixelColor;
		}
		
		count++;
		
		if (count == maxNumberOfIterations) {
			stop = true;
		}
		
	}
	
	VisualHistogram::PixelColorHistogram histogramOut = thresholdPass(histogram);
	
	std::sort(histogramOut.begin(), histogramOut.end(), CompareHistogramValuesByCountDescending());
	
	return histogramOut;
}


VisualHistogram::PixelColorHistogram VisualHistogram::thresholdPass(PixelColorHistogram& inputValues) {
	
	PixelColorHistogram histogram;
	
	if (inputValues.size() < 1) return histogram;
	
	std::sort(inputValues.begin(), inputValues.end(), CompareHistogramValuesByRedComponentDescending());
	
	uint8 minColorComponentDelta = 20;
	uint8 otherColorComponentTolerance = 12;
	
	uint8 a, r, g, b;
	uint8 prev_a, prev_r, prev_g, prev_b;
	PixelColor avgColor;
	uint32 histogramIdx = 0;
	histogram.push_back(inputValues[histogramIdx]);
	VisualColorTools::getColorComponentValues(inputValues[histogramIdx].pixelColor, prev_a, prev_r, prev_g, prev_b);
	histogramIdx++;
	for (uint32 i = 1; i < inputValues.size(); i++) {
		VisualColorTools::getColorComponentValues(inputValues[i].pixelColor, a, r, g, b);
		if (abs(r - prev_r) < minColorComponentDelta && abs(g - prev_g) < otherColorComponentTolerance && abs(b - prev_b) < otherColorComponentTolerance) {
			avgColor = VisualColorTools::getMeanRGBPixelColor(inputValues[i].pixelColor, histogram[histogramIdx - 1].pixelColor, histogram[histogramIdx - 1].count);
			histogram[histogramIdx - 1].pixelColor = avgColor;
			histogram[histogramIdx - 1].count += inputValues[i].count;
			histogram[histogramIdx - 1].percent += inputValues[i].percent;
		} else {
			histogram.push_back(inputValues[i]);
			histogramIdx++;
		}
		prev_a = a;
		prev_r = r;
		prev_g = g;
		prev_b = b;
	}
	
	inputValues = histogram;
	histogram.clear();
	histogramIdx = 0;
	
	std::sort(inputValues.begin(), inputValues.end(), CompareHistogramValuesByGreenComponentDescending());
	
	histogram.push_back(inputValues[histogramIdx]);
	VisualColorTools::getColorComponentValues(inputValues[histogramIdx].pixelColor, prev_a, prev_r, prev_g, prev_b);
	histogramIdx++;
	for (uint32 i = 1; i < inputValues.size(); i++) {
		VisualColorTools::getColorComponentValues(inputValues[i].pixelColor, a, r, g, b);
		if (abs(g - prev_g) < minColorComponentDelta && abs(r - prev_r) < otherColorComponentTolerance && abs(b - prev_b) < otherColorComponentTolerance) {
			avgColor = VisualColorTools::getMeanRGBPixelColor(inputValues[i].pixelColor, histogram[histogramIdx - 1].pixelColor, histogram[histogramIdx - 1].count);
			histogram[histogramIdx - 1].pixelColor = avgColor;
			histogram[histogramIdx - 1].count += inputValues[i].count;
			histogram[histogramIdx - 1].percent += inputValues[i].percent;
		} else {
			histogram.push_back(inputValues[i]);
			histogramIdx++;
		}
		prev_a = a;
		prev_r = r;
		prev_g = g;
		prev_b = b;
	}
	
	inputValues = histogram;
	histogram.clear();
	histogramIdx = 0;
	
	std::sort(inputValues.begin(), inputValues.end(), CompareHistogramValuesByBlueComponentDescending());
	
	histogram.push_back(inputValues[histogramIdx]);
	VisualColorTools::getColorComponentValues(inputValues[histogramIdx].pixelColor, prev_a, prev_r, prev_g, prev_b);
	histogramIdx++;
	for (uint32 i = 1; i < inputValues.size(); i++) {
		VisualColorTools::getColorComponentValues(inputValues[i].pixelColor, a, r, g, b);
		if (abs(b - prev_b) < minColorComponentDelta && abs(r - prev_r) < otherColorComponentTolerance && abs(g - prev_g) < otherColorComponentTolerance) {
			avgColor = VisualColorTools::getMeanRGBPixelColor(inputValues[i].pixelColor, histogram[histogramIdx].pixelColor, histogram[histogramIdx - 1].count);
			histogram[histogramIdx - 1].pixelColor = avgColor;
			histogram[histogramIdx - 1].count += inputValues[i].count;
			histogram[histogramIdx - 1].percent += inputValues[i].percent;
		} else {
			histogram.push_back(inputValues[i]);
			histogramIdx++;
		}
		prev_a = a;
		prev_r = r;
		prev_g = g;
		prev_b = b;
	}
	
	return histogram;
	
}

