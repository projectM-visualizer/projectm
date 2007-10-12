/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualEnsemble.h
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

#ifndef VisualEnsemble_h
#define VisualEnsemble_h

#include "VisualActor.h"
#include "VisualNotificationKey.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif

#include <vector>
#include <map>


namespace VizKit {

	/**
	 * The group of all actors. Only the VisualStageControl creates its own VisualEnsemble and communicates with it.
	 * The VisualEnsemble sends VisualNotifications to VisualActors that are registered as observers.
	 */

	class VisualEnsemble {

	public:

		/**
		 * The constructor.
		 */
		VisualEnsemble();

		/**
		 * The destructor.
		 */
		~VisualEnsemble();

		/**
		 * Copy constructor.
		 */		
		VisualEnsemble(const VisualEnsemble& other);

		/**
		 * Assignment operator.
		 */			
		VisualEnsemble& operator=(const VisualEnsemble& other);

		/**
		 * Adds a VisualActor to the VisualEnsemble.
		 * @param aVisualActor A pointer to a VisualActor.
		 * @remarks VisualEnsemble stores the pointer to the VisualActor and deletes the memory when VisualEnsemble is destructed.
		 */
		void addEnsembleMember(VisualActor* aVisualActor);

		/**
		 * Shows the show of the ensemble.
		 * The show method of all active actors is called.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 * @return 0 for no error. 1 if error occured.
		 */
		UInt8 showEnsemble(const VisualPlayerState& visualPlayerState);

		/**
		 * Resets the index of the internal iterator.
		 * The VisualEnsemble iterates over all actors. 
		 * This functions reset the internal pointer to the first actor.
		 */
		void resetVisualActorIterIndex(void);

		/**
		 * Resets the index of the internal iterator.
		 * The VisualEnsemble iterates over all actors. 
		 * This functions reset the internal pointer to the first actor.
		 * @return The next VisualActor. NULL if there is no next actor.
		 */
		VisualActor* getNextVisualActor(void);

		/**
		 * The VisualEnsemble receives notifications that are passed to the actors of the ensemble.
		 * Notifications ensure loose connections between external processes and the VisualEnsemble.
		 * External processes can package and send a notification to the VisualEnsemble.
		 * The VisualEnsemble processes the notification and each registered actor receives the notification (message/package).
		 * @param aNotification A notification object.
		 */
		void dispatchNotification(const VisualNotification& aNotification);

		/**
		 * Any actor can register for an event.
		 * Each registered actor (observer) receives a notification of the requested kind.
		 * @param aVisualActor A visual actor.
		 * @param aNotificationKey An enum that denotes a notification.
		 */
		void registerObserverForNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey);

		/**
		 * Any actor that can register for an event can also be removed from the list of observers.
		 * @param aVisualActor A visual actor.
		 * @param aNotificationKey An enum that denotes a notification.
		 */
		void removeObserverOfNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey);

		/**
		 * Returns the state of the actor expressed as visualActorState.
		 * @param aVisualActorName The name of the visual actor.
		 * @return The state of the actor expressed as visualActorState.
		 */
		VisualActorState getStateOfVisualActor(const char* const aVisualActorName);

		/**
		 * Returns a pointer to a VisualActor whose name is aVisualActorName.
		 * @param aVisualActorName The name of the visual actor.
		 * @return A pointer to a VisualActor whose name is aVisualActorName.
		 */	
		VisualActor* getVisualActorByName(const char* const aVisualActorName);

	private:
		
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualEnsemble.
		 */
		void copy(const VisualEnsemble& other);
		
		/** Current index of internal iterator. */
		UInt16 visualActorIterIndex;
		
		/** VisualEnsembleActors are collected as a vector of pointers to VisualActors. */
		typedef std::vector<VisualActor*> VisualEnsembleActors;
		
		/** Vector of all actors of the ensemble. */
		VisualEnsembleActors visualEnsembleActors;
		
		/** The VisualEnsembleActorsIterator is an iterator of the VisualEnsembleActors. */
		typedef VisualEnsembleActors::iterator VisualEnsembleActorsIterator;
		
		/** The ObserverMap is a multimap of events/notifications and actors. */
		typedef std::multimap<VisualNotificationKey, VisualActor*> ObserverMap;
		
		/** Multimap of all actors of the ensemble that are registered as observers. */
		ObserverMap observerMap;
		
		/** The ObserverMapIterator is an iterator of the ObserverMap. */
		typedef ObserverMap::iterator ObserverMapIterator;
		
	};

}

#endif /* VisualEnsemble_h */
