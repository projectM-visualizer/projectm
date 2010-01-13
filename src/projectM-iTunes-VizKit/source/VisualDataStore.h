/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualDataStore.h
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

#ifndef VisualDataStore_h
#define VisualDataStore_h


#include "VisualTypes.h"
#include "VisualConfiguration.h"
#include "VisualItemIdentifier.h"
#include "VisualAudioMetaData.h"


#if TARGET_OS_WIN
#include <windows.h> // DWORD WINAPI
#endif

#include <vector>
#include <string>
#include <map>


struct ITTrackInfo; // Forward declaration (to avoid include of header file).
struct ITStreamInfo; // Forward declaration (to avoid include of header file).

namespace VizKit {

	class VisualString; // Forward declaration (to avoid include of header file).
	class VisualImage; // Forward declaration (to avoid include of header file).	

	/**
	 * Stores essential data for the visualizer.
	 * The data stored is general (neither graphics related nor audio specific).
	 * All public methods are declared static.
	 */
	class VisualDataStore {

	public:

		typedef enum {
			kUnknownValue = 0,
			kTrackInfoTextureHeight,
			kLyricsTextureHeight
		} ValueKey;

		/**
		 * Disposes the VisualDataStore.
		 */	
		static void dispose(void);

		/**
		 * Sets a persistent value (integer) which is not stored in preferences. The value is accessible during runtime.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param anIntValue The value (integer) of the value.
		 */
		static void setValue(const ValueKey anIdentifier, const int anIntValue);

		/**
		 * Sets a persistent value (float) which is not stored in preferences. The value is accessible during runtime.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aFloatValue The value (float) of the value.
		 */
		static void setValue(const ValueKey anIdentifier, const float aFloatValue);

		/**
		 * Sets a persistent value (string) which is not stored in preferences. The value is accessible during runtime.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aCharValue The value (string) of the value.
		 */
		static void setValue(const ValueKey anIdentifier, const char* const aCharValue);

		/**
		 * Sets a persistent value (boolean) which is not stored in preferences. The value is accessible during runtime.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param aBoolValue The value (boolean) of the value.
		 */
		static void setValue(const ValueKey anIdentifier, const bool aBoolValue);

		/**
		 * Retrieves a persistent value (integer) which was set previously but not stored in preferences.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param wasSet Optional parameter that indicates whether the values was set previously.
		 * @return Returns a persistent value (integer) which was set previously but not stored in preferences.
		 */
		static int getValueInt(const ValueKey anIdentifier, bool* wasSet = NULL);

		/**
		 * Retrieves a persistent value (float) which was set previously but not stored in preferences.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param wasSet Optional parameter that indicates whether the values was set previously.
		 * @return Returns a persistent value (float) which was set previously but not stored in preferences.
		 */
		static float getValueFloat(const ValueKey anIdentifier, bool* wasSet = NULL);

		/**
		 * Retrieves a persistent value (string) which was set previously but not stored in preferences.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param[out] outPrefVal The character string value.
		 * @param wasSet Optional parameter that indicates whether the values was set previously.
		 * @return Returns a persistent value (string) which was set previously but not stored in preferences.
		 */
		static void getValueChar(const ValueKey anIdentifier, char* outPrefVal, bool* wasSet = NULL);

		/**
		 * Retrieves a persistent value (boolean) which was set previously but not stored in preferences.
		 * @param anIdentifier The identifier (key) of the preference value.
		 * @param wasSet Optional parameter that indicates whether the values was set previously.
		 * @return Returns a persistent value (boolean) which was set previously but not stored in preferences.
		 */
		static bool getValueBool(const ValueKey anIdentifier, bool* wasSet = NULL);

		/**
		 * Stores the major and minor version of the host application.
		 * @param majorVersionNum The major version number of the host application. 1st part of version number in BCD.
		 * @param minorVersionNum The minor version number of the host application. Minor and bug revision part of version number share a byte.
		 */	
		static void setAppVersionNum(uint8 majorVersionNum, uint8 minorVersionNum);

