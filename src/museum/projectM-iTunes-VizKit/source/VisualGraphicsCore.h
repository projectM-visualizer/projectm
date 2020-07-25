/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualGraphicsCore.h
 *
 */

/***************************************************************************

Copyright (c) 2004-2009 Heiko Wichmann (http://www.imagomat.de/vizkit)


This software is provided 'as-is', without any expressed or implied warranty. 
In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; 
   you must not claim that you wrote the original software. 
   If you use this software in a product, an acknowledgment 
   in the product documentation would be appreciated 
   but is not required.

2. Altered source versions must be plainly marked as such, 
   and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

 ***************************************************************************/

#ifndef VisualGraphicsCore_h
#define VisualGraphicsCore_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"

#if TARGET_OS_MAC
#include <AGL/agl.h>
#endif

#ifndef GRAPHICS_DEVICE
	#if TARGET_OS_WIN
		#define GRAPHICS_DEVICE	HWND
	#endif
	#if TARGET_OS_MAC
		#define GRAPHICS_DEVICE	CGrafPtr
	#endif
#endif


#if TARGET_OS_WIN
#include <windows.h> // HDC
#endif


namespace VizKit {

	/**
	 * The core operations of the graphics layer are bundled with this class. VisualGraphics is the only class that accesses VisualGraphicsCore. VisualGraphicsCore is a singleton class.
	 */
	class VisualGraphicsCore {

	public:

		/**
		 * Returns a pointer to the instance. 
		 * Initializes the Visual Graphics Core class if neccessary. 
		 * The visual graphics core class is of singleton type.
		 * @return A pointer to the initialized VisualGraphicsCore.
		 */
		static VisualGraphicsCore* getInstance(void);

		/**
		 * Cleans up the Visual Graphics Core singleton class.
		 */	
		static void dispose(void);

		/**
		 * Sets up an OpenGL context of the operating system.
		 * OS windowing system and OpenGL get connected.
		 * @return true on success, false on failure.
		 */
		bool setupContext();

		/**
		 * Sets the current context to the one that has been set up before with the setupContext() call.
		 * @return errNum: 0 if no error, > 0 error.
		 * @remarks The context must be set back to the one set up initially with each render action in case Cover Flow did change it in the meantime.
		 */
		uint16 setCurrentContext(void);
		
#if TARGET_OS_MAC
		/**
		 * Sets up the AGL canvas. Mac-only.
		 * @return ErrNum. 0 on success.
		 */
		uint8 setupAGL();

		/**
		 * Cleans up the AGL canvas. Mac-only.
		 */
		void cleanupAGL(void);
#endif

#if TARGET_OS_WIN
		/**
		 * Sets up the WGL canvas. Windows-only.
		 * @return Error number (0 = noErr).
		 */
		uint8 setupWGL();

		/**
		 * Cleans up wgl.
		 */
		void cleanUpWGL(void);

		/**
		 * Returns the device context. Windows-only.
		 * @return The device context. 
		 */
		HDC getWindowDC(void);
#endif

		/**
		 * Stores the port of the graphics device internally.
		 * @param thePort The drawing port of the graphics device.
		 * @remarks On Mac GRAPHICS_DEVICE is defined as CGrafPtr, on Windows it is defined as HWND.
		 */
		void setGraphicsDevicePort(const GRAPHICS_DEVICE thePort);

		/**
		 * Returns the port of the graphics device where the plugin performs its drawing.
		 * @return The port of the graphic device.
		 * @remarks On Mac GRAPHICS_DEVICE is defined as CGrafPtr, on Windows it is defined as HWND.
		 */
		GRAPHICS_DEVICE getGraphicsDevicePort(void);

		/**
		 * Returns the current dimensions of the screen.
		 * @return The current dimensions of the screen.
		 */
		PixelRect getScreenRect(void);

		/**
		 * Returns the current number of bits per pixel of the frame buffer.
		 * @return The current number of bits per pixel of the frame buffer.
		 */
		uint16 getBitsPerPixelOfScreen(void);

		/**
		 * Returns the current refresh rate of the screen.
		 * Can return 0 if refresh rate can't be determined.
		 * @return The current refresh rate of the screen.
		 */
		uint16 getRefreshRateOfScreen(void);

		/**
		 * Ends the GL drawing by flushing, finishing and swaping the buffer content.
		 * Every executed drawing command should show its result on screen.
		 * The function is called at the end of each RenderMessage.
		 */
		void finishGLDrawing();

		/**
		 * Sets the viewport of the drawing area.
		 * @param canvasRect The draw rect, i.e. the inner rect. These are the rect dimensions as passed to the visualizer by iTunes via messages (u.showWindowMessage.drawRect, u.setWindowMessage.drawRect).
		 * @param isFullscreen True if the context is set up on fullscreen, false if in windowed mode.
		 * @remarks The canvasRect might be smaller than the graphics device / graphics port that is bount to the OpenGL context. The drawing area is clipped by AGL_BUFFER_RECT (Mac) or GL_SCISSOR_TEST (Windows).
		 */
		void setViewport(const BottomLeftPositionedPixelRect& canvasRect, const bool isFullscreen);

		/**
		 * Returns the rect of the iTunes window relative to main screen rect.
		 * @return The dimensions of the window rectangle. 
		 */
		PixelRect getCanvasSurroundingRect(void);

	private:

		/**
		 * The constructor. VisualGraphicsCore is a singleton class. The constructor is private. New instance of class can only be created internally.
		 */
		VisualGraphicsCore();

		/**
		 * The destructor. VisualGraphicsCore is a singleton class. The destructor is private. Instance of class can only be deleted internally.
		 */
		~VisualGraphicsCore();

		/**
		 * Copy constructor.
		 * @param other Another VisualGraphicsCore.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualGraphicsCore(const VisualGraphicsCore& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualGraphicsCore& operator=(const VisualGraphicsCore& other);

		/** VisualGraphicsCore is a singleton class. Pointer to private instance is handled internally. */
		static VisualGraphicsCore* theVisualGraphicsCore;

		/** The graphics port.
		 * On Mac it is defined as CGrafPtr. On Windows it is defined as HWND. 
		 */
		GRAPHICS_DEVICE port;

#if TARGET_OS_MAC
		/** The current AGL context. Mac-only. */
		AGLContext ctx;
		
		/** The DisplayID of the CG display. Mac-only. */
		CGDirectDisplayID directDisplayId;
#endif
		
#if TARGET_OS_WIN
		/** The window RC. Windows-only. */
		HGLRC ctx;
		
		/** A handle to the current device. Windows-only. */
		HDC windowDC;
#endif

	};

}

#endif /* VisualGraphicsCore_h */
