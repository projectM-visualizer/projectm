/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualThreading.h
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

#ifndef VisualThreading_h
#define VisualThreading_h


#include "VisualTypes.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h> // signature typedef OSStatus (*ThreadingFuncPtr)(void* parameter);
#endif

#if TARGET_OS_WIN
#include <windows.h>
#endif


namespace VizKit {


#ifndef ThreadingFuncPtr_def
#define ThreadingFuncPtr_def
#if TARGET_OS_MAC
typedef OSStatus (*ThreadingFuncPtr)(void* parameter);
#endif
#if TARGET_OS_WIN
typedef DWORD (*ThreadingFuncPtr)(LPVOID lpParam);
#endif
#endif


	/**
	 * Providing multi-threading functionality. 
	 */
	class VisualThreading {
	
	public:

		/**
		 * Callback that is called after a thread died.
		 */
		typedef void (*ThreadDidDieCallback)();
		
		/**
		 * Creates a new thread by calling task function.
		 * @param aThreadingFuncPtr Pointer to function that should be started in new thread.
		 * @param param Optional name for function-thread. Useful for debugging purposes.
		 * @param callbackAfterThreadDidDie Callback that is called after the created thread did finished execution.
		 * @return True on success, false on failure.
		 */	
		static bool createThread(ThreadingFuncPtr aThreadingFuncPtr, void* param = NULL, ThreadDidDieCallback callbackAfterThreadDidDie = NULL);

	private:

		/** The constructor. VisualThreading is a collection of static methods. Class does not need any instances. Constructor is private and not implemented. */
		VisualThreading();

		/** The destructor. VisualThreading is a collection of static methods. Class does not need any instances. Destructor is private and not implemented. */
		~VisualThreading();

	};
	
}

#endif /* VisualThreading_h */
