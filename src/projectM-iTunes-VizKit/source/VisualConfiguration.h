/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualConfiguration.h
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

#ifndef VisualConfiguration_h
#define VisualConfiguration_h

#include "VisualTypes.h"

#if TARGET_OS_WIN
#include "win/resource.h"
#endif


namespace VizKit {
	
	/** Common definitions and constants that must be customized for each individual plug-in. */
	class VisualConfiguration {
	
	public:

		/** The name of the visualizer plug-in, the identifier. */
		static const char* const visualizerPluginIdentifierName;

		/** The name of the visualizer as shown in menu of iTunes. */
		static const char* const visualizerShowName;

		/**
		 * The identifier of the visualizer plug-in (as used on Mac OS X for preferences identification and as CFBundleIdentifier).
		 * Preferences can be stored with the Options Dialog. 
		 * The values are stored in preferences text file. 
		 * The preferences file is located at application data (Windows);
		 * and on Mac OS X the preferences are stored in Preferences folder inside Library folder in user's home directory.
		 * The domain identifier specifies the name of the plist file on Mac OS X.
		 * On Windows the domain identifier specifies the name of the folder inside the application data directory.
		 */
		static const char* const kVisualPluginDomainIdentifier;

		/** The major release number of the visualizer (version number). Range: 0 - 99. For instance 1.2.5 means: major release number 1, minor release number 2, revision number 5. */
		static const uint8 kMajorReleaseNumber;

		/** The minor release number of the visualizer (version number). Range: 0 - 9. For instance 1.2.5 means: major release number 1, minor release number 2, revision number 5. */
		static const uint8 kMinorReleaseNumber;

		/** The revision number of the visualizer (version number). Range: 0 - 255. For instance 1.2.5 means: major release number 1, minor release number 2, revision number 5. */
		static const uint8 kReleaseRevisionNumber;

		/**
		 * The URL of the current version information for this visualizer.
		 * The text information contains three items, separated by "|": 
		 * 1) The current version number (major.minor.revision), set each component of the version (including 0). Zero values at the end of the version are dropped when presenting the current version to the user.
		 * 2) Info URL (web page with general information about the visualizer or specific information about the advertized version)
		 * 3) Download URL (URL pointing to download file of current visualizer)
		 * This information is used to prompt the user with update information in case an older version is running.
		 * The feature is turned on/off by preference VisualPreferences::kCheckForUpdate.
		 */
		static const char* const kCurrentVersionInformationURL;

		/**
		 * Disposes the VisualConfiguration.
		 */	
		static void dispose(void);

		/** Returns the major release number encoded as BCD value.
		 * @return The major release number encoded as BCD value.
		 * @remarks The BCD value is passed to iTunes in PlayerRegisterVisualPlugin() by setting the pluginVersion of the PlayerRegisterVisualPluginMessage.
		 */
		static uint8 getMajorReleaseNumberAsBCD(void);

		/** Returns the minor release number encoded as BCD value.
		 * @return The minor release number encoded as BCD value.
		 * @remarks The BCD value is passed to iTunes in PlayerRegisterVisualPlugin() by setting the pluginVersion of the PlayerRegisterVisualPluginMessage.
		 */
		static uint8 getMinorReleaseNumberAsBCD(void);

		/** Returns the release revision number.
		 * @return The release revision number.
		 * @remarks The release revision number is not encoded as BCD.
		 */
		static uint8 getReleaseRevisionNumber(void);

		/** Returns the unicode-savvy show name of the visualizer.
		 * @return The unicode-savvy show name of the visualizer.
		 */
		static uint16* getVisualizerShowNameUnicode(uint32& numberOfCharactersOfVisualizerShowNameUnicode);

// Resources
#if TARGET_OS_WIN
#define	SPOTPNG IDR_PNG_SPOT
#endif
#if TARGET_OS_MAC
#define	SPOTPNG "spot.png"
#endif

	private:

		/**
		 * Constructs a VisualConfiguration. The VisualConfiguration acts as a singleton. Returns a pointer to the initialised VisualConfiguration.
		 * @return A pointer to the singleton instance.
		 */
		static VisualConfiguration* getInstance(void);

		/** The constructor. VisualConfiguration is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualConfiguration();
		
		/** The destructor. VisualConfiguration is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualConfiguration();

		/**
		 * Copy constructor.
		 * @param other Another VisualConfiguration.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualConfiguration(const VisualConfiguration& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualConfiguration& operator=(const VisualConfiguration& other);
	
		/** VisualConfiguration is a singleton class. Pointer to private instance is handled internally. */
		static VisualConfiguration* theVisualConfiguration;
		
		/** Internal method to prepare the unicode-savvy show name of the visualizer. */
		void prepareVisualizerShowNameUnicode(void);

		/** Returns the binary-coded decimal value of a decimal number.
		 * @param decimalNumber The decimal number to convert into binary-coded decimal number.
		 * @remarks BCD or Binary-coded decimal format is a format where each digit has a range of 0 - 9 (normal binary digits have a range of 0 - 15). Further info on encoding of version information: http://developer.apple.com/technotes/tn/tn1132.html (Technote OV 12- Version Territory)
		 */
		static uint8 getBCD(uint8 decimalNumber);

		/** The name of the visualizer as shown in menu of iTunes (declared as UTF-16 on Windows, declared as UTF-8 on Mac).
		 * @remarks Routine
		 */
#if TARGET_OS_WIN
		uint16* visualizerShowNameUnicode;
#endif
#if TARGET_OS_MAC
		uint8* visualizerShowNameUnicode;
#endif
		
		/** The internally stored unicode-savvy show name of the visualizer. */
		uint16* unicodeShowName;
		
	};

}

#endif /* VisualConfiguration_h */

