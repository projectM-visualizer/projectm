/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualColorTools.cpp
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

#include "VisualColorTools.h"
#include "VisualErrorHandling.h"

#include <math.h> // sqrt

using namespace VizKit;


double VisualColorTools::min3(const double& val1, const double& val2, const double& val3) {
	if (val1 < val2) {
		if (val1 < val3) {
			return val1;
		} else {
			return val3;
		}
	} else if (val2 < val3) {
		return val2;
	}
	return val3;
}


double VisualColorTools::max3(const double& val1, const double& val2, const double& val3) {
	if (val1 > val2) {
		if (val1 > val3) {
			return val1;
		} else {
			return val3;
		}
	} else if (val2 > val3) {
		return val2;
	}
	return val3;
}


double VisualColorTools::hueToRGB(const double& v1, const double& v2, const double& hue) {
	double vH = hue;
	if (vH < 0) vH += 1;
	if (vH > 1) vH -= 1;
	if ((6 * vH) < 1) return (v1 + (v2 - v1) * 6 * vH);
	if ((2 * vH) < 1) return v2;
	if ((3 * vH) < 2) return (v1 + (v2 - v1) * ((2.0 / 3.0 ) - vH) * 6);
	return v1;
}


PixelColor VisualColorTools::getPixelColor(const VertexColor& vertexColor) {
	uint8 a = static_cast<uint8>(vertexColor.a * 255.0);
	uint8 r = static_cast<uint8>(vertexColor.r * 255.0);
	uint8 g = static_cast<uint8>(vertexColor.g * 255.0);
	uint8 b = static_cast<uint8>(vertexColor.b * 255.0);
	return VisualColorTools::getPixelColor(a, r, g, b);
}


VertexColor VisualColorTools::getVertexColor(const PixelColor& pixelColor) {
	VertexColor vertexColor;
	uint8 a;
	uint8 r;
	uint8 g;
	uint8 b;
	VisualColorTools::getColorComponentValues(pixelColor, a, r, g, b);
	vertexColor.alpha = (double)a / 255.0;
	vertexColor.red = (double)r / 255.0;
	vertexColor.green = (double)g / 255.0;
	vertexColor.blue = (double)b / 255.0;
	vertexColor.r = vertexColor.red;
	vertexColor.g = vertexColor.green;
	vertexColor.b = vertexColor.blue;
	vertexColor.a = vertexColor.alpha;
	return vertexColor;
}


void VisualColorTools::getColorComponentValues(const PixelColor& pixelColor, uint8& alpha, uint8& red, uint8& green, uint8& blue) {
	alpha = ((pixelColor >> 24) & 0xff);
	red = ((pixelColor >> 16) & 0xff);
	green = ((pixelColor >> 8) & 0xff);
	blue = pixelColor & 0xff;
}


PixelColor VisualColorTools::getPixelColor(const uint8& alpha, const uint8& red, const uint8& green, const uint8& blue) {
	PixelColor pixelColor = (alpha << 24) | (red << 16) | (green << 8) | blue;
	return pixelColor;
}


void VisualColorTools::getMeanPixelColor(const PixelColor* colors, uint32 count, PixelColor& avgPixelColor) {
	
	double avg_ch_1 = 0.0;
	double avg_ch_2 = 0.0;
	double avg_ch_3 = 0.0;
	double avg_ch_4 = 0.0;
	
	uint32 loopCount = 0;
	for (uint32 i = 0; i < count; i++) {
		
		avg_ch_1 = (avg_ch_1 * (double)loopCount + (double)((colors[i] >> 24) & 0xff)) / (double)(loopCount + 1);
		avg_ch_2 = (avg_ch_2 * (double)loopCount + (double)((colors[i] >> 16) & 0xff)) / (double)(loopCount + 1);
		avg_ch_3 = (avg_ch_3 * (double)loopCount + (double)((colors[i] >> 8) & 0xff)) / (double)(loopCount + 1);
		avg_ch_4 = (avg_ch_4 * (double)loopCount + (double)(colors[i] & 0xff)) / (double)(loopCount + 1);
		
		loopCount++;
	}
	
	avgPixelColor = ((uint8)avg_ch_1 << 24) | ((uint8)avg_ch_2 << 16) | ((uint8)avg_ch_3 << 8) | (uint8)avg_ch_4;
	
}


