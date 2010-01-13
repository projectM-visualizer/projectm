/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAnimation.h
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

#ifndef VisualAnimation_h
#define VisualAnimation_h


#include "VisualTypes.h"
#include "VisualAnimationComponent.h"
#include "VisualAnimationTypes.h"
#include "VisualStagePosition.h"

#include <vector>


namespace VizKit {

	/** Animation callback */
	typedef void (*VisualAnimationAnimateCallback)(double currentPosition, void* userData);

	class VisualTimeline; // Forward declaration (to avoid include of header file).

	/**
	 * The type of looping.
	 */
	typedef enum {
		kNoLoop = 0x0, /**< 00000000 - After the duration of the timeline has been reached, the animation stops. */
		kLoop = 0x2, /**< 00000010 - After the duration of the timeline has been reached, the timeline starts anew at the start. The timeline is always processed in forward direction. */
		kMirroredLoop = 0x3 /**< 00000011 - After the duration of the timeline has been reached, the timeline is processed in backward direction. The moving direction toggles in mirrored loop. */
	} LoopMode;

	const sint32 kInfiniteRepetition = -1;

	/**
	 * A VisualAnimation is constructed with an AnimatedProperty. With VisualActor::addAnimation() specific properies of an asset can be animated. A provided callback function is called for the time of the duration of the animation.
	 * The animation tries to reach the stop value in the amount of time set by the duration value.
	 */
	class VisualAnimation : public VisualAnimationComponent {

	public:

		/**
		 * Constructor.
		 * @param theAnimatedProperty The property of the asset that is affected by the animation.
		 * @param aDebugMode If true, the animation is in debug mode.
		 */
		VisualAnimation(AnimatedProperty theAnimatedProperty, bool aDebugMode = false);

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
		 * Copies the current VisualAnimation and returns a pointer to a new VisualAnimation.
		 */
		virtual VisualAnimationComponent* clone(void) const;

		/**
		 * Sets the callback function that is repeatedly called in animation's duration time.
		 * @param theCallback Pointer to c-style function (static class function).
		 * @param someUserData Additional data (e.g. pointer to instance variable of initialized class).
		 */
		void setAnimateCallbackFunctionPtr(VisualAnimationAnimateCallback theCallback, void* someUserData = NULL);

		/**
		 * Sets the identifier of the parent collection (the collection which the current animation is part of).
		 * @param aCollectionIdentifier The identifier of the parent collection.
		 */
		virtual void setIdentifierOfParentCollection(const VisualItemIdentifier& aCollectionIdentifier);

		/**
		 * Executes the appropriate action for the case that one animation of the current collection died.
		 * @remarks Only appropriate for collection (like cluster and sequence). VisualAnimation simply returns after printing an error message.
		 */
		virtual void handleOneCollectionItemAnimationDied(void);

		/**
		 * Returns the VisualAnimation if the VisualItemIdentifier matches.
		 * @param anAnimationIdentifier The identifier of the animation.
		 * @return The VisualAnimation with the specified VisualItemIdentifier.
		 * @remarks Return value can be NULL if no VisualAnimation with the specified VisualItemIdentifier is found.
		 */
		virtual VisualAnimationComponent* getVisualAnimation(const VisualItemIdentifier& anAnimationIdentifier);

		/**
		 * Executes the callback function to animate a property.
		 */
		virtual void animate(void);

		/**
		 * Starts the Animation.
		 * @remarks In case the start is delayed, the animation is only running after the delaying milliseconds did have passed.
		 */
		virtual void start(void);

		/**
		 * Stops the Animation.
		 */
		virtual void stop(void);

		/**
		 * Sets the duration of the animation.
		 * @param numberOfMilliseconds The number of milliseconds the animation spans.
		 */
		void setDurationInMilliseconds(uint32 numberOfMilliseconds);

		/**
		 * Returns the duration of the animation.
		 * @return The duration of the animation.
		 */
		uint32 getDurationInMilliseconds(void) const;

		/**
		 * Sets the current value of the animation.
		 * @param aCurrentValue The current value of the animation.
		 * @remarks The animation always ranges from start value to stop value (default: 0.0 to 1.0). Setting a specific current value here means that the animation starts its first step with the specific current value.
		 * Can be useful if an animation is supposed to continue at the point in time of a prior animation. The initial value of the constructor (0.0) does not change the current value.
		 */
		void setCurrentValue(double aCurrentValue);
		
		/**
		 * Returns the current value of the animation.
		 * @return The current value of the animation.
		 */
		double getCurrentValue(void) const;
		
		/**
		 * Sets the start value of the animation.
		 * @param aStartValue The start value of the animation.
		 * @param startAnimationWithCurrentPropertyValue If true, the animation starts with the current value of the animated property. Default is false as it is assumed that a set start value is meant to act as value to start the animation with.
		 * @remarks Default is 0.0.
		 */
		void setStartValue(double aStartValue, bool startAnimationWithCurrentPropertyValue = false);

