/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: TemplateActor.h
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

#ifndef TemplateActor_h
#define TemplateActor_h


#include "VisualTypes.h"
#include "VisualActor.h"
#include "VisualItemIdentifier.h"
#include "VisualImage.h"

#include <vector>


namespace VizKit {

	class TemplateAction;

	/**
	 * A template of a VisualActor.
	 * A copy of header and implementation file can be made 
	 * to create a new concrete subclass of VisualActor.
	 */
	class TemplateActor : public VisualActor {

	public:

		/**
		 * The constructor.
		 */
		TemplateActor();

		/**
		 * The destructor.
		 */
		~TemplateActor();

		/**
		 * Initialization.
		 */
		virtual void init(void);
		
		/**
		 * Performs the show of the template.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 */
		virtual void show(const VisualPlayerState& visualPlayerState);

		/**
		 * The actor receives a notification about an event that occured.
		 * @param aNotification The notification passed in.
		 */
		virtual void handleNotification(VisualNotification& aNotification);

		/**
		 * Notification call when string data is completely loaded and constructed.
		 */
		void handleSimilarArtistsStringCompleted();

	private:

		/** A pointer to the template action. */
		TemplateAction* theTemplateAction;
		
		/** Identifier for string request. */
		VisualItemIdentifier similarArtistsStringRequestId;
		
		VisualImage similarArtistsStringImage;
		VisualString* stringOfSimilarArtists;
		
		/** Initiates the creation of the string of similar artists by using URL request. */
		void createStringOfSimilarArtists(void);

		/**
		 * Sets the similar artists and creates the bitmap image of the string.
		 * @param similarArtists String with similar artists.
		 */
		void setStringOfSimilarArtists(const VisualString& similarArtists);

		/**
		 * Vector with lines of similar artists.
		 */
		std::vector<VisualString> similarArtistsStringVector;

		/**
		 * True if similar artists string is currently created.
		 */
		bool similarArtistsStringCreationInProgress;

		/**
		 * Identifier of the current audio track for which simolar artists are queried.
		 */
		VisualItemIdentifier similarArtistsCurrentTrackIdentifier;

	};

}

#endif /* TemplateActor_h */
