/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualAudioMetaData.h
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

#ifndef VisualAudioMetaData_h
#define VisualAudioMetaData_h

#include "VisualString.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

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
		 * Sets whether data belongs to stream or track.
		 * @param isAStream If true data is stream meta data.\ If false data is track meta data.
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
		 * Sets the size of the track in bytes.
		 * @param aTrackSizeInBytes The size of the track in bytes.
		 */
		void setTrackSizeInBytes(UInt32 aTrackSizeInBytes);

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
		 * Returns the size of the track in bytes.
		 * @return The size of the track in bytes.
		 */
		UInt32 getTrackSizeInBytes(void);

		/**
		 * Answers the question whether audio meta data belongs to stream or track.
		 * @return True if audio meta data belongs to stream.\ False if audio meta data belongs to track.
		 */
		bool getIsStream(void);

		/**
		 * Returns the number of available album cover artworks.
		 * @return The number of available album cover artworks.
		 */
		UInt16 getNumberOfArtworks(void);

		/**
		 * Returns the file type of album cover artwork.
		 * @return The file type of album cover artwork.
		 */
		OSType getAlbumCoverArtworkFileType(void);

		/**
		 * Returns a handle to the album cover artwork.
		 * @return A handle to the album cover artwork.
		 */
		Handle getAlbumCoverArtworkHandle(void);

		/**
		 * Disposes any album cover artwork handle (and associated data).
		 */
		void disposeAlbumCoverArtworkHandle(void);

	private:

		/** True if audio meta data belongs to stream.\ False if audio meta data belongs to track. */
		bool isStream;
		
		/** Name of audio track. */
		VisualString trackName;
		
		/** Name of track artist. */
		VisualString trackArtist;
		
		/** Name of album the track belogs to. */
		VisualString trackAlbum;
		
		/** Lyrics of track. */
		VisualString trackLyrics;
		
		/** Size of audio track in bytes. */
		UInt32 trackSizeInBytes;
		
		/** Flag denoting whether album cover artwork data has been evaluated yet. */
		bool albumCoverArtworkHasBeenEvaluated;

		/** The number of artworks for current track.
		 * Note that iTunes can handle more than one cover art work. But the API interface we use to query iTunes about the number of 
		 * cover art works for current track cannot handle more than one cover art work. 
		 * The function PlayerGetCurrentTrackCoverArt only returns the first cover art work for current track.
		 * So the maximum value for this var will be 1.
		 */		
		UInt16 numberOfArtworks;
		
		/** The 4-char-code identification of the file type (e.g.\ JPGf, PNGf). */
		OSType albumCoverArtworkFileType;
		
		/** Handle to album cover artwork data. */
		Handle albumCoverArtworkHandle;
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualAudioMetaData.
		 */
		void copy(const VisualAudioMetaData& other);

	};
	
}

#endif /* VisualAudioMetaData_h */
