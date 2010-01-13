/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualNotification.h
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

#ifndef VisualNotification_h
#define VisualNotification_h


#include "VisualTypes.h"
#include "VisualNotificationKey.h"
#include "VisualObject.h"


namespace VizKit {

	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).
	class VisualActor; // Forward declaration (to avoid include of header file).

	/**
	 * A VisualActor is notified about events and messages with a VisualNotification. 
	 * The VisualActor can query the VisualNotification for details.
	 * The VisualActor can also set a value to pass it back to the caller.
	 */
	class VisualNotification : public VisualObject {

	public:

		/**
		 * The constructor.
		 */
		VisualNotification();

		/**
		 * Another constructor.
		 * @param anIdentifier Identifier of the notification.
		 */
		VisualNotification(const VisualItemIdentifier& anIdentifier);

		/**
		 * The destructor.
		 */
		~VisualNotification();

		/**
		 * Copy constructor.
		 */		
		VisualNotification(const VisualNotification& other);

		/**
		 * Assignment operator.
		 */			
		VisualNotification& operator=(const VisualNotification& other);

		/**
		 * Copies the current VisualNotification and returns a pointer to a new VisualNotification.
		 */
		virtual VisualNotification* clone(void) const;

		/**
		 * Sets the key of a notification. Notification keys are implemented as enums.
		 * @param aKey The enum key.
		 */
		void setKey(const VisualNotificationKey aKey);

		/**
		 * Returns the key of the notification.
		 * @return The key of the notification. Notification keys are implemented as enums.
		 */
		VisualNotificationKey getKey(void) const;

		/**
		 * Sets the value of a notification.
		 * @param someData The pointer to the memory location of the data.
		 * @param dataSizeInBytes The size of the data in bytes.
		 * @remarks The data (not only the pointer) is copied.
		 */
		void setData(const void* const someData, uint32 dataSizeInBytes);

		/**
		 * Returns the untyped data of the notification.
		 * @param[out] dataSizeInBytes The number of bytes the data occupies in memory.
		 * @return The pointer to the memory location of the data.
		 * @remarks To hold the data, the caller has to create and store a copy on his side.
		 */
		void* getData(uint32& dataSizeInBytes) const;

		/**
		 * Sets the object of a notification.
		 * @param anObject The pointer to an object.
		 * @remarks The object is copied by calling clone().
		 */
		void setObject(const VisualObject& anObject);

		/**
		 * Returns a pointer to the object of the notification.
		 * @return A pointer to the object of the notification.
		 * @remarks The object does not get copied automatically. If the caller wants to retain a copy, the data has to be copied by calling its object member function clone().
		 */
		VisualObject* getObject(void) const;

		/**
		 * Sets a pointer to some memory address.
		 * @param pointer A pointer to some memory address.
		 * @remarks If possible, copying the data is preferred (by calling setData()).
		 */
		void setPointer(void* pointer);

		/**
		 * Returns a pointer to some memory address.
		 * @return A pointer to some memory address.
		 * @remarks If possible, copying the data is preferred (by using setData() / getData()).
		 */
		void* getPointer(void);

		/**
		 * The notification is posted to the VisualNotificationQueue (pushed at the end of the VisualNotificationQueue).
		 */
		void post(void);

		/**
		 * Static helper function that converts a VisualNotificationKey to the string. Possibly useful for debugging or tracing purposes.
		 * @param aKey The key of a notification.
		 * @param outString The character string value of the VisualNotificationKey enum value.
		 */
		static void convertNotificationKeyToString(const VisualNotificationKey aKey, char* outString);

		/**
		 * Static function for convenient posting of simple enum value of VisualNotificationKey.
		 * @param aKey The notification key to post.
		 * @remarks The notification is pushed at the end of the VisualNotificationQueue.
		 */
		static void post(const VisualNotificationKey aKey);
		
		/**
		 * A VisualActor can register for an event/message. The notification is passed with VisualActor::handleNotification().
		 * @param aVisualActor A visual actor.
		 * @param aNotificationKey An enum that denotes a notification.
		 */
		static void registerNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey);
		
		/**
		 * A VisualActor can be removed from the list of observers for a specific notification.
		 * @param aVisualActor A visual actor.
		 * @param aNotificationKey An enum that denotes a notification.
		 */
		static void removeNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey);
		
	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualNotification.
		 */
		void copy(const VisualNotification& other);
		
		/** Enum value of notification as uint16. */
		VisualNotificationKey notificationEnumKey;
		
		/** Optional data of notification. */
		void* notificationData;
		
		/** The length of the optional data in bytes. */
		uint32 notificationDataSize;
		
		/** Optional object of the notification. */
		VisualObject* notificationObject;
		
		/** Internally stored pointer to some memory address. */
		void* notificationPointer;

	};

}

#endif /* VisualNotification_h */
