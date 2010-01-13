/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualStyledString.h
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

#ifndef VisualStyledString_h
#define VisualStyledString_h


#include "VisualTypes.h"
#include "VisualString.h"
#include "VisualStringStyle.h"


namespace VizKit {

	/**
	 * Unicode string with styling or formatting information. 
	 */
	class VisualStyledString : public VisualString {
	
	public:

		/**
		 * The constructor. 
		 */
		VisualStyledString();

		/**
		 * The destructor. 
		 */
		~VisualStyledString();

		/**
		 * Copy constructor.
		 * @param other Another VisualStyledString.
		 */
		VisualStyledString(const VisualStyledString& other);

		/**
		 * Assignment operator.
		 */			
		VisualStyledString& operator=(const VisualStyledString& other);

		/**
		 * Copies the current VisualStyledString and returns a pointer to a new VisualStyledString.
		 */
		virtual VisualStyledString* clone(void) const;

		/**
		 * Initializes a VisualStyledString with a VisualString.
		 * @param anUnstyledString An unstyled VisualString.
		 */
		void initWithVisualString(const VisualString& anUnstyledString);

		/**
		 * Sets the style of the VisualStyledString.
		 * @param aStringStyle A string style.
		 */
		void setStyle(const VisualStringStyle& aStringStyle);

		/**
		 * Returns the style of the string.
		 * @return The style of the string.
		 */
		VisualStringStyle& getStyle(void);

	private:
		
		/** The style information of the string. */
		VisualStringStyle stringStyle;

	};
	
	
}

#endif /* VisualStyledString_h */
