/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualDataStore.h
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

#ifndef VisualDataStore_h
#define VisualDataStore_h

#include "VisualConfiguration.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#include <windows.h> // DWORD WINAPI
#endif

#include <vector>
#include <string>
#include <map>


struct ITTrackInfo; // Forward declaration (to avoid include of header file).
struct ITStreamInfo; // Forward declaration (to avoid include of header file).

namespace VizKit {

	class VisualAudioMetaData; // Forward declaration (to avoid include of header file).
	class VisualString; // Forward declaration (to avoid include of header file).

	/**
	 * Stores essential data for the visualizer.
	 * The data stored is general (neither graphics related nor audio specific).
	 * All public methods are declared static.
	 */
	class VisualDataStore {

	public:
	
		/**
		 * Disposes the VisualDataStore.
		 */
		static void dispose(void);

		/**
		 * Sets a default preference value (integer).
		 * The default preference value is set by VisualConfiguration's setDefaultPreferences().
		 * That way there will always be a preference value present even if the user hasn't set any yet. 
		 * If it is not possible to set a default value, at least a value has to be set and stored before the value will be returned on request.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (integer) of the preference value.
		 */
		static void setDefaultPreferenceValueInt(const VisualConfiguration::PreferenceKey anIdentifier, const int aValue);

		/**
		 * Sets a default preference value (float).
		 * The default preference value is set by VisualConfiguration's setDefaultPreferences().
		 * That way there will always be a preference value present even if the user hasn't set any yet. 
		 * If it is not possible to set a default value, at least a value has to be set and stored before the value will be returned on request.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (float) of the preference value.
		 */
		static void setDefaultPreferenceValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, const float aValue);
		
		/**
		 * Sets a default preference value (character).
		 * The default preference value is set by VisualConfiguration's setDefaultPreferences(). 
		 * That way there will always be a preference value present even if the user hasn't set any yet.
		 * If it is not possible to set a default value, at least a value has to be set and stored before the value will be returned on request.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (character) of the preference value.
		 */	
		static void setDefaultPreferenceValueChar(const VisualConfiguration::PreferenceKey anIdentifier, const char* const aValue);

		/**
		 * Sets a preference value (integer).
		 * To store a preference persistently, the method storePreferences has to be called afterwards.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (integer) of the preference value.
		*/	
		static void setPreferenceValueInt(const VisualConfiguration::PreferenceKey anIdentifier, const int aValue);

