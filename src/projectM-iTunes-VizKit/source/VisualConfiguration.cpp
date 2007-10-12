/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualConfiguration.cpp
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

#include "VisualConfiguration.h"
#include "VisualDataStore.h"



using namespace VizKit;


const char* const VisualConfiguration::kVisualPluginName = "VizKit";
const char* const VisualConfiguration::kVisualPluginShowName = "VizKit";
#if TARGET_OS_MAC
const char* const VisualConfiguration::kVisualPluginDomainIdentifier = "de.imagomat.vizkit";
#endif
#if TARGET_OS_WIN
const char* const VisualConfiguration::kVisualPluginDomainIdentifier = "Imagomat";
#endif



void VisualConfiguration::convertPreferenceKeyToString(const PreferenceKey aKey, char* outString) {
	char* messageString;
	switch (aKey) {
		case kFadeInTimeOnPlayInMS:
			messageString = "kFadeInTimeOnPlayInMS";
			break;
		case kFadeInTimeOnResumeInMS:
			messageString = "kFadeInTimeOnResumeInMS";
			break;
		case kFadeOutTimeBeforeEndOfTrackInMS:
			messageString = "kFadeOutTimeBeforeEndOfTrackInMS";
			break;
		case kFadeOutTimeOnStopInMS:
			messageString = "kFadeOutTimeOnStopInMS";
			break;
		case kFadeOutTimeOnPauseInMS:
			messageString = "kFadeOutTimeOnPauseInMS";
			break;
		case kTrackInfoFont:
			messageString = "kTrackInfoFont";
			break;
		case kTrackInfoFontSize:
			messageString = "kTrackInfoFontSize";
			break;
		case kTrackInfoFontColorRedFloat:
			messageString = "kTrackInfoFontColorRedFloat";
			break;
		case kTrackInfoFontColorBlueFloat:
			messageString = "kTrackInfoFontColorBlueFloat";
			break;
		case kTrackInfoFontColorAlphaFloat:
			messageString = "kTrackInfoFontColorAlphaFloat";
			break;
		case kTrackInfoTextureColorTopLeftRed:
			messageString = "kTrackInfoTextureColorTopLeftRed";
			break;
		case kTrackInfoTextureColorTopLeftGreen:
			messageString = "kTrackInfoTextureColorTopLeftGreen";
			break;
		case kTrackInfoTextureColorTopLeftBlue:
			messageString = "kTrackInfoTextureColorTopLeftBlue";
			break;
		case kTrackInfoTextureColorTopLeftAlpha:
			messageString = "kTrackInfoTextureColorTopLeftAlpha";
			break;
		case kTrackInfoTextureColorBottomLeftRed:
			messageString = "kTrackInfoTextureColorBottomLeftRed";
			break;
		case kTrackInfoTextureColorBottomLeftGreen:
			messageString = "kTrackInfoTextureColorBottomLeftGreen";
			break;
		case kTrackInfoTextureColorBottomLeftBlue:
			messageString = "kTrackInfoTextureColorBottomLeftBlue";
			break;
		case kTrackInfoTextureColorBottomLeftAlpha:
			messageString = "kTrackInfoTextureColorBottomLeftAlpha";
			break;
		case kTrackInfoTextureColorBottomRightRed:
			messageString = "kTrackInfoTextureColorBottomRightRed";
			break;
		case kTrackInfoTextureColorBottomRightGreen:
			messageString = "kTrackInfoTextureColorBottomRightGreen";
			break;
		case kTrackInfoTextureColorBottomRightBlue:
			messageString = "kTrackInfoTextureColorBottomRightBlue";
			break;
		case kTrackInfoTextureColorBottomRightAlpha:
			messageString = "kTrackInfoTextureColorBottomRightAlpha";
			break;
		case kTrackInfoTextureColorTopRightRed:
			messageString = "kTrackInfoTextureColorTopRightRed";
			break;
		case kTrackInfoTextureColorTopRightGreen:
			messageString = "kTrackInfoTextureColorTopRightGreen";
			break;
		case kTrackInfoTextureColorTopRightBlue:
			messageString = "kTrackInfoTextureColorTopRightBlue";
			break;
		case kTrackInfoTextureColorTopRightAlpha:
			messageString = "kTrackInfoTextureColorTopRightAlpha";
			break;
		case kTrackLyricsFont:
			messageString = "kTrackLyricsFont";
			break;
		case kTrackLyricsFontSize:
			messageString = "kTrackLyricsFontSize";
			break;
		case kTrackLyricsFontColorRedFloat:
			messageString = "kTrackLyricsFontColorRedFloat";
			break;
		case kTrackLyricsFontColorGreenFloat:
			messageString = "kTrackLyricsFontColorGreenFloat";
			break;
		case kTrackLyricsFontColorBlueFloat:
			messageString = "kTrackLyricsFontColorBlueFloat";
			break;
		case kTrackLyricsFontColorAlphaFloat:
			messageString = "kTrackLyricsFontColorAlphaFloat";
			break;
		case kTrackLyricsTextureColorTopLeftRed:
			messageString = "kTrackLyricsTextureColorTopLeftRed";
			break;
		case kTrackLyricsTextureColorTopLeftGreen:
			messageString = "kTrackLyricsTextureColorTopLeftGreen";
			break;
		case kTrackLyricsTextureColorTopLeftBlue:
			messageString = "kTrackLyricsTextureColorTopLeftBlue";
			break;
		case kTrackLyricsTextureColorTopLeftAlpha:
			messageString = "kTrackLyricsTextureColorTopLeftAlpha";
			break;
		case kTrackLyricsTextureColorBottomLeftRed:
			messageString = "kTrackLyricsTextureColorBottomLeftRed";
			break;
		case kTrackLyricsTextureColorBottomLeftGreen:
			messageString = "kTrackLyricsTextureColorBottomLeftGreen";
			break;
		case kTrackLyricsTextureColorBottomLeftBlue:
			messageString = "kTrackLyricsTextureColorBottomLeftBlue";
			break;
		case kTrackLyricsTextureColorBottomLeftAlpha:
			messageString = "kTrackLyricsTextureColorBottomLeftAlpha";
			break;
		case kTrackLyricsTextureColorBottomRightRed:
			messageString = "kTrackLyricsTextureColorBottomRightRed";
			break;
		case kTrackLyricsTextureColorBottomRightGreen:
			messageString = "kTrackLyricsTextureColorBottomRightGreen";
			break;
		case kTrackLyricsTextureColorBottomRightBlue:
			messageString = "kTrackLyricsTextureColorBottomRightBlue";
			break;
		case kTrackLyricsTextureColorBottomRightAlpha:
			messageString = "kTrackLyricsTextureColorBottomRightAlpha";
			break;
		case kTrackLyricsTextureColorTopRightRed:
			messageString = "kTrackLyricsTextureColorTopRightRed";
			break;
		case kTrackLyricsTextureColorTopRightGreen:
			messageString = "kTrackLyricsTextureColorTopRightGreen";
			break;
		case kTrackLyricsTextureColorTopRightBlue:
			messageString = "kTrackLyricsTextureColorTopRightBlue";
			break;
		case kTrackLyricsTextureColorTopRightAlpha:
			messageString = "kTrackLyricsTextureColorTopRightAlpha";
			break;
		case kTrackLyricsAlignment:
			messageString = "kTrackLyricsAlignment";
			break;
		case kFullscreenWidth:
			messageString = "kFullscreenWidth";
			break;
		case kFullscreenHeight:
			messageString = "kFullscreenHeight";
			break;
		case kFullscreenBitsPerPixel:
			messageString = "kFullscreenBitsPerPixel";
			break;
		case kFullscreenRefreshRate:
			messageString = "kFullscreenRefreshRate";
			break;
		case kPreferencePane:
			messageString = "kPreferencePane";
			break;
		case kBackgroundColorRed:
			messageString = "kBackgroundColorRed";
			break;
		case kBackgroundColorGreen:
			messageString = "kBackgroundColorGreen";
			break;
		case kBackgroundColorBlue:
			messageString = "kBackgroundColorBlue";
			break;
		case kBackgroundColorAlpha:
			messageString = "kBackgroundColorAlpha";
			break;
		case kTrackInfoTextureHeight:
			messageString = "kTrackInfoTextureHeight";
			break;
		case kLyricsTextureIsAvailable:
			messageString = "kLyricsTextureIsAvailable";
			break;
		default:
			messageString = "unknownNotificationKey";
	}
	strcpy(outString, messageString);
}


