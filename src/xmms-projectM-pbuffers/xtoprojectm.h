/**
 * $Id: xtoprojectM.h,v 1.1 2004/10/08 00:35:28 cvs Exp $
 *
 * Translates X -> projectM variables
 *
 * $Log: xtoprojectM.h,v $
 * Revision 1.1  2004/10/08 00:35:28  cvs
 * Moved and imported
 *
 * Revision 1.1.1.1  2004/10/04 12:56:00  cvs
 * Imported
 *
 */

#ifndef _XTOPROJECTM_H
#define _XTOPROJECTM_H

#include <X11/Xlib.h>
#include <X11/keysym.h>


projectMEvent x2pmEvent( XEvent event ) { \
							
    switch ( event.type ) { \
        case ConfigureNotify:
            return PROJECTM_VIDEORESIZE; 
        case ClientMessage:
	    return PROJECTM_VIDEOQUIT;
        case KeyPress:
            return PROJECTM_KEYDOWN; 
        default:
            return PROJECTM_NONE; 
      } \
  } \

projectMKeycode x2pmKeycode( int keysym ) { \
    switch ( keysym ) { \
        case XK_F1: \
            return PROJECTM_K_F1; \
        case XK_F2: \
            return PROJECTM_K_F2; \
        case XK_F3: \
            return PROJECTM_K_F3; \
        case XK_F4: \
            return PROJECTM_K_F4; \
        case XK_F5: \
            return PROJECTM_K_F5; \
        case XK_F6: \
            return PROJECTM_K_F6; \
        case XK_F7: \
            return PROJECTM_K_F7; \
        case XK_F8: \
            return PROJECTM_K_F8; \
        case XK_F9: \
            return PROJECTM_K_F9; \
        case XK_F10: \
            return PROJECTM_K_F10; \
        case XK_F11: \
            return PROJECTM_K_F11; \
        case XK_F12: \
            return PROJECTM_K_F12; \
	  case XK_Escape: \
	    return PROJECTM_K_ESCAPE; 
    case XK_a:
      return PROJECTM_K_a;
    case XK_b:
      return PROJECTM_K_b;
    case XK_c:  
      return PROJECTM_K_c;
    case XK_d: 
      return PROJECTM_K_d; 
    case XK_e:
      return PROJECTM_K_e; 
    case XK_f: 
      return PROJECTM_K_f; 
    case XK_g: 
      return PROJECTM_K_g; 
    case XK_h: 
      return PROJECTM_K_h; 
    case XK_i: 
      return PROJECTM_K_i; 
    case XK_j:
      return PROJECTM_K_j;
    case XK_k:
      return PROJECTM_K_k;
    case XK_l:  
      return PROJECTM_K_l;
    case XK_m: 
      return PROJECTM_K_m; 
    case XK_n:
      return PROJECTM_K_n; 
    case XK_o: 
      return PROJECTM_K_o; 
    case XK_p: 
      return PROJECTM_K_p; 
    case XK_q: 
      return PROJECTM_K_q; 
    case XK_r: 
      return PROJECTM_K_r; 
    case XK_s: 
      return PROJECTM_K_s; 
    case XK_t:
      return PROJECTM_K_t; 
    case XK_u: 
      return PROJECTM_K_u; 
    case XK_v: 
      return PROJECTM_K_v; 
    case XK_w: 
      return PROJECTM_K_w; 
    case XK_x: 
      return PROJECTM_K_x; 
    case XK_y: 
      return PROJECTM_K_y; 
    case XK_z: 
      return PROJECTM_K_z; 
    case XK_Up:
      return PROJECTM_K_UP;
    case XK_Return:
      return PROJECTM_K_RETURN;
    case XK_Right:
      return PROJECTM_K_RIGHT;
    case XK_Left:
      return PROJECTM_K_LEFT;
    case XK_Down:
      return PROJECTM_K_DOWN;
    case XK_Page_Up:
      return PROJECTM_K_PAGEUP;
    case XK_Page_Down:
      return PROJECTM_K_PAGEDOWN;
   
        default: \
            return PROJECTM_K_NONE; \
      } \
  } \

projectMModifier x2pmModifier( int mod ) { 
    return XK_Shift_L;
  } \

#endif 
