/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: UpdateServiceActor.h
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

#ifndef UpdateServiceActor_h
#define UpdateServiceActor_h


#include "VisualTypes.h"
#include "VisualActor.h"
#include "VisualAsset.h"
#include "VisualItemIdentifier.h"


namespace VizKit {

	class VisualAnimationComponent; // Forward declaration (to avoid include of header file).

	/**
	 * UpdateServiceActor displays a visual update notification in case an update of the currently running visualizer is available.
	 */
	class UpdateServiceActor : public VisualActor {

	public:

		/**
		 * The constructor.
		 */
		UpdateServiceActor();

		/**
		 * The destructor.
		 */
		~UpdateServiceActor();

		/**
		 * Sets the state of the ~UpdateServiceActor.
		 * @param actorState The requested state of the actor expressed as VisualActorState.
		 * @remarks If the actor's state is kVisActNoShow, it is not called by the VisualEnsemble.
		 */
		virtual void setState(VisualActorState actorState);

		/**
		 * Performs the show of the update service actor.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 */
		virtual void show(const VisualPlayerState& visualPlayerState);

		/**
		 * The actor receives a notification about an event that occured.
		 * @param aNotification The notification passed in.
		 */
		virtual void handleNotification(VisualNotification& aNotification);

		/**
		 * Callback funtion that is called right before the fade out animation dies.
		 * @param animationComponent The fade out animation.
		 * @param userData Additional data (here: the UpdateServiceActor instance).
		 */
		static void fadeOutAnimationEnds(VisualAnimationComponent* animationComponent, void* userData);

	private:
	
		VisualAsset updateServiceActorAsset;
		
		VisualItemIdentifier backgroundVertexChainIdent;
		VisualItemIdentifier frontVertexChainIdent;

	};

}

#endif /* UpdateServiceActor_h */
