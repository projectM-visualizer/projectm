/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualObjectMutex.h
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

#ifndef VisualObjectMutex_h
#define VisualObjectMutex_h

#include "VisualTypes.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/Multiprocessing.h> // MPCriticalRegionID
#endif


#if TARGET_OS_WIN
#include <windows.h>
#endif

namespace VizKit {
	
	/**
	 * Mutex functionality for thread-safe access of VisualObject instance (and its asynchronously loadable data).
	 */
	class VisualObjectMutex {
		
	public:
		
		/**
		 * The constructor.
		 */
		VisualObjectMutex();
		
		/**
		 * The destructor.
		 */		
		~VisualObjectMutex();

		/**
		 * Enter the critical region of the VisualObject.
		 * @remarks The object is not destructed in its critical region phase.
		 */	
		void enterCriticalRegion(void);
		
		/**
		 * Exit critical region of the VisualObject.
		 * @remarks The object is not destructed in its critical region phase.
		 */	
		void exitCriticalRegion(void);
		
	private:

#if TARGET_OS_MAC
		/** Critical region id for instance of VisualObjectMutex. */
		MPCriticalRegionID criticalRegion;
#endif
		
#if TARGET_OS_WIN
		/** Critical section for instance of VisualObjectMutex. */
		CRITICAL_SECTION criticalSection;
#endif
	};
	
}

#endif /* VisualObjectMutex_h */