		/**
		 * Sets the start value of the animation.
		 * @param aPosition The start value of the animation as position.
		 * @param startAnimationWithCurrentPropertyValue If true, the animation starts with the current value of the animated property. Default is false as it is assumed that a set start value is meant to act as value to start the animation with.
		 * @remarks Default is 0.0.
		 */
		void setStartValue(const VisualStagePosition& aPosition, bool startAnimationWithCurrentPropertyValue = false);

		/**
		 * Returns the start value of the animation.
		 * @return The start value of the animation.
		 */
		double getStartValue(void) const;

		/**
		 * Sets the stop value of the animation.
		 * @param aStopValue The stop value of the animation.
		 * @remarks Default is 1.0.
		 */
		void setStopValue(double aStopValue);

		/**
		 * Sets the stop value of the animation.
		 * @param aPosition The stop value of the animation as position.
		 */
		void setStopValue(const VisualStagePosition& aPosition);

		/**
		 * Returns the stop value of the animation.
		 * @return The stop value of the animation.
		 */
		double getStopValue(void) const;

		/**
		 * Sets the loop mode of the animation.
		 * @param aLoopMode The requested loop mode.
		 * @param requestedNumberOfLoops The number of times the animation should be repeated (default = 1).
		 */
		void setLoopMode(LoopMode aLoopMode, sint32 requestedNumberOfLoops = 1);

		/**
		 * Sets the type of interpolation.
		 * @param anInterpolationType The requested type of interpolation.
		 */	
		void setInterpolationType(InterpolationType anInterpolationType);

		/**
		 * Returns the number of milliseconds until the animation is supposed to actually start.
		 * @return The number of milliseconds until the animation is supposed to actually start.
		 * @remarks The delay is set when calling setDurationInMilliseconds(). The value is used by VisualStageControl.
		 */
		uint32 getStartDelayInMilliseconds(void) const;

		/**
		 * Sets the number of milliseconds until the animation actually starts.
		 * @param aStartDelayInMilliseconds The number of milliseconds until the animation actually starts.
		 * @remarks The delay counts from the moment start() is called.
		 */
		void setStartDelayInMilliseconds(uint32 aStartDelayInMilliseconds);

		/**
		 * Sets the callback function that is repeatedly called back during animation component's duration time.
		 * @param theCallback Pointer to c-style function (static class function).
		 * @param userData Additional data (e.g. pointer to instance variable of initialized class).
		 */
		virtual void setCallbackToPerformAnyAdditionalAction(VisualAnimationPerformAnyAdditionalActionCallback theCallback, void* userData = NULL);

		/**
		 * Sets the callback function that is called right before the death of the animation component.
		 * @param theCallback Pointer to c-style function (static class function).
		 * @param userData Additional data (e.g. pointer to instance variable of initialized class).
		 */
		virtual void setCallbackToNotifyBeforeDeath(VisualAnimationWillDieCallback theCallback, void* userData = NULL);

		/**
		 * Calls any callback functions right before the death of the animation component.
		 */
		virtual void notifyBeforeDeath(void);

		/**
		 * Returns the animated property of the animation.
		 * @return The animated property of the animation.
		 */
		virtual AnimatedProperty getAnimatedProperty(void) const;

		/**
		 * Returns the moving direction (running direction) of the animation.
		 * @return The currently moving direction (running direction) of the animation.
		 */	
		MovingDirection getMovingDirection(void) const;

		/**
		 * Returns the current repeat mode of the animation.
		 * @return The current repeat mode of the animation.
		 */
		RepeatMode getRepeatMode(void) const;

		/**
		 * Updates the state of the animation.
		 * @remarks Called by VisualAnimationQueue. Updates the current value of the animation. Also tests whether the animation is supposed to be started in case the start was delayed.
		 */
		virtual void update(void);

		/**
		 * Answers the question whether the animation is (still) running.
		 * @return True if the animation is (still) running. False otherwise.
		 */
		virtual bool isRunning(void) const;

		/**
		 * Answers the question whether the animation ended and is ready to be destroyed.
		 * @return True is the animation ended. False otherwise.
		 * @remarks The animation is redy to die when the duration is exceeded. VisualAnimationQueue acts on a true return value and deletes the VisualAnimation.
		 */
		virtual bool isReadyToDie(void) const;

		/**
		 * Sets the debug mode.
		 * @param requestedDebugMode The debug mode. True turns debug mode on, false turns it off.
		 */
		void setDebugMode(bool requestedDebugMode);
		
		/**
		 * Prepares the animation before it is added to an asset.
		 * @param visualAsset The asset the animation is supposed to be added to.
		 */
		virtual void preparePriorToAddingToAsset(VisualAsset& visualAsset);

		/**
		 * Returns the asset the animation has been added to.
		 * @return The asset the animation has been added to.
		 */
		virtual const VisualAsset* const getEnclosingAsset(void) const;

		/**
		 * Sets the speed of the animation.
		 * @param animationSpeed The speed of the animation (as it has been calculated by static function VisualAnimation::calcSpeed());
		 */
		void setSpeed(AnimationSpeed animationSpeed);

