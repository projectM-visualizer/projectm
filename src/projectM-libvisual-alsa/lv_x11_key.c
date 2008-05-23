/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2004 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#ifdef __SVR4
#include <X11/Sunkeysym.h>
#endif
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include "lv_x11_key.h"

int lv_x11_key_init (LVX11Key *x11key)
{
	int i;

	/* Odd keys used in international keyboards */
	for ( i=0; i<VISUAL_TABLESIZE(x11key->ODD_keymap); ++i )
		x11key->ODD_keymap[i] = VKEY_UNKNOWN;

#ifdef XK_dead_circumflex
	/* These X keysyms have 0xFE as the high byte */
	x11key->ODD_keymap[XK_dead_circumflex&0xFF] = VKEY_CARET;
#endif
#ifdef XK_ISO_Level3_Shift
	x11key->ODD_keymap[XK_ISO_Level3_Shift&0xFF] = VKEY_MODE; /* "Alt Gr" key */
#endif

	/* Map the miscellaneous keys */
	for ( i=0; i<VISUAL_TABLESIZE(x11key->MISC_keymap); ++i )
		x11key->MISC_keymap[i] = VKEY_UNKNOWN;

	/* These X keysyms have 0xFF as the high byte */
	x11key->MISC_keymap[XK_BackSpace&0xFF] = VKEY_BACKSPACE;
	x11key->MISC_keymap[XK_Tab&0xFF] = VKEY_TAB;
	x11key->MISC_keymap[XK_Clear&0xFF] = VKEY_CLEAR;
	x11key->MISC_keymap[XK_Return&0xFF] = VKEY_RETURN;
	x11key->MISC_keymap[XK_Pause&0xFF] = VKEY_PAUSE;
	x11key->MISC_keymap[XK_Escape&0xFF] = VKEY_ESCAPE;
	x11key->MISC_keymap[XK_Delete&0xFF] = VKEY_DELETE;

	x11key->MISC_keymap[XK_KP_0&0xFF] = VKEY_KP0;		/* Keypad 0-9 */
	x11key->MISC_keymap[XK_KP_1&0xFF] = VKEY_KP1;
	x11key->MISC_keymap[XK_KP_2&0xFF] = VKEY_KP2;
	x11key->MISC_keymap[XK_KP_3&0xFF] = VKEY_KP3;
	x11key->MISC_keymap[XK_KP_4&0xFF] = VKEY_KP4;
	x11key->MISC_keymap[XK_KP_5&0xFF] = VKEY_KP5;
	x11key->MISC_keymap[XK_KP_6&0xFF] = VKEY_KP6;
	x11key->MISC_keymap[XK_KP_7&0xFF] = VKEY_KP7;
	x11key->MISC_keymap[XK_KP_8&0xFF] = VKEY_KP8;
	x11key->MISC_keymap[XK_KP_9&0xFF] = VKEY_KP9;
	x11key->MISC_keymap[XK_KP_Insert&0xFF] = VKEY_KP0;
	x11key->MISC_keymap[XK_KP_End&0xFF] = VKEY_KP1;	
	x11key->MISC_keymap[XK_KP_Down&0xFF] = VKEY_KP2;
	x11key->MISC_keymap[XK_KP_Page_Down&0xFF] = VKEY_KP3;
	x11key->MISC_keymap[XK_KP_Left&0xFF] = VKEY_KP4;
	x11key->MISC_keymap[XK_KP_Begin&0xFF] = VKEY_KP5;
	x11key->MISC_keymap[XK_KP_Right&0xFF] = VKEY_KP6;
	x11key->MISC_keymap[XK_KP_Home&0xFF] = VKEY_KP7;
	x11key->MISC_keymap[XK_KP_Up&0xFF] = VKEY_KP8;
	x11key->MISC_keymap[XK_KP_Page_Up&0xFF] = VKEY_KP9;
	x11key->MISC_keymap[XK_KP_Delete&0xFF] = VKEY_KP_PERIOD;
	x11key->MISC_keymap[XK_KP_Decimal&0xFF] = VKEY_KP_PERIOD;
	x11key->MISC_keymap[XK_KP_Divide&0xFF] = VKEY_KP_DIVIDE;
	x11key->MISC_keymap[XK_KP_Multiply&0xFF] = VKEY_KP_MULTIPLY;
	x11key->MISC_keymap[XK_KP_Subtract&0xFF] = VKEY_KP_MINUS;
	x11key->MISC_keymap[XK_KP_Add&0xFF] = VKEY_KP_PLUS;
	x11key->MISC_keymap[XK_KP_Enter&0xFF] = VKEY_KP_ENTER;
	x11key->MISC_keymap[XK_KP_Equal&0xFF] = VKEY_KP_EQUALS;

	x11key->MISC_keymap[XK_Up&0xFF] = VKEY_UP;
	x11key->MISC_keymap[XK_Down&0xFF] = VKEY_DOWN;
	x11key->MISC_keymap[XK_Right&0xFF] = VKEY_RIGHT;
	x11key->MISC_keymap[XK_Left&0xFF] = VKEY_LEFT;
	x11key->MISC_keymap[XK_Insert&0xFF] = VKEY_INSERT;
	x11key->MISC_keymap[XK_Home&0xFF] = VKEY_HOME;
	x11key->MISC_keymap[XK_End&0xFF] = VKEY_END;
	x11key->MISC_keymap[XK_Page_Up&0xFF] = VKEY_PAGEUP;
	x11key->MISC_keymap[XK_Page_Down&0xFF] = VKEY_PAGEDOWN;

	x11key->MISC_keymap[XK_F1&0xFF] = VKEY_F1;
	x11key->MISC_keymap[XK_F2&0xFF] = VKEY_F2;
	x11key->MISC_keymap[XK_F3&0xFF] = VKEY_F3;
	x11key->MISC_keymap[XK_F4&0xFF] = VKEY_F4;
	x11key->MISC_keymap[XK_F5&0xFF] = VKEY_F5;
	x11key->MISC_keymap[XK_F6&0xFF] = VKEY_F6;
	x11key->MISC_keymap[XK_F7&0xFF] = VKEY_F7;
	x11key->MISC_keymap[XK_F8&0xFF] = VKEY_F8;
	x11key->MISC_keymap[XK_F9&0xFF] = VKEY_F9;
	x11key->MISC_keymap[XK_F10&0xFF] = VKEY_F10;
	x11key->MISC_keymap[XK_F11&0xFF] = VKEY_F11;
	x11key->MISC_keymap[XK_F12&0xFF] = VKEY_F12;
	x11key->MISC_keymap[XK_F13&0xFF] = VKEY_F13;
	x11key->MISC_keymap[XK_F14&0xFF] = VKEY_F14;
	x11key->MISC_keymap[XK_F15&0xFF] = VKEY_F15;

	x11key->MISC_keymap[XK_Num_Lock&0xFF] = VKEY_NUMLOCK;
	x11key->MISC_keymap[XK_Caps_Lock&0xFF] = VKEY_CAPSLOCK;
	x11key->MISC_keymap[XK_Scroll_Lock&0xFF] = VKEY_SCROLLOCK;
	x11key->MISC_keymap[XK_Shift_R&0xFF] = VKEY_RSHIFT;
	x11key->MISC_keymap[XK_Shift_L&0xFF] = VKEY_LSHIFT;
	x11key->MISC_keymap[XK_Control_R&0xFF] = VKEY_RCTRL;
	x11key->MISC_keymap[XK_Control_L&0xFF] = VKEY_LCTRL;
	x11key->MISC_keymap[XK_Alt_R&0xFF] = VKEY_RALT;
	x11key->MISC_keymap[XK_Alt_L&0xFF] = VKEY_LALT;
	x11key->MISC_keymap[XK_Meta_R&0xFF] = VKEY_RMETA;
	x11key->MISC_keymap[XK_Meta_L&0xFF] = VKEY_LMETA;
	x11key->MISC_keymap[XK_Super_L&0xFF] = VKEY_LSUPER; /* Left "Windows" */
	x11key->MISC_keymap[XK_Super_R&0xFF] = VKEY_RSUPER; /* Right "Windows */
	x11key->MISC_keymap[XK_Mode_switch&0xFF] = VKEY_MODE; /* "Alt Gr" key */
	x11key->MISC_keymap[XK_Multi_key&0xFF] = VKEY_COMPOSE; /* Multi-key compose */

	x11key->MISC_keymap[XK_Help&0xFF] = VKEY_HELP;
	x11key->MISC_keymap[XK_Print&0xFF] = VKEY_PRINT;
	x11key->MISC_keymap[XK_Sys_Req&0xFF] = VKEY_SYSREQ;
	x11key->MISC_keymap[XK_Break&0xFF] = VKEY_BREAK;
	x11key->MISC_keymap[XK_Menu&0xFF] = VKEY_MENU;
	x11key->MISC_keymap[XK_Hyper_R&0xFF] = VKEY_MENU;   /* Windows "Menu" key */

	return 0;
}

