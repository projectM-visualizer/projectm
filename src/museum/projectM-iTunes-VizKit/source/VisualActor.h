/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualActor.h
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

#ifndef VisualActor_h
#define VisualActor_h


#include "VisualTypes.h"
#include "VisualActorState.h"


namespace VizKit {

	class VisualPlayerState; // Forward declaration (to avoid include of header file).
	class VisualNotification; // Forward declaration (to avoid include of header file).

	/**
	 * VisualActor defines the interface for each specific actor.
	 * VisualActor is an abstract class each actor has to implement.
	 * Some virtual methods are pure abstract interface methods which must be overridden by the concrete actor.
	 * Some virtual methods provide a default implementation which can be overridden by the concrete actor.
	 */
	class VisualActor {

	public:

		/**
		 * The constructor.
		 * Each actor should implement its own constructor that is called additionally.
		 */
		VisualActor();

		/**
		 * The destructor.
		 * Each actor can implement its own destructor.
		 */
		virtual ~VisualActor();

		/**
		 * Copy constructor.
		 */		
		VisualActor(const VisualActor& other);

		/**
		 * Assignment operator.
		 */			
		VisualActor& operator=(const VisualActor& other);

		/**
		 * Copies the current VisualActor and returns a pointer to a new VisualActor.
		 */
		virtual VisualActor* clone(void) const;
		
		/**
		 * Initialization method called immediately after construction.
		 * @remarks The initialization routine can be used to register for notifications by calling VisualNotification::registerNotification().
		 */		
		virtual void init(void);

		/**
		 * Returns the name of the actor. The name is used as identifier by the VisualStageControl.
		 * @remarks Virtual method that does not need to be implemented by subclass. The name of the implemented visual actor is returned if the variable actorName is set correctly.
		 */
		virtual const char* const getName(void);

		/**
		 * Asks the actor to perform individual show.
		 * This is the main method of the actor.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 * @remarks Show() is called by the VisualEnsemble as part of the showEnsemble() method. This interface method should be implemented by subclass to display the main graphic art show of the actor.
		 */
		virtual void show(const VisualPlayerState& visualPlayerState);

		/**
		 * Returns the state of the actor.
		 * Interface method that does not need to be implemented by subclass. 
		 * Default implementation returns the value of the state variable.
		 * @return The state of the actor expressed as VisualActorState.
		 * @remarks If the actor's state is kVisActNoShow, it is not called by the VisualEnsemble.
		 */
		virtual VisualActorState getState(void);

		/**
		 * Sets the state of the actor.
		 * Interface method that does not need to be implemented by subclass. 
		 * Default implementation sets the value of the state variable.
		 * @param actorState The requested state of the actor expressed as VisualActorState.
		 * @remarks If the actor's state is kVisActNoShow, it is not called by the VisualEnsemble.
		 */
		virtual void setState(VisualActorState actorState);

		/**
		 * The actor receives a notification.
		 * Each actor can register for a notification (event/message) by calling VisualNotification's registerNotification().
		 * This function receives the notification.
		 * Hook method that can be implemented by subclass.
		 * @param aNotification The notification package.
		 * @remarks For debugging and diagnostical puposes, the subclass can explicitly call the method of the VisualActor interface class. Each notification key will be written to error log then.
		 */
		virtual void handleNotification(VisualNotification& aNotification);

		/**
		 * After the completion of the show, the actor is called to see if any errors occured.
		 * This method is implemented by the VisualActor interface class. It calls the OpenGL error handler.
		 * Should not be overridden by subclass. Subclasses can implement the method but should then call the superclass implementation inside the subclass implementation also.
		 * @param[out] errorString The error c-string.
		 * @return The error number. 0 is no error.
		 */
		virtual uint8 getError(char* errorString);

	protected:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualActor.
		 */
		void copy(const VisualActor& other);
		
		/** The state of the VisualActor. */
		VisualActorState state;
		
		/** The name of the VisualActor. */
		char actorName[64];
		
	};

}

#endif /* VisualActor_h */
