/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualStageControl.h
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

#ifndef VisualStageControl_h
#define VisualStageControl_h


#include "VisualTypes.h"
#include "VisualPlayerState.h" // PlayerShowMode
#include "VisualNotificationKey.h"


namespace VizKit {

	class VisualNotification; // Forward declaration (to avoid include of header file).
	class VisualEnsemble; // Forward declaration (to avoid include of header file).
	class VisualPlayerState; // Forward declaration (to avoid include of header file).
	class VisualActor; // Forward declaration (to avoid include of header file).

	/**
	 * Singleton mediator that controls the show of the VisualEnsemble.
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
		 * Returns a pointer to a VisualActor whose name is aVisualActorName.
		 * @param aVisualActorName The name of the visual actor.
		 * @return A pointer to a VisualActor whose name is aVisualActorName or NULL if none is found.
		 */	
		static VisualActor* getVisualActorByName(const char* const aVisualActorName);

		/**
		 * Registers a VisualActor for a specific notification (event/message).
		 * @param aVisualActor A visual actor.
		 * @param aNotificationKey An enum that denotes a notification.
		 */
		static void registerObserverForNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey);
		
		/**
		 * Removes a VisualActor from the list of observers for a specific notification (event/message).
		 * @param aVisualActor A visual actor.
		 * @param aNotificationKey An enum that denotes a notification.
		 */
		static void removeObserverOfNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey);
		
		/**
		 * Dispatches a VisualNotification.
		 * @param aNotification A notification.
		 */
		static void dispatchNotification(VisualNotification& aNotification);

		/**
		 * Checks whether the fade out event should be sent. If it should, a notification is added to the queue.
		 * @remarks The kAudioPlayReachedFadeOutTimeEvt event is sent if remaining audio time of track is less than kFadeOutTimeBeforeEndOfTrackInMS.
		 */
		static void checkForFadeOutEvent(void);

		/**
		 * Processes keyboard events.
		 * @param keyboadVal The value of the key that was pressed.
		 * @param showMode The show mode.
		 */
		static void handleKeyPressed(const char keyboadVal, const PlayerShowMode showMode);

	private:

		/** The constructor. VisualStageControl is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualStageControl();

		/** The destructor. VisualStageControl is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
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
		 * Initializes the VisualStageControl.
		 * @remarks In VisualStageControl's init() the VisualActors are constructed and registered for notification messages.
		 */
		void init(void);

		/** VisualStageControl is a singleton class. Pointer to private instance is handled internally. */
		static VisualStageControl* theVisualStageControl;
		
		/** A pointer to a visual ensemble. */
		VisualEnsemble* aVisualEnsemble;

	};

}


#endif /* VisualStageControl_h */
