/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: ProcessMonitorActor.h
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

#ifndef ProcessMonitorActor_h
#define ProcessMonitorActor_h


#include "VisualTypes.h"
#include "VisualActor.h"

#include <string>
#include <map>


namespace VizKit {

	class ProcessMonitor;

	/**
	 * This actor displays textual and graphical information useful for debugging, diagnostics and monitoring purposes.
	 * Visual plug-ins can be debugged by writing output to stdout.
	 * If it is neccessary to monitor reoccuring and changing data in realtime the data can be registered with the 
	 * ProcessMonitorActor and refreshed with each render call. The registered output is displayed on screen.
	 */
	class ProcessMonitorActor : public VisualActor {

	public:

		/**
		 * The constructor.
		 */
		ProcessMonitorActor();

		/**
		 * The destructor.
		 */
		~ProcessMonitorActor(void);

		/**
		 * Initialization.
		 */
		virtual void init(void);
		
		/**
		 * Performs the show of the process monitor.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 */
		virtual void show(const VisualPlayerState& visualPlayerState);

		/**
		 * The actor receives a notification about an event that occured.
		 * @param aNotification The notification passed in.
		 */	
		void handleNotification(VisualNotification& aNotification);

		/**
		 * Sets the state of the process monitor actor.
		 * @param aVisualActorState The state of the process monitor actor.
		 */
		void setState(VisualActorState aVisualActorState);
		
		/**
		 * The processMonitorInfoMap is registered.
		 * @param processMonitorInfoMap All associative key-value-pairs of the process monitor info map.
		 */	
		void registerProcessMonitorInfoMap(const std::map<std::string, std::string>* const processMonitorInfoMap);

		/**
		 * Answers the question whether the audio info is shown or not.
		 * @return True if audio info is shown, false if not.
		 */
		bool isAudioInfoShown(void);

		/**
		 * Sets the state of the audio info.
		 * @param requestedAudioInfoState True if audio info show be shown, false if it should be hidden.
		 */	
		void setShowAudioInfo(bool requestedAudioInfoState);

	private:

		/** A pointer to the process monitor action. */
		ProcessMonitor* processMonitor;
		
		/** Should the audio info be shown? */
		bool showAudioInfoBool;
		
		/** The elapsed time of the current audio track in milliseconds. */
		uint32 elapsedAudioTime;
		
		/** The remaining time of the current audio track in milliseconds. */
		uint32 remainingAudioTime;
		
	};

}

#endif /* ProcessMonitorActor_h */
