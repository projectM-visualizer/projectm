#include <caca.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "cacadriver.h"

#define CACA_NATIVE(obj)					(VISUAL_CHECK_CAST ((obj), CacaNative))


typedef struct _CacaNative CacaNative;

static int native_create (SADisplay *display, VisVideoDepth depth, VisVideoAttributeOptions *vidoptions,
		int width, int height, int resizable);
static int native_close (SADisplay *display);
static int native_lock (SADisplay *display);
static int native_unlock (SADisplay *display);
static int native_fullscreen (SADisplay *display, int fullscreen, int autoscale);
static int native_getvideo (SADisplay *display, VisVideo *screen);
static int native_updaterect (SADisplay *display, VisRectangle *rect);
static int native_drainevents (SADisplay *display, VisEventQueue *eventqueue);


static int caca_initialized;

struct _CacaNative {
	VisObject object;

	struct caca_bitmap *bitmap;
	unsigned char *area;

	VisVideoDepth depth;
	int width;
	int height;

	unsigned int red[256];
	unsigned int green[256];
	unsigned int blue[256];
	unsigned int alpha[256];
};

SADisplayDriver *caca_driver_new ()
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
	CacaNative *native = CACA_NATIVE (display->native);
	unsigned int rmask;
	unsigned int gmask;
	unsigned int bmask;
	unsigned int amask;

	if (native == NULL) {
		native = visual_mem_new0 (CacaNative, 1);

		visual_object_initialize (VISUAL_OBJECT (native), TRUE, NULL);
	}

	if (caca_initialized == FALSE) {
		if (caca_init () < 0)
			return -1;

		caca_set_window_title ("Libcaca libvisual display");
	}

	if (native->bitmap != NULL)
		caca_free_bitmap (native->bitmap);


	if (native->area != NULL)
		visual_mem_free (native->area);

	switch (depth) {
		case VISUAL_VIDEO_DEPTH_32BIT:
			rmask = 0xff000000;
			gmask = 0x00ff0000;
			bmask = 0x0000ff00;
			amask = 0x000000ff;

			break;

		case VISUAL_VIDEO_DEPTH_24BIT:
			rmask = 0xff0000;
			gmask = 0x00ff00;
			bmask = 0x0000ff;
			amask = 0x000000;

			break;

		case VISUAL_VIDEO_DEPTH_16BIT:
			rmask = 0x7c00;
			gmask = 0x03e0;
			bmask = 0x001f;
			amask = 0x0000;

			break;

		case VISUAL_VIDEO_DEPTH_8BIT:
			rmask = gmask = bmask = amask = 0;

			break;

		default:
			rmask = gmask = bmask = amask = 0;

			break;
	}

	native->bitmap = caca_create_bitmap (visual_video_depth_value_from_enum (depth),
			width,
			height,
			width * (visual_video_depth_value_from_enum (depth) / 8),
			rmask, gmask, bmask, amask);

	native->area = visual_mem_malloc0 (width * height * (visual_video_depth_value_from_enum (depth) / 8));

	native->width = width;
	native->height = height;
	native->depth = depth;

	display->native = VISUAL_OBJECT (native);

	caca_initialized = TRUE;

	return 0;
}

static int native_close (SADisplay *display)
{
	caca_end ();

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
	return 0;
}

static int native_getvideo (SADisplay *display, VisVideo *screen)
{
	CacaNative *native = CACA_NATIVE (display->native);

	visual_video_set_depth (screen, native->depth);

	visual_video_set_dimension (screen, native->width, native->height);
	visual_video_set_buffer (screen, native->area);

	return 0;
}

static int native_updaterect (SADisplay *display, VisRectangle *rect)
{
	CacaNative *native = CACA_NATIVE (display->native);
	VisPalette *pal;
	int i;

	pal = display->screen->pal;

	if (display->screen->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		for (i = 0; i < 256; i++) {
			native->red[i] = pal->colors[i].r * 16;
			native->green[i] = pal->colors[i].g * 16;
			native->blue[i] = pal->colors[i].b * 16;
		}

		caca_set_bitmap_palette (native->bitmap,
				native->red,
				native->green,
				native->blue,
				native->alpha);
	}

	caca_draw_bitmap (0, 0, caca_get_width() - 1, caca_get_height() - 1,
			native->bitmap, native->area);

	caca_refresh ();

	return 0;
}

static int native_drainevents (SADisplay *display, VisEventQueue *eventqueue)
{
	CacaNative *native = CACA_NATIVE (display->native);
	unsigned int event;

	while ((event = caca_get_event (CACA_EVENT_ANY)) > 0) {

		if (event & CACA_EVENT_KEY_PRESS) {

			visual_event_queue_add_keyboard (eventqueue, event & 0xff, 0,
					VISUAL_KEY_DOWN);

		} else if (event & CACA_EVENT_KEY_RELEASE) {

			visual_event_queue_add_keyboard (eventqueue, event & 0xff, 0,
					VISUAL_KEY_UP);

		} else if (event & CACA_EVENT_MOUSE_MOTION) {

			visual_event_queue_add_mousemotion (eventqueue, caca_get_mouse_x (), caca_get_mouse_y ());

		} else if (event & CACA_EVENT_MOUSE_PRESS) {

			visual_event_queue_add_mousebutton (eventqueue, event & 0xff, VISUAL_MOUSE_DOWN,
					caca_get_mouse_x (), caca_get_mouse_y ());

		} else if (event & CACA_EVENT_MOUSE_RELEASE) {

			visual_event_queue_add_mousebutton (eventqueue, event & 0xff, VISUAL_MOUSE_UP,
					caca_get_mouse_x (), caca_get_mouse_y ());
		} else if (event & CACA_EVENT_RESIZE) {


			native_create (display, display->screen->depth,
					NULL,
					caca_get_window_width (),
					caca_get_window_height (),
					TRUE);

			visual_event_queue_add_resize (eventqueue, display->screen,
					caca_get_window_width (),
					caca_get_window_height ());

		}
	}

	return 0;
}

