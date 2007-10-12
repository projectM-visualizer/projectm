/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualNotificationKey.h
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

#ifndef VisualNotificationKey_h
#define VisualNotificationKey_h


#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	/** Constants for various notifications that can be sent to VisualActors or dispatched to individual modules. */
	typedef enum {
		kNoNotificationKey = 0, /**< No defined notification. */
		kKeyPressedEvt, /**< A key of the keyboard has been pressed. */
		kAudioPlayStartedEvt, /**< Audio play started. */
		kAudioPlayStoppedEvt, /**< Audio play stopped. */
		kAudioPlayPausedEvt, /**< Audio play paused. The current audio track did not play until its end. */
		kAudioPlayResumedEvt, /**< Audio play resumed. After pause play resumed. */
		kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt, /** The remaining time of the audio track is shorter than the kFadeOutTimeBeforeEndOfTrackInMS. */
		kToggleShowProcessMonitorMsg, /**< The ProcessMonitor values should be shown if they are hidden or hidden if they are shown. */
		kToggleProcessMonitorAudioInfoMsg, /**< The audio data of the ProcessMonitor should be shown if it is hidden or hidden if it is shown. */
		kCanvasReshapeEvt, /**< The dimensions of the canvas changed. */
		kAudioMetadataIsAvailableMsg, /**< The meta data of the current audio track (e.g.\ the title, artist) is available. */
		kTrackInfoTextureIsAvailableMsg, /**< The texture of the track info string is available. */
		kCoverTextureIsAvailableMsg, /**< The album cover art texture is available. */
		kLyricsAreAvailableMsg, /**< Lyrics of the current audio track are available. */
		kLyricsTextureIsAvailableMsg, /**< The texture of the lyrics is available. */
		kTrackInfoTextureChangedMsg /**< The texture of the track info changed. */
	} VisualNotificationKey;

}

#endif /* VisualNotificationKey_h */
