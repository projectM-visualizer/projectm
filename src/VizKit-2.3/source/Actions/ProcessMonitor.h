/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: ProcessMonitor.h
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

#ifndef ProcessMonitor_h
#define ProcessMonitor_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"

#include <string>
#include <map>


namespace VizKit {

	class VisualAsset; // Forward declaration (to avoid include of header file).

	/**
	 * Shows some information on screen for diagnostical and monitoring purposes.
	 * ProcessMonitor performs the action of the ProcessMonitorActor.
	 */

	class ProcessMonitor {

	public:

		/**
		 * The constructor.
		 */	
		ProcessMonitor();
		
		/**
		 * The destructor.
		 */
		~ProcessMonitor();

		/**
		 * Prepares the show of the ProcessMonitor.
		 */
		void prepareProcessMonitorShow(void);
		
		/**
		 * Shows the info strings of the process monitor.
		 */
		void showInfoStrings(void);

		/**
		 * Shows diagnostic info about audio data (waveform and spectrum).
		 * @param elapsedAudioTime The elapsed time of the current track in milliseconds. 0 if no time value is available.
		 * @param remainingAudioTime The remaining time of the current track in milliseconds. 0 if no time value is available.
		 */
		void showAudioInfo(const uint32 elapsedAudioTime, const uint32 remainingAudioTime);

		/**
		 * The processMonitorInfoMap is registered.
		 * @param processMonitorInfoMap All associative key-value-pairs of the process monitor info map.
		 */
		void registerProcessMonitorInfoMap(const std::map<std::string, std::string>* const processMonitorInfoMap);

		/**
		 * Finishes the ProcessMonitor.
		 */
		void finishProcessMonitorShow(void);

		/**
		 * Updates the vertices of the components of the trackProgressMeter (e.g. after a reshape of the canvas).
		 */	
		void updateProgressMeterVertices(void);
		
	private:

		/**
		 * Sets up the vertices of the components of the trackProgressMeter.
		 */	
		void setupProgressMeterVertices(void);
		
		/**
		 * Calculates the vertices of the components of the trackProgressMeter.
		 */
		void calcTrackProgressMeterVertices(const uint32 elapsedAudioTime, const uint32 remainingAudioTime);
		
		/**
		 * Cleans up the memory allocated for the vertices of the components of the trackProgressMeter.
		 */
		void cleanupProgressMeterVertices(void);
		
		/** The ProcessInfoMap is a map of the process monitor info key-value-pairs. */
		typedef std::map<std::string, std::string> ProcessInfoMap;
		
		/** The map of the process monitor info key-value-pairs. */
		ProcessInfoMap processInfoMap;
		
		/** The ProcessInfoMapIterator is an iterator of the ProcessInfoMap. */
		typedef ProcessInfoMap::iterator ProcessInfoMapIterator;
		
		/** Vertices of background of progress meter. */
		VertexChain progressMeterBackgroundVertices;
		
		/** Vertices of progress meter. */
		VertexChain progressMeterVertices;
		
		/** Vertices of outline of progress meter. */
		VertexChain progressMeterOutlineVertices;
		
		/** Asset of the process monitor. */
		VisualAsset* processMonitorAsset;

	};

}


#endif /* ProcessMonitor_h */
