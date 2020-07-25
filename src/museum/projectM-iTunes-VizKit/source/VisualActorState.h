/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualActorState.h
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

#ifndef VisualActorState_h
#define VisualActorState_h


#include "VisualTypes.h"


namespace VizKit {

	/**
	 * The VisualActorState defines the possible state of the VisualActor.
	 */
	typedef enum {
		kVisActUndefinedState = 0, /**< Undefined state that e.g. can be used to check against previous state with initialization state. */
		kVisActOff = 1, /**< The actor is turned off. The actor is called during the show nevertheless. The actor can decide if it wants to display anything. */
		kVisActOn = 2, /**< The actor is turned on. */
		kVisActNoShow = 3 /**< The actor is turned off. The actor is not called during the show at all. Therefore the actor can not be turned on by ensemble. */
	} VisualActorState;

	/**
	 * Converts VisualActorState into char string. Possibly useful for debugging or tracing purposes.
	 * @param aVisualActorState The state to convert.
	 * @param outString The char string that can be printed and used for debugging output.
	 */
	void convertVisualActorStateToString(const VisualActorState aVisualActorState, char* outString);

}

#endif /* VisualActorState_h */
