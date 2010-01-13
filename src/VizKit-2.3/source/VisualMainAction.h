/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualMainAction.h
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

#ifndef VisualMainAction_h
#define VisualMainAction_h

#include "VisualTypes.h"

#if TARGET_OS_MAC
#include <Carbon/Carbon.h> // CGrafPtr
#endif

#if TARGET_OS_WIN
#include <windows.h> // HWND
#endif


namespace VizKit {

#if TARGET_OS_WIN
#define VIZKIT_GRAPHICS_DEVICE		HWND
#else
#define VIZKIT_GRAPHICS_DEVICE		CGrafPtr
#endif

	/**
	 * Handles the events and messages passed in by iTunes.
	 * In its public section VisualMain contains the functions called by iTunes.
	 * Private static functions access the various internal classes of the visualizer.
	 * Essential events are:
	 * audio play start, audio play stop, show start, show stop, and render message.
	 * When displaying graphics, the visualizer spends most of its time with executing the render message.
	 */
	class VisualMainAction {

	public:

		/**
		 * Updates the current timestamp of the timing engine.
		 */	
		static void updateTiming(void);

		/**
		 * Stored the dimensions of the draw rectangle.
		 * @param top The top pixel position.
		 * @param left The left pixel position.
		 * @param bottom The bottom pixel position.
		 * @param right The right pixel position.
		 */	
		static void setDrawRect(int top, int left, int bottom, int right);

		/**
		 * Stored the dimensions of the total visualizer rectangle.
		 * @param top The top pixel position.
		 * @param left The left pixel position.
		 * @param bottom The bottom pixel position.
		 * @param right The right pixel position.
		 */	
		static void setTotalVisualizerRect(int top, int left, int bottom, int right);

		/**
		 * Stores the vertical offset of the surface rect from the bottom of the iTunes window.
		 * @param top The top pixel position of the total visualizer rect.
		 * @param height The height of the total visualizer rect.
		 */	
		static void setTotalVisualizerRectVerticalOffsetFromBottom(int top, int height);

		/**
		 * Handles initialization.
		 * @param majorRev Major revision number.
		 * @param minorRev Minor revision number.
		 */	
		static void setAppVersionNum(int majorRev, int minorRev);

		/**
		 * Handles initialization.
		 * @param maxDataChannels The number of data channels (2 for stereo).
		 * @param numberOfWaveformEntries The number of waveform data entries.
		 * @param numberOfSpectrumEntries The number of spectrum data entries.
		 */	
		static void handleInitMessage(int maxDataChannels, int numberOfWaveformEntries, int numberOfSpectrumEntries);

		/**
		 * Sets the pointer to the graphics device.
		 * @param device The device to use for rendering.
		 */	
		static void setGraphicsDevice(VIZKIT_GRAPHICS_DEVICE device);

		/**
		 * Handles the show windows message.
		 * @param isFullScreen True if fullscreen.
		 */
		static void handleShowWindowMessage(bool isFullScreen);

		/**
		 * Handles the set window message.
		 * @param isFullScreen True if fullscreen.
		 */	
		static void handleSetWindowMessage(bool isFullScreen);

		/**
		 * Handles the render message.
		 * @param currentPositionInMS The current position in the audio track.
		 * @param timeStampID Numeric id.
		 * @param numberOfWaveformChannels The number of waveform channels.
		 * @param numberOfWaveformEntries The number of waveform data entries.
		 * @param waveformData The waveform data.
		 * @param numberOfSpectrumChannels The number of spectrum channels.
		 * @param numberOfSpectrumEntries The number of spectrum data entries.
		 * @param spectrumData The spectrum data.
		 */	
		static void handleRenderMessage(int currentPositionInMS, int timeStampID, int numberOfWaveformChannels, int numberOfWaveformEntries, char* waveformData, int numberOfSpectrumChannels, int numberOfSpectrumEntries, char* spectrumData);

		/**
		 * Resets the audio lab data for the play message (in case the audio track does not unpause).
		 */	
		static void resetVisualAudioLabDataIfNotPaused();

		/**
		 * Prepares rendering.
		 * @return False on any failure.
		 */	
		static bool prepareRenderAction(void);

		/**
		 * The rendering action executed with each kVisualPluginRenderMessage/kVisualPluginIdleMessage.
		 * @remarks Probably the most often called function.
		 */	
		static void renderAction(void);

		/**
		 * Finishes rendering.
		 */	
		static void finishRenderAction(void);

		/**
		 * Handles the hide window message.
		 */	
		static void handleHideWindowMessage(void);

		/**
		 * Handles the idle message.
		 */	
		static void handleIdleAndUpdateMessage(void);

		/**
		 * Called when iTunes stops playing audio.
		 * Gathered audio data is cleared when the audio stops playing.
		 */
		static void handleAudioPlayStoppedEvent(void);

