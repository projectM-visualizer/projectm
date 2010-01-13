/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: TrackLyrics.h
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

#ifndef TrackLyrics_h
#define TrackLyrics_h


#include "VisualTypes.h"
#include "VisualAsset.h"
#include "VisualItemIdentifier.h"
#include "VisualStringStyle.h"


namespace VizKit {

	class VisualString; // Forward declaration (to avoid include of header file).

	/**
	 * Shows the lyrics of the current audio track.
	 * TrackLyrics performs the action of the TrackLyricsActor.
	 */
	class TrackLyrics {

	public:

		/**
		 * The constructor.
		 */
		TrackLyrics();

		/**
		 * The destructor.
		 */
		~TrackLyrics();

		/**
		 * Shows the track lyrics action.
		 */
		void show(void);

		/**
		 * Returns the current string style of the track lyrics string.
		 * @return The current string style of the track lyrics string.
		 */
		VisualStringStyle getTrackLyricsStringStyle(void);

		/**
		 * Sets the image with the track lyrics string.
		 * @param styledTrackLyricsStringImage The image with the track lyrics string.
		 */
		void setTrackLyricsImage(VisualImage& styledTrackLyricsStringImage);

		/**
		 * Calculates the screen position of the track lyrics texture.
		 */
		void reshape(void);

		/**
		 * Starts fading in the texture of the track lyrics.
		 * @param durationInMilliseconds The duration of the animation measured in milliseconds.
		 */
		void fadeIn(uint32 durationInMilliseconds);

		/**
		 * Starts fading out the texture of the track lyrics.
		 * @param durationInMilliseconds The duration of the animation measured in milliseconds.
		 * @param clampValue The clamp value of the fade out process (default = 0.0f).
		 */
		void fadeOut(uint32 durationInMilliseconds, float clampValue = 0.0f);

		/**
		 * Clears the memory allocated for the track lyrics action.
		 */
		void clear(void);

	private:

		/** The asset of the track lyrics action (contains texture and position too). */
		VisualAsset trackLyricsAsset;
		
		/** Identifier of the vertex chain (the vertices used for display of the track lyrics string texture). */
		VisualItemIdentifier vertexChainId;
		
	};

}

#endif /* TrackLyrics_h */
