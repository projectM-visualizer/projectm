/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualStringStyle.h
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

#ifndef VisualStringStyle_h
#define VisualStringStyle_h

#include "VisualGraphicTypes.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	/**
	 * Definition of a style for a string (text).
	 */
	class VisualStringStyle {
		public:
		VisualStringStyle(); /**< Constructor. */
		~VisualStringStyle(); /**< Destructor. */
		char fontNameStr[512]; /**< The name of the font. */
		float fontSize; /**< The size of the font (in points). */
		RGBAColor fontColor; /**< The color of the font. */
		HorizontalAlignment horizontalAlignment; /** The horizontal alignment of the string. */
	};
	
}

#endif /* VisualString_h */