		/**
		 * Returns the major version of the host application.
		 * @return The major version number of the host application.
		 */	
		static uint8 getAppVersionMajorNum(void);

		/**
		 * Returns the minor version of the host application.
		 * @return The minor version number of the host application.
		 */	
		static uint8 getAppVersionMinorNum(void);

		/**
		 * Resets the currently set audio meta data.
		 */
		static void resetCurrAudioMetaData(void);

		/**
		 * Sets the audio track name.
		 * @param audioTrackName The name of the current audio track.
		 * @param audioTrackNameLength The number of characters of the name of the current audio track.
		 */	
		static void setAudioTrackName(const uint16* const audioTrackName, uint32 audioTrackNameLength);

		/**
		 * Sets the audio track artist name.
		 * @param audioTrackArtistName The name of the artist of the current audio track.
		 * @param audioTrackArtistNameLength The number of characters of the name of the artist of the current audio track.
		 */	
		static void setAudioTrackArtistName(const uint16* const audioTrackArtistName, uint32 audioTrackArtistNameLength);

		/**
		 * Sets the audio track album name.
		 * @param audioTrackAlbumName The name of the album of the current audio track.
		 * @param audioTrackAlbumNameLength The number of characters of the name of the album of the current audio track.
		 */	
		static void setAudioTrackAlbumName(const uint16* const audioTrackAlbumName, uint32 audioTrackAlbumNameLength);

		/**
		 * Sets the audio track composer.
		 * @param audioTrackComposer The name of the composer of the current audio track.
		 * @param audioTrackComposerLength The number of characters of the name of the composer of the current audio track.
		 */	
		static void setAudioTrackComposer(const uint16* const audioTrackComposer, uint32 audioTrackComposerLength);

		/**
		 * Sets the title of an audio stream.
		 * @param audioStreamTitle The title of an audio stream.
		 * @param audioStreamTitleLength The number of characters of title of an audio stream.
		 */	
		static void setAudioStreamTitle(const char* const audioStreamTitle, uint32 audioStreamTitleLength);

		/**
		 * Sets the title of an audio stream.
		 * @param audioStreamTitle The title of an audio stream.
		 * @param audioStreamTitleLength The number of characters of title of an audio stream.
		 */	
		static void setAudioStreamTitle(const uint16* const audioStreamTitle, uint32 audioStreamTitleLength);

		/**
		 * Sets the message of an audio stream.
		 * @param audioStreamMessage The title of an audio stream.
		 * @param audioStreamMessageLength The number of characters of the message of an audio stream.
		 */	
		static void setAudioStreamMessage(const char* const audioStreamMessage, uint32 audioStreamMessageLength);

		/**
		 * Sets the message of an audio stream.
		 * @param audioStreamMessage The message of an audio stream.
		 * @param audioStreamMessageLength The number of characters of the message of an audio stream.
		 */	
		static void setAudioStreamMessage(const uint16* const audioStreamMessage, uint32 audioStreamMessageLength);

		/**
		 * Sets the URL of an audio stream.
		 * @param audioStreamURL The URL of an audio stream.
		 * @param audioStreamURLLength The number of characters of the URL of an audio stream.
		 */	
		static void setAudioStreamURL(const char* const audioStreamURL, uint32 audioStreamURLLength);

		/**
		 * Sets the URL of an audio stream.
		 * @param audioStreamURL The URL of an audio stream.
		 * @param audioStreamURLLength The number of characters of the URL of an audio stream.
		 */	
		static void setAudioStreamURL(const uint16* const audioStreamURL, uint32 audioStreamURLLength);

		/**
		 * Sets the size in bytes of the audio track.
		 * @param sizeInBytes The size in bytes of the audio track.
		 */	
		static void setAudioTrackSizeInBytes(int sizeInBytes);

		/**
		 * Sets the year of the audio track.
		 * @param aYear The year of the audio track.
		 */	
		static void setAudioTrackYear(uint16 aYear);

		/**
		 * Sets whether the current audio data is a stream.
		 */
		static void setAudioDataIsStream(bool isStream);