VisKeySym *lv_x11_key_lookup (LVX11Key *x11key, Display *display, XKeyEvent *xkey, KeyCode kc, VisKeySym *keysym,
		int pressed)
{
	KeySym xsym;

	/* Get the raw keyboard scancode */
//	keysym->scancode = kc;
	xsym = XKeycodeToKeysym(display, kc, 0);
#ifdef DEBUG_KEYS
	fprintf(stderr, "Translating key 0x%.4x (%d)\n", xsym, kc);
#endif
	/* Get the translated SDL virtual keysym */
	keysym->sym = VKEY_UNKNOWN;
	if ( xsym ) {
		switch (xsym>>8) {
			case 0x1005FF:
#ifdef SunXK_F36
				if ( xsym == SunXK_F36 )
					keysym->sym = VKEY_F11;
#endif
#ifdef SunXK_F37
				if ( xsym == SunXK_F37 )
					keysym->sym = VKEY_F12;
#endif
				break;
			case 0x00:	/* Latin 1 */
			case 0x01:	/* Latin 2 */
			case 0x02:	/* Latin 3 */
			case 0x03:	/* Latin 4 */
			case 0x04:	/* Katakana */
			case 0x05:	/* Arabic */
			case 0x06:	/* Cyrillic */
			case 0x07:	/* Greek */
			case 0x08:	/* Technical */
			case 0x0A:	/* Publishing */
			case 0x0C:	/* Hebrew */
			case 0x0D:	/* Thai */
				keysym->sym = (VisKey)(xsym&0xFF);
				/* Map capital letter syms to lowercase */
				if ((keysym->sym >= 'A')&&(keysym->sym <= 'Z'))
					keysym->sym += ('a'-'A');
				break;
			case 0xFE:
				keysym->sym = x11key->ODD_keymap[xsym&0xFF];
				break;
			case 0xFF:
				keysym->sym = x11key->MISC_keymap[xsym&0xFF];
				break;
			default:
				fprintf(stderr,
					"X11: Unknown xsym, sym = 0x%04x\n",
					(unsigned int)xsym);
				break;
		}
	} else {
		/* X11 doesn't know how to translate the key! */
		switch (kc) {
			/* Caution:
			   These keycodes are from the Microsoft Keyboard
			 */
			case 115:
				keysym->sym = VKEY_LSUPER;
				break;
			case 116:
				keysym->sym = VKEY_RSUPER;
				break;
			case 117:
				keysym->sym = VKEY_MENU;
				break;
			default:
				/*
				 * no point in an error message; happens for
				 * several keys when we get a keymap notify
				 */
				break;
		}
	}
	keysym->mod = VKMOD_NONE;

	if (keysym->sym == VKEY_LSHIFT)
		x11key->lshift = pressed;
	else if (keysym->sym == VKEY_RSHIFT)
		x11key->rshift = pressed;

	else if (keysym->sym == VKEY_LCTRL)
		x11key->lctrl = pressed;
	else if (keysym->sym == VKEY_RCTRL)
		x11key->rctrl = pressed;

	else if (keysym->sym == VKEY_LALT)
		x11key->lalt = pressed;
	else if (keysym->sym == VKEY_RALT)
		x11key->ralt = pressed;

	else if (keysym->sym == VKEY_NUMLOCK)
		x11key->num = pressed;
	else if (keysym->sym == VKEY_CAPSLOCK)
		x11key->caps = pressed;

	keysym->mod |= x11key->lshift ? VKMOD_LSHIFT : VKMOD_NONE;
	keysym->mod |= x11key->rshift ? VKMOD_RSHIFT : VKMOD_NONE;
	keysym->mod |= x11key->lctrl ? VKMOD_LCTRL : VKMOD_NONE;
	keysym->mod |= x11key->rctrl ? VKMOD_RCTRL : VKMOD_NONE;
	keysym->mod |= x11key->lalt ? VKMOD_LALT : VKMOD_NONE;
	keysym->mod |= x11key->ralt ? VKMOD_RALT : VKMOD_NONE;
	keysym->mod |= x11key->num ? VKMOD_NUM : VKMOD_NONE;
	keysym->mod |= x11key->caps ? VKMOD_CAPS : VKMOD_NONE;

	/* If UNICODE is on, get the UNICODE value for the key */
#if 0
	keysym->unicode = 0;
	if ( SDL_TranslateUNICODE && xkey ) {
		static XComposeStatus state;
		/* Until we handle the IM protocol, use XLookupString() */
		unsigned char keybuf[32];

#define BROKEN_XFREE86_INTERNATIONAL_KBD
/* This appears to be a magical flag that is used with AltGr on
   international keyboards to signal alternate key translations.
   The flag doesn't show up when in fullscreen mode (?)
   FIXME:  Check to see if this code is safe for other servers.
*/
#ifdef BROKEN_XFREE86_INTERNATIONAL_KBD
		/* Work around what appears to be a bug in XFree86 */
		if ( SDL_GetModState() & VKMOD_MODE ) {
			xkey->state |= (1<<13);
		}
#endif
		/* Look up the translated value for the key event */
		if ( XLookupString(xkey, (char *)keybuf, sizeof(keybuf),
							NULL, &state) ) {
			/*
			 * FIXME,: XLookupString() may yield more than one
			 * character, so we need a mechanism to allow for
			 * this (perhaps generate null keypress events with
			 * a unicode value)
			 */
			keysym->unicode = keybuf[0];
		}
	}
#endif
	return(keysym);
}
#if 0
static void get_modifier_masks(Display *display)
{
	static unsigned got_masks;
	int i, j;
	XModifierKeymap *xmods;
	unsigned n;

	if(got_masks)
		return;

	xmods = XGetModifierMapping(display);
	n = xmods->max_keypermod;
	for(i = 3; i < 8; i++) {
		for(j = 0; j < n; j++) {
			KeyCode kc = xmods->modifiermap[i * n + j];
			KeySym ks = XKeycodeToKeysym(display, kc, 0);
			unsigned mask = 1 << i;
			switch(ks) {
			case XK_Num_Lock:
				num_mask = mask; break;
			case XK_Alt_L:
				alt_l_mask = mask; break;
			case XK_Alt_R:
				alt_r_mask = mask; break;
			case XK_Meta_L:
				meta_l_mask = mask; break;
			case XK_Meta_R:
				meta_r_mask = mask; break;
			case XK_Mode_switch:
				mode_switch_mask = mask; break;
			}
		}
	}
	XFreeModifiermap(xmods);
	got_masks = 1;
}

#endif
