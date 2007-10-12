/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualTimeline.h
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

#ifndef VisualTimeline_h
#define VisualTimeline_h

#include "VisualInterpolation.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).

	/**
	 * The type of repetition.
	 */
	typedef enum {
		kRepeatFromStart = 0, /**< After the duration of the timeline has been reached, the timeline starts anew at the start.\ The timeline is always processed in forward direction. */
		kRepeatMirrored /**< After the duration of the timeline has been reached, the timeline is processed in backward direction.\ The moving direction toggles in mirrored loop. */
	} RepeatMode;	

	/**
	 * The internal type of direction.
	 */
	typedef enum {
		kForward = 0, /**< The timeline is processed in forward direction. */
		kBackward /**< The timeline is processed in backward direction. */
	} MovingDirection;

	/** Callback function that is called each time when elapsed time exceeds duration time. */
	typedef void (*VisualTimelineElapsedTimeDidExceedDurationCallback)(void* userData);

	/**
	 * Time synchronized interpolation between values.
	 * Calculates timed interpolated values.
	 */
	class VisualTimeline {

	public:

		/**
		 * The constructor.
		 */
		VisualTimeline();

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
		 * Returns the start value of the timeline.
		 * @return The start value of the timeline.
		 */
		double getStartValue(void);

		/**
		 * Sets the end value of the timeline.
		 * @param anEndValue An end value.
		 */
		void setEndValue(double anEndValue);

		/**
		 * Returns the end value of the timeline.
		 * @return The end value of the timeline.
		 */
		double getEndValue(void);

		/**
		 * Sets the duration of the timeline.
		 * @param numberOfMilliseconds The number of milliseconds the timeline spans.
		 */
		void setDurationInMilliseconds(UInt32 numberOfMilliseconds);

		/**
		 * Returns the duration of the timeline.
		 * @return The duration of the timeline.
		 */
		UInt32 getDurationInMilliseconds(void);

		/**
		 * Sets the repeat mode of the timeline.
		 * @param aRepeatMode The requested repeat mode.
		 */
		void setRepeatMode(RepeatMode aRepeatMode);

		/**
		 * Returns the current repeat mode of the timeline.
		 * @return The current repeat mode of the timeline.
		 */
		RepeatMode getRepeatMode(void);

		/**
		 * Sets the current value of the timeline.
		 * @param newValue The value to which the timeline is supposed to jump.
		 * @remarks The value of the timeline can be set from the outside (e.g.\ in case it is created new and should start with a established value).\ Normally only the start and end values are set and the client asks for the current value.
		 */
		void setCurrentValue(double newValue);

		/**
		 * Returns the current value of the timeline.
		 * @return The current value of the timeline.
		 */
		double getCurrentValue(void);

		/**
		 * Resets the timeline.\ The timeline starts anew at start.
		 */
		void reset(void);

		/**
		 * Starts the timeline.
		 * @param aCallbackFunction A callback function that should be called each time when elapsed time exceeds duration time.
		 * @param someUserData Additional data (e.g.\ pointer to instance variable of initialized class).
		 */
		void start(VisualTimelineElapsedTimeDidExceedDurationCallback aCallbackFunction = NULL, void* someUserData = NULL);

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
		 * Sets the running direction of the timeline.
		 * @param direction The requested running direction of the timeline.
		 */	
		void setMovingDirection(MovingDirection direction);

		/**
		 * Returns the running direction of the timeline.
		 * @return The currently running direction of the timeline.
		 */	
		MovingDirection getMovingDirection(void);

		/**
		 * Sets the debug mode of the timeline object.
		 * @param requestedDebugMode True if debug mode should be turned on, false if debug mode should be turned off.
		 * @remarks Can be used to instrument custom ad hoc debugging.
		 */	
		void setDebugMode(bool requestedDebugMode);
			
	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualTimeline.
		 */
		void copy(const VisualTimeline& other);

		/** Call callback function that is called each time when elapsed time exceeds duration time. */
		void elapsedTimeDidExceedDuration(void);

		UInt32 durationInMilliseconds; /**< The duration of the timeline in milliseconds. */
		
		RepeatMode repeatMode; /**< The repeat mode of the timeline. */
		
		VisualItemIdentifier* durationIdentifier; /**< The internal identifier of the timeline. */
		
		MovingDirection movingDirection; /**< The moving direction of the timeline. */
		
		VisualInterpolation* visualInterpolation; /**< The interpolation model. */
		
		UInt32 elapsedMilliseconds; /**< The elapsed milliseconds. */
		
		UInt32 offsetMilliseconds; /**< Internally used offset in milliseconds. */
		
		double currentValue; /**< The current value as it has been evaluated the last time. */
		
		bool isStopped; /**< True if the timeline has been stopped, false otherwise. */
		
		bool debugMode; /**< True if in debug mode, false otherwise. */

		/** Pointer to callback function that is called each time when elapsed time exceeds duration time. */
		VisualTimelineElapsedTimeDidExceedDurationCallback elapsedTimeGreaterThanDurationCallback;

		/** Internally stored pointer to provided user data (e.g.\ pointer to instance variable of initialized class). */
		void* userData;
	};
	

}

#endif /* VisualTimeline_h */