PixelColor VisualColorTools::getMeanRGBPixelColor(const PixelColor& colorX, const PixelColor& colorY, const uint32& count) {
	
	uint8 a_x, r_x, g_x, b_x;
	VisualColorTools::getColorComponentValues(colorX, a_x, r_x, g_x, b_x);
	
	uint8 a_y, r_y, g_y, b_y;
	VisualColorTools::getColorComponentValues(colorY, a_y, r_y, g_y, b_y);
	
	//float h_x, s_x, l_x;
	//float h_y, s_y, l_y;
	
	//VisualColorTools::rgb2hsl((double)r_x / (double)255.0, (double)g_x / (double)255.0, (double)b_x / (double)255.0, h_x, s_x, l_x);
	//VisualColorTools::rgb2hsl((double)r_y / (double)255.0, (double)g_y / (double)255.0, (double)b_y / (double)255.0, h_y, s_y, l_y);
	
	//float avg_h, avg_s, avg_l;
	
	
	uint32 avg_a = 0xff000000;

	uint8 avg_r_8bit = (uint8)((double)((r_x * count) + r_y) / (double)(count + 1));
	uint8 avg_g_8bit = (uint8)((double)((g_x * count) + g_y) / (double)(count + 1));
	uint8 avg_b_8bit = (uint8)((double)((b_x * count) + b_y) / (double)(count + 1));
	
	//float avg_h = (float)((float)((h_x * (float)count) + h_y) / (float)(count + 1));
	//float avg_s = (float)((float)((s_x * (float)count) + s_y) / (float)(count + 1));
	//float avg_l = (float)((float)((l_x * (float)count) + l_y) / (float)(count + 1));

	/*
	float avg_r, avg_g, avg_b;
	VisualColorTools::hsl2rgb(avg_h, avg_s, avg_l, avg_r, avg_g, avg_b);
	uint32 avg_r_8bit = (uint32)(avg_r * 255.0f);
	uint32 avg_g_8bit = (uint32)(avg_g * 255.0f);
	uint32 avg_b_8bit = (uint32)(avg_b * 255.0f);
	*/

	PixelColor pixelColor = avg_a | (avg_r_8bit << 16) | (avg_g_8bit << 8) | avg_b_8bit;
	
	return pixelColor;
	
}


void VisualColorTools::RGBToHSL(const double& r, const double& g, const double& b, double* h, double* s, double* l) {
	
	double vmin, vmax, delta;
	double dr,dg,db;
	
	vmin = VisualColorTools::min3(r, g, b);
	vmax = VisualColorTools::max3(r, g, b);
	delta = vmax - vmin;
	
	*l = (vmax + vmin) / 2;
	
	if (delta == 0) { // gray
		*h = 0;
		*s = 0;
	} else {
		if (*l < 0.5) {
			*s = delta / (vmax + vmin);
		} else {
			*s = delta / (2 - vmax - vmin);
		}
		
		dr = (((vmax - r) / 6) + (delta / 2)) / delta;
		dg = (((vmax - g) / 6) + (delta / 2)) / delta;
		db = (((vmax - b) / 6) + (delta / 2)) / delta;
		
		if (r == vmax) {
			*h = db - dg;
		} else if (g == vmax) {
			*h = (1.0 / 3.0) + dr - db;
		} else if (b == vmax) {
			*h = (2.0 / 3.0) + dg - dr;
		}
		
		if (*h < 0) {
			*h += 1;
		}
		if (*h > 1) {
			*h -= 1;
		}
	}
	
}


void VisualColorTools::HSLToRGB(const double& h, const double& s, const double& l, double* r, double* g, double* b) {
	
	double v1, v2;
	
	if (s == 0) {
		*r = l * 255;
		*g = l * 255;
		*b = l * 255;
	} else {
		if (l < 0.5) {
			v2 = l * (1 + s);
		} else {           
			v2 = (l + s) - (s * l);
		}
		
		v1 = 2 * l - v2;
		
		*r = 255 * VisualColorTools::hueToRGB(v1, v2, h + (1.0 / 3.0));
		*g = 255 * VisualColorTools::hueToRGB(v1, v2, h);
		*b = 255 * VisualColorTools::hueToRGB(v1, v2, h - (1.0 / 3.0));
	} 
	
}


void VisualColorTools::RGBToHSV(const double& r, const double& g, const double& b, double* h, double* s, double* v) {
	
	double var_Min;
	double var_Max;
	double del_Max;
	
	var_Min = VisualColorTools::min3(r, g, b);
	var_Max = VisualColorTools::max3(r, g, g);
	del_Max = var_Max - var_Min;
	
	*v = var_Max;
	
	if (del_Max == 0) {
		// gray
		*h = 0;
		*s = 0;
	} else {
		double del_R = (((var_Max - r) / 6) + (del_Max / 2)) / del_Max;
		double del_G = (((var_Max - g) / 6) + (del_Max / 2)) / del_Max;
		double del_B = (((var_Max - b) / 6) + (del_Max / 2)) / del_Max;
		
		*s = del_Max / var_Max;
		
		if (r == var_Max) {
			*h = del_B - del_G;
		} else if (g == var_Max) {
			*h = (1.0 / 3.0) + del_R - del_B;
		} else if (b == var_Max) {
			*h = (2.0 / 3.0) + del_G - del_R;
		}
		
		if (*h < 0) {
			*h += 1;
		}
		if (*h > 1) {
			*h -= 1;
		}
	}

}


