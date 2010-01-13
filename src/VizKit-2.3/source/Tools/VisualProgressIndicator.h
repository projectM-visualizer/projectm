/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualProgressIndicator.h
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

#ifndef VisualProgressIndicator_h
#define VisualProgressIndicator_h


#include "VisualTypes.h"


namespace VizKit {

	/**
	 * A VisualProgressIndicator communicates the progress of a process. Basically the indicator goes from 0.0 to 1.0.
	 */
	class VisualProgressIndicator {
	
	public:

		/** Progress did not start yet. */
		static const double kProgressNotStarted;

		/** Progress is not known (possibly because the complete amount of data is not known). */
		static const double kProgressUnknown;

		/**
		 * The constructor. 
		 */
		VisualProgressIndicator();

		/**
		 * The destructor.
		 */
		~VisualProgressIndicator();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualProgressIndicator.
		 */
		VisualProgressIndicator(const VisualProgressIndicator& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualProgressIndicator.
		 */
		VisualProgressIndicator& operator=(const VisualProgressIndicator& other);

		/**
		 * Returns the current state of the progress indicator.
		 * @return The current state of the progress indicator.
		 */
		double getState(void) const;

		/**
		 * Sets the state of the progress indicator.
		 * @param state The state of the progress indicator.
		 */
		void setState(double state);

	private:

		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualProgressIndicator.
		 */
		void copy(const VisualProgressIndicator& other);
		
		/** The current state of the progress. */
		double currentProgressState;

	};

}

#endif /* VisualProgressIndicator_h */
