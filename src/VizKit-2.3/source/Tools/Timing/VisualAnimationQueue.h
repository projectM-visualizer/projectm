/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAnimationQueue.h
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

#ifndef VisualAnimationQueue_h
#define VisualAnimationQueue_h


#include "VisualTypes.h"
#include "VisualItemIdentifier.h"
#include "VisualAnimationTypes.h"

#include <vector>


namespace VizKit {

	class VisualAnimationComponent; // Forward declaration (to avoid include of header file).
	class VisualAsset; // Forward declaration (to avoid include of header file).

	/**
	 * Queue of animations. Singleton. All methods are static for convenient access.
	 */
	class VisualAnimationQueue {
	
	public:

		/**
		 * Cleans up the VisualAnimationQueue singleton.
		 */	
		static void dispose(void);

		/**
		 * Pushes a pointer to a VisualAnimationComponent at the end of the VisualAnimationQueue.
		 * @param anAnimation The animation.
		 * @param anOwnerIdentifier Identifier associated with the animation.
		 * @remarks The animation is copied by calling clone() before it is added to the queue.
		 */
		static void push(const VisualAnimationComponent& anAnimation, const VisualItemIdentifier& anOwnerIdentifier);

		/**
		 * Returns the number of animations currently stored in VisualAnimationQueue.
		 */		
		static size_t size(void);

		/**
		 * Removes all VisualAnimations where the associated VisualItemIdentifier matches.
		 * @param anOwnerIdentifier An identifier that is connected with the animation.
		 * @param anAnimatedProperty The animated property of the animation.
		 */
		static void removeVisualAnimationsWithOwnerIdentifier(const VisualItemIdentifier& anOwnerIdentifier, const AnimatedProperty anAnimatedProperty);

		/**
		 * Removes all VisualAnimations where the associated VisualItemIdentifier matches.
		 * @param anOwnerIdentifier An identifier that is connected with the animation.
		 */
		static void removeVisualAnimationsWithOwnerIdentifier(const VisualItemIdentifier& anOwnerIdentifier);

		/**
		 * Removes VisualAnimations of an asset.
		 * @param anAsset The asset.
		 */
		static void removeVisualAnimationsOfAsset(const VisualAsset* const anAsset);

		/**
		 * Copies animations from one asset to another.
		 * @param sourceAsset The source asset (the asset from which the animations are copied).
		 * @param destinationAsset The destination asset (the asset to which the animations are copied).
		 */
		static void copyAnimationsFromAssetToAsset(const VisualAsset& sourceAsset, VisualAsset& destinationAsset);

		/**
		 * Removes the VisualAnimation where the VisualItemIdentifier matches.
		 * @param animationIdentifier The identifier of the VisualAnimation which should be removed.
		 */
		static void removeVisualAnimation(const VisualItemIdentifier& animationIdentifier);

		/**
		 * Returns the VisualAnimation with the specified VisualItemIdentifier.
		 * @param anAnimationIdentifier The identifier of the animation.
		 * @return The VisualAnimation with the specified VisualItemIdentifier.
		 * @remarks Return value can be NULL if no VisualAnimation with the specified VisualItemIdentifier is found.
		 */
		static VisualAnimationComponent* getVisualAnimation(const VisualItemIdentifier& anAnimationIdentifier);

		/**
		 * Returns the VisualAnimation where the VisualItemIdentifier and the animated property matches.
		 * @param anOwnerIdentifier An identifier that is connected with the animation.
		 * @param anAnimatedProperty Parameter that specifies the animated property of the animations.
		 * @return The VisualAnimation where the VisualItemIdentifier and the animated property matches.
		 * @remarks Return value can be NULL.
		 */
		static const VisualAnimationComponent* const getFirstVisualAnimation(const VisualItemIdentifier& anOwnerIdentifier, const AnimatedProperty& anAnimatedProperty);

		/**
		 * Processes te animations of the queue.
		 * @remarks Called by VisualStageControl prior to show of ensemble.
		 */
		static void processAnimations(void);

	private:
	
		/** VisualAnimationQueue is a singleton class. Pointer to private instance is handled internally. */
		static VisualAnimationQueue* theVisualAnimationQueue;
	
		/** The constructor. VisualAnimationQueue is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualAnimationQueue();
		
		/** The destructor. VisualAnimationQueue is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualAnimationQueue();

		/**
		 * Copy constructor.
		 * @param other Another VisualAnimationQueue.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualAnimationQueue(const VisualAnimationQueue& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualAnimationQueue& operator=(const VisualAnimationQueue& other);

		/**
		 * Returns a pointer to the instance. 
		 * Initializes the VisualAnimationQueue class if neccessary. 
		 * The VisualAnimationQueue is of singleton type.
		 * @return A pointer to the initialized VisualAnimationQueue.
		 */
		static VisualAnimationQueue* getInstance(void);

		/** An AnimationQueue is a queue of animations.
		 * @remarks The VisualAnimationQueue stores its events as pointers to VisualAnimationComponents and VisualItemIdentifiers as keys for later retrieval.
		 */
		typedef std::multimap<VisualItemIdentifier, VisualAnimationComponent*> AnimationQueue;
		
		/** A AnimationQueueIterator is an iterator of a AnimationQueue. */
		typedef AnimationQueue::iterator AnimationQueueIterator;

		/** A ConstAnimationQueueIterator is a constant iterator of a AnimationQueue. */
		typedef AnimationQueue::const_iterator ConstAnimationQueueIterator;

		/** The queue of the animations. */
		AnimationQueue animationQueue;

	};
	
}

#endif /* VisualAnimationQueue_h */
