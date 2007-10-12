/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualThreading.h
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

#ifndef VisualThreading_h
#define VisualThreading_h

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#include <windows.h>
#endif


namespace VizKit {


#if TARGET_OS_MAC
typedef OSStatus (*ThreadingFuncPtr)(void* parameter);
#endif
#if TARGET_OS_WIN
typedef DWORD (*ThreadingFuncPtr)(LPVOID lpParam);
//typedef LPTHREAD_START_ROUTINE ThreadingFuncPtr;
#endif


	/**
	 * Providing multi-threading functionality. 
	 */
	class VisualThreading {
	
	public:

		/**
		 * Creates a new thread by starting function.
		 * @param aThreadingFuncPtr Pointer to function that should be started in new thread.
		 * @param name Optional name for function-thread.\ Useful for debugging purposes.
		 */	
		static OSStatus createThread(ThreadingFuncPtr aThreadingFuncPtr, char* name = NULL);

	private:

		/** The constructor.\ VisualThreading is a collection of static methods.\ Class does not need any instances.\ Constructor is private and not implemented. */
		VisualThreading();

		/** The destructor.\ VisualThreading is a collection of static methods.\ Class does not need any instances.\ Destructor is private and not implemented. */
		~VisualThreading();

	};
	
}

#endif /* VisualThreading_h */
