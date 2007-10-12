/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualInterpolation.h
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

#ifndef VisualInterpolation_h
#define VisualInterpolation_h

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	/**
	 * Types of interpolation.
	 */
	typedef enum {
		kLinearInterpolation = 0, /**< Linear interpolation. */
		kSinusoidalInterpolation /**< Sinusoidal interpolation. */
	} InterpolationType;

	/**
	 * Interpolation between two values.
	 */
	class VisualInterpolation {

	public:

		/**
		 * The constructor.
		 * @param anInterpolationType Default interpolation is linear.
		 */	
		VisualInterpolation(InterpolationType anInterpolationType = kLinearInterpolation);

		/**
		 * The destructor.
		 */	
		~VisualInterpolation();

		/**
		 * Copy constructor.
		 * @param other Another VisualInterpolation.
		 */
		VisualInterpolation(const VisualInterpolation& other);

		/**
		 * Assignment operator.
		 * @param other Another VisualInterpolation.
		 */
		VisualInterpolation& operator=(const VisualInterpolation& other);
		
		/**
		 * Sets the start value of the interpolation.
		 * @param aStartValue A start value.
		 */
		void setStartValue(double aStartValue);

		/**
		 * Sets the end value of the interpolation.
		 * @param anEndValue An end value.
		 */
		void setEndValue(double anEndValue);

		/**
		 * Returns the start value of the interpolation.
		 * @return The start value.
		 */
		double getStartValue(void);

		/**
		 * Returns the end value of the interpolation.
		 * @return The end value.
		 */
		double getEndValue(void);

		/**
		 * Returns the computed value at a specific position.
		 * @param position The position of the interpolation.\ Position can be between 0.0 and 1.0.
		 * @return The interpolated value.
		 */
		double getValueAtPosition(double position);

		/**
		 * Sets the type of interpolation.
		 * @param anInterpolationType The type of interpolation.
		 * @remarks Linear and sinusoidal interpolation are possible.
		 */
		void setType(InterpolationType anInterpolationType);
	
	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualInterpolation.
		 */
		void copy(const VisualInterpolation& other);
		
		void calcDistance(); /**< Internal method to compute the distance between start and end value. */
		
		double startValue; /**< The start value. */
		
		double endValue; /**< The end value. */
		
		double distance; /**< The calculated distance between start and end value. */
		
		InterpolationType interpolationType; /**< The type of interpolation used (linear, sinusoidal). */
	
	};
	
	typedef VisualInterpolation* VisualInterpolationRef; /**< Pointer to instance typedefed as ref data type. */

}

#endif /* VisualInterpolation_h */
