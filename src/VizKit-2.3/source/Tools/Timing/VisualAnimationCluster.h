/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAnimationCluster.h
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

#ifndef VisualAnimationCluster_h
#define VisualAnimationCluster_h


#include "VisualTypes.h"
#include "VisualAnimationComponent.h"

#include <vector>


namespace VizKit {

	/**
	 * A VisualAnimationCluster is a collection of animations where the animations run concurrently. Animations are added by calling add().
	 */
	class VisualAnimationCluster : public VisualAnimationComponent {

	public:

		/**
		 * The constructor.
		 */
		VisualAnimationCluster();

		/**
		 * The destructor.
		 */		
		~VisualAnimationCluster();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualAnimationCluster.
		 */
		VisualAnimationCluster(const VisualAnimationCluster& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualAnimationCluster.
		 */
		VisualAnimationCluster& operator=(const VisualAnimationCluster& other);
		
		/**
		 * Copies the current VisualAnimationCluster and returns a pointer to a new VisualAnimationCluster.
		 */
		virtual VisualAnimationComponent* clone(void) const;

		/**
		 * Adds an animation to the collection.
		 * @param anAnimation Reference to an animation (single VisualAnimation or clustered/sequenced composite collection of multiple VisualAnimations).
		 */		
		void add(VisualAnimationComponent& anAnimation);

		/**
		 * Executes the callback function to animate a property.
		 */
		virtual void animate(void);

		/**
		 * Updates the state of the animation cluster.
		 * @remarks Called by VisualAnimationQueue. Updates the current value of the animations. Also tests whether an animation is supposed to be started as the start was delayed.
		 */
		virtual void update(void);

		/**
		 * Answers the question whether at least one animation is (still) running.
		 * @return True if at least one of the animations is (still) running. False otherwise.
		 */
		virtual bool isRunning(void) const;

		/**
		 * Answers the question whether the animation ended and is ready to be destroyed.
		 * @return True is the animation ended. False otherwise.
		 * @remarks The animation is redy to die when the duration is exceeded. VisualAnimationQueue acts on a true return value and deletes the VisualAnimation.
		 */
		virtual bool isReadyToDie(void) const;

		/**
		 * Starts the Animation collection.
		 * @remarks Each contained animation is sent a call to start().
		 */
		virtual void start(void);

		/**
		 * Stops the Animation collection.
		 */
		virtual void stop(void);

		/**
		 * Returns the animated property of the animation.
		 * @return The animated property of the animation.
		 * @remarks In case the collection only contains one animation, the animated property of the contained single animation is returned. Otherwise kUndefinedAnimatedProperty is returned.
		 */
		virtual AnimatedProperty getAnimatedProperty(void) const;

		/**
		 * Prepares the animation before it is added to an asset.
		 * @param visualAsset The asset the animation is supposed to be added to..
		 */
		virtual void preparePriorToAddingToAsset(VisualAsset& visualAsset);

		/**
		 * Returns the asset the animation has been added to.
		 * @return The asset the animation has been added to.
		 */
		virtual const VisualAsset* const getEnclosingAsset(void) const;

		/**
		 * Sets the identifier of the parent collection (the collection which the current animation is part of).
		 * @param aCollectionIdentifier The identifier of the parent collection.
		 */
		virtual void setIdentifierOfParentCollection(const VisualItemIdentifier& aCollectionIdentifier);

		/**
		 * Executes the appropriate action for the case that one animation of the current collection died.
		 * @remarks Only approcpriate for collection (like cluster and sequence). VisualAnimation simply returns after printing an error message.
		 */
		virtual void handleOneCollectionItemAnimationDied(void);

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
		 * Returns the VisualAnimation with the specified VisualItemIdentifier.
		 * @param anAnimationIdentifier The identifier of the animation.
		 * @return The VisualAnimation with the specified VisualItemIdentifier.
		 * @remarks Return value can be NULL if no VisualAnimation with the specified VisualItemIdentifier is found.
		 */
		virtual VisualAnimationComponent* getVisualAnimation(const VisualItemIdentifier& anAnimationIdentifier);

	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualAnimationCluster.
		 */
		void copy(const VisualAnimationCluster& other);

		/** Animations are collected as a vector of pointers to VisualAnimationComponents. */
		typedef std::vector<VisualAnimationComponent*> AnimationVector;
		
		/** The AnimationVectorIterator is an iterator of a AnimationVector. */
		typedef AnimationVector::iterator AnimationVectorIterator;

		/** The AnimationVectorConstIterator is a const iterator of a AnimationVector. */
		typedef AnimationVector::const_iterator AnimationVectorConstIterator;

		/** The vector of animations. */
		AnimationVector animationVector;

		/** The optional identifier of the parent collection of the cluster (the cluster resp. sequence the current cluster is part of). */
		VisualItemIdentifier* parentCollectionIdentifier;
		
		/** The asset the animation has been added to. */
		const VisualAsset* enclosingAsset;

		/** The optional callback function that is called during the animation. */
		VisualAnimationPerformAnyAdditionalActionCallback performAnyAdditionalActionCallback;

		/** The optional user data that is passed to callback function that is called during the animation. */
		void* performAnyAdditionalActionCallbackUserData;

		/** The optional callback function that is called right before the death of the animation cluster. */
		VisualAnimationWillDieCallback willDieCallback;

		/** The optional user data that is passed to callback function that is called right before the death of the animation cluster. */
		void* willDieCallbackUserData;
		
	};

}

#endif /* VisualAnimation_h */
