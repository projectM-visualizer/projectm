#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

//#include "lv_gl_sdl.h"
#include "x11driver.h"



#define X11_NATIVE(obj)					(VISUAL_CHECK_CAST ((obj), X11Native))

// http://xander.ncl.ac.uk/game/src6/

typedef struct _X11Native X11Native;

static int native_create (SADisplay *display, VisVideoDepth depth, VisVideoAttributeOptions *vidoptions,
		int width, int height, int resizable);
static int native_close (SADisplay *display);
static int native_lock (SADisplay *display);
static int native_unlock (SADisplay *display);
static int native_fullscreen (SADisplay *display, int fullscreen, int autoscale);
static int native_getvideo (SADisplay *display, VisVideo *screen);
static int native_updaterect (SADisplay *display, VisRectangle *rect);
static int native_drainevents (SADisplay *display, VisEventQueue *eventqueue);

static int get_nearest_resolution (SADisplay *display, int *width, int *height);

struct _X11Native {
	VisObject object;

	Display *dpy;
	Window win;
	int screen;

	VisVideoDepth requested_depth;

	int oldx;
	int oldy;

	int oldwidth;
	int oldheight;

	int resizable;

	int active;
};

SADisplayDriver *x11_driver_new ()
{
	SADisplayDriver *driver;

	driver = visual_mem_new0 (SADisplayDriver, 1);

	visual_object_initialize (VISUAL_OBJECT (driver), TRUE, NULL);

	driver->create = native_create;
	driver->close = native_close;
	driver->lock = native_lock;
	driver->unlock = native_unlock;
	driver->fullscreen = native_fullscreen;
	driver->getvideo = native_getvideo;
	driver->updaterect = native_updaterect;
	driver->drainevents = native_drainevents;

//	visual_gl_set_callback_attribute_set (native_gl_attribute_set);
//	visual_gl_set_callback_attribute_get (native_gl_attribute_get);

	return driver;
}

static int native_create (SADisplay *display, VisVideoDepth depth, VisVideoAttributeOptions *vidoptions,
		int width, int height, int resizable)
{
	X11Native *native = X11_NATIVE (display->native);
	Window rootwin;

	if (native != NULL)
		visual_object_unref (VISUAL_OBJECT (native));

	native = visual_mem_new0 (X11Native, 1);

	visual_object_initialize (VISUAL_OBJECT (native), TRUE, NULL);

	native->dpy = XOpenDisplay (NULL);

	if (native->dpy == NULL)
		visual_log (VISUAL_LOG_ERROR, "Can't open X Display");

	native->screen = DefaultScreen (display);
	rootwin = RootWindow (native->dpy, native->screen);
	native->win = XCreateSimpleWindow (native->dpy, rootwin, 10, 10, width, height, 0, 0, 0);
	XMapWindow (native->dpy, native->win);

	display->native = VISUAL_OBJECT (native);

	return 0;
}

static int native_close (SADisplay *display)
{
	return 0;
}

static int native_lock (SADisplay *display)
{
	return 0;
}

static int native_unlock (SADisplay *display)
{
	return 0;
}

static int native_fullscreen (SADisplay *display, int fullscreen, int autoscale)
{
	X11Native *native = X11_NATIVE (display->native);
//	SDL_Surface *screen = native->screen;
/*
	if (fullscreen == TRUE) {
		if (!(screen->flags & SDL_FULLSCREEN)) {
			if (autoscale == TRUE) {
				int width = display->screen->width;
				int height = display->screen->height;

				native->oldwidth = width;
				native->oldheight = height;

				get_nearest_resolution (display, &width, &height);

				native_create (display, native->requested_depth, NULL, width, height, native->resizable);
			}

			SDL_ShowCursor (SDL_FALSE);
			SDL_WM_ToggleFullScreen (screen);
		}
	} else {
		if ((screen->flags & SDL_FULLSCREEN)) {
			SDL_ShowCursor (SDL_TRUE);
			SDL_WM_ToggleFullScreen (screen);

			if (autoscale == TRUE)
				native_create (display, native->requested_depth, NULL, native->oldwidth,
						native->oldheight, native->resizable);
		}
	}
*/
	return 0;
}

static int native_getvideo (SADisplay *display, VisVideo *screen)
{
	/*
	X11Native *native = X11_NATIVE (display->native);
	SDL_Surface *sdlscreen = native->screen;

	if (native->requested_depth == VISUAL_VIDEO_DEPTH_GL)
		visual_video_set_depth (screen, VISUAL_VIDEO_DEPTH_GL);
	else
		visual_video_set_depth (screen, visual_video_depth_enum_from_value (sdlscreen->format->BitsPerPixel));

	visual_video_set_dimension (screen, sdlscreen->w, sdlscreen->h);
	visual_video_set_pitch (screen, sdlscreen->pitch);
	visual_video_set_buffer (screen, sdlscreen->pixels);
*/
	return 0;
}

static int native_updaterect (SADisplay *display, VisRectangle *rect)
{
	X11Native *native = X11_NATIVE (display->native);

	return 0;
}

static int native_drainevents (SADisplay *display, VisEventQueue *eventqueue)
{
	X11Native *native = X11_NATIVE (display->native);

	return 0;
}

static int get_nearest_resolution (SADisplay *display, int *width, int *height)
{

	return 0;
}

