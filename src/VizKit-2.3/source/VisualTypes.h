/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualTypes.h
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

#ifndef VisualTypes_h
#define VisualTypes_h


#ifdef _MSC_VER
#include <stddef.h>
#else
#include <stdint.h>
#endif

#ifdef _MSC_VER
typedef __int8 sint8;
typedef unsigned __int8 uint8;
typedef __int16 sint16;
typedef unsigned __int16 uint16;
typedef __int32 sint32;
typedef unsigned __int32 uint32;
#else
typedef int8_t sint8;
typedef uint8_t uint8;
typedef int16_t sint16;
typedef uint16_t uint16;
typedef int32_t sint32;
typedef uint32_t uint32;
#endif


#endif /* VisualTypes_h */
