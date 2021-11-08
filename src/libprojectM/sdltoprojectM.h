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
 * $Id: sdltoprojectM.hpp,v 1.1 2004/10/08 00:35:28 cvs Exp $
 *
 * Translates SDL -> projectM variables
 *
 * $Log: sdltoprojectM.hpp,v $
 * Revision 1.1  2004/10/08 00:35:28  cvs
 * Moved and imported
 *
 * Revision 1.1.1.1  2004/10/04 12:56:00  cvs
 * Imported
 *
 */

#ifndef _SDLTOPROJECTM_H
#define _SDLTOPROJECTM_H

#include "event.h"

 //#include "projectM/projectM.hpp"
#ifdef WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

inline projectMEvent sdl2pmEvent(const SDL_Event *event ) { \
							
    switch ( event->type ) { \
        case SDL_WINDOWEVENT_RESIZED:
            return PROJECTM_VIDEORESIZE; \
        case SDL_KEYUP: \
            return PROJECTM_KEYUP; \
        case SDL_KEYDOWN: \
            return PROJECTM_KEYDOWN; \
        default:
            return PROJECTM_KEYUP; \
      } \
  } \

inline projectMKeycode sdl2pmKeycode( SDL_Keycode keycode , SDL_Keymod mod ) { \
    switch ( keycode ) { \
        case SDLK_F1: \
            return PROJECTM_K_F1; \
        case SDLK_F2: \
            return PROJECTM_K_F2; \
        case SDLK_F3: \
            return PROJECTM_K_F3; \
        case SDLK_F4: \
            return PROJECTM_K_F4; \
        case SDLK_F5: \
            return PROJECTM_K_F5; \
        case SDLK_F6: \
            return PROJECTM_K_F6; \
        case SDLK_F7: \
            return PROJECTM_K_F7; \
        case SDLK_F8: \
            return PROJECTM_K_F8; \
        case SDLK_F9: \
            return PROJECTM_K_F9; \
        case SDLK_F10: \
            return PROJECTM_K_F10; \
        case SDLK_F11: \
            return PROJECTM_K_F11; \
        case SDLK_F12: \
            return PROJECTM_K_F12; \
	  case SDLK_ESCAPE: \
	    return PROJECTM_K_ESCAPE; 
    case SDLK_a:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_A;
      return PROJECTM_K_a;
    case SDLK_b:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_B;
      return PROJECTM_K_b;
    case SDLK_c:  
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_C;
      return PROJECTM_K_c;
    case SDLK_d:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_D;
      return PROJECTM_K_d; 
    case SDLK_e:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_E;
      return PROJECTM_K_e; 
    case SDLK_f: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_F;
      return PROJECTM_K_f; 
    case SDLK_g: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_G;
      return PROJECTM_K_g; 
    case SDLK_h: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_H;
      return PROJECTM_K_h; 
    case SDLK_i:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_I;
      return PROJECTM_K_i; 
    case SDLK_j:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_J;
      return PROJECTM_K_j;
    case SDLK_k:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_K;
      return PROJECTM_K_k;
    case SDLK_l:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_L;
      return PROJECTM_K_l;
    case SDLK_m:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_M;
      return PROJECTM_K_m; 
    case SDLK_n:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_N;
      return PROJECTM_K_n; 
    case SDLK_o:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_O;
      return PROJECTM_K_o; 
    case SDLK_p: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_P;
      return PROJECTM_K_p; 
    case SDLK_q:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_Q;
      return PROJECTM_K_q; 
    case SDLK_r: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_R;
      return PROJECTM_K_r; 
    case SDLK_s: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_S;
      return PROJECTM_K_s; 
    case SDLK_t:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_T;
      return PROJECTM_K_t; 
    case SDLK_u:
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_U;
      return PROJECTM_K_u; 
    case SDLK_v: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_V;
      return PROJECTM_K_v; 
    case SDLK_w: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_W;
      return PROJECTM_K_w; 
    case SDLK_x: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_X;
      return PROJECTM_K_x; 
    case SDLK_y: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_Y;
      return PROJECTM_K_y; 
    case SDLK_z: 
      if (mod & KMOD_SHIFT)
        return PROJECTM_K_Z;
      return PROJECTM_K_z; 
    case SDLK_UP:
      return PROJECTM_K_UP;
    case SDLK_RETURN:
      return PROJECTM_K_RETURN;
    case SDLK_SLASH:
      return PROJECTM_K_SLASH;
    case SDLK_BACKSLASH:
      return PROJECTM_K_BACKSLASH;
    case SDLK_RIGHT:
      return PROJECTM_K_RIGHT;
    case SDLK_LEFT:
      return PROJECTM_K_LEFT;
    case SDLK_DOWN:
      return PROJECTM_K_DOWN;
    case SDLK_PAGEUP:
      return PROJECTM_K_PAGEUP;
    case SDLK_PAGEDOWN:
      return PROJECTM_K_PAGEDOWN;
    case SDLK_HOME:
      return PROJECTM_K_HOME;
    case SDLK_END:
      return PROJECTM_K_END;
        default: \
            return PROJECTM_K_NONE; \
      } \
  } \

inline projectMModifier sdl2pmModifier( SDL_Keymod mod ) { \
    return PROJECTM_KMOD_LSHIFT; \
  } \

#endif /** _SDLTOPROJECTM_H */
