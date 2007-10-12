/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualAnimation.h
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

#ifndef VisualAnimation_h
#define VisualAnimation_h

#include <vector>

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class VisualTimeline; // Forward declaration (to avoid include of header file).
	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).

	/** Animation callback */
	typedef OSStatus (*VisualAnimationCallback)(double currentPosition, void* userData);

	/**
	 * The type of looping.
	 */
	typedef enum {
		kNoLoop = 0x0, /**< 00000000 - After the duration of the timeline has been reached, the animation stops. */
		kLoop = 0x2, /**< 00000010 - After the duration of the timeline has been reached, the timeline starts anew at the start.\ The timeline is always processed in forward direction. */
		kMirroredLoop = 0x3 /**< 00000011 - After the duration of the timeline has been reached, the timeline is processed in backward direction.\ The moving direction toggles in mirrored loop. */
	} LoopMode;

	const SInt32 kInfiniteRepetition = -1;

	/**
	 * The property of the asset that is affected by the animation.
	 */
	typedef enum {
		kUndefinedAnimatedProperty = 0, /**< No defined value. */
		kCustomAnimatedProperty, /**< Custom property is animated. */
		kAnimatedSize, /**< The animation affects the size of the asset. */
		kAnimatedOpacity, /**< The animation affects the opacity of the asset. */
		kAnimatedRotation /**< The animation affects the rotation of the asset. */
	} AnimatedProperty;

	/**
	 * Calls a provided callback function for the time of the duration of the animation.\ The values range from 0.0 to 1.0.
	 * The animation tries to reach the end value in the amount of time set by the duation value.
	 * The speed of the animation is determined by the start value of the animation.
	 */
	class VisualAnimation {

	public:

		/**
		 * The constructor.
		 * @param anAnimatedProperty The property of the asset that is affected by the animation. 
		 */
		VisualAnimation(AnimatedProperty anAnimatedProperty);

		/**
		 * The destructor.
		 */		
		~VisualAnimation();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualAnimation.
		 */
		VisualAnimation(const VisualAnimation& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualAnimation.
		 */
		VisualAnimation& operator=(const VisualAnimation& other);

		/**
		 * Sets the callback function that is repeatedly called in animation's duration time.
		 * @param theCallback Pointer to c-style function (static/class function).
		 * @param someUserData Additional data (e.g.\ pointer to instance variable of initialized class).
		 */
		void setAnimateCallbackFunctionPtr(VisualAnimationCallback theCallback, void* someUserData);

		/**
		 * Calls the callback function to animate a property.
		 * @return The return status of the callback function.
		 */
		OSStatus callAnimateCallbackFunction(void);

		/**
		 * Returns the VisualTimeline of the animation.
		 * @return The VisualTimeline of the animation.
		 */
		VisualTimeline* getTimeline(void);

		/**
		 * Starts the Animation.
		 * @remarks In case the start is delayed, the animation is only running after the delaying milliseconds did have passed.
		 */
		void start(void);

		/**
		 * Stops the Animation.
		 */
		void stop(void);

		/**
		 * Sets the duration of the animation.
		 * @param numberOfMilliseconds The number of milliseconds the animation spans.
		 */
		void setDurationInMilliseconds(UInt32 numberOfMilliseconds);

		/**
		 * Returns the duration of the animation.
		 * @return The duration of the animation.
		 */
		UInt32 getDurationInMilliseconds(void);

		/**
		 * Sets the current value of the animation.
		 * @param aCurrentValue The current value of the animation.
		 * @remarks The animation always ranges from start value to end value (default: 0.0 to 1.0).\ Setting a specific current value here means that the animation starts its first step with the specific current value.
		 * Can be useful if an animation is supposed to continue at the point in time of a prior animation.\ The initial value of the constructor (0.0) does not change the current value.
		 */
		void setCurrentValue(double aCurrentValue);
		
		/**
		 * Sets the start value of the animation.
		 * @param aStartValue The start value of the animation.
		 * @remarks Default is 0.0.
		 */
		void setStartValue(double aStartValue);

		/**
		 * Returns the start value of the animation.
		 * @return The start value of the animation.
		 */
		double getStartValue(void);

		/**
		 * Sets the end value of the animation.
		 * @param anEndValue The end value of the animation.
		 * @remarks Default is 1.0.
		 */
		void setEndValue(double anEndValue);

		/**
		 * Returns the end value of the animation.
		 * @return The end value of the animation.
		 */
		double getEndValue(void);

		/**
		 * Sets a minimum clamp value for the animation.
		 * @param aMinClampValue The minimum clamp value of the animation.
		 * @remarks The animation stops when the current value is lower than the minimum clamp value.\ The clamp value does not affect the speed of the animation (unlike start and end value).
		 */
		void setMinClampValue(double aMinClampValue);

		/**
		 * Sets a maximum clamp value for the animation.
		 * @param aMaxClampValue The maximum clamp value of the animation.
		 * @remarks The animation stops when the current value is higher than the maximum clamp value.\ The clamp value does not affect the speed of the animation (unlike start and end value).
		 */
		void setMaxClampValue(double aMaxClampValue);

		/**
		 * Sets the loop mode of the animation.
		 * @param aLoopMode The requested loop mode.
		 * @param requestedNumberOfLoops The number of times the animation should be repeated (default = 1).
		 */
		void setLoopMode(LoopMode aLoopMode, SInt32 requestedNumberOfLoops = 1);

		/**
		 * Returns the number of milliseconds until the animation is supposed to actually start.
		 * @return The number of milliseconds until the animation is supposed to actually start.
		 * @remarks The delay is set when calling setDurationInMilliseconds().\ The value is used by VisualStageControl.
		 */
		UInt32 getStartDelayInMilliseconds(void);

		/**
		 * Sets the number of milliseconds until the animation actually starts.
		 * @param aStartDelayInMilliseconds The number of milliseconds until the animation actually starts.
		 * @remarks The delay counts from the moment start() is called.
		 */
		void setStartDelayInMilliseconds(UInt32 aStartDelayInMilliseconds);

		/**
		 * Returns the animated property of the animation.
		 * @return The animated property of the animation.
		 */
		const AnimatedProperty getAnimatedProperty(void) const;

		/**
		 * Answers the question whether the animation is (still) running.
		 * @return True is the animation is (still) running.\ False otherwise.
		 */
		const bool isRunning(void);

		/**
		 * Answers the question whether the animation is dead.
		 * @return True is the animation is dead.\ False otherwise.
		 * @remarks The animation is dead when the duration is exceeded.\ VisualStageControl acts on a true return value and deletes the VisualAnimation.
		 */
		const bool isDead(void) const;

		/**
		 * Returns a pointer to the identifier of the animation.
		 * @return A pointer to the identifier of the animation.
		 */
		const VisualItemIdentifier* const getIdentifier(void) const;

		/**
		 * Sets the debug mode of the animation
		 * @param requestedDebugMode True if debug mode should be turned on, false if debug mode should be turned off.
		 * @remarks Can be used to instrument custom ad hoc debugging.
		 */	
		void setDebugMode(bool requestedDebugMode);

		/**
		 * Callback function that is called by the timeline each time when the duration time is exceeded.
		 * @param visualAnimation Pointer to instance of VisualAnimation.
		 */
		static void durationIsExceeded(void* visualAnimation);
		
	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualAnimation.
		 */
		void copy(const VisualAnimation& other);
		
		/** The property of the asset that is affected by the animation. */
		AnimatedProperty animatedProperty;

		/**
		 * Decrement the number of remaining repeats.
		 */
		void decrementRemainingNumberOfRepeats(void);
		
		/** Specified current value. */
		double currentValue;

		/** True if the current value has been set, false otherwise. */
		bool currentValueIsSet;

		/** The pointer to the timeline of the animation. */
		VisualTimeline* theTimeline;

		/** The remaining number of times the animation should pass. */
		SInt32 remainingNumberOfRepeats;
		
		/** The callback function that is called during the animation. */
		VisualAnimationCallback animateCallbackFunctionPtr;
		
		/** Internally stored pointer to provided user data (e.g.\ pointer to instance variable of initialized class). */
		void* userData;
	
		/** The identifier of the animation. */
		VisualItemIdentifier* animationIdentifier;
		
		/** The number of milliseconds until the animation will be actually used by VisualStageControl. */
		UInt32 startDelayInMilliseconds;
		
		bool debugMode; /**< True if in debug mode, false otherwise. */
		
		static const UInt32 maxNumberOfDebugHistoryEntries; /**< The maximum number of data entries stored in history for debug and monitoring purposes. */
		
		UInt32 currDebugHistoryEntry; /**< The current entry of the debug history. */
		
		/** Data entries stored for debug and monitoring purposes. */
		typedef std::vector<double> DebugHistory;
		
		/** The DebugHistoryIterator is an iterator of the DebugHistory. */
		typedef DebugHistory::iterator DebugHistoryIterator;
		
		/** The data entries stored for debug and monitoring purposes. */
		DebugHistory debugHistory;
		
		/** True if the maximum number of data entries of debugHistory are initialized, false otherwise. */
		bool debugHistoryIsInitialized;
		
		/** The possible status values of the VisualAnimation. */
		typedef enum {
			kIsNotRunning = 0, /**< Animation is not running. */
			kIsRunning, /**< Animation is running. */
			kIsDead /**< Animation ended. */
		} VisualAnimationStatus;
		
		/** The status of the VisualAnimation. */
		VisualAnimationStatus status;
		
		/** A possible minimum clamp value. */
		double minimumClampValue;
		
		/** True if the minimum clamp value was set. */
		bool minimumClampValueIsSet;

		/** A possible maximum clamp value. */
		double maximumClampValue;
		
		/** True if the maximum clamp value was set. */
		bool maximumClampValueIsSet;

	};

}

#endif /* VisualAnimation_h */