		/**
		 * Sets a preference value (float).
		 * To store a preference persistently, the method storePreferences has to be called afterwards.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (float) of the preference value.
		 */	
		static void setPreferenceValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, const float aValue);

		/**
		 * Sets a preference value (character).
		 * To store a preference persistently, the method storePreferences has to be called afterwards.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (character) of the preference value.
		 */
		static void setPreferenceValueChar(const VisualConfiguration::PreferenceKey anIdentifier, const char* const aValue);

		/**
		 * Sets a persistent value (integer) which is not stored in preferences.\ The value is accessible during runtime.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (integer) of the value.
		 */
		static void setValueInt(const VisualConfiguration::PreferenceKey anIdentifier, const int aValue);

		/**
		 * Sets a persistent value (float) which is not stored in preferences.\ The value is accessible during runtime.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (float) of the value.
		 */
		static void setValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, const float aValue);

		/**
		 * Sets a persistent value (string) which is not stored in preferences.\ The value is accessible during runtime.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aValue The value (string) of the value.
		 */
		static void setValueChar(const VisualConfiguration::PreferenceKey anIdentifier, const char* const aValue);

		/**
		 * Retrieves a persistent value (integer) which was set previously but not stored in preferences.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param wasSet Optional parameter that indicates whether the values was set previously.
		 * @return Returns a persistent value (integer) which was set previously but not stored in preferences.
		 */
		static int getValueInt(const VisualConfiguration::PreferenceKey anIdentifier, bool* wasSet = NULL);

		/**
		 * Retrieves a persistent value (float) which was set previously but not stored in preferences.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param wasSet Optional parameter that indicates whether the values was set previously.
		 * @return Returns a persistent value (float) which was set previously but not stored in preferences.
		 */
		static float getValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, bool* wasSet = NULL);

		/**
		 * Retrieves a persistent value (string) which was set previously but not stored in preferences.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param[out] outPrefVal The character string value.
		 * @param wasSet Optional parameter that indicates whether the values was set previously.
		 * @return Returns a persistent value (string) which was set previously but not stored in preferences.
		 */
		static void getValueChar(const VisualConfiguration::PreferenceKey anIdentifier, char* outPrefVal, bool* wasSet = NULL);
		
		/**
		 * Retrieves a default preference value (integer).
		 * The default preference is handled as a fallback if a preference value is requested.
		 * It is handled as an error if a preference value is requested and no default value has been specified before 
		 * nor any value with the requested key has been saved before.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param isDefaultValue True if default preference value was found.
		 * @return The default integer preference value.
		 */
		static int getDefaultPreferenceValueInt(const VisualConfiguration::PreferenceKey anIdentifier, bool* isDefaultValue);
		
		/**
		 * Retrieves a default preference value (float).
		 * The default preference is handled as a fallback if a preference value is requested.
		 * It is handled as an error if a preference value is requested and no default value has been specified before 
		 * nor any value with the requested key has been saved before.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param isDefaultValue True if default preference value was found.
		 * @return The default float preference value.
		 */
		static float getDefaultPreferenceValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, bool* isDefaultValue);

		/**
		 * Retrieves a default preference value (character).
		 * The default preference is handled as a fallback if a preference value is requested.
		 * It is handled as an error if a preference value is requested and no default value has been specified before 
		 * nor any value with the requested key has been saved before.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param[out] outPrefVal The value of the character preference.
		 * @return True if default preference value was found.
		 */
		static bool getDefaultPreferenceValueChar(const VisualConfiguration::PreferenceKey anIdentifier, char* outPrefVal);

		/**
		 * Retrieves a persistent preference value (integer).
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param isDefaultValue True if returned value is default value.
		 */	
		static int getPreferenceValueInt(const VisualConfiguration::PreferenceKey anIdentifier, bool* isDefaultValue = NULL);

		/**
		 * Retrieves a persistent preference value (float).
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param isDefaultValue True if returned value is default value.
		 */	
		static float getPreferenceValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, bool* isDefaultValue = NULL);

		/**
		 * Retrieves a persistent preference value (character).
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param[out] outPrefVal The value of the character preference.
		 * @param isDefaultValue True if returned value is default value.
		 */
		static void getPreferenceValueChar(const VisualConfiguration::PreferenceKey anIdentifier, char* outPrefVal, bool* isDefaultValue = NULL);
		
		/**
		 * Stores the preferences in persistent storage.
		 */		
		static void storePreferences(void);

		/**
		 * Stores the major and minor version of the host application.
		 * @param majorVersionNum The major version number of the host application.\ 1st part of version number in BCD.
		 * @param minorVersionNum The minor version number of the host application.\ Minor and bug revision part of version number share a byte.
		 */	
		static void setAppVersionNum(UInt8 majorVersionNum, UInt8 minorVersionNum);

		/**
		 * Returns the major version of the host application.
		 * @return The major version number of the host application.
		 */	
		static UInt8 getAppVersionMajorNum(void);

		/**
		 * Returns the minor version of the host application.
		 * @return The minor version number of the host application.
		 */	
		static UInt8 getAppVersionMinorNum(void);

		/**
		 * Stores textual audio track information.
		 * @param trackMetadata Pointer to all accessable track information.
		 * @return True if audio track changed compared to previous one.\ False if metadata is the same as with the previous audio track.
		 */
		static bool storeAudioTrackMetadata(const ITTrackInfo* const trackMetadata);

		/**
		 * Stores textual audio stream information.
		 * @param streamMetadata Pointer to all accessable stream information.
		 * @return True if audio track changed compared to previous one.\ False if metadata is the same as with the previous audio track.
		 */
		static bool storeAudioStreamMetadata(const ITStreamInfo* const streamMetadata);
		
		/**
		 * Answers the question whether the audio meta data has been sent to ensemble already.
		 * @return True if the audio meta data has been sent to ensemble already, false if not.
		 */
		static bool audioMetaDataHasBeenSentToEnsemble(void);

		/**
		 * Flags if the audio meta data has been sent to ensemble.
		 * @param hasBeenSent True if audio meta data has been sent to ensemble, false if not.
		 * @remarks This method is called when the kAudioMetadataIsAvailableMsg has been sent to ensemble members.
		 */
		static void setAudioMetaDataHasBeenSentToEnsemble(bool hasBeenSent);

		/**
		 * Returns the number of bytes of the current track.
		 * @result The number of bytes of the current track.
		 */			
		static UInt32 getTrackSizeInBytes(void);
		
		/**
		 * Returns the name of the current audio track.
		 * @return A reference to the name of current track.
		 */
		static const VisualString& getNameOfCurrentTrack(void);

		/**
		 * Returns the name of the artist of the current audio track.
		 * @return A reference to the name of artist of the current track.
		 */
		static const VisualString& getArtistOfCurrentTrack(void);

		/**
		 * Returns the name of the album of the current audio track.
		 * @return A reference to the name of album of the current track.
		 */
		static const VisualString& getAlbumOfCurrentTrack(void);

		/* Returns the lyrics of the current audio track.
		 * After new lyrics for the current track are available, a notification is sent (kLyricsAreAvailableMsg).
		 * Clients that are interested in being informed about the availability of new lyrics for the current track should register for the notification kLyricsAreAvailableMsg.
		 * @return A reference to the lyrics of the current track. 
		*/
		static const VisualString& getLyricsOfCurrentTrack(void);

		/**
		 * Answers the question whether currently playing audio is stream or track.
		 * @return True if currently playing audio is stream.\ False if currently playing audio is a track.
		 */
		static bool currentlyPlayingAudioIsStream(void);

		/* The lyrics are created.
		 * @remarks The lyrics are created in a separate thread.
		 */
		static void createLyricsOfCurrentTrack(void);

		/* The separate thread in which lyrics are created.
		 * @param parameter Additional parameters (ignored).
		 * @return Status.
		 */