		/**
		 * Returns the current speed of the animation.
		 * @return The current speed of the animation.
		 */
		AnimationSpeed getSpeed(void) const;

		/**
		 * Calculates the speed of an animation if it would use the passed start and stop values with the provided duration time.
		 * @param aStartValue A start value of an animation.
		 * @param aStopValue A stop value of an animation.
		 * @param aDurationInMilliseconds A duration of an animation measured in milliseconds.
		 * @return The speed of an animation.
		 */
		static AnimationSpeed calcSpeed(double aStartValue, double aStopValue, uint32 aDurationInMilliseconds);

		/**
		 * Calculates the speed of an animation of a certain distance and duration.
		 * @param aDistance The distance of the values of the animation.
		 * @param aDurationInMilliseconds Duration of the animation measured in milliseconds.
		 * @return The speed of the animation.
		 */
		static AnimationSpeed calcSpeed(double aDistance, uint32 aDurationInMilliseconds);

		/**
		 * Calculates the time an animation will take to travel a certain distance with a certain constant speed.
		 * @param aDistance The distance of the values of the animation.
		 * @param aSpeed Speed of the animation.
		 * @return The duration of the animation measured in milliseconds.
		 */
		static uint32 calcDurationInMilliseconds(double aDistance, AnimationSpeed aSpeed);

	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualAnimation.
		 */
		void copy(const VisualAnimation& other);
		
		/** The property of the asset that is affected by the animation. */
		AnimatedProperty animatedProperty;

		/**
		 * Internal method called when the duration time of the timeline did exceed.
		 */
		void durationIsExceeded(void);

		/**
		 * Internal method to adjust the current timeline value to the current value of the animated property.
		 * @param visualAsset The asset where a property is animated.
		 * @param anAnimatedProperty The animated property.
		 */
		void setCurrentTimelineValueToCurrentPropertyValue(const VisualAsset& visualAsset, AnimatedProperty anAnimatedProperty);

		/**
		 * Decrement the number of remaining repeats.
		 */
		void decrementRemainingNumberOfRepeats(void);

		/** The pointer to the timeline of the animation. */
		VisualTimeline* theTimeline;

		/** The remaining number of times the animation should pass. */
		sint32 remainingNumberOfRepeats;
		
		/** The callback function that is called during the animation. */
		VisualAnimationAnimateCallback animateCallback;

		/** Internally stored pointer to provided user data (e.g. pointer to instance variable of initialized class). */
		void* animationUserData;

		/** The optional callback function that is called during the animation. */
		VisualAnimationPerformAnyAdditionalActionCallback performAnyAdditionalActionCallback;
		
		/** The optional user data that is passed to callback function that is called during the animation. */
		void* performAnyAdditionalActionCallbackUserData;

		/** The number of milliseconds until the animation will be actually used by VisualStageControl. */
		uint32 startDelayInMilliseconds;
		
		bool debugMode; /**< True if in debug mode, false otherwise. */
		
		static const uint32 maxNumberOfDebugHistoryEntries; /**< The maximum number of data entries stored in history for debug and monitoring purposes. */
		
		uint32 currDebugHistoryEntry; /**< The current entry of the debug history. */
		
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
			kIsReadyToDie /**< Animation ended and will be destroyed next. */
		} VisualAnimationStatus;
		
		/** The status of the VisualAnimation. */
		VisualAnimationStatus status;

		/** The optional identifier of the collection of the animation (the cluster resp. sequence the current animation is part of). */
		VisualItemIdentifier* collectionIdentifier;

		/** The asset the animation has been added to. */
		const VisualAsset* enclosingAsset;
		
		/** The speed of the animation (the speed is driving value in case no start value has been set). */
		AnimationSpeed speed;
		
		/** True if the animation is supposed to start with the current value of the animated property. Default is true. By calling setStartValue() it is usually set to false. */
		bool doStartAnimationWithCurrentPropertyValue;
		
		/** The start value of the animation as it has been set by setStartValue(). */
		double startValue;
		
		/** The start value of the animation as it has been set by setStartValue(), with data type VisualStagePosition. */
		VisualStagePosition startValueVisualStagePosition;

		/** The stop value of the animation as it has been set by setStopValue(). */
		double stopValue;

		/** The stop value of the animation as it has been set by setStopValue(), with data type VisualStagePosition. */
		VisualStagePosition stopValueVisualStagePosition;

		/**
		 * Whether the animation is bound to speed or duration.
		 */
		typedef enum {
			kDurationBound, /**< The speed of the animation is determined by setting the duration time. */
			kSpeedBound /**< The duration of the animation is dependent on the speed. */
		} DurationSpeedConstraint;
		
		/** The constraint of the animation (whether bound to speed or duration). */
		DurationSpeedConstraint durationSpeedConstraint;

		/** The optional callback function that is called right before the death of the animation cluster. */
		VisualAnimationWillDieCallback willDieCallback;

		/** The optional user data that is passed to callback function that is called right before the death of the animation cluster. */
		void* willDieCallbackUserData;
		
	};

}

#endif /* VisualAnimation_h */
