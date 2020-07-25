/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualHostCommunication.h
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

#ifndef VisualHostCommunication_h
#define VisualHostCommunication_h


#include "VisualTypes.h"


namespace VizKit {

	/**
	 * Collection of static methods to communicate with iTunes host application (e.g. to receive album cover artwork data).
	 */
	class VisualHostCommunication {
	
	public:

		/**
		 * Retrieves the cover artwork of the current audio track.
		 * @param[out] albumCoverArtworkImageData The image data of the album cover artwork data.
		 * @param[out] numberOfBytes The size in bytes of the album cover artwork data.
		 * @return The number of cover artworks.
		*/
		static int getCurrentTrackCoverArt(void** albumCoverArtworkImageData, uint32& numberOfBytes);

		/**
		 * Disposes the cover artwork data of the current audio track as it has been retrieved the last time with getCurrentTrackCoverArt().
		 */
		static void dispose(void);

	private:
	
		/** VisualHostCommunication is a singleton class. Pointer to private instance is handled internally. */
		static VisualHostCommunication* theVisualHostCommunication;
	
		/** The constructor. VisualHostCommunication is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualHostCommunication();
		
		/** The destructor. VisualHostCommunication is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualHostCommunication();

		/**
		 * Returns a pointer to the instance. 
		 * Initializes the VisualNotificationQueue class if neccessary. 
		 * The VisualNotificationQueue is of singleton type.
		 * @return A pointer to the initialized VisualNotificationQueue.
		 */
		static VisualHostCommunication* getInstance(void);

		/**
		 * The Mac memory handle of the cover album artwork image data.
		 * @remarks Not typed as Handle to avoid MacType dependency.
		 */
		void* coverArtImageDataHandle;

	};
	
}

#endif /* VisualHostCommunication_h */