#if TARGET_OS_MAC
		static OSStatus createLyricsOfCurrentTrackThread(void* parameter);
#endif
#if TARGET_OS_WIN
		static DWORD WINAPI createLyricsOfCurrentTrackThread(LPVOID lpParam);
#endif

		/**
		 * Returns the number of cover art works for current track.
		 * Due to a limitation of the API interface of iTunes, currently the maximum is 1.
		 * @return The number of cover art works for current track. 0 if none is found.
		 */
		static UInt16 getNumberOfCoverArtworksOfCurrentTrack(void);
		
		/**
		 * Receives info about cover art of current audio track.
		 * @param[out] albumCoverArtworkFileType The file type of the album cover artwork.
		 * @param[out] albumCoverArtworkHandle A handle to the album cover artwork data.
		 * @return The number of cover artworks.
		 */
		static UInt16 evaluateCoverArtInfo(OSType* albumCoverArtworkFileType, Handle* albumCoverArtworkHandle);
		
		/**
		 * Returns the file type of cover art data.
		 * @return The 4-char-code identification of the file type (e.g.\ JPEG, PNGf).
		 */
		static OSType getCoverArtFileType(void);
		
		/**
		 * Returns a handle to internally stored cover art data for further processing.
		 * @return Handle to cover art data.
		 */
		static Handle getHandleOfCoverArt(void);

		/**
		 * Disposes the cover art data.
		 */
		static void disposeHandleOfCoverArt(void);
		
		/**
		 * Tells iTunes about our preferred fullscreen resolution.
		 * Once the preferred display resolution has been set it can not be revoked.
		 * Only with the next restart of iTunes the default resolution of iTunes can be set again (by not calling PlayerSetFullScreenOptions).
		 * @param minBitsPerPixel Minimum bit depth.
		 * @param maxBitsPerPixel Maximum bit depth.
		 * @param preferredBitsPerPixel Preferred bit depth.
		 * @param horizontalPixels Desired width.
		 * @param verticalPixels Desired height.
		 */	
		static void setPreferredDisplayResolution(UInt16 minBitsPerPixel, UInt16 maxBitsPerPixel, UInt16 preferredBitsPerPixel, UInt16 horizontalPixels, UInt16 verticalPixels);

		/**
		 * Stores label and value of a process info entry that is sent later to the VisualProcessMonitor.
		 * @param labelStr The string of the label.
		 * @param valueStr The value of the process info.
		 */
		static void setProcessInfo(const char* const labelStr, const char* const valueStr);

		/**
		 * Returns a pointer to the process info map.
		 * @return The process info map.
		 */
		static const std::map<std::string, std::string>* const getProcessInfoMap();

		/**
		 * Helper function to convert from data type OSType to char buffer.
		 * @param x in: The 4-char-code OSType string.
		 * @param[out] dest The buffer that receives the 4-char-code as string.
		 */	
		static void OSTypeToString(OSType x, char* dest);

	private:

		/** The constructor.\ VisualDataStore is a singleton class.\ The constructor is private.\ New instance of class can only be created internally. */
		VisualDataStore();

		/** The destructor.\ VisualDataStore is a singleton class.\ The destructor is private.\ Instance of class can only be destructed internally. */
		~VisualDataStore();

		/**
		 * Constructs a VisualDataStore.\ The VisualDataStore internally is a singleton.\ Returns a pointer to the initialized VisualDataStore.
		 * @return A pointer to the singleton instance.
		 */
		static VisualDataStore* getInstance(void);

		/**
		 * Copy constructor.
		 * @param other Another VisualDataStore.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualDataStore(const VisualDataStore& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualDataStore& operator=(const VisualDataStore& other);

		/** VisualDataStore is a singleton class. Pointer to private instance is handled internally. */
		static VisualDataStore* theVisualDataStore;

		/** The format of the data that is stored with the preference. */
		typedef enum {
			kIntPrefType = 0, /**< Integer type. */
			kCharPrefType, /**< 8-bit character type. */
			kFloatPrefType /**< Float type. */
		} PrefDataType;

		/** The status of the preference. */
		typedef enum {
			kDefaultPref = 0, /**< Default (factory) preference. */
			kPrefSetButNotStored, /**< The preference has been set but not stored. */
			kPrefSetAndStored, /**< The preference value is stored persistently. */
			kDoNotStore /**< A runtime value which is not stored in preference system. */
		} PrefStatus;
			
		/** Stores a key-value-pair of preferences. The type of data of the preference is set with the field dataType. */
		class Preference {
		public:
			/** The key of the preference (e.g.\ kFullscreenWidth). */
			VisualConfiguration::PreferenceKey key;
			/** The integer value of the preference (e.g.\ 32). */
			int valueInt;
			/** The float value of the preference (e.g.\ 1.35). */
			float valueFloat;
			/** The char value of the preference (e.g.\ "LucidaGrande"). */
			char valueChar[256];
			/** The default integer value of the preference (e.g.\ 32). */
			int defaultValueInt;
			/** The default float value of the preference (e.g.\ 1.35). */
			float defaultValueFloat;
			/** The default char value of the preference (e.g.\ "LucidaGrande"). */
			char defaultValueChar[256];
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

		/** The major version number of the host application iTunes. */
		UInt8 appVersionMajorNum;
		
		/** The minor version number of the host application iTunes. */
		UInt8 appVersionMinorNum;
			
		/** The number of entries in history of meta data of audio track or stream. */
		static const size_t audioMetaDataHistoryCount;
	
		/** The current index of the history of meta data of audio track or stream. */
		size_t currAudioMetaDataHistoryIdx;

		/** AudioMetaData entries are collected as a vector of pointers to AudioMetaData. */
		typedef std::vector<VisualAudioMetaData*> AudioMetaDataVector;
		
		/** The AudioMetaDataVectorIterator is an iterator of the AudioMetaDataVector. */
		typedef AudioMetaDataVector::iterator AudioMetaDataVectorIterator;
		
		/** Vector of AudioMetaData. */
		AudioMetaDataVector audioMetaDataHistory;
		
		/** Advance (increment) the history of AudioMetaData.\ The current index is moved one forward. */
		void advanceAudioMetaDataHistory(void);	

		/** True if audio meta data has been sent to ensemble of actors. */
		bool audioMetaDataHasBeenSentToEnsembleBool;
		
		/** Semaphore that flags whether the lyrics creation thread is currently running. */
		static bool lyricsCreationThreadIsRunning;

		/** The ProcessInfoMap is a map of strings that denote key and value of process info records. */
		typedef std::map<std::string, std::string> ProcessInfoMap;
		
		/** A map of the process monitor info key-value-pairs. */
		ProcessInfoMap processInfoMap;
		
		/** The ProcessInfoMapIterator is an iterator of the ProcessInfoMap. */
		typedef ProcessInfoMap::iterator ProcessInfoMapIterator;

#if TARGET_OS_WIN

		/** Helper function for Windows. Makes sure that a certain path exists.
		 * @param iPath The path.
		 * @param FilenameIncluded True if path contains filename, false if path is directory path.
		 * @return Returns the access identifier.
		 */
		int make_sure_path_exists(const char *iPath, bool FilenameIncluded = true);

		/** Helper function for Windows. Ansers the question whether a certain file exists.
		 * @param szFileName The path to a file.
		 * @param FilenameIncluded True if path contains filename, false if path is directory path.
		 * @return True if file exists, false if not.
		 * @remarks The file is not created if it does not exist.
		 */
		bool fileExists(LPCSTR szFileName);
#endif

	};

}

#endif /* VisualDataStore_h */
