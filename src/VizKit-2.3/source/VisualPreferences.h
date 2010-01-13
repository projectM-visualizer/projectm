/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualPreferences.h
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

#ifndef VisualPreferences_h
#define VisualPreferences_h


#include "VisualTypes.h"

#include <vector>


namespace VizKit {

	/**
	 * Stores preference values of different data type (integer, float, char, boolean).
	 * All public methods are declared static.
	 */
	class VisualPreferences {

	public:

		/**
		 * Integer preferences.
		 */
		typedef enum {
			kUnknownPrefInt = 0,

			kFadeInTimeOnPlayInMS,
			kFadeInTimeOnResumeInMS,
			kFadeOutTimeBeforeEndOfTrackInMS,
			kFadeOutTimeOnPauseInMS,
			
			kTrackInfoFontSize,

			kTrackLyricsFontSize,

			kPreferencePane

		} PreferenceKeyInt;

		/**
		 * Float preferences.
		 */
		typedef enum {
			kUnknownPrefFloat = 0,
			
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

			kCanvasBackgroundColorRed,
			kCanvasBackgroundColorGreen,
			kCanvasBackgroundColorBlue,
			kCanvasBackgroundColorAlpha,

			kSurfaceBackgroundColorRed,
			kSurfaceBackgroundColorGreen,
			kSurfaceBackgroundColorBlue,
			kSurfaceBackgroundColorAlpha

		} PreferenceKeyFloat;

		/**
		 * Char preferences.
		 */
		typedef enum {
			kUnknownPrefChar = 0,			
			kTrackInfoDisplayItemsDelimiterCharacters,
			kTrackInfoFont,
			kTrackLyricsFont,
			kFallbackSystemFont,
			kTrackLyricsAlignment
		} PreferenceKeyChar;

		/**
		 * Boolean preferences.
		 */
		typedef enum {
			kUnknownPrefBool = 0,
			kCheckForUpdate
		} PreferenceKeyBool;

		/**
		 * Disposes the VisualPreferences.
		 */
		static void dispose(void);

		/**
		 * Sets a preference value (integer).
		 * To store a preference persistently, the method storeValues has to be called afterwards.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (integer) of the preference value.
		*/	
		static void setValue(const PreferenceKeyInt anIdentifier, const int aValue);

		/**
		 * Sets a preference value (float).
		 * To store a preference persistently, the method storeValues has to be called afterwards.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (float) of the preference value.
		 */	
		static void setValue(const PreferenceKeyFloat anIdentifier, const float aValue);

		/**
		 * Sets a preference value (char).
		 * To store a preference persistently, the method storeValues has to be called afterwards.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (character) of the preference value.
		 */
		static void setValue(const PreferenceKeyChar anIdentifier, const char* const aValue);

		/**
		 * Sets a preference value (boolean).
		 * To store a preference persistently, the method storeValues has to be called afterwards.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (boolean) of the preference value.
		*/	
		static void setValue(const PreferenceKeyBool anIdentifier, const bool aValue);

		/**
		 * Retrieves a default preference value (integer).
		 * The default preference is handled as a fallback if a preference value is requested.
		 * It is handled as an error if a preference value is requested and no default value has been specified before 
		 * nor any value with the requested key has been saved before.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param isDefaultValue True if default preference value was found.
		 * @return The default integer preference value.
		 */
		static int getDefault(const PreferenceKeyInt anIdentifier, bool* isDefaultValue);
		
		/**
		 * Retrieves a default preference value (float).
		 * The default preference is handled as a fallback if a preference value is requested.
		 * It is handled as an error if a preference value is requested and no default value has been specified before 
		 * nor any value with the requested key has been saved before.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param isDefaultValue True if default preference value was found.
		 * @return The default float preference value.
		 */
		static float getDefault(const PreferenceKeyFloat anIdentifier, bool* isDefaultValue);

		/**
		 * Retrieves a default preference value (char).
		 * The default preference is handled as a fallback if a preference value is requested.
		 * It is handled as an error if a preference value is requested and no default value has been specified before 
		 * nor any value with the requested key has been saved before.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param[out] outPrefVal The value of the character preference.
		 * @return True if default preference value was found.
		 */
		static bool getDefault(const PreferenceKeyChar anIdentifier, char* outPrefVal);

		/**
		 * Retrieves a default preference value (boolean).
		 * The default preference is handled as a fallback if a preference value is requested.
		 * It is handled as an error if a preference value is requested and no default value has been specified before 
		 * nor any value with the requested key has been saved before.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param isDefaultValue True if default preference value was found.
		 * @return The default integer preference value.
		 */
		static bool getDefault(const PreferenceKeyBool anIdentifier, bool* isDefaultValue);

