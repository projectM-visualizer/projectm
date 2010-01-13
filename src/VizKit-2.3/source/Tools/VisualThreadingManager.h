/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualThreadingManager.h
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

#ifndef VisualThreadingManager_h
#define VisualThreadingManager_h


#include "VisualTypes.h"
#include "VisualItemIdentifier.h"
#include "VisualThreading.h"

#if TARGET_OS_MAC
//#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h> // signature typedef OSStatus (*ThreadingFuncPtr)(void* parameter);
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/Multiprocessing.h> // MPTaskID
#endif


#if TARGET_OS_WIN
#include <windows.h>
#endif


#include <vector>


namespace VizKit {

/*
#ifndef ThreadingFuncPtr_def
#define ThreadingFuncPtr_def
#if TARGET_OS_MAC
typedef OSStatus (*ThreadingFuncPtr)(void* parameter);
#endif
#if TARGET_OS_WIN
typedef DWORD (*ThreadingFuncPtr)(LPVOID lpParam);
#endif
#endif
*/
	
	/**
	 * VisualThreadingManager holds identifiers of currently running threads. 
	 */
	class VisualThreadingManager {
	
	public:

		/**
		 * Identifiers of critical regions in VizKit.
		 */
		typedef enum {
			downloadQueueSingletonMutex = 0, /**< VisualDownloadQueue initialization. */
			downloadQueueAccessMutex = 1, /**< VisualDownloadQueue access or iteration. */
			addRequestDownloadQueueAccess = 2, /**< Add to download queue. */
			stopRequestDownloadQueueAccess = 3, /**< Remove from download queue. */
			notificationQueueSingletonMutex = 4, /**< VisualNotificationQueue initialization. */
			notificationQueueAccessMutex = 5, /**< VisualNotificationQueue access or iteration. */
			inputNotificationQueueAccessMutex = 6, /**< VisualNotificationQueue access or iteration. */
			timeStoreSingletonMutex = 7, /**< Initialization of VisualTiming. */
			timeStoreAccessMutex = 8 /**< Access of or iteration over internal map of VisualTiming. */
		} VisualCriticalRegion;
		
		/**
		 * Disposes the VisualThreadingManager.
		 */	
		static void dispose(void);

		/**
		 * Creates a new thread by calling task function.
		 * @param aThreadingFuncPtr Pointer to function that should be started in new thread.
		 * @param param Optional name for function-thread. Useful for debugging purposes.
		 * @param callbackAfterThreadDidDie Callback that is called after the created thread did finished execution.
		 * @return True on success, false on failure.
		 */	
		static bool createThread(ThreadingFuncPtr aThreadingFuncPtr, void* param = NULL, VisualThreading::ThreadDidDieCallback callbackAfterThreadDidDie = NULL);

		/**
		 * Returns the number of currently running threads.
		 * @return The number of currently running threads.
		 */
		static uint32 getNumberOfCurrentlyRunningThreads(void);

		/**
		 * Updates the current internal statistics by asking for the status of registered threads.
		 */	
		static void update(void);

		/**
		 * Returns the critical region id of a specific critical region (as declared as enum in VisualCriticalRegion).
		 * @return The critical region id.
		 */
#if TARGET_OS_MAC
		static MPCriticalRegionID getCriticalRegionID(VisualCriticalRegion criticalRegionId);
#endif
#if TARGET_OS_WIN
		static LPCRITICAL_SECTION getCriticalSectionPointer(VisualCriticalRegion criticalRegionId);
#endif

	private:

		/**
		 * Constructs a VisualThreadingManager. The VisualThreadingManager acts as a singleton. Returns a pointer to the initialised VisualThreadingManager.
		 * @return A pointer to the singleton instance.
		 */
		static VisualThreadingManager* getInstance(void);

		/** The constructor. VisualThreadingManager is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualThreadingManager();
		
		/** The destructor. VisualThreadingManager is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualThreadingManager();

		/**
		 * Copy constructor.
		 * @param other Another VisualNetwork.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualThreadingManager(const VisualThreadingManager& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualThreadingManager& operator=(const VisualThreadingManager& other);
	
		/** VisualThreadingManager is a singleton class. Pointer to private instance is handled internally. */
		static VisualThreadingManager* theVisualThreadingManager;

