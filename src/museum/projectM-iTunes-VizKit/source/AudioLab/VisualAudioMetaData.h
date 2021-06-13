/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAudioMetaData.h
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

#ifndef VisualAudioMetaData_h
#define VisualAudioMetaData_h


#include "VisualTypes.h"
#include "VisualString.h"


namespace VizKit {

	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).

	/**
	 * Meta data (like track name, artist name, or lyrics, etc.) of audio track or audio stream. 
	 */
	class VisualAudioMetaData {
	
	public:

		/**
		 * Constructor.
		 */
		VisualAudioMetaData();

		/**
		 * Destructor.
		 */
		~VisualAudioMetaData();

		/**
		 * Copy constructor.
		 * @param other Another VisualAudioMetaData.
		 */
		VisualAudioMetaData(const VisualAudioMetaData& other);

		/**
		 * Assignment operator.
		 */			
		VisualAudioMetaData& operator=(const VisualAudioMetaData& other);
		
		/**
		 * Equality operator.
		 * @param other Another VisualAudioMetaData.
		 */
		bool operator==(const VisualAudioMetaData& other);

		/**
		 * Inequality operator.
		 * @param other Another VisualAudioMetaData.
		 */
		bool operator!=(const VisualAudioMetaData& other);

		/**
		 * Resets the values of the audio meta data to their initial default empty values.
		 */
		void init(void);
		
		/**
		 * Sets whether data belongs to stream or track.
		 * @param isAStream If true data is stream meta data. If false data is track meta data.
		 */
		void setIsStream(bool isAStream);

		/**
		 * Sets the name of the track.
		 * @param aTrackName The name of the track.
		 */		
		void setTrackName(const VisualString& aTrackName);

		/**
		 * Sets the artist of the track.
		 * @param aTrackArtist The name of the track artist.
		 */		
		void setTrackArtist(const VisualString& aTrackArtist);

		/**
		 * Sets the name of the album.
		 * @param aTrackAlbum The name of the album the track belongs to.
		 */
		void setTrackAlbum(const VisualString& aTrackAlbum);

		/**
		 * Sets the lyrics of the track.
		 * @param someTrackLyrics The lyrics of the track.
		 */
		void setTrackLyrics(const VisualString& someTrackLyrics);

		/**
		 * Sets the composer of the track.
		 * @param aComposer The composer of the track.
		 */
		void setTrackComposer(const VisualString& aComposer);

		/**
		 * Sets the title of a stream.
		 * @param aStreamTitle The title of a stream.
		 */
		void setStreamTitle(const VisualString& aStreamTitle);

		/**
		 * Sets the message of a stream.
		 * @param aStreamMessage The message of a stream.
		 */
		void setStreamMessage(const VisualString& aStreamMessage);

		/**
		 * Sets the URL of a stream.
		 * @param aStreamURL The URL of a stream.
		 */
		void setStreamURL(const VisualString& aStreamURL);

		/**
		 * Sets the size of the track in bytes.
		 * @param aTrackSizeInBytes The size of the track in bytes.
		 */
		void setTrackSizeInBytes(uint32 aTrackSizeInBytes);

		/**
		 * Sets the year of the audio data.
		 * @param aYear The year of the audio data.
		 */
		void setYear(uint16 aYear);

		/**
		 * Returns the name of the track.
		 * @return The name of the track.
		 */
		const VisualString& getTrackName(void);

		/**
		 * Returns the artist of the track.
		 * @return The artist of the track.
		 */
		const VisualString& getTrackArtist(void);

		/**
		 * Returns the name of the album.
		 * @return The name of the album the track belongs to.
		 */
		const VisualString& getTrackAlbum(void);

		/**
		 * Returns the lyrics of the track.
		 * @return The lyrics of the track.
		 */
		const VisualString& getTrackLyrics(void);

		/**
		 * Returns the composer of the track.
		 * @return The composer of the track.
		 */
		const VisualString& getTrackComposer(void);

		/**
		 * Returns the title of a stream.
		 * @return The title of a stream.
		 */
		const VisualString& getStreamTitle(void);

		/**
		 * Returns the message of a stream.
		 * @return The message of a stream.
		 */
		const VisualString& getStreamMessage(void);

		/**
		 * Returns the URL of a stream.
		 * @return The URL of a stream.
		 */
		const VisualString& getStreamURL(void);

		/**
		 * Returns the size of the track in bytes.
		 * @return The size of the track in bytes.
		 */
		uint32 getTrackSizeInBytes(void);
		
		/**
		 * Returns the year of the audio data in bytes.
		 * @return The year of the audio data in bytes.
		 */
		uint16 getYear(void);

		/**
		 * Returns the unique identifer of the track for the meta data.
		 * @return The unique identifer of the track for the meta data.
		 */
		VisualItemIdentifier getTrackIdentifier(void);

		/**
		 * Answers the question whether audio meta data belongs to stream or track.
		 * @return True if audio meta data belongs to stream. False if audio meta data belongs to track.
		 */
		bool getIsStream(void);

	private:

		/** True if audio meta data belongs to stream. False if audio meta data belongs to track. */
		bool isStream;
		
		/** Name of audio track. */
		VisualString trackName;
		
		/** Name of track artist. */
		VisualString trackArtist;
		
		/** Name of album the track belongs to. */
		VisualString trackAlbum;
		
		/** Lyrics of track. */
		VisualString trackLyrics;

		/** Composer of track. */
		VisualString trackComposer;

		/** Title of a stream. */
		VisualString streamTitle;

		/** Message of a stream. */
		VisualString streamMessage;

		/** URL of a stream. */
		VisualString streamURL;
		
		/** Size of audio track in bytes. */
		uint32 trackSizeInBytes;

		/** The year of the audio track. */
		uint16 year;
		
		/** The identifier of the audio track. */
		VisualItemIdentifier* trackIdentifier;
			
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualAudioMetaData.
		 */
		void copy(const VisualAudioMetaData& other);

	};
	
}

#endif /* VisualAudioMetaData_h */
