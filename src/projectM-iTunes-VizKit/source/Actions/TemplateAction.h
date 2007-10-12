/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: TemplateAction.h
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

#ifndef TemplateAction_h
#define TemplateAction_h

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	/**
	 * Performs the action of the TemplateActor.
	 * A copy of header and implementation file can be made
	 * to add to a new concrete subclass of VisualActor.
	 * The example implementation of the TemplateAction can be used
	 * to get a feeling for working with the perspective projection of OpenGL.
	 */
	class TemplateAction {

	public:

		/**
		 * The constructor.
		 */
		TemplateAction();

		/**
		 * The destructor.
		 */
		~TemplateAction();

		/**
		 * Prepares the action of the template.
		 */	
		void prepareTemplateAction(void);

		/**
		 * Shows the TemplateAction.
		 */
		void show(void);
		
		/**
		 * Finishes the action of the template.
		 */	
		void finishTemplateAction(void);

	private:

		/** The texture number (name/id). */	
		UInt32 textureNumber;
		
		/** The texture width (in pixels). */
		UInt32 textureWidth;
		
		/** The texture height (in pixels). */
		UInt32 textureHeight;
		
		/** The image width (in pixels). */
		UInt32 imageWidth;
		
		/** The image height (in pixels). */
		UInt32 imageHeight;

	};

}

#endif /* TemplateAction_h */
