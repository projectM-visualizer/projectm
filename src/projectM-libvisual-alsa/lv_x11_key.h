/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_x11_key.h,v 1.2 2006/03/22 18:24:09 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _LV_X11_KEY_H
#define _LV_X11_KEY_H

#include <libvisual/libvisual.h>

VISUAL_BEGIN_DECLS

typedef struct _LVX11Key LVX11Key;

struct _LVX11Key {
	VisKey ODD_keymap[256];
	VisKey MISC_keymap[256];

	XComposeStatus compose_state;

	int lshift;
	int rshift;
	int lctrl;
	int rctrl;
	int lalt;
	int ralt;
	int num;
	int caps;
};

int lv_x11_key_init (LVX11Key *x11key);

VisKeySym *lv_x11_key_lookup (LVX11Key *x11key, Display *display, XKeyEvent *xkey, KeyCode kc, VisKeySym *keysym,
		int pressed);

VISUAL_END_DECLS

#endif /* _LV_X11_KEY_H */
