/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualTimeline.h
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

#ifndef VisualTimeline_h
#define VisualTimeline_h


#include "VisualTypes.h"
#include "VisualAnimationTypes.h"


namespace VizKit {

	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).
	class VisualInterpolation; // Forward declaration (to avoid include of header file).

	/** Possible return values of the update() call. */
	typedef enum {
		kTimelineUpdateOK, /**< No error, no special state. */
		kElapsedTimeDidExceedDuration, /**< The elapsed time exceeded the set duration time. */
		kStopValueHit, /**< The set stop value is hit or crossed (depending on the current move direction). */
		kTimelineIsStopped, /**< The VisualTimeline is stopped. */
		kNoDurationTime /**< The amount of the duration time is 0. */
	} TimelineUpdateResult;

	/**
	 * Time synchronized interpolation between values.
	 * Calculates timed interpolated values.
	 */
	class VisualTimeline {

	public:

		/**
		 * The constructor.
		 * @param aDebugMode If true, the timeline is in debug mode.
		 */
		VisualTimeline(bool aDebugMode = false);

		/**
		 * The destructor.
		 */		
		~VisualTimeline();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualTimeline.
		 */
		VisualTimeline(const VisualTimeline& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualTimeline.
		 */
		VisualTimeline& operator=(const VisualTimeline& other);

		/**
		 * Sets the start value of the timeline.
		 * @param aStartValue A start value.
		 */
		void setStartValue(double aStartValue);

		/**
		 * Sets the stop value of the timeline.
		 * @param aStopValue A stop value.
		 */
		void setStopValue(double aStopValue);

		/**
		 * Returns the minimum value of the timeline.
		 * @return The minimum value of the timeline.
		 */
		double getMinValue(void) const;

		/**
		 * Returns the maximum value of the timeline.
		 * @return The maximum value of the timeline.
		 */
		double getMaxValue(void) const;

		/**
		 * Returns the distance of the start and stop values of the timeline.
		 * @return The distance of the start and stop values of the timeline.
		 * @remarks The distance is never a positive value.
		 */
		double getDistance(void) const;

		/**
		 * Sets the duration of the timeline.
		 * @param numberOfMilliseconds The number of milliseconds the timeline spans.
		 */
		void setDurationInMilliseconds(uint32 numberOfMilliseconds);

		/**
		 * Returns the duration of the timeline.
		 * @return The duration of the timeline.
		 */
		uint32 getDurationInMilliseconds(void) const;

		/**
		 * Sets the repeat mode of the timeline.
		 * @param aRepeatMode The requested repeat mode.
		 */
		void setRepeatMode(RepeatMode aRepeatMode);

		/**
		 * Returns the current repeat mode of the timeline.
		 * @return The current repeat mode of the timeline.
		 */
		RepeatMode getRepeatMode(void) const;

		/**
		 * Sets the current value of the timeline.
		 * @param newCurrValue The value to which the timeline is supposed to jump.
		 * @return True if the new current value was outside of current min and max value range (and distance and duration were adjusted because of that).
		 * @remarks The value of the timeline can be set from the outside (e.g. in case it is created new and should start with a established value).
		 */
		bool setCurrentValue(double newCurrValue);

		/**
		 * Returns the current value of the timeline.
		 * @return The current value of the timeline.
		 */
		double getCurrentValue(void) const;

		/**
		 * Updates the current value of the timeline.
		 * @return The result of the update operation expressed as enum value.
		 * @remarks Called on update of the animation.
		 */
		TimelineUpdateResult update(void);

		/**
		 * Resets the timeline. The timeline starts anew at start.
		 */
		void reset(void);

		/**
		 * Starts the timeline.
		 */
		void start(void);

		/**
		 * Stops the timeline.
		 */		
		void stop(void);

		/**
		 * Resumes the timeline at the point in time where it was stopped the last time.
		 */	
		void resume(void);
		
		/**
		 * Sets the type of interpolation.
		 * @param anInterpolationType The requested type of interpolation.
		 */	
		void setInterpolationType(InterpolationType anInterpolationType);

		/**
		 * Returns the moving direction (running direction) of the timeline.
		 * @return The currently moving direction (running direction) of the timeline.
		 */	
		MovingDirection getMovingDirection(void) const;

		/**
		 * Toggles the current moving direction of the timeline.
		 */
		void toggleMovingDirection(void);

		/**
		 * Sets the debug mode.
		 * @param requestedDebugMode The debug mode. True turns debug mode on, false turns it off.
		 */
		void setDebugMode(bool requestedDebugMode);

		/**
		 * Static helper function that converts a TimelineUpdateResult to the string. Possibly useful for debugging or tracing purposes.
		 * @param aResult A timelineUpdateResult.
		 * @param outString The character string value of the TimelineUpdateResult enum value.
		 */
		static void convertTimelineUpdateResultToString(const TimelineUpdateResult aResult, char* outString);

	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualTimeline.
		 */
		void copy(const VisualTimeline& other);

		uint32 durationInMilliseconds; /**< The duration of the timeline in milliseconds. */

		double minValue; /**< The minimum value. */
		
		double maxValue; /**< The maximum value. */
		
		double distance; /**< The calculated distance between start and stop value. */

		RepeatMode repeatMode; /**< The repeat mode of the timeline. */
		
		VisualItemIdentifier* durationIdentifier; /**< The internal identifier of the timeline. */
		
		MovingDirection movingDirection; /**< The moving direction of the timeline. */
		
		VisualInterpolation* visualInterpolation; /**< The interpolation model. */
		
		uint32 elapsedMilliseconds; /**< The elapsed milliseconds. */
		
		uint32 offsetMilliseconds; /**< Internally used offset in milliseconds. */
		
		double currentValue; /**< The current value as it has been evaluated the last time. */
		
		bool isStopped; /**< True if the timeline has been stopped, false otherwise. */
		
		bool debugMode; /**< True if in debug mode, false otherwise. */

	};
	

}

#endif /* VisualTimeline_h */
