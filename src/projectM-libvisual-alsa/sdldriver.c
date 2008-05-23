#include <SDL/SDL.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "sdldriver.h"

#define SDL_NATIVE(obj)					(VISUAL_CHECK_CAST ((obj), SDLNative))


typedef struct _SDLNative SDLNative;

static SDL_GLattr __lv_sdl_gl_attribute_map[] = {
	[VISUAL_GL_ATTRIBUTE_NONE]		= -1,
	[VISUAL_GL_ATTRIBUTE_BUFFER_SIZE]	= SDL_GL_BUFFER_SIZE,
	[VISUAL_GL_ATTRIBUTE_LEVEL]		= -1,
	[VISUAL_GL_ATTRIBUTE_RGBA]		= -1,
	[VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER]	= SDL_GL_DOUBLEBUFFER,
	[VISUAL_GL_ATTRIBUTE_STEREO]		= SDL_GL_STEREO,
	[VISUAL_GL_ATTRIBUTE_AUX_BUFFERS]	= -1,
	[VISUAL_GL_ATTRIBUTE_RED_SIZE]		= SDL_GL_RED_SIZE,
	[VISUAL_GL_ATTRIBUTE_GREEN_SIZE]	= SDL_GL_GREEN_SIZE,
	[VISUAL_GL_ATTRIBUTE_BLUE_SIZE]		= SDL_GL_BLUE_SIZE,
	[VISUAL_GL_ATTRIBUTE_ALPHA_SIZE]	= SDL_GL_ALPHA_SIZE,
	[VISUAL_GL_ATTRIBUTE_DEPTH_SIZE]	= SDL_GL_DEPTH_SIZE,
	[VISUAL_GL_ATTRIBUTE_STENCIL_SIZE]	= SDL_GL_STENCIL_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE]	= SDL_GL_ACCUM_RED_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_GREEN_SIZE]	= SDL_GL_ACCUM_GREEN_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_BLUE_SIZE]	= SDL_GL_ACCUM_BLUE_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_ALPHA_SIZE]	= SDL_GL_ACCUM_ALPHA_SIZE,
	[VISUAL_GL_ATTRIBUTE_LAST]		= -1
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


static int sdl_initialized;

struct _SDLNative {
	VisObject object;

	SDL_Surface *screen;

	VisVideoDepth requested_depth;

	int oldx;
	int oldy;

	int oldwidth;
	int oldheight;

	int resizable;

	int active;
};

SADisplayDriver *sdl_driver_new ()
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
	SDLNative *native = SDL_NATIVE (display->native);
        const SDL_VideoInfo *videoinfo;
	int videoflags = 0;
	int bpp;
	int i;

	if (native == NULL) {
		native = visual_mem_new0 (SDLNative, 1);

		visual_object_initialize (VISUAL_OBJECT (native), TRUE, NULL);
	}

	if (native->screen != NULL) {
		SDL_FreeSurface (native->screen);
	}

	videoflags |= resizable ? SDL_RESIZABLE : 0;

	if (sdl_initialized == FALSE) {
		if (SDL_Init (SDL_INIT_VIDEO) < 0) {
			fprintf (stderr, "Unable to init SDL VIDEO: %s\n", SDL_GetError ());

			exit (0);
		}

		sdl_initialized = TRUE;
	}

	native->resizable = resizable;
	native->requested_depth = depth;

	if (depth == VISUAL_VIDEO_DEPTH_GL) {
		videoinfo = SDL_GetVideoInfo ();

		if (!videoinfo) {
			return -1;
		}

		videoflags |= SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE;

		if (videoinfo->hw_available)
			videoflags |= SDL_HWSURFACE;
		else
			videoflags |= SDL_SWSURFACE;

		if (videoinfo->blit_hw)
			videoflags |= SDL_HWACCEL;

		if (vidoptions != NULL) {
			for (i = VISUAL_GL_ATTRIBUTE_NONE; i < VISUAL_GL_ATTRIBUTE_LAST; i++) {
				if (vidoptions->gl_attributes[i].mutated == TRUE) {
					SDL_GLattr sdl_attribute =
						__lv_sdl_gl_attribute_map[
						vidoptions->gl_attributes[i].attribute];

					if (sdl_attribute < 0)
						continue;

					SDL_GL_SetAttribute (sdl_attribute, vidoptions->gl_attributes[i].value);
				}
			}
		}

		bpp = videoinfo->vfmt->BitsPerPixel;
		native->screen = SDL_SetVideoMode (width, height, bpp, videoflags);

	} else {
		native->screen = SDL_SetVideoMode (width, height,
				visual_video_depth_value_from_enum (depth),
				videoflags);
	}

	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	display->native = VISUAL_OBJECT (native);

	return 0;
}

static int native_close (SADisplay *display)
{
	return 0;
}

static int native_lock (SADisplay *display)
{
	SDLNative *native = SDL_NATIVE (display->native);
	SDL_Surface *screen = native->screen;

	if (SDL_MUSTLOCK (screen) == SDL_TRUE)
		SDL_LockSurface (screen);

	return 0;
}