		/**
		 * Called when iTunes starts playing audio.
		 */
		static void handleAudioPlayStartedEvent(bool trackDidChange);

		/**
		 * Updates the tracked play position.
		 */
		static void setTrackPlayPositionInMS(int trackPlayPosition);

		/**
		 * Called when iTunes starts showing the visualizer.
		 * @param isFullscreen Set to true if fullscreen.
		 */
		static void handleShowShouldStartMessage(bool isFullscreen);

		/**
		 * Called when rendering of visualizer plug-in is closed.
		 */
		static void closeThreads(void);

		/**
		 * Called when visualizer plug-in is unloaded.
		 */
		static void closePersistentStore(void);

		/**
		 * Called when canvas is resized.
		 */
		static void handleCanvasReshape(void);

		/**
		 * Handles an OS event.
		 * @param anEvent The event to handle.
		 */
		static void handleOSEvent(void* anEvent);

		/**
		 * Opens the dialog UI for configuration options.
		 */
		static void showConfigurationDialog(void);

		/**
		 * Resets the currently set audio meta data.
		 */
		static void resetCurrAudioMetaData(void);

		/**
		 * Sets the name of the audio track.
		 */
		static void setAudioTrackName(const void* const audioTrackName, int audioTrackNameLength);

		/**
		 * Sets the artist name of the audio track.
		 */
		static void setAudioTrackArtistName(const void* const audioTrackArtistName, int audioTrackArtistNameLength);

		/**
		 * Sets the album name of the audio track.
		 */
		static void setAudioTrackAlbumName(const void* const audioTrackAlbumName, int audioTrackAlbumNameLength);

		/**
		 * Sets the name of the composer of the audio track.
		 */
		static void setAudioTrackComposer(const void* const audioTrackComposer, int audioTrackComposerLength);

		/**
		 * Sets the title of an audio stream.
		 */
		static void setAudioStreamTitleV1(const char* const audioStreamTitle, int audioStreamTitleLength);

		/**
		 * Sets the title of an audio stream.
		 */
		static void setAudioStreamTitle(const void* const audioStreamTitle, int audioStreamTitleLength);

		/**
		 * Sets the message of an audio stream.
		 */
		static void setAudioStreamMessageV1(const char* const audioStreamMessage, int audioStreamMessageLength);

		/**
		 * Sets the message of an audio track.
		 */
		static void setAudioStreamMessage(const void* const audioStreamMessage, int audioStreamMessageLength);

		/**
		 * Sets the URL of an audio stream.
		 */
		static void setAudioStreamURLV1(const char* const audioStreamURL, int audioStreamURLLength);

		/**
		 * Sets the URL of an audio track.
		 */
		static void setAudioStreamURL(const void* const audioStreamURL, int audioStreamURLLength);

		/**
		 * Sets the size in bytes of the audio track.
		 */
		static void setAudioTrackSizeInBytes(int audioTrackSizeInBytes);

		/**
		 * Sets the year of the audio track.
		 */
		static void setAudioTrackYear(int aYear);

		/**
		 * Sets whether the current audio data is a stream.
		 */
		static void setAudioDataIsStream(bool isStream);

		/**
		 * Analyzes the currently set audio track meta data.
		 * @return True if the track information changed (meaning: a new audio track started).
		 */
		static bool analyzeCurrentlySetMetadata(void);

		/**
		 * Sets the total time of the current audio track.
		 */
		static void setTotalTimeOfCurrentTrack(int totalTimeInMS);

		/**
		 * Sets the start time of the current audio track.
		 */
		static void setStartTimeOfCurrentTrack(int startTimeInMS);

		/**
		 * Sets the stop time of the current audio track.
		 */
		static void setStopTimeOfCurrentTrack(int stopTimeInMS);

		/**
		 * Posts the track info string of the current audio data.
		 */
		static void postTrackInfoOfCurrentAudioData(void);

		/**
		 * Posts the lyrics of the current audio data.
		 * @return True on success, false on failure.
		 */
		static bool postLyricsOfCurrentAudioData(void);

		/**
		 * Posts the album cover artwork of the current audio data.
		 */
		static void postAlbumCoverArtworkOfCurrentAudioData(void);

		/**
		 * Writes a 8-bit string to the system log file.
		 * @param infoStr The 8-bit string to write to log file.
		 */
		static void writeLogEntry(const char* const infoStr);

	private:

		/** The constructor. VisualMainAction is a collection of static methods. Class does not need any instances. Constructor is private and not implemented. */
		VisualMainAction();
		
		/** The destructor. VisualMainAction is a collection of static methods. Class does not need any instances. Destructor is private and not implemented. */
		~VisualMainAction();

	};

}

#endif /* VisualMainAction_h */
