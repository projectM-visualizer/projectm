/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualNotificationKey.h
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

#ifndef VisualNotificationKey_h
#define VisualNotificationKey_h


#include "VisualTypes.h"


namespace VizKit {

	/** Constants for various notifications that can be sent to VisualActors or dispatched to individual modules. */
	typedef enum {
		kNoNotificationKey = 0, /**< No defined notification. */
		kKeyPressedEvt, /**< A key of the keyboard has been pressed. */
		kAudioPlayTrackChangedEvt, /**< Audio track did change. */
		kAudioPlayStartedEvt, /**< Audio play started (without changing audio track). */
		kAudioPlayStoppedEvt, /**< Audio play stopped. */
		kAudioPlayPausedEvt, /**< Audio play paused. The current audio track did not play until its end. */
		kAudioPlayResumedEvt, /**< Audio play resumed. After pause play resumed. */
		kAudioPlayReachedFadeOutTimeEvt, /**< The remaining time of the audio track is shorter than kFadeOutTimeBeforeEndOfTrackInMS. */
		kCanvasReshapeEvt, /**< The dimensions of the canvas changed. */
		kBeatImpulseEvt, /**< A beat impulse has been detected. */
		kTrackInfoImageMsg, /**< The texture of the track info string is available. */
		kTrackLyricsImageMsg, /**< The texture of the track lyrics string is available. */
		kLoadingEncodedImageDataCompletedMsg, /**< Loading of image data completed. */
		kImageHistogramCompletedMsg, /**< A histogram of an image is available. */
		kImageWithIdentifierLoadedAndCreatedMsg, /**< Loading and creation of image completed. */
		kImageWithIdentifierMsg, /**< An image with an identifier is available. */
		kImageWriteToPNGFileMsg, /**< An image is supposed to be written as PNG file on disk. */
		kImageWriteToPNGFileAndDeleteMsg, /**< An image is supposed to be written as PNG file on disk, with the VisualImage object being deleted afterwards. */
		kGetTrackInfoStringStyleMsg, /**< Evaluate the string style of the track info string. */
		kGetTrackLyricsStringStyleMsg, /**< Evaluate the string style of the lyics string. */
		kStyledStringMsg, /**< A styled string is available. */
		kStringWithIdentifierLoadedAndCreatedMsg, /**< Loading and creation of string completed. */
		kLyricsAreWrittenIntoTempFileMsg, /**< Lyrics are written into temp file (internal notification, not for actors). */
		kLyricsAreAvailableInMetadataMsg, /**< Lyrics of the current audio track are available. */
		kTrackInfoTextureChangedMsg, /**< The texture of the track info changed. */
		kToggleShowProcessMonitorMsg, /**< The ProcessMonitor values should be shown if they are hidden or hidden if they are shown. */
		kToggleProcessMonitorAudioInfoMsg /**< The audio data of the ProcessMonitor should be shown if it is hidden or hidden if it is shown. */
		
	} VisualNotificationKey;

}

#endif /* VisualNotificationKey_h */