		/**
		 * Retrieves a persistent preference value (integer).
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param isDefaultValue True if returned value is default value.
		 */	
		static int getValue(const PreferenceKeyInt anIdentifier, bool* isDefaultValue = NULL);

		/**
		 * Retrieves a persistent preference value (float).
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param isDefaultValue True if returned value is default value.
		 */	
		static float getValue(const PreferenceKeyFloat anIdentifier, bool* isDefaultValue = NULL);

		/**
		 * Retrieves a persistent preference value (char).
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param[out] outPrefVal The value of the character preference.
		 * @param isDefaultValue True if returned value is default value.
		 */
		static void getValue(const PreferenceKeyChar anIdentifier, char* outPrefVal, bool* isDefaultValue = NULL);

		/**
		 * Retrieves a persistent preference value (boolean).
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param isDefaultValue True if returned value is default value.
		 */	
		static bool getValue(const PreferenceKeyBool anIdentifier, bool* isDefaultValue = NULL);

		/**
		 * Stores the preferences in persistent storage.
		 */		
		static void storeValues(void);

		/**
		 * Called on init of the plug-in in VisualMainAction::handleInitMessage().
		 * The default (factory) preference values are set.
		 */
		static void setDefaultValues(void);

		/**
		 * Static helper function that converts an integer PreferenceKey to the string. Possibly useful for debugging or tracing purposes.
		 * @param aKey The key of a preference.
		 * @param outString The character string value of the PreferenceKey enum value.
		 */
		static void convertPreferenceKeyToString(const PreferenceKeyInt aKey, char* outString);

		/**
		 * Static helper function that converts a float PreferenceKey to the string. Possibly useful for debugging or tracing purposes.
		 * @param aKey The key of a preference.
		 * @param outString The character string value of the PreferenceKey enum value.
		 */
		static void convertPreferenceKeyToString(const PreferenceKeyFloat aKey, char* outString);

		/**
		 * Static helper function that converts a char PreferenceKey to the string. Possibly useful for debugging or tracing purposes.
		 * @param aKey The key of a preference.
		 * @param outString The character string value of the PreferenceKey enum value.
		 */
		static void convertPreferenceKeyToString(const PreferenceKeyChar aKey, char* outString);

		/**
		 * Static helper function that converts a boolean PreferenceKey to the string. Possibly useful for debugging or tracing purposes.
		 * @param aKey The key of a preference.
		 * @param outString The character string value of the PreferenceKey enum value.
		 */
		static void convertPreferenceKeyToString(const PreferenceKeyBool aKey, char* outString);

		/**
		 * Static helper function that converts a string with an integer PreferenceKey to the according PreferenceKey. Possibly useful for debugging or tracing purposes.
		 * @param aPreferenceKeyString A string with a PreferenceKey
		 * @return The PreferenceKey enum value.
		 */
		static VisualPreferences::PreferenceKeyInt convertPreferenceKeyIntStringToKey(const char* const aPreferenceKeyString);

		/**
		 * Static helper function that converts a string with a float PreferenceKey to the according PreferenceKey. Possibly useful for debugging or tracing purposes.
		 * @param aPreferenceKeyString A string with a PreferenceKey
		 * @return The PreferenceKey enum value.
		 */
		static VisualPreferences::PreferenceKeyFloat convertPreferenceKeyFloatStringToKey(const char* const aPreferenceKeyString);

		/**
		 * Static helper function that converts a string with a char PreferenceKey to the according PreferenceKey. Possibly useful for debugging or tracing purposes.
		 * @param aPreferenceKeyString A string with a PreferenceKey
		 * @return The PreferenceKey enum value.
		 */
		static VisualPreferences::PreferenceKeyChar convertPreferenceKeyCharStringToKey(const char* const aPreferenceKeyString);

		/**
		 * Static helper function that converts a string with a boolean PreferenceKey to the according PreferenceKey. Possibly useful for debugging or tracing purposes.
		 * @param aPreferenceKeyString A string with a PreferenceKey
		 * @return The PreferenceKey enum value.
		 */
		static VisualPreferences::PreferenceKeyBool convertPreferenceKeyBoolStringToKey(const char* const aPreferenceKeyString);

	private:

		/** The constructor. VisualPreferences is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualPreferences();

		/** The destructor. VisualPreferences is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualPreferences();

		/**
		 * Constructs a VisualPreferences. The VisualPreferences internally is a singleton. Returns a pointer to the initialized VisualPreferences.
		 * @return A pointer to the singleton instance.
		 */
		static VisualPreferences* getInstance(void);

		/**
		 * Copy constructor.
		 * @param other Another VisualPreferences.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualPreferences(const VisualPreferences& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualPreferences& operator=(const VisualPreferences& other);

		/** VisualPreferences is a singleton class. Pointer to private instance is handled internally. */
		static VisualPreferences* theVisualPreferences;

