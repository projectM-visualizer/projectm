/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualPlayerState.h
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

#ifndef VisualPlayerState_h
#define VisualPlayerState_h


#include "VisualTypes.h"


namespace VizKit {

	/**
	 * Constants for different modes of audio play.
	 * @remarks To query whether audio is playing or not, AudioPlayState can be bitwise AND'ed with kAudioIsPlaying.
	 */
	typedef enum {
		kAudioIsNotPlaying = 0x0, /**< 00000000 - No audio is playing. */
		kAudioPlayStarted = 0x3, /**< 00000011 - Audio started playing. */
		kAudioIsPlaying = 0x1, /**< 00000001 - Audio is playing. */
		kAudioPlayResumed = 0x5, /**< 00000101 - Audio play resumed after pause. */
		kAudioPlayPaused = 0x2 /**< 00000010 - Audio play is pausing. */
	} AudioPlayState;

	/**
	 * Different modes of showing. Used by var showMode of VisualPlayerState.
	 * @remarks To query whether the visualizer is showing or not, PlayerShowMode can be bitwise AND'ed with kIsShowing.
	 */
	typedef enum {
		kIsNotShowing = 0x0, /**< 00000000 - Default; the visualizer is not showing. */
		kErrorState = 0x80, /**< 10000000 - The visualizer is in error state. Therefore it is showing an error message or nothing. */
		kIsShowing = 0x1, /**< 00000001 - The visualizer is showing. */
		kIsShowingInWindow = 0x1, /**< 00000001 - The visualizer is showing. And it is showing in a window. */
		kIsShowingInFullScreen = 0x3 /**< 00000011 - The visualizer is showing. And it is showing fullscreen. */
	} PlayerShowMode;

	/**
	 * Provides access to the state of the audio player host.
	 */
	class VisualPlayerState {

	public:

		/**
		 * Constructs a VisualPlayerState. The VisualPlayerState acts as a singleton. Returns a pointer to the initialised VisualPlayerState.
		 * @return A pointer to the singleton instance.
		 */
		static VisualPlayerState* getInstance(void);
		
		/**
		 * Disposes the VisualPlayerState.
		 */	
		static void dispose(void);

		/**
		 * Answers the question whether currently audio is playing.
		 * @return True if audio is playing, false if playing of audio is stopped.
		 */
		bool isAudioPlaying(void) const;

		/**
		 * Returns the play state of the visual player.
		 * @return A value of audioPlayState enum.
		 */
		AudioPlayState getAudioPlayState(void) const;

		/**
		 * Returns the current mode of the show state.
		 * @return The current mode of the show state.
		 */
		PlayerShowMode getPlayerShowMode(void) const;
		
		/**
		 * Returns the number of milliseconds of the current audio track that have been passed. 0 if no time value is available. The player position is returned.
		 * @return The number of milliseconds of the current audio track that have been passed.
		 */
		uint32 getElapsedAudioTime(void) const;

		/**
		 * Answers the question whether the remaining time of the current audio track is known.
		 * Streamed audio has no info about remaining time of current track.
		 * @return True if the remaining time of the current audio track is known, false if not.
		 */
		bool remainingAudioTimeIsKnown(void) const;

		/**
		 * Returns the number of milliseconds of the current audio track that are waiting to be played. 0 if no time value is available.
		 * @return The number of milliseconds of the current audio track that are waiting to be played.
		 */
		uint32 getRemainingAudioTime(void) const;

		/**
		 * Returns true if the fadeOut event should be sent because the remaining time is less than kAudioPlayReachedFadeOutTimeEvt.
		 * @return True if the fadeOut event should be sent because the remaining time is less than kAudioPlayReachedFadeOutTimeEvt.
		 */
		bool fadeOutEventShouldBeSent(void);

		/**
		 * Returns the actual number of milliseconds that have passed since the last audio play start event.
		 * AudioPlayStartTime and audioPlayStopTime are mutual exclusive.
		 * If audioPlayStartTime > 0 then audio is playing and audioPlayStopTime == 0.
		 * If audioPlayStopTime > 0 then audio is not playing and audioPlayStartTime == 0.
		 * @return The actual number of milliseconds that have passed since the last audio play start event.
		 */
		uint32 getElapsedAudioPlayStartTime(void) const;

		/**
		 * Returns the actual number of milliseconds that have passed since the last audio play stop event.
		 * AudioPlayStartTime and audioPlayStopTime are mutual exclusive.
		 * If audioPlayStartTime > 0 then audio is playing and audioPlayStopTime == 0.
		 * If audioPlayStopTime > 0 then audio is not playing and audioPlayStartTime == 0.
		 * @return The actual number of milliseconds that have passed since the last audio play stop event.
		 */
		uint32 getElapsedAudioPlayStopTime(void) const;

		/**
		 * Returns true if player is in debug mode, false otherwise.
		 * Useful for temporarily and dynamically activated debug mode that can be checked against in different places.
		 * @return True if player is in debug mode, false otherwise.
		 * @remarks The debug mode does not change anything fundamentally. It is only a flag that can be set and asked for.
		 */
		bool isInDebugMode(void) const;

		/**
		 * Stores the state of audio play.
		 * @param playState A value of audioPlayState enum.
		 */
		void setAudioPlayState(const AudioPlayState playState);
		
		/**
		 * Stores the current mode of the show state.
		 * @param aShowMode The current mode of the show state.
		 */
		void setPlayerShowMode(const PlayerShowMode aShowMode);

		/**
		 * Sets the debug mode.
		 * @param requestedDebugMode The debug mode. True turns debug mode on, false turns it off.
		 */
		void setDebugMode(bool requestedDebugMode);

		/**
		 * The the play position of the current track is set.
		 * @param positionInMS The play position in milliseconds.
		 */	
		void setTrackPlayPositionInMS(const uint32 positionInMS);

		/**
		 * Static helper function that converts an audioPlayState to the string. Possibly useful for debugging or tracing purposes.
		 * @param anAudioPlayState An audioPlayState.
		 * @param outString The character string value of the AudioPlayState enum value.
		 */
		static void convertAudioPlayStateToString(const AudioPlayState anAudioPlayState, char* outString);

		/**
		 * Static helper function that converts an showMode to the string. Possibly useful for debugging or tracing purposes.
		 * @param aShowMode A showMode.
		 * @param outString The character string value of the ShowMode enum value.
		 */
		static void convertPlayerShowModeToString(const PlayerShowMode aShowMode, char* outString);

	private:

		/** The constructor. VisualPlayerState is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualPlayerState();
		
		/** The destructor. VisualPlayerState is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualPlayerState();

		/**
		 * Copy constructor.
		 * @param other Another VisualPlayerState.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualPlayerState(const VisualPlayerState& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualPlayerState& operator=(const VisualPlayerState& other);
	
		/** VisualPlayerState is a singleton class. Pointer to private instance is handled internally. */
		static VisualPlayerState* theVisualPlayerState;

		/** Uses enum audioPlayState to store the state of audio play. 
		 * See enum audioPlayState for possible different states of audio play. 
		 */
		AudioPlayState audioPlayState;
		
		/** Uses enum showMode to store the show mode.
		 * See enum showMode for possible different show modes. 
		 */
		PlayerShowMode showMode;
		
		/** Stores whether the fadeOut event has been sent once during the play of the current audio track. */
		bool fadeOutEventHasBeenSent;

		/** The current play position of the current track (in milliseconds). */
		uint32 trackPlayPositionInMS;
		
		/** True if player is in debug mode, false otherwise. */
		bool debugMode;

	};

}

#endif /* VisualPlayerState_h */
