/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualUpdateManager.h
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

#ifndef VisualUpdateManager_h
#define VisualUpdateManager_h


#include "VisualTypes.h"
#include "VisualString.h"

namespace VizKit {

	class VisualString; // Forward declaration (to avoid include of header file).

	/**
	 * Checking for, prompting for, and downloading available updates for this visualizer. 
	 */
	class VisualUpdateManager {
	
	public:
		
		/**
		 * Disposes the VisualUpdateManager.
		 */	
		static void dispose(void);

		/**
		 * Checks to see whether an update for the current visualizer is available online.
		 * @remarks The check is not performed if preference VisualPreferences::kCheckForUpdate is set to false.
		 */	
		static void checkForUpdate(void);

		/**
		 * Launches external application to display more information about available update.
		 */	
		static void getMoreInfoAboutAvailableUpdate(void);

		/**
		 * Launches external application to download available update.
		 */	
		static void doDownloadAvailableUpdate(void);

		/**
		 * Evaluates the update information.
		 * @return True if update is available, false if not.
		 */	
		static bool evaluateUpdateInformation(void);

	private:

		/**
		 * Constructs a VisualUpdateManager. The VisualUpdateManager acts as a singleton. Returns a pointer to the initialised VisualUpdateManager.
		 * @return A pointer to the singleton instance.
		 */
		static VisualUpdateManager* getInstance(void);

		/** The constructor. VisualUpdateManager is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualUpdateManager();
		
		/** The destructor. VisualUpdateManager is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualUpdateManager();

		/**
		 * Copy constructor.
		 * @param other Another VisualNetwork.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualUpdateManager(const VisualUpdateManager& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualUpdateManager& operator=(const VisualUpdateManager& other);
	
		/** VisualUpdateManager is a singleton class. Pointer to private instance is handled internally. */
		static VisualUpdateManager* theVisualUpdateManager;
		
		/** String containing the update information (fetched from VisualConfiguration::kCurrentVersionInformationURL). */
		VisualString* updateInformation;
		
		/** The complete string of the current version. */
		VisualString currentVersionString;

		/** URL pointing to more information about visualizer. */
		VisualString infoURL;

		/** Download URL. */
		VisualString downloadURL;
		
		/** Launches browser application to open remote URL. */
		void launchBrowserToOpenURL(const char* const URL);
		
	};
	
}



#endif /* VisualUpdateManager_h */
