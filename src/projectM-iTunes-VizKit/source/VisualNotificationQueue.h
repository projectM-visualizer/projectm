/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualNotificationQueue.h
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

#ifndef VisualNotificationQueue_h
#define VisualNotificationQueue_h


#include "VisualTypes.h"
#include "VisualNotification.h"

#include <queue>


namespace VizKit {

	/**
	 * Queue of notifications. Singleton.
	 */
	class VisualNotificationQueue {
	
	public:

		/**
		 * Returns a pointer to the instance. 
		 * Initializes the VisualNotificationQueue class if neccessary. 
		 * The VisualNotificationQueue is of singleton type.
		 * @return A pointer to the initialized VisualNotificationQueue.
		 */
		static VisualNotificationQueue* getInstance(void);

		/**
		 * Cleans up the VisualNotificationQueue singleton.
		 */	
		static void dispose(void);

		/**
		 * Pushes a Notification at the end of the VisualNotificationQueue.
		 * @param aNotification The VisualNotification.
		 */
		static void push(VisualNotification& aNotification);

		/**
		 * Erases the top VisualNotification of the VisualNotificationQueue.
		 */			
		static VisualNotification pop(void);

		/**
		 * Returns the number of items currently stored in VisualNotificationQueue.
		 */		
		static size_t size(void);

		/**
		 * Updates the notification queue with the most recent input notifications.
		 */	
		static void update(void);

	private:
	
		/** VisualNotificationQueue is a singleton class. Pointer to private instance is handled internally. */
		static VisualNotificationQueue* visualNotificationQueueInstance;
	
		/** The constructor. VisualNotificationQueue is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualNotificationQueue();
		
		/** The destructor. VisualNotificationQueue is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualNotificationQueue();

		/**
		 * Copy constructor.
		 * @param other Another VisualNotificationQueue.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualNotificationQueue(const VisualNotificationQueue& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualNotificationQueue& operator=(const VisualNotificationQueue& other);
		
		/** A NotificationQueue is a dequeue of notifications.
		 * @remarks The VisualNotificationQueue stores its events as a NotificationQueue.
		 */
		typedef std::deque<VisualNotification> NotificationQueue;
		
		/** A NotificationQueueIterator is an iterator of the NotificationQueue. */
		typedef NotificationQueue::iterator NotificationQueueIterator;

		/** The queue of the notifications. */
		NotificationQueue notificationQueue;
		
		/**
		 * Input queue of notifications.
		 * @remarks On push the arrived notification is stored in inputNotificationQueue to exclude concurrency conflicts with processing of notificationQueue in VisualMainAction::renderAction().
		 */
		NotificationQueue inputNotificationQueue;
		
	};
	
}

#endif /* VisualNotificationQueue_h */
