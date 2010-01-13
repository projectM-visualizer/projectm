/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: TemplateAction.h
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

#ifndef TemplateAction_h
#define TemplateAction_h


#include "VisualTypes.h"
#include "VisualItemIdentifier.h"


namespace VizKit {

	class VisualAsset; // Forward declaration (to avoid include of header file).
	class VisualImage; // Forward declaration (to avoid include of header file).

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
		 * Shows the TemplateAction.
		 */
		void show(void);

		/**
		 * Sets the image of TemplateAction's testAsset.
		 * @param anImage The image of the template.
		 */
		void setImage(const VisualImage& anImage);

		/**
		 * Removes the image of TemplateAction's testAsset.
		 */
		void removeImage(void);

	private:

		/**
		 * Asset of the TemplateAction for visualization of spatial layout.
		 */
		VisualAsset* templateAsset;

		/**
		 * An asset for testing.
		 */
		VisualAsset* testAsset;

		/**
		 * The vertex chain of the TemplateAction's asset.
		 */
		VisualItemIdentifier vertexChainId;

	};

}

#endif /* TemplateAction_h */
