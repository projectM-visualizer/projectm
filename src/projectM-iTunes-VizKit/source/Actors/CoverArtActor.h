/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: CoverArtActor.h
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

#ifndef CoverArtActor_h
#define CoverArtActor_h

#include "VisualActor.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class CoverArt;

	/**
	 * A visual representation of the cover art data included with the audio track. The cover rotates slowly around the y-axis.
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
		 * Prepares the show of the cover art.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 */
		virtual void prepareShow(const VisualPlayerState& visualPlayerState);
				
		/**
		 * Performs the show of the cover art.
		 */
		virtual void show();

		/**
		 * Finishes the show of the cover art.
		 */	
		virtual void finishShow();

		/**
		 * The actor receives a notification about an event that occured.
		 * @param aNotification The notification passed in.
		 */
		virtual void handleNotification(const VisualNotification& aNotification);

	private:
	
		/** A pointer to the cover art action. */
		CoverArt* coverArt;
		
		/** True if current audio track has cover art data. */
		bool currentAudioTrackHasCoverArt;
		
	};

}

#endif /* CoverArtActor_h */
