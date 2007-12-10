//video_init.c - SDL/Opengl Windowing Creation/Resizing Functions
//
//by Peter Sperl & Ray Telford
//
//Opens an SDL Window and creates an OpenGL session
//also able to handle resizing and fullscreening of windows
//just call init_display again with different variables
#include <SDL/SDL.h>

void resize_display(int w, int h, bool f)
{
	int flags = SDL_OPENGL|SDL_HWSURFACE|(f ? SDL_FULLSCREEN : SDL_RESIZABLE);
	// 0 : use current bits per pixel
	if(!SDL_SetVideoMode(w, h, 0, flags))
	{
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		return;
	}
	SDL_ShowCursor(f ? SDL_DISABLE : SDL_ENABLE);
}

//init_display
//
//Sets screen to new width and height (w,h)
//Also switches between fullscreen and windowed
//with the boolean f (fullscreen)
void init_display(int w, int h, int *fvw, int *fvh, bool f)
{
	/* First, initialize SDL's video subsystem. */
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		fprintf(stderr, "Video initialization failed: %s\n",
				SDL_GetError());
		return;
	}
	
	/* Let's get some video information. */
	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	if(!info)
	{
		/* This should probably never happen. */
		fprintf(stderr, "Video query failed: %s\n",
				SDL_GetError());
		
		// todo:what is this?
		//    projectM_vtable.disable_plugin (&projectM_vtable);
		return;
	}
	
	printf("Screen Resolution: %d x %d\n", info->current_w, info->current_h);
	*fvw = info->current_w;
	*fvh = info->current_h;
	
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	if (f)
		resize_display(*fvw, *fvh, f);
	else
		resize_display(w, h, f);
}
