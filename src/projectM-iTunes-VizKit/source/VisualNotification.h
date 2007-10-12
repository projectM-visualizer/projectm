/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualNotification.h
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

#ifndef VisualNotification_h
#define VisualNotification_h

#include "VisualNotificationKey.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	/** Callback function. */
	typedef void (*VisualNotificationCallback)(void* userData);

	/**
	 * A message that can be sent between VisualStageControl and VisualEnsemble.
	 * Messaging object that is passed to VisualEnsemble.
	 * VisualActors are notified about events and messages via an object. 
	 * The VisualActors can query the VisualNotification for details.
	 * The VisualActors can also set the value to pass it back to the caller.
	 */
	class VisualNotification {

	public:

		/**
		 * The constructor.
		 */
		VisualNotification();

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
		 * Sets the key of a notification. Notification keys are implemented as enums.
		 * @param aKey The enum key.
		 */
		void setKey(const VisualNotificationKey aKey);

		/**
		 * Returns the key of the notification.
		 * @return The key of the notification.\ Notification keys are implemented as enums.
		 */
		VisualNotificationKey getKey(void) const;

		/**
		 * Sets the value of a notification.
		 * @param aValue The pointer to the memory location of the value.
		 * @param valueLengthInBytes The length of the value in bytes.
		 * @remarks The value (not only the pointer) is copied.
		 */
		void setValue(const void* const aValue, UInt32 valueLengthInBytes);

		/**
		 * Returns the value of the notification.
		 * @param[out] numberOfBytes The number of bytes the value occupies in memory.
		 * @return The pointer to the memory location of the value.
		 */
		const void* const getValue(UInt32& numberOfBytes) const;

		/**
		 * Sets the second value of a notification.
		 * @param aCallbackFunction Pointer to a callback function.
		 * @param someUserData Optional user data.
		 */
		void setCallbackFunction(VisualNotificationCallback aCallbackFunction, void* someUserData = NULL);

		/**
		 * Calls the callback function.
		 */
		void callCallbackFunction(void);

		/**
		 * The notification is posted to the VisualNotificationQueue (pushed at the end of the VisualNotificationQueue).
		 */
		void post(void);

		/**
		 * Static helper function that converts a VisualNotificationKey to the string.\ Possibly useful for debugging or tracing purposes.
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
		
	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualNotification.
		 */
		void copy(const VisualNotification& other);
		
		/** Enum value of notification as UInt16. */
		VisualNotificationKey notificationEnumKey;
		
		/** Optional value of notification as pure data (unspecified datatype void). */
		void* notificationValue;
		
		/** The length of the optional value in bytes. */
		UInt32 notificationValueLength;

		/** Callback function. */
		VisualNotificationCallback callbackFunction;
		
		/** Internally stored pointer to provided user data (e.g.\ pointer to instance variable of initialized class). */
		void* userData;
	};

}

#endif /* VisualNotification_h */
