#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>

//#include "lv_gl_sdl.h"
#include "glxdriver.h"

#include "lv_x11_key.h"


#define GLX_NATIVE(obj)					(VISUAL_CHECK_CAST ((obj), GLXNative))


typedef struct _GLXNative GLXNative;

static int __lv_glx_gl_attribute_map[] = {
	[VISUAL_GL_ATTRIBUTE_NONE]              = -1,
	[VISUAL_GL_ATTRIBUTE_BUFFER_SIZE]       = GLX_BUFFER_SIZE,
	[VISUAL_GL_ATTRIBUTE_LEVEL]             = GLX_LEVEL,
	[VISUAL_GL_ATTRIBUTE_RGBA]              = GLX_RGBA,
	[VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER]      = GLX_DOUBLEBUFFER,
	[VISUAL_GL_ATTRIBUTE_STEREO]            = GLX_STEREO,
	[VISUAL_GL_ATTRIBUTE_AUX_BUFFERS]       = GLX_AUX_BUFFERS,
	[VISUAL_GL_ATTRIBUTE_RED_SIZE]          = GLX_RED_SIZE,
	[VISUAL_GL_ATTRIBUTE_GREEN_SIZE]        = GLX_GREEN_SIZE,
	[VISUAL_GL_ATTRIBUTE_BLUE_SIZE]         = GLX_BLUE_SIZE,
	[VISUAL_GL_ATTRIBUTE_ALPHA_SIZE]        = GLX_ALPHA_SIZE,
	[VISUAL_GL_ATTRIBUTE_DEPTH_SIZE]        = GLX_DEPTH_SIZE,
	[VISUAL_GL_ATTRIBUTE_STENCIL_SIZE]      = GLX_STENCIL_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE]    = GLX_ACCUM_RED_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_GREEN_SIZE]  = GLX_ACCUM_GREEN_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_BLUE_SIZE]   = GLX_ACCUM_BLUE_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_ALPHA_SIZE]  = GLX_ACCUM_ALPHA_SIZE,
	[VISUAL_GL_ATTRIBUTE_LAST]              = -1
};

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

static int X11_Pending(Display *display);

static XVisualInfo *get_xvisualinfo_filter_capabilities (Display *dpy, int screen, VisVideoAttributeOptions *vidoptions);


struct _GLXNative {
	VisObject object;

	Display *dpy;
	Window win;
	int screen;
	GLXContext ctx;
	XSetWindowAttributes attr;
	Bool fs;
	Bool doubleBuffered;
	XF86VidModeModeInfo deskMode;

	VisVideoDepth requested_depth;

	LVX11Key key;

	unsigned int lastwidth;
	unsigned int lastheight;

	unsigned int width;
	unsigned int height;
	int x;
	int y;

	unsigned int depth;

	int oldx;
	int oldy;

	int oldwidth;
	int oldheight;

	int resizable;

	int active;

	VisVideo *video;

	/* Atoms */
	Atom WM_DELETE_WINDOW;
};

SADisplayDriver *glx_driver_new ()
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

	return driver;
}

