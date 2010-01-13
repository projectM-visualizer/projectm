/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualActorState.cpp
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

#include "VisualActorState.h"

#include <string>


using namespace VizKit;


void convertVisualActorStateToString(const VisualActorState aVisualActorState, char* outString) {
	const char* messageString;
	switch (aVisualActorState) {
		case kVisActUndefinedState:
			messageString = "kVisActUndefinedState";
			break;
		case kVisActOff:
			messageString = "kVisActOff";
			break;
		case kVisActOn:
			messageString = "kVisActOn";
			break;
		case kVisActNoShow:
			messageString = "kVisActNoShow";
			break;
		default:
			messageString = "unknownVisualActorState";
	}
	strcpy(outString, messageString);
}
