/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualDownloadQueue.h
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

#ifndef VisualDownloadQueue_h
#define VisualDownloadQueue_h


#include "VisualTypes.h"
#include "VisualItemIdentifier.h"


#if TARGET_OS_MAC
#include <Carbon/Carbon.h> // CFReadStreamRef, CFURLRef
#endif

#if TARGET_OS_WIN
#include <windows.h>
#include <winInet.h>
#endif


#include <vector>


namespace VizKit {

	class VisualObject; // Forward declaration (to avoid include of header file).
	class VisualObjectData; // Forward declaration (to avoid include of header file).
	class VisualString; // Forward declaration (to avoid include of header file).

	/**
	 * Providing networking functionality. 
	 */
	class VisualDownloadQueue {
	
	public:
		
		/**
		 * Disposes the VisualDownloadQueue.
		 */	
		static void dispose(void);

		/**
		 * Adds a URL to the download queue.
		 * @param aURL The URL from where to fetch the data.
		 * @param theRequestingObject The object requesting remote data.
		 * @param identifier An identifier of the download process.
		 * @return True on success, false on failure.
		 */
		static bool addToDownloadQueue(const VisualString& aURL, VisualObject* theRequestingObject, const VisualItemIdentifier& identifier);

		/**
		 * Removes an item from the download queue.
		 * @param identifier Identifier of the download item.
		 * @return True on success, false on failure.
		 */	
		static bool removeDownload(const VisualItemIdentifier& identifier);

		/**
		 * Removes an item from the download queue.
		 * @param visualObjectIdentifier Identifier of the object.
		 * @return True on success, false on failure.
		 */	
		static bool removeDownloadForObject(const VisualItemIdentifier& visualObjectIdentifier);

		/**
		 * If download items are waiting to be fetched, the download process is started.
		 * @return True if download process started, false if not.
		 * @remarks Called by main loop. Download data is fetched in a separate thread.
		 */	
		static bool startDownloadThreadIfNeeded(void);

		/**
		 * Set the internal variable downloadIsProcessing to false.
		 * @remarks This is called after the download thread ended.
		 */	
		static void setDownloadIsProcessingToFalse(void);

		/* A separate thread in which data is downloaded from a remote source.
		 * @param parameter Additional parameters (ignored).
		 * @return Status.
		 * @remarks processDownloadQueue() runs in (maximum) one thread.
		 */
#if TARGET_OS_MAC
		static OSStatus processDownloadQueue(void* parameter);
#endif
#if TARGET_OS_WIN
		static DWORD WINAPI processDownloadQueue(LPVOID lpParam);
#endif

	private:

		/**
		 * Constructs a VisualDownloadQueue. The VisualDownloadQueue acts as a singleton. Returns a pointer to the initialised VisualDownloadQueue.
		 * @return A pointer to the singleton instance.
		 */
		static VisualDownloadQueue* getInstance(void);

		/** The constructor. VisualDownloadQueue is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualDownloadQueue();
		
		/** The destructor. VisualDownloadQueue is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualDownloadQueue();

		/**
		 * Copy constructor.
		 * @param other Another VisualNetwork.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualDownloadQueue(const VisualDownloadQueue& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualDownloadQueue& operator=(const VisualDownloadQueue& other);
	
		/** VisualDownloadQueue is a singleton class. Pointer to private instance is handled internally. */
		static VisualDownloadQueue* theVisualDownloadQueue;
	
		/**
		 * A DownloadItem is an item of the download queue where data is retrieved from a remote source.
		 */
		typedef struct {
			VisualItemIdentifier identifier; /**< The identifier of the download item. */
#if TARGET_OS_MAC
			CFReadStreamRef readStreamRef; /**< The read stream. */
#endif
#if TARGET_OS_WIN
			HINTERNET readRequest; /**< The read stream. */
			HINTERNET session; /**< The session. */
#endif
			VisualObject* requestingObject; /**< Object for which network data is retrieved. */
			VisualObjectData* requestingObjectData; /**< Data of object for which network data is retrieved. */
			size_t bytesRead; /** The number of bytes currently retrieved from the remote source. */
			VisualItemIdentifier timingId; /** The identifier used to ask for timing information. */
			VisualItemIdentifier requestId; /** The identifier of the request. */
			bool streamDidClose; /** True if stream is closed, false if not. */
			bool isActive; /** True if download item is active, false if not. */
		} DownloadItem;

		/** An downloadQueue is a queue of scheduled and running downloads. */
		typedef std::vector<DownloadItem> DownloadQueue;

		/** An DownloadQueueIterator is an iterator of a DownloadQueue. */
		typedef DownloadQueue::iterator DownloadQueueIterator;
		
		/** The queue with the scheduled and running downloads of remote data. */
		DownloadQueue downloadQueue;

		/** An DownloadAddRequestQueue is a queue of DownloadItems which are supposed to be added as soon as possible. */
		typedef std::vector<DownloadItem> DownloadAddRequestQueue;
		
		/** An DownloadAddRequestQueueIterator is an iterator of a DownloadAddRequestQueue. */
		typedef DownloadAddRequestQueue::iterator DownloadAddRequestQueueIterator;
		
		/** The queue with the add requests. */
		DownloadAddRequestQueue downloadAddRequestQueue;
		
		/** Synchronized flag which answers whether the downloadAddRequestQueue is empty or not. */
		bool downloadAddRequestQueueIsEmpty;

		/** An DownloadStopRequestQueue is a queue of VisualItemIdentifiers of DownloadItems which are supposed to be stopped as soon as possible. */
		typedef std::vector<VisualItemIdentifier> DownloadStopRequestQueue;
		
		/** An DownloadStopRequestQueueIterator is an iterator of a DownloadStopRequestQueue. */
		typedef DownloadStopRequestQueue::iterator DownloadStopRequestQueueIterator;
		
		/** The queue with the stop requests. */
		DownloadStopRequestQueue downloadStopRequestQueue;
		
		/**
		 * Answers the question whether there are download items waiting to be fetched.
		 * @return True if download should start, false if not.
		 * @remarks Download data is fetched in a separate thread.
		 */	
		bool shouldStartDownload(void);
		
		/** Answers the question whether currently download data is being fetched. */
		bool downloadIsProcessing;

		/** Signal to stop the download process by external thread. */
		bool stopDownloadProcess;

	};
	
}



#endif /* VisualDownloadQueue_h */