		/** The format of the data that is stored with the preference. */
		typedef enum {
			kIntPrefType = 0, /**< Integer type. */
			kCharPrefType, /**< 8-bit character type. */
			kFloatPrefType, /**< Float type. */
			kBoolPrefType /**< Bool type. */
		} PrefDataType;

		/** The status of the preference. */
		typedef enum {
			kDefaultPref = 0, /**< Default (factory) preference value. */
			kPrefSetButNotStored, /**< The preference has been set but not stored. */
			kPrefSetAndStored /**< The preference value is stored persistently. */
		} PrefStatus;
			
		/** Stores a key-value-pair of preferences. The type of data of the preference is set with the field dataType. */
		class Preference {
		public:
			/** The int key of the preference. */
			PreferenceKeyInt keyInt;
			/** The float key of the preference. */
			PreferenceKeyFloat keyFloat;
			/** The char key of the preference (e.g. kTrackInfoFont). */
			PreferenceKeyChar keyChar;
			/** The bool key of the preference. */
			PreferenceKeyBool keyBool;
			/** The integer value of the preference (e.g. 32). */
			int valueInt;
			/** The float value of the preference (e.g. 1.35). */
			float valueFloat;
			/** The char value of the preference (e.g. "LucidaGrande"). */
			char valueChar[256];
			/** The boolean value of the preference (true or false). */
			bool valueBool;
			/** The default integer value of the preference (e.g. 32). */
			int defaultValueInt;
			/** The default float value of the preference (e.g. 1.35). */
			float defaultValueFloat;
			/** The default char value of the preference (e.g. "LucidaGrande"). */
			char defaultValueChar[256];
			/** The default bool value of the preference (true or false). */
			bool defaultValueBool;
			/** The format of the preference data. */
			PrefDataType dataType;
			/** Answers the question whether the value is persistently stored or not. Enum value prefStatus. */
			PrefStatus status;
		};
		
		/** Preferences are collected as a vector of pointers to Preferences. */
		typedef std::vector<Preference*> PreferenceVector;
		
		/** The PreferenceVectorIterator is an iterator of the PreferenceVector. */
		typedef PreferenceVector::iterator PreferenceVectorIterator;
		
		/** Vector of preferences. */
		PreferenceVector preferenceVector;

		/**
		 * Sets a default preference value (integer).
		 * The default preference value is set by VisualPreferences' setDefaultValues().
		 * That way there will always be a preference value present even if the user hasn't set any yet. 
		 * If it is not possible to set a default value, at least a value has to be set and stored before the value will be returned on request.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (integer) of the preference value.
		 */
		static void setDefault(const PreferenceKeyInt anIdentifier, const int aValue);

		/**
		 * Sets a default preference value (float).
		 * The default preference value is set by VisualPreferences' setDefaultValues().
		 * That way there will always be a preference value present even if the user hasn't set any yet. 
		 * If it is not possible to set a default value, at least a value has to be set and stored before the value will be returned on request.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (float) of the preference value.
		 */
		static void setDefault(const PreferenceKeyFloat anIdentifier, const float aValue);
		
		/**
		 * Sets a default preference value (char).
		 * The default preference value is set by VisualPreferences' setDefaultValues(). 
		 * That way there will always be a preference value present even if the user hasn't set any yet.
		 * If it is not possible to set a default value, at least a value has to be set and stored before the value will be returned on request.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (character) of the preference value.
		 */	
		static void setDefault(const PreferenceKeyChar anIdentifier, const char* const aValue);

		/**
		 * Sets a default preference value (boolean).
		 * The default preference value is set by VisualPreferences' setDefaultValues().
		 * That way there will always be a preference value present even if the user hasn't set any yet. 
		 * If it is not possible to set a default value, at least a value has to be set and stored before the value will be returned on request.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (boolean) of the preference value.
		 */
		static void setDefault(const PreferenceKeyBool anIdentifier, const bool aValue);

#if TARGET_OS_WIN

		/** Helper function for Windows. Makes sure that a certain path exists.
		 * @param iPath The path.
		 * @param FilenameIncluded True if path contains filename, false if path is directory path.
		 * @return Returns the access identifier.
		 */
		int make_sure_path_exists(const char *iPath, bool FilenameIncluded = true);

		/** Helper function for Windows. Answers the question whether a certain file exists.
		 * @param szFileName The path to a file.
		 * @param FilenameIncluded True if path contains filename, false if path is directory path.
		 * @return True if file exists, false if not.
		 * @remarks The file is not created if it does not exist.
		 */
		bool fileExists(const char* szFileName);
#endif

	};

}

#endif /* VisualPreferences_h */
