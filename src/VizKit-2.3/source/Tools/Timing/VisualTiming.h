/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualTiming.h
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
 
#ifndef VisualTiming_h
#define VisualTiming_h


#include "VisualTypes.h"
#include "VisualItemIdentifier.h"

#include <map>
#include <string>


#if TARGET_OS_WIN
#include <windows.h> // LARGE_INTEGER
#endif


namespace VizKit {

	/**
	 * Provides facilities to deal with and calculate time values. All public methods are declared static.
	 */
	class VisualTiming {

	public:

		/**
		 * Disposes the visual timing.
		 */
		static void dispose(void);
		
		/**
		 * Refreshes the current timestamp.
		 */
		static void update(void);

		/**
		 * Returns the number of milliseconds that have elapsed since first call of the duration item.
		 * Time is measured individually for single duration items. 
		 * The duration items are itentified by name.
		 * This function returns the milliseconds that have passed since 
		 * the duration item first asked for its elapsed milliseconds.
		 * @param durationName The identifier of the duration.
		 * @return The number of milliseconds that have elapsed since first call of the duration item.
		 */
		static uint32 getElapsedMilliSecsSinceReset(const char* const durationName);

		/**
		 * Returns the number of milliseconds that have elapsed since first call of the duration item.
		 * Time is measured individually for single duration items. 
		 * The duration items are itentified by name.
		 * This function returns the milliseconds that have passed since 
		 * the duration item first asked for its elapsed milliseconds.
		 * @param visualDurationIdentifier The identifier of the duration.
		 * @return The number of milliseconds that have elapsed since first call of the duration item.
		 */
		static uint32 getElapsedMilliSecsSinceReset(const VisualItemIdentifier& visualDurationIdentifier);
		
		/**
		 * Returns the number of milliseconds that have elapsed since the last call of the duration item.
		 * Time is measured individually for single duration items. 
		 * The duration items are itentified by name.
		 * This function returns the milliseconds that have passed since the last render call.
		 * @param durationName The identifier of the duration.
		 * @return The number of milliseconds that have elapsed since the last call.
		 */
		static uint32 getElapsedMilliSecsSinceLastCall(const char* const durationName);

		/**
		 * Returns the number of milliseconds that have elapsed since the last call of the duration item.
		 * Time is measured individually for single duration items. 
		 * The duration items are itentified by name.
		 * This function returns the milliseconds that have passed since the last render call.
		 * @param visualDurationIdentifier The identifier of the duration.
		 * @return The number of milliseconds that have elapsed since the last call.
		 */
		static uint32 getElapsedMilliSecsSinceLastCall(const VisualItemIdentifier& visualDurationIdentifier);
		
		/**
		 * Resets the stored timing values for this duration item.
		 * Time is measured individually for single duration items. 
		 * The duration items are itentified by name.
		 * This function resets the timing values for this duration item.
		 * @param durationName The identifier of the duration.
		 */
		static void resetTimestamp(const char* const durationName);

		/**
		 * Resets the stored timing values for this duration item.
		 * Time is measured individually for single duration items. 
		 * The duration items are itentified by name.
		 * This function resets the timing values for this duration item.
		 * @param visualDurationIdentifier The identifier of the duration.
		 */
		static void resetTimestamp(const VisualItemIdentifier& visualDurationIdentifier);

		/**
		 * Returns the current date and time including milliseconds.
		 * The date and time are represented as YYYYMMDDHHMISS000 (year-month-day-hour-minute-seconds-milliseconds).
		 */	
		static const char* const getCurrentMillisecsDateTime(void);
		
		/**
		 * Adds a duration item to the duration map.
		 * @param visualDurationIdentifier The identifier of the duration item.
		 */	
		static void addDurationItemToDurationMap(const VisualItemIdentifier& visualDurationIdentifier);
		
	private:

		/**
		 * Returns a pointer to the VisualTiming. Initializes VisualTiming if neccessary. 
		 * VisualTiming is a singleton.
		 * @return A pointer to the VisualTiming.
		 */
		static VisualTiming* getInstance(void);
		
		/**
		 * A data entry that stores timing values of a duration item.
		 */
		typedef struct {
#if TARGET_OS_MAC
			double firstTimestamp; /**< The timestamp of the first call of the duration item. */
			double lastTimestamp; /**< The timestamp of the last call of the duration item. */
#endif
#if TARGET_OS_WIN
			LARGE_INTEGER firstTimestamp;
			LARGE_INTEGER lastTimestamp;
#endif
		} DurationItem;

		/** A DurationMap is a map of duration items. */
		typedef std::map<VisualItemIdentifier, DurationItem*> DurationMap;

		/** The map of the duration items. */
		DurationMap durationMap;

		/** A DurationMapIterator is the iterator of a DurationMap. */
		typedef DurationMap::iterator DurationMapIterator;
		
		/**
		 * Returns the difference between one timestamp and another.
		 * @param young_timestamp The timestamp that was recorded before old_timestamp.
		 * @param old_timestamp The timestamp that was recorded after young_timestamp.
		 * @return The difference between young_timestamp and old_timestamp.
		 */
#if TARGET_OS_MAC
		double getTimeDelta(double young_timestamp, double old_timestamp);
#endif
#if TARGET_OS_WIN
		double getTimeDelta(LARGE_INTEGER young_timestamp, LARGE_INTEGER old_timestamp);
#endif

		/** The timestamp of the current render call. */
#if TARGET_OS_MAC
		double currentTime;
#endif
#if TARGET_OS_WIN
		LARGE_INTEGER currentTime;
#endif

		/** VisualTiming is a singleton class. Pointer to private instance is handled internally. */
		static VisualTiming* theVisualTiming;
		
		/** The frequency of QueryPerformanceFrequency. Windows only. */
#if TARGET_OS_WIN
		LARGE_INTEGER freq; // QueryPerformanceFrequency (unsigned __int64 ?)
#endif

		/** A string representing date and time (YYYYMMDDHHMISS). */
		char dateTimeStr[32];

		/** The constructor. VisualTiming is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualTiming();

		/**
		 * The destructor.
		 */
		~VisualTiming();

		/**
		 * Copy constructor.
		 * @param other Another VisualTiming.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualTiming(const VisualTiming& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualTiming& operator=(const VisualTiming& other);

	};

}


#endif /* VisualTiming_h */
