/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualColorTools.h
 *
 */


#ifndef VisualColorTools_h
#define VisualColorTools_h


#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Rgbhsl is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Rgbhsl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
// This library converts between colors defined with RGB values and HSL
// values.  It also finds in-between colors by moving linearly through
// HSL space.
// All functions take values for r, g, b, h, s, and l between 0.0 and 1.0
// (RGB = red, green, blue;  HSL = hue, saturation, luminosity)



namespace VizKit {

	/**
	 * Various functionality to deal with rgb and hsl color values.
	 */
	class VisualColorTools {

	public:

		/**
		 * Converts rgb values to their hsl equivalent.
		 * @param r The red value.
		 * @param g The green value.
		 * @param b The blue value.
		 * @param[out] h The hue value.
		 * @param[out] s The saturation value.
		 * @param[out] l The lightness value.
		 */
		static void rgb2hsl(const float r, const float g, const float b, float &h, float &s, float &l);

		/**
		 * Converts hsl values to their rgb equivalent.
		 * @param h The hue value.
		 * @param s The saturation value.
		 * @param l The lightness value.
		 * @param[out] r The red value.
		 * @param[out] g The green value.
		 * @param[out] b The blue value.
		 */
		static void hsl2rgb(const float h, const float s, const float l, float &r, float &g, float &b);

		/**
		 * Calculates a tween value for two sets of hsl values.
		 * A tween value of 0.0 will output the first
		 * color while a tween value of 1.0 will output the second color.
		 * A value of 0 for direction indicates a positive progression around
		 * the color wheel (i.e. red -> yellow -> green -> cyan...).  A value of
		 * 1 does the opposite.
		 * @param h1 The first hue value.
		 * @param s1 The first saturation value.
		 * @param l1 The first lightness value.
		 * @param h2 The second hue value.
		 * @param s2 The second saturation value.
		 * @param l2 The second lightness value.
		 * @param tween The tween value.
		 * @param direction The direction value.
		 * @param[out] outh The calculated hue value.
		 * @param[out] outs The calculated saturation value.
		 * @param[out] outl The calculated lightness value.
		 */
		static void hslTween(const float h1, const float s1, const float l1,
					  const float h2, const float s2, const float l2, const float tween, const int direction,
					  float &outh, float &outs, float &outl);

		/**
		 * Calculates a tween value for two sets of rgb values.
		 * A tween value of 0.0 will output the first
		 * color while a tween value of 1.0 will output the second color.
		 * A value of 0 for direction indicates a positive progression around
		 * the color wheel (i.e. red -> yellow -> green -> cyan...).  A value of
		 * 1 does the opposite.
		 * @param r1 The first hue value.
		 * @param g1 The first saturation value.
		 * @param b1 The first lightness value.
		 * @param r2 The second hue value.
		 * @param g2 The second saturation value.
		 * @param b2 The second lightness value.
		 * @param tween The tween value.
		 * @param direction The direction value.
		 * @param[out] outr The calculated red value.
		 * @param[out] outg The calculated green value.
		 * @param[out] outb The calculated blue value.
		 */
		static void rgbTween(const float r1, const float g1, const float b1,
					  const float r2, const float g2, const float b2, const float tween, const int direction,
					  float &outr, float &outg, float &outb);

	private:

		/** The constructor.\ VisualColorTools is a collection of static methods.\ Class does not need any instances.\ Constructor is private and not implemented. */
		VisualColorTools();

		/** The destructor.\ VisualColorTools is a collection of static methods.\ Class does not need any instances.\ Destructor is private and not implemented. */
		~VisualColorTools();
		
	};

}

#endif /* VisualColorTools_h */