void VisualColorTools::HSVToRGB(const double& h, const double& s, const double& v, double* r, double* g, double* b) {
	
	if (s == 0) {
		*r = v;
		*g = v;
		*b = v;
	} else {
		
		double var_h = h * 6;
		double var_i = floor(var_h);
		double var_1 = v * (1 - s);
		double var_2 = v * (1 - s * (var_h - var_i));
		double var_3 = v * (1 - s * (1 - (var_h - var_i)));
		
		if ( var_i == 0 ) { 
			*r = v;
			*g = var_3;
			*b = var_1;
		} else if (var_i == 1) {
			*r = var_2;
			*g = v;
			*b = var_1;
		} else if (var_i == 2) {
			*r = var_1;
			*g = v;
			*b = var_3;
		} else if (var_i == 3) {
			*r = var_1;
			*g = var_2;
			*b = v;
		} else if (var_i == 4) {
			*r = var_3;
			*g = var_1;
			*b = v;
		} else {
			*r = v;
			*g = var_1;
			*b = var_2;
		}
		
	}
}


void VisualColorTools::convertInterleavedPixels1234To4321(PixelColor* pixels, uint32 numberOfPixels) {
	
	uint32 a, r, g, b;
	uint32 pixel;
	
	for (uint32 i = 0; i < numberOfPixels; i++) {
		
		b = ((pixels[i] >> 24) & 0xff);
		g = ((pixels[i] >> 16) & 0xff);
		r = ((pixels[i] >> 8) & 0xff);
		a = (pixels[i] & 0xff);
		pixel = (a << 24) | (r << 16) | (g << 8) | b;
		
		pixels[i] = pixel;
	}
	
}


void VisualColorTools::convertInterleavedPixels1234To3214(PixelColor* pixels, uint32 numberOfPixels) {
	
	uint32 a, r, g, b;
	uint32 pixel;
	
	for (uint32 i = 0; i < numberOfPixels; i++) {
		
		r = ((pixels[i] >> 24) & 0xff);
		g = ((pixels[i] >> 16) & 0xff);
		b = ((pixels[i] >> 8) & 0xff);
		a = (pixels[i] & 0xff);
		pixel = (a << 24) | (r << 16) | (g << 8) | b;
		
		pixels[i] = pixel;
	}
	
}


void VisualColorTools::convertARGBPixelToRGB(PixelColor& argbPixel) {
	
	uint8 a = (argbPixel >> 24) & 0xff;
	if (a == 255) return; // already premultiplied
	
	if (a == 0) {
		argbPixel = 0x00000000;
		return;
	}
	
	double alphaMultiplier = (double)a / 255.0;
	
	uint32 r = (argbPixel >> 16) & 0xff;
	uint32 g = (argbPixel >> 8) & 0xff;
	uint32 b = argbPixel & 0xff;
	
	r = (uint32)((double)r * alphaMultiplier);
	g = (uint32)((double)g * alphaMultiplier);
	b = (uint32)((double)b * alphaMultiplier);
	
	argbPixel = 0xff000000;
	argbPixel = argbPixel | (r << 16) | (g << 8) | b;
	
}


double VisualColorTools::getDistance(const PixelColor& colorX, const PixelColor& colorY) {
	double distance = 0;
	uint8 a_x, r_x, g_x, b_x;
	uint8 a_y, r_y, g_y, b_y;
	VisualColorTools::getColorComponentValues(colorX, a_x, r_x, g_x, b_x);
	VisualColorTools::getColorComponentValues(colorY, a_y, r_y, g_y, b_y);
	double r_x_double = (double)r_x / 255.0;
	double g_x_double = (double)g_x / 255.0;
	double b_x_double = (double)b_x / 255.0;
	double r_y_double = (double)r_y / 255.0;
	double g_y_double = (double)g_y / 255.0;
	double b_y_double = (double)b_y / 255.0;
	double delta_r = r_x_double - r_y_double;
	double delta_g = g_x_double - g_y_double;
	double delta_b = b_x_double - b_y_double;
	distance = sqrt((delta_r * delta_r) + (delta_g * delta_g) + (delta_b * delta_b));
	return distance;
}


