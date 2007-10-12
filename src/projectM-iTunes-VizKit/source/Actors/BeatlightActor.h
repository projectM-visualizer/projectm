/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: BeatlightActor.h
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

#ifndef BeatlightActor_h
#define BeatlightActor_h

#include "VisualActor.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class Beatlight;

	/**
	 * A moving point indicating beat impulses of the music.
	 * Subclass of VisualActor.
	 * Still fake. Beat detection not yet implemented.
	 */
	class BeatlightActor : public VisualActor {

	public:

		/** The constructor. */
		BeatlightActor();

		/** The destructor. */
		~BeatlightActor();		

		/**
		 * Prepares the show of the beatlight.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 */
		virtual void prepareShow(const VisualPlayerState& visualPlayerState);

		/**
		 * Performs the show of the beatlight.
		 */
		virtual void show();

		/**
		 * Finishes the show of the beatlight.
		 */
		virtual void finishShow();

		/**
		 * The actor receives a notification about an event that occured.
		 * @param aNotification The notification passed in.
		 */
		virtual void handleNotification(const VisualNotification& aNotification);

	private:
	
		/** A pointer to the beatlight action. */
		Beatlight* beatlight;
		
	};

}

#endif /* BeatlightActor_h */