		/**
		 * Stores the lyrics of the current audio track.
		 * @param lyricsString The lyrics of the current audio track.
		 */
		static void setLyricsOfCurrentTrack(const VisualString& lyricsString);

		/**
		 * Analyzes the currently set audio track meta data.
		 * @return True if the track information changed (meaning: a new audio track started).
		 */
		static bool analyzeCurrentlySetMetadata(void);

		/**
		 * Returns a string with the info for the current audio data which can be used for display.
		 * @return String with the info for the current audio data.
		 */
		static VisualString getInfoOfCurrentAudioDataForDisplay(void);

		/**
		 * Returns the number of bytes of the current track.
		 * @return The number of bytes of the current track.
		 */			
		static uint32 getTrackSizeInBytes(void);

		/**
		 * Returns the year of the current track.
		 * @return The year of the current track.
		 */			
		static uint16 getTrackYear(void);
		
		/**
		 * Returns the name of the current audio track.
		 * @return The name of current track.
		 */
		static VisualString getNameOfCurrentTrack(void);

		/**
		 * Returns the name of the artist of the current audio track.
		 * @return The name of artist of the current track.
		 */
		static VisualString getArtistOfCurrentTrack(void);

		/**
		 * Returns the name of the album of the current audio track.
		 * @return The name of album of the current track.
		 */
		static VisualString getAlbumOfCurrentTrack(void);

		/* Returns the lyrics of the current audio track.
		 * After new lyrics for the current track are available, a notification is sent (kLyricsAreAvailableInMetadataMsg).
		 * Clients that are interested in being informed about the availability of new lyrics for the current track should register for the notification kLyricsAreAvailableInMetadataMsg.
		 * @return The lyrics of the current track. 
		*/
		static VisualString getLyricsOfCurrentTrack(void);
		
		/* Returns the identifier of the current audio track.
		 * @return The identifier of the current audio track. 
		*/
		static VisualItemIdentifier getIdentifierOfCurrentTrack(void);

		/**
		 * Answers the question whether currently playing audio is stream or track.
		 * @return True if currently playing audio is stream. False if currently playing audio is a track.
		 */
		static bool currentlyPlayingAudioIsStream(void);

		/**
		 * Creates an image of the album cover artwork of the current audio track.
		 * @return The image of the album cover artwork. Returns NULL if cover image is not available.
		 * @remarks The caller has to release the allocated memory by calling delete().
		 */
		static VisualImage* createCoverArtImage(void);

		/* The lyrics are created.
		 * @return True on success, false on failure.
		 * @remarks The lyrics are created in a separate thread.
		 */
		static bool createLyricsOfCurrentTrack(void);

		/* A separate thread in which lyrics are written into a file on disk.
		 * @param parameter Additional parameters (ignored).
		 * @return Status.
		 * @remarks The target file into which the lyrics were written is added to the notification queue.
		 */
#if TARGET_OS_MAC
		static OSStatus writeLyricsToFile(void* parameter);
#endif
#if TARGET_OS_WIN
		static DWORD WINAPI writeLyricsToFile(LPVOID lpParam);
#endif

		/* Contents of a file are read and the string contents are sent as notification.
		 * @param fileWithStringContent File on disk with string contents.
		 * @return Status.
		 */
#if TARGET_OS_MAC
		static OSStatus readFileAndSendNotificationWithString(void* fileWithStringContent);
#endif
#if TARGET_OS_WIN
		static DWORD readFileAndSendNotificationWithString(LPVOID fileWithStringContent);
#endif

		/* Contents of a file are read, the file is deleted, and the string contents are sent as notification.
		 * @param fileWithStringContent File on disk with string contents.
		 * @return Status.
		 */
#if TARGET_OS_MAC
		static OSStatus readFileAndRemoveFileAndSendNotificationWithString(void* fileWithStringContent);
#endif
#if TARGET_OS_WIN
		static DWORD readFileAndRemoveFileAndSendNotificationWithString(LPVOID fileWithStringContent);
#endif
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
		 * The identifier of the image of the album cover artwork.
		 */
		static const VisualItemIdentifier albumCoverArtworkImageId;

