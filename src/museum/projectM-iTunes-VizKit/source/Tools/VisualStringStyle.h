/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualStringStyle.h
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

#ifndef VisualStringStyle_h
#define VisualStringStyle_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"
#include "VisualObject.h"


namespace VizKit {

	/**
	 * Definition of a style for a string (text).
	 */
	class VisualStringStyle : public VisualObject {
	public:
		VisualStringStyle(); /**< Constructor. */
		~VisualStringStyle(); /**< Destructor. */
		/**
		 * Copy constructor.
		 * @param other Reference to another VisualStringStyle.
		 */
		VisualStringStyle(const VisualStringStyle& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualStringStyle.
		 */
		VisualStringStyle& operator=(const VisualStringStyle& other);
		char fontNameStr[512]; /**< The name of the font. */
		double fontSize; /**< The size of the font (in points). */
		RGBAColor fontColor; /**< The color of the font. */
		HorizontalAlignment horizontalAlignment; /** The horizontal alignment of the string. */
		
		/**
		 * Copies the current VisualStringStyle and returns a pointer to a new VisualStringStyle.
		 */
		virtual VisualStringStyle* clone(void) const;
		
	private:
		/**
		 * Copy method for assignmentVisualStringStyleoperator and copy constructor.
		 * @param other Another VisualString.
		 */
		void copy(const VisualStringStyle& other);
	};
	
}

#endif /* VisualString_h */