		/**
		 * Adds a thread to the manager.
		 * @param threadId The id of the thread.
		 * @param threadIdentifier VisualItemIdentifier of the thread.
		 * @param callbackAfterThreadDidDie Callback that is called after the created thread did finished execution.
		 */
#if TARGET_OS_MAC
		void addThread(MPTaskID threadId, VisualItemIdentifier* threadIdentifier, VisualThreading::ThreadDidDieCallback callbackAfterThreadDidDie = NULL);
#endif
#if TARGET_OS_WIN
		void addThread(HANDLE threadId, VisualItemIdentifier* threadIdentifier, VisualThreading::ThreadDidDieCallback callbackAfterThreadDidDie = NULL);
#endif

		/** Notification queue which gets called on termination of a thread. */
#if TARGET_OS_MAC
		MPQueueID notificationQueue;
#endif

		/**
		 * A downloadItem is an item of the download queue where data is retrieved from a remote source.
		 */
		typedef struct {
#if TARGET_OS_MAC
			MPTaskID threadId; /**< Task Id (i. e. id of thread). */
#endif
#if TARGET_OS_WIN
			HANDLE threadId; /**< Thread Id (i. e. handle of thread). */
#endif
			VisualItemIdentifier* threadIdentifier; /**< VisualItemIdentifier of the thread. */
			VisualThreading::ThreadDidDieCallback callbackAfterDeathOfThread; /**< Callback is called after death of thread. */
			
		} ThreadItem;

		/** An ThreadVector is a vector of running threads. */
		typedef std::vector<ThreadItem> ThreadVector;
		
		/** An ThreadVectorIterator is an iterator of a ThreadVector. */
		typedef ThreadVector::iterator ThreadVectorIterator;
		
		/** The vector with the running threads. */
		ThreadVector threadVector;

		/** Signal to stop all threads. */
		volatile bool terminateAllThreads;

#if TARGET_OS_MAC
		/** Critical region id for singleton initialization of VisualDownloadQueue. */
		MPCriticalRegionID downloadQueueSingletonCriticalRegion;

		/** Critical region id for accessing or iterating visualDownloadQueue. */
		MPCriticalRegionID downloadQueueAccessCriticalRegion;
		
		/** Critical region id for access of add request queue of VisualDownloadQueue. */
		MPCriticalRegionID downloadQueueAddRequestCriticalRegion;

		/** Critical region id for access of stop request queue of VisualDownloadQueue. */
		MPCriticalRegionID downloadQueueStopRequestCriticalRegion;

		/** Critical region id for singleton initialization of VisualNotificationQueue. */
		MPCriticalRegionID notificationQueueSingletonCriticalRegion;
		
		/** Critical region id for accessing or iterating visualNotificationQueue. */
		MPCriticalRegionID notificationQueueAccessCriticalRegion;

		/** Critical region id for accessing or iterating inputQueue of visualNotificationQueue. */
		MPCriticalRegionID inputNotificationQueueAccessCriticalRegion;
		
		/** Critical region id for singleton initialization of VisualTiming. */
		MPCriticalRegionID timeStoreSingletonCriticalRegion;
		
		/** Critical region id for accessing or iterating map of VisualTiming. */
		MPCriticalRegionID timeStoreAccessCriticalRegion;
#endif
#if TARGET_OS_WIN
		/** Critical section for singleton initialization of VisualDownloadQueue. */
		CRITICAL_SECTION downloadQueueSingletonCriticalSection;

		/** Critical section for accessing or iterating visualDownloadQueue. */
		CRITICAL_SECTION downloadQueueAccessCriticalSection;
		
		/** Critical section for access of add request queue of VisualDownloadQueue. */
		CRITICAL_SECTION downloadQueueAddRequestCriticalSection;

		/** Critical section for access of stop request queue of VisualDownloadQueue. */
		CRITICAL_SECTION downloadQueueStopRequestCriticalSection;

		/** Critical section for singleton initialization of VisualNotificationQueue. */
		CRITICAL_SECTION notificationQueueSingletonCriticalSection;
		
		/** Critical section for accessing or iterating visualNotificationQueue. */
		CRITICAL_SECTION notificationQueueAccessCriticalSection;

		/** Critical section for accessing or iterating inputQueue of visualNotificationQueue. */
		CRITICAL_SECTION inputNotificationQueueAccessCriticalSection;

		/** Critical section for singleton initialization of VisualTiming. */
		CRITICAL_SECTION timeStoreSingletonCriticalSection;
		
		/** Critical section for accessing or iterating map of VisualTiming. */
		CRITICAL_SECTION timeStoreAccessCriticalSection;
#endif
	};
	
}


#endif /* VisualThreadingManager_h */
