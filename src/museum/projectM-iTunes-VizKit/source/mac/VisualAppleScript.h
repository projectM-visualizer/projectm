/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAppleScript.h
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

#ifndef VisualAppleScript_h
#define VisualAppleScript_h


#include "VisualTypes.h"


namespace VizKit {

	class VisualFile; // Forward declaration (to avoid include of header file).

	/**
	 * Interaction with AppleScript.
	 */
	class VisualAppleScript {

	public:
		
		/**
		 * Executes a script file.
		 * @param scriptFile The script file.
		 */
		static bool executeScriptFile(VisualFile& scriptFile);
	
	private:

		/** The constructor. VisualAppleScript is a collection of static methods. Class does not need any instances. Constructor is private and not implemented. */
		VisualAppleScript();

		/** The destructor. VisualAppleScript is a collection of static methods. Class does not need any instances. Destructor is private and not implemented. */
		~VisualAppleScript();

	};
	
}

#endif /* VisualAppleScript_h */