void VisualConfiguration::setDefaultPreferences() {
	
	// Factory Preferences
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kFadeInTimeOnPlayInMS, 5000);
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kFadeInTimeOnResumeInMS, 2000);
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kFadeOutTimeBeforeEndOfTrackInMS, 5000);
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kFadeOutTimeOnStopInMS, 5000);
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kFadeOutTimeOnPauseInMS, 2000);

	VisualDataStore::setDefaultPreferenceValueChar(VisualConfiguration::kTrackInfoFont, "Devroye");
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kTrackInfoFontSize, 48);

	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoFontColorRedFloat, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoFontColorGreenFloat, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoFontColorBlueFloat, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoFontColorAlphaFloat, 1.0f);

	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopLeftRed, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopLeftGreen, 0.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopLeftBlue, 0.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopLeftAlpha, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomLeftRed, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomLeftGreen, 0.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomLeftBlue, 0.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomLeftAlpha, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomRightRed, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomRightGreen, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomRightBlue, 0.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomRightAlpha, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopRightRed, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopRightGreen, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopRightBlue, 0.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopRightAlpha, 1.0f);

#if TARGET_OS_MAC
	VisualDataStore::setDefaultPreferenceValueChar(VisualConfiguration::kTrackLyricsFont, "Lucida Grande");
#endif
#if TARGET_OS_WIN
	VisualDataStore::setDefaultPreferenceValueChar(VisualConfiguration::kTrackLyricsFont, "Tahoma");
#endif
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kTrackLyricsFontSize, 18);

	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsFontColorRedFloat, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsFontColorGreenFloat, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsFontColorBlueFloat, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsFontColorAlphaFloat, 1.0f);

	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopLeftRed, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopLeftGreen, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopLeftBlue, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopLeftAlpha, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomLeftRed, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomLeftGreen, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomLeftBlue, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomLeftAlpha, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomRightRed, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomRightGreen, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomRightBlue, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomRightAlpha, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopRightRed, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopRightGreen, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopRightBlue, 1.0f);
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopRightAlpha, 1.0f);
	
	VisualDataStore::setDefaultPreferenceValueChar(VisualConfiguration::kTrackLyricsAlignment, "center");

	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kBackgroundColorRed, (12.0f / 255.0f));
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kBackgroundColorGreen, (9.0f / 255.0f));
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kBackgroundColorBlue, (172.0f / 255.0f));
	VisualDataStore::setDefaultPreferenceValueFloat(VisualConfiguration::kBackgroundColorAlpha, 1.0f);

	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kFullscreenWidth, 0);
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kFullscreenHeight, 0);
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kFullscreenBitsPerPixel, 32);
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kFullscreenRefreshRate, 0);
#if TARGET_OS_MAC
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kPreferencePane, 1);
#endif
#if TARGET_OS_WIN
	VisualDataStore::setDefaultPreferenceValueInt(VisualConfiguration::kPreferencePane, 0);
#endif

}
