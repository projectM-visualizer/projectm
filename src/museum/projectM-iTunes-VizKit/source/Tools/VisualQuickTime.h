/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualQuickTime.h
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

#ifndef VisualQuickTime_h
#define VisualQuickTime_h

#include "VisualTypes.h"


namespace VizKit {


	/**
	 * Collection of QuickTime functionality. Access of QuickTime functionality is restricted to this class.
	 */
	class VisualQuickTime {
	
	public:

		/**
		 * Returns the number of bytes allocated by a Mac memory handle.
		 * @return The number of bytes allocated by a Mac memory handle.
		 * @remarks Album cover artwork data is passed to the visualizer as Mac memory handle.
		 */	
		static uint32 getHandleSize(void* aHandle);

		/**
		 * Disposes a Mac memory handle. 
		 * @param aHandle A Mac memory handle.
		 * @remarks Album cover artwork data is passed to the visualizer as Mac memory handle.
		 */
		static void disposeHandle(void* aHandle);

		/**
		 * Handles an OS event and returns the character that has been pressed on the keyboard.
		 * @param theEvent The event to handle.
		 * @param[out] aChar The character key that has been pressed on the keyboard. 
		 * @return True on success, false on failure.
		 */
		static bool getPressedKeyOfEvent(void* theEvent, char& aChar);

	private:

		/** The constructor. VisualQuickTime is a collection of static methods. Class does not need any instances. Constructor is private and not implemented. */
		VisualQuickTime();

		/** The destructor. VisualQuickTime is a collection of static methods. Class does not need any instances. Destructor is private and not implemented. */
		~VisualQuickTime();

	};

}

#endif /* VisualQuickTime_h */
