/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualHostCommunication.h
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

#ifndef VisualHostCommunication_h
#define VisualHostCommunication_h


#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	/**
	 * Collection of static methods to communicate with iTunes host application (e.g.\ to receive album cover artwork data).
	 */
	class VisualHostCommunication {
	
	public:

		/**
		 * Returns the cover artwork of the current audio track.
		 * @param[out] albumCoverArtworkFileType The file type of the album cover artwork.
		 * @param[out] albumCoverArtworkHandle A handle to the album cover artwork data.
		 * @return The number of cover artworks.
		*/
		static UInt16 getCurrentTrackCoverArt(OSType* albumCoverArtworkFileType, Handle* albumCoverArtworkHandle);

		/**
		 * Tells iTunes about our preferred fullscreen resolution.
		 * Once the preferred display resolution has been set it can not be revoked.
		 * Only with the next restart of iTunes the default resolution of iTunes can be set again (by not calling PlayerSetFullScreenOptions).
		 * @param minBitsPerPixel Minimum bit depth.
		 * @param maxBitsPerPixel Maximum bit depth.
		 * @param preferredBitsPerPixel Preferred bit depth.
		 * @param horizontalPixels Desired width.
		 * @param verticalPixels Desired height.
		 */
		static OSStatus setPreferredDisplayResolution(UInt16 minBitsPerPixel, UInt16 maxBitsPerPixel, UInt16 preferredBitsPerPixel, UInt16 horizontalPixels, UInt16 verticalPixels);

	private:
	
		/** The constructor.\ VisualHostCommunication is a collection of static methods.\ Class does not need any instances.\ Constructor is private and not implemented. */
		VisualHostCommunication();
		
		/** The destructor.\ VisualHostCommunication is a collection of static methods.\ Class does not need any instances.\ Destructor is private and not implemented. */
		~VisualHostCommunication();
		
	};
	
}

#endif /* VisualSignature_h */