static int native_create (SADisplay *display, VisVideoDepth depth, VisVideoAttributeOptions *vidoptions,
		int width, int height, int resizable)
{
	GLXNative *native = GLX_NATIVE (display->native);
	XVisualInfo *vi;
	Colormap cmap;
	int dpyWidth, dpyHeight;
	int i;
	int glxMajorVersion, glxMinorVersion;
	int vidModeMajorVersion, vidModeMinorVersion;
	XF86VidModeModeInfo **modes;
	int modeNum;
	int bestMode;
	Atom wmDelete;
	Window winDummy;
	unsigned int borderDummy;

	if (native != NULL)
		visual_object_unref (VISUAL_OBJECT (native));

	native = visual_mem_new0 (GLXNative, 1);

	visual_object_initialize (VISUAL_OBJECT (native), TRUE, NULL);

	lv_x11_key_init (&native->key);

	/* set best mode to current */
	bestMode = 0;

	/* get a connection */
	native->dpy = XOpenDisplay(0);
	native->screen = DefaultScreen(native->dpy);
	XF86VidModeQueryVersion(native->dpy, &vidModeMajorVersion,
			&vidModeMinorVersion);
	printf("XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion,
			vidModeMinorVersion);
	XF86VidModeGetAllModeLines(native->dpy, native->screen, &modeNum, &modes);
	/* save desktop-resolution before switching modes */
	native->deskMode = *modes[0];
	/* look for mode with requested resolution */
	for (i = 0; i < modeNum; i++)
	{
		if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
		{
			bestMode = i;
		}
	}

	/* get an appropriate visual */
	vi = get_xvisualinfo_filter_capabilities (native->dpy, native->screen, vidoptions);
	if (vi == NULL)	{
		printf ("No visual found.\n");
		visual_error_raise ();
	}

	glXQueryVersion(native->dpy, &glxMajorVersion, &glxMinorVersion);
	printf("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);

	/* create a GLX context */
	native->ctx = glXCreateContext(native->dpy, vi, 0, GL_TRUE);

	/* create a color map */
	cmap = XCreateColormap(native->dpy, RootWindow(native->dpy, vi->screen),
			vi->visual, AllocNone);
	native->attr.colormap = cmap;
	native->attr.border_pixel = 0;

	/* create a window in window mode*/
	native->attr.event_mask = KeyPressMask | ButtonPressMask |
		StructureNotifyMask | VisibilityChangeMask | KeyReleaseMask | ButtonReleaseMask;

	native->win = XCreateWindow(native->dpy, RootWindow(native->dpy, vi->screen),
			0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
			CWBorderPixel | CWColormap | CWEventMask, &native->attr);


	XFree (vi);
	/* only set window title and handle wm_delete_events if in windowed mode */
	wmDelete = XInternAtom(native->dpy, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(native->dpy, native->win, &wmDelete, 1);
	XSetStandardProperties(native->dpy, native->win, "jahoor",
			"jahoor", None, NULL, 0, NULL);
	XMapRaised(native->dpy, native->win);

	/* connect the glx-context to the window */
	glXMakeCurrent(native->dpy, native->win, native->ctx);
	XGetGeometry(native->dpy, native->win, &winDummy, &native->x, &native->y,
			&native->width, &native->height, &borderDummy, &native->depth);

	printf("Depth %d\n", native->depth);

	if (glXIsDirect(native->dpy, native->ctx))
		printf("Congrats, you have Direct Rendering!\n");
	else
		printf("Sorry, no Direct Rendering possible!\n");

	native->WM_DELETE_WINDOW = XInternAtom(native->dpy, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(native->dpy, native->win, &native->WM_DELETE_WINDOW, 1);

	native->lastwidth = width;
	native->lastheight = height;

	display->native = VISUAL_OBJECT (native);

	return 0;
}

static int native_close (SADisplay *display)
{
	GLXNative *native = GLX_NATIVE (display->native);

	if (native->ctx) {
		if (!glXMakeCurrent(native->dpy, None, NULL)) {
			printf("Could not release drawing context.\n");
		}

		glXDestroyContext(native->dpy, native->ctx);
		native->ctx = NULL;
	}
	/* switch back to original desktop resolution if we were in fs */
	if (native->fs)
	{
		XF86VidModeSwitchToMode(native->dpy, native->screen, &native->deskMode);
		XF86VidModeSetViewPort(native->dpy, native->screen, 0, 0);
	}
	XCloseDisplay(native->dpy);

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
	GLXNative *native = GLX_NATIVE (display->native);
//	Surface *screen = native->screen;
/*
	if (fullscreen == TRUE) {
		if (!(screen->flags & FULLSCREEN)) {
			if (autoscale == TRUE) {
				int width = display->screen->width;
				int height = display->screen->height;

				native->oldwidth = width;
				native->oldheight = height;

				get_nearest_resolution (display, &width, &height);

				native_create (display, native->requested_depth, NULL, width, height, native->resizable);
			}

			ShowCursor (SDL_FALSE);
			WM_ToggleFullScreen (screen);
		}
	} else {
		if ((screen->flags & FULLSCREEN)) {
			ShowCursor (SDL_TRUE);
			WM_ToggleFullScreen (screen);

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
	GLXNative *native = GLX_NATIVE (display->native);

	visual_video_set_depth (screen, VISUAL_VIDEO_DEPTH_GL);

	visual_video_set_dimension (screen, native->width, native->height);

	native->video = screen;

	return 0;
}

static int native_updaterect (SADisplay *display, VisRectangle *rect)
{
	GLXNative *native = GLX_NATIVE (display->native);

	glXSwapBuffers (native->dpy, native->win);

	return 0;
}

static int native_drainevents (SADisplay *display, VisEventQueue *eventqueue)
{
	GLXNative *native = GLX_NATIVE (display->native);
	XEvent xevent;
	int sym;
	int mod;

	while (X11_Pending (native->dpy) > 0) {
		VisKeySym keysym;

		XNextEvent (native->dpy, &xevent);

		switch (xevent.type) {
			case ConfigureNotify:
				if ((xevent.xconfigure.width != native->lastwidth) ||
						(xevent.xconfigure.height != native->lastheight)) {

					native->width = xevent.xconfigure.width;
					native->height = xevent.xconfigure.height;

					visual_event_queue_add_resize (eventqueue, native->video,
							xevent.xconfigure.width, xevent.xconfigure.height);
				}

				break;

			case ButtonPress:
				visual_event_queue_add_mousebutton (eventqueue, xevent.xbutton.button, VISUAL_MOUSE_DOWN,
						xevent.xbutton.x, xevent.xbutton.y);

				break;

			case ButtonRelease:
				visual_event_queue_add_mousebutton (eventqueue, xevent.xbutton.button, VISUAL_MOUSE_UP,
						xevent.xbutton.x, xevent.xbutton.y);

				break;

			case KeyPress:
				lv_x11_key_lookup (&native->key, native->dpy, &xevent.xkey, xevent.xkey.keycode, &keysym,
						TRUE);

				visual_event_queue_add_keyboard (eventqueue, keysym.sym, keysym.mod,
						VISUAL_KEY_DOWN);

				break;

			case KeyRelease:
				lv_x11_key_lookup (&native->key, native->dpy, &xevent.xkey, xevent.xkey.keycode, &keysym,
						FALSE);

				visual_event_queue_add_keyboard (eventqueue, keysym.sym, keysym.mod,
						VISUAL_KEY_UP);

				break;

			case ClientMessage:
				if ((xevent.xclient.format == 32) &&
						(xevent.xclient.data.l[0] == native->WM_DELETE_WINDOW)) {

					visual_event_queue_add_quit (eventqueue, FALSE);
				}

				break;

			case MotionNotify:
                                visual_event_queue_add_mousemotion (eventqueue, xevent.xmotion.x, xevent.xmotion.y);

				break;

			case VisibilityNotify:
				if (xevent.xvisibility.state == VisibilityUnobscured ||
						xevent.xvisibility.state == VisibilityPartiallyObscured) {
					visual_event_queue_add_visibility (eventqueue, TRUE);
				} else if (xevent.xvisibility.state == VisibilityFullyObscured) {
					visual_event_queue_add_visibility (eventqueue, FALSE);
				}

				break;
		}
	}

	return 0;
}

static int get_nearest_resolution (SADisplay *display, int *width, int *height)
{

	return 0;
}

/* Ack!  XPending() actually performs a blocking read if no events available */
/* Taken from SDL */
static int X11_Pending(Display *display)
{
	/* Flush the display connection and look to see if events are queued */
	XFlush(display);
	if ( XEventsQueued(display, QueuedAlready) ) {
		return(1);
	}

	/* More drastic measures are required -- see if X is ready to talk */
	{
		static struct timeval zero_time;        /* static == 0 */
		int x11_fd;
		fd_set fdset;

		x11_fd = ConnectionNumber(display);
		FD_ZERO(&fdset);
		FD_SET(x11_fd, &fdset);
		if ( select(x11_fd+1, &fdset, NULL, NULL, &zero_time) == 1 ) {
			return(XPending(display));
		}
	}

	/* Oh well, nothing is ready .. */
	return(0);
}

static XVisualInfo *get_xvisualinfo_filter_capabilities (Display *dpy, int screen, VisVideoAttributeOptions *vidoptions)
{
	int attrList[64];
	int attrc = 0;
	int i;

	if (vidoptions == NULL)
		return NULL;

	/* FIXME filter for capabilities, like doublebuffer */
	for (i = VISUAL_GL_ATTRIBUTE_NONE; i < VISUAL_GL_ATTRIBUTE_LAST; i++) {
		if (vidoptions->gl_attributes[i].mutated == TRUE) {
			int glx_attribute =
				__lv_glx_gl_attribute_map[
				vidoptions->gl_attributes[i].attribute];

			if (glx_attribute < 0)
				continue;

			attrList[attrc++] = glx_attribute;

			/* Check if it's a non boolean attribute */
			if (glx_attribute != GLX_RGBA && glx_attribute != GLX_DOUBLEBUFFER && glx_attribute != GLX_STEREO) {
				attrList[attrc++] = vidoptions->gl_attributes[i].value;
			}
		}
	}

	attrList[attrc++] = None;

	return glXChooseVisual (dpy, screen, attrList);
}

