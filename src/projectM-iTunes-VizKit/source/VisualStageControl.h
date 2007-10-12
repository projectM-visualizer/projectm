/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualStageControl.h
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

#ifndef VisualStageControl_h
#define VisualStageControl_h

#include "VisualPlayerState.h" // ShowMode
#include "VisualItemIdentifier.h"
#include "VisualAnimation.h"

#include <map>

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class VisualNotification; // Forward declaration (to avoid include of header file).
	class VisualEnsemble; // Forward declaration (to avoid include of header file).
	class VisualPlayerState; // Forward declaration (to avoid include of header file).

	/**
	 * Mediator that controls the show of the VisualEnsemble.
	 * All public methods are declared static.
	 * Controls the show of the VisualEnsemble and communicates with individual actors.
	 * The VisualStageControl can manipulate the behaviour of individual VisualActors.
	 */
	class VisualStageControl {

	public:

		/**
		 * Disposes the VisualStageControl.
		 */
		static void dispose(void);

		/**
		 * Sends each actor a message to prepare, perform and finish show.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 */
		static void doEnsembleShow(const VisualPlayerState& visualPlayerState);

		/**
		 * Dispatches a VisualNotification.
		 * @param aNotification A notification.
		 */
		static void dispatchNotification(const VisualNotification& aNotification);

		/**
		 * Checks whether the fade out event should be sent.\ If it should, a notification is added to the queue.
		 * @remarks The kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt event is sent if remaining audio time of track is less than kFadeOutTimeBeforeEndOfTrackInMS.
		 */
		static void checkForFadeOutEvent(void);
		
		/**
		 * Adds a VisualAnimation.
		 * @param aVisualAnimation A VisualAnimation to add.
		 * @param anOwnerIdentifier The identifier of an owner who is connected with the animation.
		 * @return The identifier of the added VisualAnimation.
		 * @remarks Since the VisualAnimation is copied by the VisualStageControl, the returned VisualItemIdentifier provides access to the animation afterwards for the caller.
		 */
		static const VisualItemIdentifier* const addVisualAnimation(VisualAnimation& aVisualAnimation, VisualItemIdentifier& anOwnerIdentifier);

		/**
		 * Removes all VisualAnimations where the associated VisualItemIdentifier matches.
		 * @param anOwnerIdentifier The identifier of a VisualAsset that is connected with the animation.
		 * @param anAnimatedProperty Optional parameter that specifies the animated property of the animations.
		 */
		static void removeVisualAnimationsWithOwnerIdentifier(VisualItemIdentifier& anOwnerIdentifier, AnimatedProperty anAnimatedProperty = kUndefinedAnimatedProperty);

		/**
		 * Removes the VisualAnimation where the VisualItemIdentifier matches.
		 * @param animationIdentifier The identifier of the VisualAnimation which should be removed.
		 */
		static void removeVisualAnimation(const VisualItemIdentifier* const animationIdentifier);

		/**
		 * Resets the iterator over the animations to the first one where the identifier matches as a preparation to iteration.
		 * @param aVisualAssetIdentifier The identifier of a VisualAsset that is connected with a animation.
		 */
		static void resetVisualAnimationsIterIndex(VisualItemIdentifier& aVisualAssetIdentifier);

		/**
		 * Advances the internal animation iterator that was set with resetVisualAnimationsIterIndex().
		 */
		static void advanceAnimationIterator(void);

		/**
		 * Returns the next VisualAnimation with each call.
		 * @return The next VisualAnimation with each call.
		 */
		static VisualAnimation* getNextVisualAnimation(void);

		/**
		 * Key-events are processed.
		 * @param keyboadVal The value of the key that was pressed.
		 * @param showMode The show mode.
		 * @return noErr for any keyPress that is handled completely by the plug-in, 
		 * unimpErr for any keyPress that should be passed through to iTunes.
		 */
		static OSStatus handleKeyPressed(const char keyboadVal, const PlayerShowMode showMode);

	private:

		/** The constructor.\ VisualStageControl is a singleton class.\ The constructor is private.\ New instance of class can only be created internally. */
		VisualStageControl();

		/** The destructor.\ VisualStageControl is a singleton class.\ The destructor is private.\ Instance of class can only be destructed internally. */
		~VisualStageControl();

		/**
		 * Initializes the VisualStageControl if neccessary. Returns a pointer to the VisualStageControl.
		 * The VisualStageControl is a singleton.
		 * @return A pointer to the VisualStageControl.
		 */
		static VisualStageControl* getInstance(void);

		/**
		 * Copy constructor.
		 * @param other Another VisualStageControl.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualStageControl(const VisualStageControl& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualStageControl& operator=(const VisualStageControl& other);

		/**
		 * Initializes the VisualStageControl.\ Returns a pointer to the initialised VisualStageControl.\ The VisualStageControl is a singleton.
		 * @return A pointer to the initialised VisualStageControl.
		 * @remarks In VisualStageControl's initVisualStageControl() the VisualActors are constructed and registered for notification messages.
		 */
		VisualStageControl* initVisualStageControl(void);

		/**
		 * The VisualStageControl is processed.
		 * The VisualStageControl is processed before the start of the show of the VisualEnsemble.
		 * The actors of the show can be prepared or manipulated according to rules of the VisualStageControl.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 */
		void processVisualStageControl(const VisualPlayerState& visualPlayerState);

		/** VisualStageControl is a singleton class. Pointer to private instance is handled internally. */
		static VisualStageControl* theVisualStageControl;
		
		/** A pointer to a visual ensemble. */
		VisualEnsemble* aVisualEnsemble;

		/** VisualAnimations is a multimap of VisualAnimations (VisualItemIdentifiers act as keys for later access by the owner). */
		typedef std::multimap<VisualItemIdentifier, VisualAnimation> VisualAnimations;

		/** A VisualAnimationsIterator is an iterator of VisualAnimations. */
		typedef VisualAnimations::iterator VisualAnimationsIterator;

		/** Vector of all currently running VisualAnimations. */
		VisualAnimations visualAnimations;
		
		/** Internal persistent iterator that is used by getNextVisualAnimation(). */
		VisualAnimationsIterator persistentAnimationIterator;
		
		/** Internal persistent identifier that is used by getNextVisualAnimation(). */
		VisualItemIdentifier persistentIdentifier;

	};

}


#endif /* VisualStageControl_h */
