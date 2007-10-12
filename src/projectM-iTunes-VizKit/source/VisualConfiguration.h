/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualConfiguration.h
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

#ifndef VisualConfiguration_h
#define VisualConfiguration_h

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#include "win/resource.h"
#endif


namespace VizKit {
	
	/** Common definitions and constants that must be customized for each individual plug-in. */
	class VisualConfiguration {
	
	public:

		/** The name of the plug-in, the identifier. */
		static const char* const kVisualPluginName;

		/** The name of the visualizer as shown in visual menu of iTunes. */
		static const char* const kVisualPluginShowName;

		/**
		 * The identifier of the visualizer plug-in (as used on Mac OS X for preferences identification and as CFBundleIdentifier).
		 * Preferences can be stored with the Options Dialog. 
		 * The values are stored in preferences text file. 
		 * The preferences file is located at application data (Windows);
		 * and on Mac OS X the preferences are stored in Preferences folder inside Library folder in user's home directory.
		 * The domain identifier specifies the name of the plist file on Mac OS X;
		 * and on Windows the domain identifier specifies the name of the folder inside the application data directory.
		 */
		static const char* const kVisualPluginDomainIdentifier;

		typedef enum {
			kFadeInTimeOnPlayInMS = 0,
			kFadeInTimeOnResumeInMS,
			kFadeOutTimeBeforeEndOfTrackInMS,
			kFadeOutTimeOnStopInMS,
			kFadeOutTimeOnPauseInMS,
			
			kTrackInfoFont,
			kTrackInfoFontSize,
			kTrackInfoFontColorRedFloat,
			kTrackInfoFontColorGreenFloat,
			kTrackInfoFontColorBlueFloat,
			kTrackInfoFontColorAlphaFloat,
			
			kTrackInfoTextureColorTopLeftRed,
			kTrackInfoTextureColorTopLeftGreen,
			kTrackInfoTextureColorTopLeftBlue,
			kTrackInfoTextureColorTopLeftAlpha,
			kTrackInfoTextureColorBottomLeftRed,
			kTrackInfoTextureColorBottomLeftGreen,
			kTrackInfoTextureColorBottomLeftBlue,
			kTrackInfoTextureColorBottomLeftAlpha,
			kTrackInfoTextureColorBottomRightRed,
			kTrackInfoTextureColorBottomRightGreen,
			kTrackInfoTextureColorBottomRightBlue,
			kTrackInfoTextureColorBottomRightAlpha,
			kTrackInfoTextureColorTopRightRed,
			kTrackInfoTextureColorTopRightGreen,
			kTrackInfoTextureColorTopRightBlue,
			kTrackInfoTextureColorTopRightAlpha,

			kTrackLyricsFont,
			kTrackLyricsFontSize,
			kTrackLyricsFontColorRedFloat,
			kTrackLyricsFontColorGreenFloat,
			kTrackLyricsFontColorBlueFloat,
			kTrackLyricsFontColorAlphaFloat,

			kTrackLyricsTextureColorTopLeftRed,
			kTrackLyricsTextureColorTopLeftGreen,
			kTrackLyricsTextureColorTopLeftBlue,
			kTrackLyricsTextureColorTopLeftAlpha,
			kTrackLyricsTextureColorBottomLeftRed,
			kTrackLyricsTextureColorBottomLeftGreen,
			kTrackLyricsTextureColorBottomLeftBlue,
			kTrackLyricsTextureColorBottomLeftAlpha,
			kTrackLyricsTextureColorBottomRightRed,
			kTrackLyricsTextureColorBottomRightGreen,
			kTrackLyricsTextureColorBottomRightBlue,
			kTrackLyricsTextureColorBottomRightAlpha,
			kTrackLyricsTextureColorTopRightRed,
			kTrackLyricsTextureColorTopRightGreen,
			kTrackLyricsTextureColorTopRightBlue,
			kTrackLyricsTextureColorTopRightAlpha,

			kTrackLyricsAlignment,

			kFullscreenWidth,
			kFullscreenHeight,
			kFullscreenBitsPerPixel,
			kFullscreenRefreshRate,

			kPreferencePane,

			kBackgroundColorRed,
			kBackgroundColorGreen,
			kBackgroundColorBlue,
			kBackgroundColorAlpha,

			kTrackInfoTextureHeight,
			kLyricsTextureIsAvailable
		} PreferenceKey;

/* version info set by call to SetNumVersion */
#define	gVisualPluginMajorVersion 1
#define	gVisualPluginMinorVersion 9
#if TARGET_OS_MAC
#define gVisualPluginReleaseStage finalStage
#endif
#if TARGET_OS_WIN
#define	gVisualPluginReleaseStage 0x80
#endif
#define	gVisualPluginNonFinalRelease 0


// Resources
#if TARGET_OS_WIN
#define	SPOTPNG IDR_PNG_SPOT
#endif
#if TARGET_OS_MAC
#define	SPOTPNG "spot.png"
#endif

		/**
		 * Called upon init of the plug-in in VisualMain.
		 * The default (factory) preferences are set.
		 */
		static void setDefaultPreferences(void);

		/**
		 * Static helper function that converts a PreferenceKey to the string.\ Possibly useful for debugging or tracing purposes.
		 * @param aKey The key of a preference.
		 * @param outString The character string value of the PreferenceKey enum value.
		 */
		static void convertPreferenceKeyToString(const PreferenceKey aKey, char* outString);

	private:

		/** The constructor.\ VisualConfiguration is a collection of static methods.\ Class does not need any instances.\ Constructor is private and not implemented. */
		VisualConfiguration();

		/** The destructor.\ VisualConfiguration is a collection of static methods.\ Class does not need any instances.\ Destructor is private and not implemented. */
		~VisualConfiguration();
		
	};

}

#endif /* VisualConfiguration_h */

