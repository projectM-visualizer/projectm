/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualMain.h
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

#ifndef VisualMain_h
#define VisualMain_h

#include "iTunesVisualAPI.h"

#include "VisualTypes.h"


namespace VizKit {

	/**
	 * Handles the events and messages passed in by iTunes.
	 * In its public section VisualMain contains the functions called by iTunes.
	 * Private static functions access the various internal classes of the visualizer.
	 * Essential events are:
	 * audio play start, audio play stop, show start, show stop, and render message.
	 * When displaying graphics, the visualizer spends most of its time with executing the render message.
	 */
	class VisualMain {

	public:

		/**
		 * Called by iTunes to send messages to visualizer plug-in.
		 * Known by iTunes via call to PlayerRegisterVisualPlugin.
						These types of messages can be received:
						kVisualPluginInitMessage
						kVisualPluginCleanupMessage
						kVisualPluginEnableMessage
						kVisualPluginDisableMessage
						kVisualPluginIdleMessage
						kVisualPluginConfigureMessage
						kVisualPluginShowWindowMessage
						kVisualPluginHideWindowMessage
						kVisualPluginSetWindowMessage
						kVisualPluginRenderMessage
						kVisualPluginUpdateMessage
						kVisualPluginPlayMessage
						kVisualPluginChangeTrackMessage
						kVisualPluginStopMessage
						kVisualPluginSetPositionMessage
						kVisualPluginPauseMessage
						kVisualPluginUnpauseMessage
						kVisualPluginEventMessage
						kVisualPluginDisplayChangedMessage
		 * @param message Type of message.
		 * @param visualPluginMessageInfo Additional data.
		 * @param refCon Pointer to untyped additional data.
		 * @return Error status.
		 */
		static OSStatus VisualPluginHandler(OSType message, VisualPluginMessageInfo* visualPluginMessageInfo, void* refCon);

		/**
		 * Called by iTunes, dispatched from main function. Registers the visualizer plug-in to iTunes.
		 * The function calls PlayerRegisterVisualPlugin to be registered by iTunes.
		 * @param pluginMessageInfo A pointer to a data structure passed in by iTunes and sent on to PlayerRegisterVisualPlugin.
		 * @return Status of the operation.
		*/
		static OSStatus RegisterVisualPlugin(PluginMessageInfo* pluginMessageInfo);

	private:

		/** The constructor. VisualMain is a collection of static methods. Class does not need any instances. Constructor is private and not implemented. */
		VisualMain();
		
		/** The destructor. VisualMain is a collection of static methods. Class does not need any instances. Destructor is private and not implemented. */
		~VisualMain();

		/**
		 * Stores textual audio track information.
		 * @param trackMetadata Pointer to all accessible track information.
		 * @param isAudioStream True if track meta data belongs to audio stream. False if meta data belongs to audio track.
		 * @return True if audio track changed compared to previous one. False if metadata is the same as with the previous audio track.
		 */
		static bool storeAudioTrackMetadata(const ITTrackInfo* const trackMetadata, bool isAudioStream);

		/**
		 * Stores textual audio stream information.
		 * @param streamMetadata Pointer to all accessible stream information.
		 */
		static void storeAudioStreamMetadata(const ITStreamInfo* const streamMetadata);

		/**
		 * Stores textual audio stream information (single byte string data).
		 * @param streamMetadata Pointer to all accessible stream information.
		 */
		static void storeAudioStreamMetadata(const ITStreamInfoV1* const streamMetadata);

		/**
		 * Static helper function that converts a VisualPluginMessage to the string. Possibly useful for debugging or tracing purposes.
		 * @param visualPluginMessage The visualPluginMessage.
		 * @param outString The character string value of the VisualNotificationKey enum value.
		 */
		static void convertVisualPluginMessageToString(const OSType visualPluginMessage, char* outString);

	};

}


#ifdef __cplusplus
extern "C" {
#endif

/**
 * C-callable function exported to iTunes. Calls the register function.
 * Gets called by iTunes on init and on cleanup.
 * On init the visualizer plug-in registers itself to iTunes via function RegisterVisualPlugin().
 * Message types: kPluginInitMessage, kPluginCleanupMessage.
 * On Mac the function is named iTunesPluginMainMachO for Mach-O plug-in. The function returns OSStatus.
 * On Windows the function is named iTunesPluginMain and declared with __declspec(dllexport).
 * @param message The kind of message passed in (e.g. kPluginInitMessage).
 * @param pluginMessageInfo Additional data passed in.
 * @param refCon Pointer to untyped additional data.
 * @return Status of the operation.
 */
#if TARGET_OS_MAC
__attribute__((visibility("default")))
extern OSStatus iTunesPluginMainMachO(OSType message, PluginMessageInfo* pluginMessageInfo, void* refCon);
#endif
#if TARGET_OS_WIN
extern __declspec(dllexport) OSStatus iTunesPluginMain(OSType message, PluginMessageInfo* pluginMessageInfo, void* refCon);
#endif

#ifdef __cplusplus
}
#endif

#endif /* VisualMain_h */
