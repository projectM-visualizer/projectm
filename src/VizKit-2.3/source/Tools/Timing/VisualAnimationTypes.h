/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAnimationTypes.h
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

#ifndef VisualAnimationTypes_h
#define VisualAnimationTypes_h


#include "VisualTypes.h"


namespace VizKit {

	/**
	 * The property of the asset that is affected by the animation.
	 */
	typedef enum {
		kUndefinedAnimatedProperty = 0, /**< No defined value. */
		kCustomAnimatedProperty, /**< Custom property is animated. */
		kAnimatedSize, /**< The animation affects the size of the asset. */
		kAnimatedLocation, /**< The animation affects the location of the asset. */
		kAnimatedOpacity, /**< The animation affects the opacity of the asset. */
		kAnimatedRotation /**< The animation affects the rotation of the asset. */
	} AnimatedProperty;

	/**
	 * Types of interpolation between two values.
	 */
	typedef enum {
		kLinearInterpolation = 0, /**< Linear interpolation. */
		kSinusoidalInterpolation /**< Sinusoidal interpolation. */
	} InterpolationType;

	/**
	 * The type of repetition.
	 */
	typedef enum {
		kRepeatFromStart = 0, /**< After the duration of the timeline has been reached, the timeline starts anew at the start. The timeline is always processed in forward direction. */
		kRepeatMirrored /**< After the duration of the timeline has been reached, the timeline is processed in backward direction. The moving direction toggles in mirrored loop. */
	} RepeatMode;	

	/**
	 * The internal type of direction.
	 */
	typedef enum {
		kAscending = 0, /**< The timeline is processed from lower values to higher values. */
		kDescending /**< The timeline is processed from higher values to lower values. */
	} MovingDirection;
	
	typedef double AnimationSpeed; /**< Speed of animation (measured in number of units per millisecond). */

}

#endif /* VisualAnimationTypes_h */
