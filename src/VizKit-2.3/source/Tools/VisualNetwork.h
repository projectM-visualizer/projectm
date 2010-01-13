/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualNetwork.h
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

#ifndef VisualNetwork_h
#define VisualNetwork_h

#include "VisualTypes.h"


#if TARGET_OS_MAC
#include <Carbon/Carbon.h> // CFStringRef, CFURLRef
#endif


#include <vector>


namespace VizKit {

	class VisualString; // Forward declaration (to avoid include of header file).
	class VisualObject; // Forward declaration (to avoid include of header file).
	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).

	/**
	 * Providing networking functionality. 
	 */
	class VisualNetwork {
	
	public:
		
		/**
		 * Disposes resources and allocations of VisualNetwork (VisualDownloadQueue).
		 */	
		static void dispose(void);

		/**
		 * Adds an URL to the download queue.
		 * @param aURL The URL from where to fetch the data.
		 * @param visualObject The object requesting remote data.
		 * @param identifier Identifier of the download item.
		 * @return True on success, false on failure.
		 */	
		static bool addToDownloadQueue(VisualString& aURL, VisualObject* visualObject, const VisualItemIdentifier& identifier);

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
		 * @remarks Download data is fetched in a separate thread.
		 */
		static bool startDownloadThreadIfNeeded(void);

		/**
		 * Ensures a normalized URL reference (by escaping special characters).
		 * @param str A possibly non-escaped URL string.
		 * @return A normalized URL string.
		 * @remarks The caller has to delete the created string.
		 */
		static VisualString* createNormalizedURLString(const VisualString& str);

	private:

		/** The constructor. VisualNetwork is a collection of static methods. Class does not need any instances. Constructor is private and not implemented. */
		VisualNetwork();
		
		/** The destructor. VisualNetwork is a collection of static methods. Class does not need any instances. Destructor is private and not implemented. */
		~VisualNetwork();

	};
	
}


#endif /* VisualNetwork_h */
