/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualColorTools.h
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

#ifndef VisualColorTools_h
#define VisualColorTools_h

#include "VisualTypes.h"
#include "VisualGraphicTypes.h"


namespace VizKit {

	/**
	 * Various utility functions dealing with pixel color values.
	 */
	class VisualColorTools {

	public:

		/**
		 * Converts from VertexColor to PixelColor.
		 * @param vertexColor The vertexColor to convert.
		 * @return Returns the pixel color value of the vertex color.
		 */
		static PixelColor getPixelColor(const VertexColor& vertexColor);
		
		/**
		 * Converts from PixelColor to VertexColor.
		 * @param pixelColor The pixelColor to convert.
		 * @return Returns the vertex color value of the pixel color.
		 */
		static VertexColor getVertexColor(const PixelColor& pixelColor);
		
		/**
		 * Returns the alpha, red, green, and blue color channel values.
		 * @param pixelColor The pixelColor to analyze.
		 * @param[out] alpha The 8-bit value of the alpha component.
		 * @param[out] red The 8-bit value of the red component.
		 * @param[out] green The 8-bit value of the green component.
		 * @param[out] blue The 8-bit value of the blue component.
		 */
		static void getColorComponentValues(const PixelColor& pixelColor, uint8& alpha, uint8& red, uint8& green, uint8& blue);
		
		/**
		 * Returns the Pixel color value for given alpha, red, green, and blue color channel values.
		 * @param[out] alpha The 8-bit value of the alpha component.
		 * @param[out] red The 8-bit value of the red component.
		 * @param[out] green The 8-bit value of the green component.
		 * @param[out] blue The 8-bit value of the blue component.
		 * @return The pixel color.
		 */
		static PixelColor getPixelColor(const uint8& alpha, const uint8& red, const uint8& green, const uint8& blue);
		
		/**
		 * Calculates the average pixel color for a number of pixel colors.
		 * @param colors Array of pixel colors.
		 * @param count The number of pixel colors to summarize.
		 * @param[out] avgPixelColor The average pixel color.
		 */	
		static void getMeanPixelColor(const PixelColor* colors, uint32 count, PixelColor& avgPixelColor);
		
		/**
		 * Returns the weighted average value.
		 * @param colorX The existing color value.
		 * @param colorY The new incoming color value.
		 * @param count The number of existing color values. The new incoming color value (colorY) is weighted with 1 while the existing color value (colorX) is weighted with (count - 1).
		 */
		static PixelColor getMeanRGBPixelColor(const PixelColor& colorX, const PixelColor& colorY, const uint32& count);

		/**
		 * Converts a color from RGB to HSL.
		 * @param r Red color component value.
		 * @param g Green color component value.
		 * @param b Blue color component value.
		 * @param[out] h Hue color value.
		 * @param[out] s Saturation color value.
		 * @param[out] l Lightness color value.
		 */
		static void RGBToHSL(const double& r, const double& g, const double& b, double* h, double* s, double* l);

		/**
		 * Converts a color from HSL to RGB.
		 * @param h Hue color value.
		 * @param s Saturation color value.
		 * @param l Lightness color value.
		 * @param[out] r Red color component value.
		 * @param[out] g Green color component value.
		 * @param[out] b Blue color component value.
		 */
		static void HSLToRGB(const double& h, const double& s, const double& l, double* r, double* g, double* b);

		/**
		 * Converts a color from RGB to HSV.
		 * @param r Red color component value.
		 * @param g Green color component value.
		 * @param b Blue color component value.
		 * @param[out] h Hue color value.
		 * @param[out] s Saturation color value.
		 * @param[out] v Value color value.
		 */
		static void RGBToHSV(const double& r, const double& g, const double& b, double* h, double* s, double* v);

		/**
		 * Converts a color from HSV to RGB.
		 * @param h Hue color value.
		 * @param s Saturation color value.
		 * @param v Value color value.
		 * @param[out] r Red color component value.
		 * @param[out] g Green color component value.
		 * @param[out] b Blue color component value.
		 */
		static void HSVToRGB(const double& h, const double& s, const double& v, double* r, double* g, double* b);
		
