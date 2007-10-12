/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualActor.h
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

#ifndef VisualActor_h
#define VisualActor_h

#include "VisualActorState.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class VisualPlayerState; // Forward declaration (to avoid include of header file).
	class VisualNotification; // Forward declaration (to avoid include of header file).

	/**
	 * Defines the interface for each specific actor.
	 * It is the superclass each actor has to be a subclass of.
	 * VisualActor is an abstract class each actor has to extend.
	 * Some virtual methods are pure abstract interface methods which must be overridden by the concrete actor.
	 * Some virtual methods provide a default implementation which can be overridden by the concrete actor.
	 */
	class VisualActor {

	public:

		/**
		 * The constructor.
		 * Each actor should implement his/her own constructor that is called additionally.
		 */
		VisualActor();

		/**
		 * The destructor.
		 * Each actor can implement his/her own destructor.
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
		 * Returns the name of the actor. The name is used as identifier by the VisualStageControl.
		 * Virtual method that does not need to be implemented by subclass.\ The name of the implemented visual actor is returned if the variable actorName is set correctly.
		 */
		virtual const char* const getName(void);

		/**
		 * Asks the actor to do preparations for his/her show.
		 * Hook method that can be implemented by subclass.
		 * @param visualPlayerState Read-only access to the VisualPlayerState.
		 * @remarks PrepareShow() is called by the VisualEnsemble as part of the showEnsemble() method.
		 */
		virtual void prepareShow(const VisualPlayerState& visualPlayerState);

		/**
		 * Asks the actor to perform his/her show.
		 * This is the main method of the actor.
		 * This method triggers the main show of the actor.
		 * Interface method that should be implemented by subclass.
		 * @remarks Show() is called by the VisualEnsemble as part of his/her showEnsemble() method.
		 */
		virtual void show(void);

		/**
		 * Asks the actor to finish his/her show.
		 * Hook method that can be implemented by subclass.
		 * @remarks FinishShow() is called by the VisualEnsemble as part of its showEnsemble() method.
		 */
		virtual void finishShow(void);

		/**
		 * Returns the state of the actor.
		 * Interface method that does not need to be implemented by subclass. 
		 * Default implementation returns the value of the state variable.
		 * @return The state of the actor expressed as visualActorState.
		 * @remarks If the actor's state is kVisActNoShow, it is not called by the VisualEnsemble.
		 */
		virtual VisualActorState getState(void);

		/**
		 * The actor receives a notification.
		 * Each actor can register for a notification (event/message) at compile time in VisualStageControl::initVisualStageControl() or in runtime by calling VisualEnsemble's registerObserverForNotification().
		 * This function receives the notification.
		 * Hook method that can be implemented by subclass.
		 * @param aNotification The notification package.
		 * @remarks For debugging puposes, the subclass can explicitly call this method.\ Each notification key will be written to error log then.
		 */
		virtual void handleNotification(const VisualNotification& aNotification);

		/**
		 * After the completion of the show, the actor is called to see if any errors occured.
		 * This method is implemented by the super class.\ It calls the OpenGL error handler.
		 * Should not be overridden by subclass.\ Subclasses can implement the method but should then call the superclass implementation inside the subclass implementation also.
		 * @param[out] errorString The error c-string.
		 * @return The error number. 0 is no error.
		 */
		virtual UInt8 getError(char* errorString);

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
