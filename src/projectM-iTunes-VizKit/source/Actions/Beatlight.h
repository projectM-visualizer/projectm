/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: Beatlight.h
 *
 */

/*************************************************************************

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

 *************************************************************************/


#ifndef Beatlight_h
#define Beatlight_h


#include "VisualTypes.h"
#include "VisualAsset.h"
#include "VisualItemIdentifier.h"
#include "VisualNotification.h"


namespace VizKit {

	/**
	 * The Beatlight shows a moving point indicating beat impulses.
	 */
	class Beatlight {

	public:

		/**
		 * The constructor of the Beatlight action.
		 */
		Beatlight();

		/**
		 * Shows the Beatlight.
		 * @param audioIsPlaying True if audio is playing, false if not.
		 */
		void showBeatlight(const bool audioIsPlaying);

		/**
		 * Shows the BeatMeterLight.
		 * @param audioIsPlaying True if audio is playing, false if not.
		 */	
		void showBeatMeterLight(const bool audioIsPlaying);

		/**
		 * Calculates the screen position of the beatlight action.
		 */
		void reshape(void);
		
		/** By applying a fade out anmation, the behavior reflects the beat detection. */
		void applyBehavior(void);
		

	private:

		/** The asset of the beatlight action (contains texture and position, too). */
		VisualAsset beatlightAsset;

		/** Identifier of the vertex chain (the vertices used for display of the beatlight texture). */
		VisualItemIdentifier vertexChainId;

	};

}

#endif /* Beatlight_h */