		/**
		 * Performs a pixel conversion in-place. E.g. ARGB pixel data is converted to BGRA.
		 * @param pixels The buffer of 32bpp pixel data.
		 * @param numberOfPixels The number of pixels.
		 */
		static void convertInterleavedPixels1234To4321(PixelColor* pixels, uint32 numberOfPixels);
		
		/**
		 * Performs a pixel conversion in-place. E.g. BGRA pixel data is converted to RGBA.
		 * @param pixels The buffer of 32bpp pixel data.
		 * @param numberOfPixels The number of pixels.
		 */
		static void convertInterleavedPixels1234To3214(PixelColor* pixels, uint32 numberOfPixels);
		
		/**
		 * Performs a pixel conversion in-place. ARGB is converted to RGB with alpha value being premultiplied.
		 * @param[in,out] argbPixel An ARGB pixel (on return the ARGB pixel is converted to RGB with premultiplied alpha).
		 */
		static void convertARGBPixelToRGB(PixelColor& argbPixel);
		
		/**
		 * Returns the distance between two RGB pixel values.
		 * @param colorX One color.
		 * @param colorY The other color.
		 * @return The distance between two RGB pixel values.
		 */
		static double getDistance(const PixelColor& colorX, const PixelColor& colorY);

		/**
		 * Creates an image buffer with 8-bit per color/alpha channel ARGB pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @param checkColor The color of the check squares of the sample image.
		 * @return A pointer to allocated pixel data.
		 * @remarks The caller has to deallocate the pixel buffer by calling free().
		 */
		static PixelColor* createARGBCheckPixels(uint32 width, uint32 height, const VertexColor& checkColor = red);
		
		/**
		 * Creates an image buffer with 8-bit per color/alpha channel BGRA pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @param checkColor The color of the check squares of the sample image.
		 * @return A pointer to allocated pixel data.
		 * @remarks The caller has to deallocate the pixel buffer by calling free().
		 */
		static PixelColor* createBGRACheckPixels(uint32 width, uint32 height, const VertexColor& checkColor = red);
		
		/**
		 * Creates an image buffer with 8-bit per color/alpha channel RGBA pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @param checkColor The color of the check squares of the sample image.
		 * @return A pointer to allocated pixel data.
		 * @remarks The caller has to deallocate the pixel buffer by calling free().
		 */
		static PixelColor* createRGBACheckPixels(uint32 width, uint32 height, const VertexColor& checkColor = red);
		
		/**
		 * Creates an image buffer with 8-bit per color/alpha channel ABGR pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @param checkColor The color of the check squares of the sample image.
		 * @return A pointer to allocated pixel data.
		 * @remarks The caller has to deallocate the pixel buffer by calling free().
		 */
		static PixelColor* createABGRCheckPixels(uint32 width, uint32 height, const VertexColor& checkColor = red);
		
	private:

		/** The constructor. VisualColorTools is a collection of static methods. Class does not need any instances. Constructor is private and not implemented. */
		VisualColorTools();

		/** The destructor. VisualColorTools is a collection of static methods. Class does not need any instances. Destructor is private and not implemented. */
		~VisualColorTools();

		/**
		 * Returns the minimum value of three values.
		 * @param val1 First value.
		 * @param val2 Second value.
		 * @param val3 Third value.
		 * @return The minimum value of the three values.
		 */
		static double min3(const double& val1, const double& val2, const double& val3);

		/**
		 * Returns the maximum value of three values.
		 * @param val1 First value.
		 * @param val2 Second value.
		 * @param val3 Third value.
		 * @return The maximum value of the three values.
		 */
		static double max3(const double& val1, const double& val2, const double& val3);
		
		/**
		 * Helper function to match hue to a red, green, or blue value.
		 * @param v1 First value.
		 * @param v2 Second value.
		 * @param hue Hue value.
		 * @return Color component value (red, green, or blue).
		 */
		static double hueToRGB(const double& v1, const double& v2, const double& hue);
		
		/**
		 * Internal method to create an image buffer with 8-bit per color/alpha channel pixel data in given format (ARGB, BGRA, RGBA, ABGR).
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @param format The format of the pixel data (e.g. ARGB).
		 * @param checkColor The color of the check squares of the sample image.
		 * @return A pointer to allocated pixel data.
		 */
		static PixelColor* createCheckPixels(uint32 width, uint32 height, const char* const format, const VertexColor& checkColor);
		
	};

}

#endif /* VisualColorTools_h */
