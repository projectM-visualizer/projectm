/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualInterpolation.h
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

#ifndef VisualInterpolation_h
#define VisualInterpolation_h


#include "VisualTypes.h"
#include "VisualAnimationTypes.h"


namespace VizKit {

	/**
	 * Interpolation between values 0.0 and 1.0.
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
		 * Returns the computed value at a specific position.
		 * @param position The position of the interpolation. Position can be between 0.0 and 1.0.
		 * @return The interpolated value.
		 */
		double getValueAtPosition(double position) const;

		/**
		 * Sets the type of interpolation.
		 * @param anInterpolationType The type of interpolation.
		 * @remarks Linear and sinusoidal interpolation are possible.
		 */
		void setType(InterpolationType anInterpolationType);

		/**
		 * Returns a random value between 0.0 and 1.0.
		 * @return A random value between 0.0 and 1.0.
		 */
		static double getRandomValue(void);
	
	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualInterpolation.
		 */
		void copy(const VisualInterpolation& other);
		
		void calcDistance(); /**< Internal method to compute the distance between start and end value. */
		
		static const double startValue; /**< The start value. */
		
		static const double endValue; /**< The end value. */
		
		double distance; /**< The calculated distance between start and end value. */
		
		InterpolationType interpolationType; /**< The type of interpolation used (linear, sinusoidal). */
	
	};
	
}

#endif /* VisualInterpolation_h */