PixelColor* VisualColorTools::createARGBCheckPixels(uint32 width, uint32 height, const VertexColor& checkColor) {
	return VisualColorTools::createCheckPixels(width, height, "ARGB", checkColor);
}


uint32* VisualColorTools::createBGRACheckPixels(uint32 width, uint32 height, const VertexColor& checkColor) {
	return VisualColorTools::createCheckPixels(width, height, "BGRA", checkColor);
}


uint32* VisualColorTools::createRGBACheckPixels(uint32 width, uint32 height, const VertexColor& checkColor) {
	return VisualColorTools::createCheckPixels(width, height, "RGBA", checkColor);
}


uint32* VisualColorTools::createABGRCheckPixels(uint32 width, uint32 height, const VertexColor& checkColor) {
	return VisualColorTools::createCheckPixels(width, height, "ABGR", checkColor);
}


PixelColor* VisualColorTools::createCheckPixels(uint32 width, uint32 height, const char* const format, const VertexColor& checkColor) {
	
	uint8 colorChannelFormat = 0;
	if (strcmp(format, "ARGB") == 0) {
		colorChannelFormat = 1;
	} else if (strcmp(format, "BGRA") == 0) {
		colorChannelFormat = 2;
	} else if (strcmp(format, "RGBA") == 0) {
		colorChannelFormat = 3;
	} else if (strcmp(format, "ABGR") == 0) {
		colorChannelFormat = 4;
	} else {
		char errStr[256];
		sprintf(errStr, "Err: unknown colorChannelFormat in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return NULL;
	}
	
	uint8 color = 0;
	if (checkColor.r == 1.0 && checkColor.g == 0.0 && checkColor.b == 0.0 && checkColor.a == 1.0 && checkColor.red == 1.0 && checkColor.green == 0.0 && checkColor.blue == 0.0 && checkColor.alpha == 1.0) {
		color = 1; // red
	} else if (checkColor.r == 0.0 && checkColor.g == 1.0 && checkColor.b == 0.0 && checkColor.a == 1.0 && checkColor.red == 0.0 && checkColor.green == 1.0 && checkColor.blue == 0.0 && checkColor.alpha == 1.0) {
		color = 2; // green
	} else if (checkColor.r == 0.0 && checkColor.g == 0.0 && checkColor.b == 1.0 && checkColor.a == 1.0 && checkColor.red == 0.0 && checkColor.green == 0.0 && checkColor.blue == 1.0 && checkColor.alpha == 1.0) {
		color = 3; // blue
	} else if (checkColor.r == 1.0 && checkColor.g == 1.0 && checkColor.b == 1.0 && checkColor.a == 1.0 && checkColor.red == 1.0 && checkColor.green == 1.0 && checkColor.blue == 1.0 && checkColor.alpha == 1.0) {
		color = 4; // white
	} else {
		char errStr[256];
		sprintf(errStr, "unknown checkColor in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return NULL;
	}
	
	const uint8 numberOfChannels = 4; // ARGB, BGRA, RGBA, ABGR
	PixelColor* pixels = (PixelColor*)malloc(width * height * numberOfChannels);
	if (pixels == NULL) {
		char errStr[256];
		sprintf(errStr, "pixels == NULL in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errStr);
		return pixels;
	}
	
	uint32 cnt = 0;
	uint32 i;
	uint32 j;
	uint32 c;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			uint8 a, r, g, b;
			if (c == 255) {
				a = 0xff;
			} else {
				a = 0x00;
			}
			// red or white
			if ((color == 1) || (color == 4)) {
				r = (uint8)c;
			} else {
				r = 0x00;
			}
			// green or white
			if ((color == 2) || (color == 4)) {
				g = (uint8)c;
			} else {
				g = 0x00;
			}
			// blue or white
			if ((color == 3) || (color == 4)) {
				b = (uint8)c;
			} else {
				b = 0x00;
			}
			switch (colorChannelFormat) {
				case 1:
					pixels[cnt] = (a << 24) | (r << 16) | (g << 8) | b;
					break;
				case 2:
					pixels[cnt] = (b << 24) | (g << 16) | (r << 8) | a;
					break;
				case 3:
					pixels[cnt] = (r << 24) | (g << 16) | (b << 8) | a;
					break;
				case 4:
					pixels[cnt] = (a << 24) | (b << 16) | (g << 8) | r;
					break;
				default:
					char errStr[256];
					sprintf(errStr, "unknown colorChannelFormat in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
					writeLog(errStr);
			}
			cnt++;
		}
	}
	
	return pixels;
}
