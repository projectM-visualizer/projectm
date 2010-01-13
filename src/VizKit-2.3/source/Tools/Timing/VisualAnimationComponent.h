/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAnimationComponent.h
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

#ifndef VisualAnimationComponent_h
#define VisualAnimationComponent_h


#include "VisualTypes.h"
#include "VisualObject.h"
#include "VisualAnimationTypes.h"


namespace VizKit {

	class VisualAnimationComponent; // Forward declaration

	/** Callback to do any additional action or check for any condition in duration of animation component*/
	typedef void (*VisualAnimationPerformAnyAdditionalActionCallback)(VisualAnimationComponent* animationComponent, void* userData);

	/** Callback to notify of death of animation component*/
	typedef void (*VisualAnimationWillDieCallback)(VisualAnimationComponent* animationComponent, void* userData);

	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).
	class VisualAsset; // Forward declaration (to avoid include of header file).

	/**
	 * Abstract base class for composite implementation of Animation.
	 */
	class VisualAnimationComponent : public VisualObject {

	public:

		/**
		 * The destructor.
		 */		
		virtual ~VisualAnimationComponent();

		/**
		 * Copies the current VisualAnimationComponent and returns a pointer to a new VisualAnimationComponent.
		 * @remarks We are overriding VisualObject's virtual clone() method to ensure to return a VisualAnimationComponent.
		 */
		virtual VisualAnimationComponent* clone(void) const = 0;

		/**
		 * Executes the callback function to animate a property.
		 */
		virtual void animate(void) = 0;

		/**
		 * Updates the state of the animation.
		 * @remarks Called by VisualAnimationQueue. Updates the current value of the animation. Also tests whether the animation is supposed to be started as the start was delayed.
		 */
		virtual void update(void) = 0;

		/**
		 * Answers the question whether the animation is (still) running.
		 * @return True is the animation is (still) running. False otherwise.
		 */
		virtual bool isRunning(void) const = 0;

		/**
		 * Answers the question whether the animation ended and is ready to be destroyed.
		 * @return True if the animation ended. False otherwise.
		 * @remarks The animation is ready to die when the duration is exceeded. VisualAnimationQueue acts on a true return value and deletes the VisualAnimation.
		 */
		virtual bool isReadyToDie(void) const = 0;

		/**
		 * Starts the Animation.
		 * @remarks In case the start is delayed, the animation is only running after the delaying milliseconds did have passed.
		 */
		virtual void start(void) = 0;

		/**
		 * Stops the Animation.
		 */
		virtual void stop(void) = 0;

		/**
		 * Returns the animated property of the animation.
		 * @return The animated property of the animation.
		 */
		virtual AnimatedProperty getAnimatedProperty(void) const = 0;

		/**
		 * Prepares the animation before it is added to an asset.
		 * @param anAsset The asset the animation is supposed to be added to.
		 * @remarks Called by VisualAsset.
		 */
		virtual void preparePriorToAddingToAsset(VisualAsset& anAsset) = 0;

		/**
		 * Returns the asset the animation has been added to.
		 * @return The asset the animation has been added to.
		 */
		virtual const VisualAsset* const getEnclosingAsset(void) const = 0;

		/**
		 * Sets the identifier of the parent collection (the collection which the current animation is part of).
		 * @param aCollectionIdentifier The identifier of the parent collection.
		 */
		virtual void setIdentifierOfParentCollection(const VisualItemIdentifier& aCollectionIdentifier) = 0;

		/**
		 * Sets the callback function that is repeatedly called back during animation component's duration time.
		 * @param theCallback Pointer to c-style function (static class function).
		 * @param userData Additional data (e.g. pointer to instance variable of initialized class).
		 */
		virtual void setCallbackToPerformAnyAdditionalAction(VisualAnimationPerformAnyAdditionalActionCallback theCallback, void* userData = NULL) = 0;

		/**
		 * Sets the callback function that is called right before the death of the animation component.
		 * @param theCallback Pointer to c-style function (static class function).
		 * @param userData Additional data (e.g. pointer to instance variable of initialized class).
		 */
		virtual void setCallbackToNotifyBeforeDeath(VisualAnimationWillDieCallback theCallback, void* userData = NULL) = 0;

		/**
		 * Executes the appropriate action for the case that one animation of the current collection died.
		 * @remarks Only appropriate for collection (like cluster and sequence). VisualAnimation simply returns after printing an error message.
		 */
		virtual void handleOneCollectionItemAnimationDied(void) = 0;

		/**
		 * Returns the VisualAnimation with the specified VisualItemIdentifier.
		 * @param anAnimationIdentifier The identifier of the animation.
		 * @return The VisualAnimation with the specified VisualItemIdentifier.
		 * @remarks Return value can be NULL if no VisualAnimation with the specified VisualItemIdentifier is found.
		 */
		virtual VisualAnimationComponent* getVisualAnimation(const VisualItemIdentifier& anAnimationIdentifier) = 0;

		/**
		 * Calls any callback functions right before the death of the animation component.
		 */
		virtual void notifyBeforeDeath(void) = 0;

	protected:
	
		/**
		 * The constructor.
		 */
		VisualAnimationComponent();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualAnimationComponent.
		 */
		VisualAnimationComponent(const VisualAnimationComponent& other);

	};

}

#endif /* VisualAnimationComponent_h */
