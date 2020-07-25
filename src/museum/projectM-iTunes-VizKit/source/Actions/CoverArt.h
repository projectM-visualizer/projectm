/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: CoverArt.h
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

#ifndef CoverArt_h
#define CoverArt_h

#include "VisualTypes.h"
#include "VisualAsset.h"
#include "VisualItemIdentifier.h"


namespace VizKit {

	/**
	 * The cover artwork image is displayed.
	 * CoverArt performs the action of the CoverArtActor.
	 */
	class CoverArt {

	public:

		/**
		 * The constructor.
		 */
		CoverArt();

		/**
		 * The destructor.
		 */
		~CoverArt();

		/**
		 * Prepares the action of the cover art.
		 */	
		void prepareCoverAction(void);

		/**
		 * Shows the cover art.
		 */
		void showCover();

		/**
		 * Finishes the action of the cover art.
		 */	
		void finishCoverAction(void);

		/**
		 * Sets the image with the artwork of the album cover.
		 * @param albumCoverArtworkImage The image with the artwork of the album cover.
		 */
		void setAlbumCoverArtWorkImage(VisualImage& albumCoverArtworkImage);

		/**
		 * Calculates the screen position of the cover art.
		 */
		void reshape(void);

		/**
		 * Starts fading in the texture of the album cover artwork.
		 * @param durationInMilliseconds The duration of the animation measured in milliseconds.
		 */
		void fadeIn(uint32 durationInMilliseconds);

		/**
		 * Starts fading out the texture of the album cover artwork.
		 * @param durationInMilliseconds The duration of the animation measured in milliseconds.
		 * @param stopValue The stop value of the fade out process (default = 0.0).
		 */
		void fadeOut(uint32 durationInMilliseconds, double stopValue = 0.0);

		/**
		 * Starts rotating the texture of the album cover artwork.
		 */
		void rotate(void);

		/**
		 * Calculates the coord depth.
		 */
		double calcCoordDepth(void);

		/**
		 * Scales the cover art by multiplying with the provided scaleFactor.
		 * @param durationInMilliseconds The duration of the animation measured in milliseconds.
		 * @param from The start scale factor of the animation.
		 * @param to The end scale factor of the animation.
		 */
		void scaleSize(uint32 durationInMilliseconds, double from, double to);

		/**
		 * Clears the memory allocated for the album cover artwork.
		 */
		void clear(void);

	private:

		/** The asset of the cover art (contains texture and position, too). */
		VisualAsset coverArtAsset;
		
		/** Identifier of the vertex chain (the vertices used for display of the album cover artwork texture). */
		VisualItemIdentifier vertexChainId;
		
		/** 
		 * Returns the scale factor of the box.
		 * @return The scale factor of the box.
		 * @remarks The dimension of the stage box has to take into account the rotation and screen constraints. 
		 * On front view the rotating bitmap should have the same dimensions on screen as in 2-D world but available canvas space must not be exceeded.
		 */
		//double getBoxScaleFactor(void);
		
	};

}

#endif /* CoverArt_h */
