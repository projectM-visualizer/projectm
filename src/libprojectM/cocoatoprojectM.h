/**
  * projectM -- Milkdrop-esque visualisation SDK
  * Copyright (C)2003-2007 projectM Team
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  * See 'LICENSE.txt' included within this release
  *
  */
/**
 *
 * Translates cocoa -> projectM variables
 *
 * $Log$
 */

#ifndef _CARBONTOPROJECTM_H
#define _CARBONTOPROJECTM_H

#ifdef WIN32
#else
#endif

projectMKeycode cocoa2pmKeycode( NSEvent *event ) {
    projectMKeycode char_code = (projectMKeycode)[event keyCode];
    switch ( char_code ) {
        case kVK_F1:
        case kVK_ANSI_H:
            return PROJECTM_K_F1;
        case kVK_F2:
            return PROJECTM_K_F2;
        case kVK_F3:
            return PROJECTM_K_F3;
        case kVK_F4:
            return PROJECTM_K_F4;
        default:
            // try and get ascii code
            NSString *chars = [event charactersIgnoringModifiers];
            if ([chars length]) {
                return (projectMKeycode)[chars characterAtIndex:0];
            }
            return char_code;
    }
}

projectMModifier cocoa2pmModifier( NSEvent *event ) {
    NSUInteger mod = [event modifierFlags];
    if (mod & NSShiftKeyMask)
        return (projectMModifier)PROJECTM_K_LSHIFT;
    if (mod & NSControlKeyMask)
        return (projectMModifier)PROJECTM_K_LCTRL;
    
    return (projectMModifier)0;
}

#endif /** _CARBONTOPROJECTM_H */
