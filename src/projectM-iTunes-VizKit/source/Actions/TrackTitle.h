/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: TrackTitle.h
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

#ifndef TrackTitle_h
#define TrackTitle_h

#include "VisualAsset.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class VisualString; // Forward declaration (to avoid include of header file).

	/**
	 * Shows the title of the current audio track.
	 * TrackTitle performs the action of the TrackTitleActor.
	 */
	class TrackTitle {

	public:

		/**
		 * The constructor.
		 */
		TrackTitle();

		/**
		 * The destructor.
		 */
		~TrackTitle();

		/**
		 * Shows the track title action.
		 */
		void show(void);

		/**
		 * Creates the texture of the track title.
		 * @param trackName The track title string.
		 * @return Error state as SInt32 OSStatus.
		 */
		OSStatus makeTextureOfTrackTitle(const VisualString& trackName);

		/**
		 * Returns the height of the track title texture in pixels.
		 * @return The height of the track title texture in pixels.
		 */
		UInt16 getTrackInfoTextureHeightInPixels(void);

		/**
		 * Calculates the screen position of the track title texture.
		 */
		void calcPositionOnScreen(void);

		/**
		 * Starts fading in the texture of the track title.
		 * @param durationInMilliseconds The duration of the animation measured in milliseconds.
		 */
		void fadeIn(UInt32 durationInMilliseconds);

		/**
		 * Starts fading out the texture of the track title.
		 * @param durationInMilliseconds The duration of the animation measured in milliseconds.
		 */
		void fadeOut(UInt32 durationInMilliseconds);

		/**
		 * Starts pulsating the texture (fade in / fade out) of the track title.
		 */
		void pulsate(void);	

		/**
		 * Clears the memory allocated for the track title action.
		 */
		void clear(void);

	private:

		/** The asset of the track info action (contains texture and position too). */
		VisualAsset trackInfoAsset;
		
	};

}

#endif /* TrackTitle_h */
