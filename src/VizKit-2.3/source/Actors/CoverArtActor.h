/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: CoverArtActor.h
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

#ifndef CoverArtActor_h
#define CoverArtActor_h


#include "VisualTypes.h"
#include "VisualActor.h"


namespace VizKit {

	class CoverArt;

	/**
	 * A visual representation of the album cover art data included with the audio track. The cover rotates slowly around the y-axis.
	 * iTunes can handle embedded cover art data. 
	 * The CoverArtActor is responsible for displaying cover art data if there exists any with the current track. 
	 * The visual representation of the cover art data is created as a texture. 
	 * The texture of the cover art can be manipulated in various ways.
	 * The rotation is a basic example for simplicity's sake.
	 */
	class CoverArtActor : public VisualActor {

	public:

		/**
		 * The constructor.
		 */
		CoverArtActor();

		/**
		 * The destructor.
		 */
		~CoverArtActor();

		/**
		 * Initialization.
		 */
		virtual void init(void);
				
		/**
		 * Performs the show of the cover art.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 */
		virtual void show(const VisualPlayerState& visualPlayerState);

		/**
		 * The actor receives a notification about an event that occured.
		 * @param aNotification The notification passed in.
		 */
		virtual void handleNotification(VisualNotification& aNotification);

	private:
	
		/** A pointer to the cover art action. */
		CoverArt* coverArt;
		
		/** True if current audio track has album cover art data. */
		bool currentAudioTrackHasCoverArt;
		
	};

}

#endif /* CoverArtActor_h */
