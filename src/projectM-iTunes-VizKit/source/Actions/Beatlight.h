/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: Beatlight.h
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

#ifndef Beatlight_h
#define Beatlight_h

#include "VisualAsset.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	/**
	 * Performs the action of the BeatlightActor.
	 * The Beatlight shows a moving point indicating beat impulses.
	 * Beat detection not implemented yet.
	 */
	class Beatlight {

	public:

		/**
		 * The constructor.
		 */
		Beatlight();

		/**
		 * Prepares the Beatlight for the show.
		 */
		void prepareBeatlightShow(void);

		/**
		 * Shows the Beatlight.
		 */
		void showBeatlight(void);

		/**
		 * Finishes the Beatlight show.
		 */
		void finishBeatlightShow(void);

		/**
		 * Calculates the screen position of the beatlight.
		 */
		void calcPositionOnScreen(void);
		
	private:

		/**
		 * Adds the resource spotlight image to Beatlight's private asset.
		 */	
		void addSpotlightImage(void);

		/**
		 * Beatlight's private asset.
		 */	
		VisualAsset beatlightAsset;

	};

}

#endif /* Beatlight_h */
