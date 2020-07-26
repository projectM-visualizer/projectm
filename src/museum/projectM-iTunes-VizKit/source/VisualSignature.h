/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualSignature.h
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

#ifndef VisualSignature_h
#define VisualSignature_h

#include "iTunesVisualAPI.h"

#include "VisualTypes.h"


namespace VizKit {

	/**
	 * Collection of static methods for specific data to establish communication with iTunes host application.
	 */
	class VisualSignature {
	
	public:

		/**
		 * Sets the appCookie.
		 * @param anAppCookie Pointer to an appCookie.
		 */	
		static void setAppCookie(void* anAppCookie);

		/**
		 * Gets the appCookie.
		 * @return Pointer to appCookie.
		 */	
		static void* getAppCookie(void);

		/**
		 * Sets the appProc.
		 * @param anAppProc Pointer to an anAppProc.
		 */	
		static void setAppProc(ITAppProcPtr anAppProc);

		/**
		 * Gets the appProc.
		 * @return Pointer to an appProc.
		 */	
		static ITAppProcPtr getAppProc(void);

	private:
	
		/** The constructor. VisualSignature is a collection of static methods. Class does not need any instances. Constructor is private and not implemented. */
		VisualSignature();
		
		/** The destructor. VisualSignature is a collection of static methods. Class does not need any instances. Destructor is private and not implemented. */
		~VisualSignature();

		/** Pointer to AppCookie. */
		static void* appCookie;

		/** Pointer to AppProc. */
		static ITAppProcPtr appProc;
		
	};
	
}

#endif /* VisualSignature_h */
