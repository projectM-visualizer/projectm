/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: BeatlightActor.h
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

#ifndef BeatlightActor_h
#define BeatlightActor_h


#include "VisualTypes.h"
#include "VisualActor.h"
#include "VisualImage.h"
#include "VisualString.h"

#include "VisualItemIdentifier.h"


namespace VizKit {

	class Beatlight; // Forward declaration (to avoid include of header file).

	/**
	 * A beat indicating VisualActor.
	 * A pulsating light in the lower left corner indicates beat impulses.
	 */
	class BeatlightActor : public VisualActor {

	public:

		/**
		 * The constructor of BeatlightActor.
		 */
		BeatlightActor();

		/**
		 * The destructor of BeatlightActor.
		 */
		~BeatlightActor();

		/**
		 * Initialization.
		 */
		virtual void init(void);

		/**
		 * Performs the show of the beatlight.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 */
		virtual void show(const VisualPlayerState& visualPlayerState);

		/**
		 * The actor receives a notification about an event that occured.
		 * @param aNotification The notification passed in.
		 */
		virtual void handleNotification(VisualNotification& aNotification);

	private:
	
		/** A Pointer to the beatlight action. */
		Beatlight* beatlight;
	
	};

}

#endif /* BeatlightActor_h */