static int native_unlock (SADisplay *display)
{
	SDLNative *native = SDL_NATIVE (display->native);
	SDL_Surface *screen = native->screen;

	if (SDL_MUSTLOCK (screen) == SDL_TRUE)
		SDL_UnlockSurface (screen);

	return 0;
}

static int native_fullscreen (SADisplay *display, int fullscreen, int autoscale)
{
	SDLNative *native = SDL_NATIVE (display->native);
	SDL_Surface *screen = native->screen;

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

	return 0;
}

static int native_getvideo (SADisplay *display, VisVideo *screen)
{
	SDLNative *native = SDL_NATIVE (display->native);
	SDL_Surface *sdlscreen = native->screen;

	if (native->requested_depth == VISUAL_VIDEO_DEPTH_GL)
		visual_video_set_depth (screen, VISUAL_VIDEO_DEPTH_GL);
	else
		visual_video_set_depth (screen, visual_video_depth_enum_from_value (sdlscreen->format->BitsPerPixel));

	visual_video_set_dimension (screen, sdlscreen->w, sdlscreen->h);
	visual_video_set_pitch (screen, sdlscreen->pitch);
	visual_video_set_buffer (screen, sdlscreen->pixels);

	return 0;
}

static int native_updaterect (SADisplay *display, VisRectangle *rect)
{
	SDLNative *native = SDL_NATIVE (display->native);
	SDL_Surface *sdlscreen = native->screen;

	if (sdlscreen->format->BitsPerPixel == 8) {
		SDL_Color colors[256];
		VisPalette *pal = display->screen->pal;

		visual_mem_set (colors, 0, sizeof (colors));

		if (pal != NULL && pal->ncolors <= 256) {
			int i;

			for (i = 0; i < pal->ncolors; i++) {
				colors[i].r = pal->colors[i].r;
				colors[i].g = pal->colors[i].g;
				colors[i].b = pal->colors[i].b;
			}

			SDL_SetColors (sdlscreen, colors, 0, 256);
		}
	}

	if (native->requested_depth == VISUAL_VIDEO_DEPTH_GL)
		SDL_GL_SwapBuffers ();
	else
		SDL_UpdateRect (sdlscreen, rect->x, rect->y, rect->width, rect->height);

	return 0;
}

static int native_drainevents (SADisplay *display, VisEventQueue *eventqueue)
{
	SDLNative *native = SDL_NATIVE (display->native);
	SDL_Event event;

	/* Visible or not */
	if (((SDL_GetAppState () & SDL_APPACTIVE) == 0) && (native->active == TRUE)) {
		native->active = FALSE;
		visual_event_queue_add_visibility (eventqueue, FALSE);
	} else if (((SDL_GetAppState () & SDL_APPACTIVE) != 0) && (native->active == FALSE)) {
		native->active = TRUE;
		visual_event_queue_add_visibility (eventqueue, TRUE);
	}

	/* Events */
	while (SDL_PollEvent (&event)) {

		switch (event.type) {
			case SDL_KEYUP:
				visual_event_queue_add_keyboard (eventqueue, event.key.keysym.sym, event.key.keysym.mod,
						VISUAL_KEY_UP);
				break;

			case SDL_KEYDOWN:
				visual_event_queue_add_keyboard (eventqueue, event.key.keysym.sym, event.key.keysym.mod,
						VISUAL_KEY_DOWN);
				break;

			case SDL_VIDEORESIZE:
				visual_event_queue_add_resize (eventqueue, display->screen, event.resize.w, event.resize.h);

				native_create (display, display->screen->depth, NULL, event.resize.w, event.resize.h, native->resizable);
				break;

			case SDL_MOUSEMOTION:
				visual_event_queue_add_mousemotion (eventqueue, event.motion.x, event.motion.y);
				break;

			case SDL_MOUSEBUTTONDOWN:
				visual_event_queue_add_mousebutton (eventqueue, event.button.button, VISUAL_MOUSE_DOWN,
						event.button.x, event.button.y);
				break;

			case SDL_MOUSEBUTTONUP:
				visual_event_queue_add_mousebutton (eventqueue, event.button.button, VISUAL_MOUSE_UP,
						event.button.x, event.button.y);
				break;

			case SDL_QUIT:
				visual_event_queue_add_quit (eventqueue, FALSE);
				break;

			default:
				break;
		}
	}

	return 0;
}

static int get_nearest_resolution (SADisplay *display, int *width, int *height)
{
	SDL_Rect **modelist;
	int w, h;
	int i;

	modelist = SDL_ListModes (NULL, SDL_FULLSCREEN);

	if (modelist == NULL)
		return -1;

	w = *width;
	h = *height;

	/* Window is bigger than highest resolution */
	if (modelist[0]->w <= *width || modelist[0]->h <= *height) {
		*width = modelist[0]->w;
		*height = modelist[0]->h;

		return 0;
	}

	for (i = 0; modelist[i]; i++) {
		if (modelist[i]->w >= *width && modelist[i]->h >= *height) {
			w = modelist[i]->w;
			h = modelist[i]->h;
		}
	}

	*width = w;
	*height = h;

	return 0;
}