		/**
		 * The identifier of the file with lyrics string data.
		 */
		static VisualItemIdentifier fileWithLyricsStringId;

		/**
		 * The identifier of the track for which the lyrics data is gathered.
		 */
		static VisualItemIdentifier trackIdentifierOfLyricsMetadata;

		/**
		 * The identifier of the styled lyrics string.
		 */
		static const VisualItemIdentifier styledTrackLyricsStringId;

		/**
		 * The identifier of the styled track info string.
		 */
		static const VisualItemIdentifier styledTrackInfoStringId;

		/**
		 * The identifier of the update information string.
		 */
		static const VisualItemIdentifier updateInformationStringId;

	private:

		/** The constructor. VisualDataStore is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualDataStore();

		/** The destructor. VisualDataStore is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualDataStore();

		/**
		 * Constructs a VisualDataStore. The VisualDataStore internally is a singleton. Returns a pointer to the initialized VisualDataStore.
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

		/** The format of the data that is stored with the values. */
		typedef enum {
			kIntPrefType = 0, /**< Integer type. */
			kCharPrefType, /**< 8-bit character type. */
			kFloatPrefType, /**< Float type. */
			kBoolPrefType /**< Float type. */
		} ValueDataType;

		/** Stores a key-value-pair of values. The type of data of the value is set with the field dataType. */
		class Value {
		public:
			/** The key of the value (e.g. kTrackInfoTextureHeight). */
			ValueKey key;
			/** The integer value of the value (e.g. 32). */
			int valueInt;
			/** The float value of the value (e.g. 1.35). */
			float valueFloat;
			/** The char value of the value (e.g. "LucidaGrande"). */
			char valueChar[256];
			/** The boolean value of the value (true or false). */
			bool valueBool;
			/** The format of the value data. */
			ValueDataType dataType;
		};
		
		/** Values are collected as a vector of pointers to Values. */
		typedef std::vector<Value*> ValueVector;
		
		/** The ValueVectorIterator is an iterator of the ValueVector. */
		typedef ValueVector::iterator ValueVectorIterator;
		
		/** Vector of values. */
		ValueVector valueVector;

		/** The major version number of the host application iTunes. */
		uint8 appVersionMajorNum;
		
		/** The minor version number of the host application iTunes. */
		uint8 appVersionMinorNum;
			
		/** The number of entries in history of meta data of audio track or stream. */
		static const size_t audioMetaDataHistoryCount;
	
		/** The current index of the history of meta data of audio track or stream. */
		size_t currAudioMetaDataHistoryIdx;

		/** AudioMetaData entries are collected as a vector of pointers to AudioMetaData. */
		typedef std::vector<VisualAudioMetaData> AudioMetaDataVector;
		
		/** The AudioMetaDataVectorIterator is an iterator of the AudioMetaDataVector. */
		typedef AudioMetaDataVector::iterator AudioMetaDataVectorIterator;
		
		/** Vector of AudioMetaData. */
		AudioMetaDataVector audioMetaDataHistory;
		
		/** Advance (increment) the history of AudioMetaData. The current index is moved one forward. */
		void advanceAudioMetaDataHistory(void);

		/** Instance variable with some temporarily current audio meta data. */
		VisualAudioMetaData currAudioMetaData;
		
		/** Flag that answers whether the lyrics creation thread is currently running. */
		static bool lyricsCreationThreadIsRunning;

		/** The ProcessInfoMap is a map of strings that denote key and value of process info records. */
		typedef std::map<std::string, std::string> ProcessInfoMap;
		
		/** A map of the process monitor info key-value-pairs. */
		ProcessInfoMap processInfoMap;
		
		/** The ProcessInfoMapIterator is an iterator of the ProcessInfoMap. */
		typedef ProcessInfoMap::iterator ProcessInfoMapIterator;

	};

}

#endif /* VisualDataStore_h */
