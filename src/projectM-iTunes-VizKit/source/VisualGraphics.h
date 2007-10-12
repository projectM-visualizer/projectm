/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualGraphics.h
 *
 */

/***************************************************************************

Copyright (c) 2004-2007 Heiko Wichmann (http://www.imagomat.de/vizkit)


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

#ifndef VisualGraphics_h
#define VisualGraphics_h


#include "VisualGraphicTypes.h"

#include <map>
#include <vector>

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#include <ApplicationServices/../Frameworks/QD.framework/Headers/Quickdraw.h> // CGrafPtr
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#include <windows.h>
#endif

#ifndef GRAPHICS_DEVICE
	#if TARGET_OS_WIN
		#define GRAPHICS_DEVICE	HWND
	#endif
	#if TARGET_OS_MAC
		#define GRAPHICS_DEVICE	CGrafPtr
	#endif
#endif


namespace VizKit {

	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).
	class VisualNurbs; // Forward declaration (to avoid include of header file).
	class VisualConvolutionFilter; // Forward declaration (to avoid include of header file).

	/**
	 * Stores the attributes of a display resolution.
	 */
	class DisplayResolution {
	public:
		UInt16 horizontalPixels; /**< The horizontal dimension of the resolution measured in pixels. */
		UInt16 verticalPixels; /**< The vertical dimension of the resolution measured in pixels. */
		UInt16 bitsPerPixel; /**< The number of bits per pixel of the resolution. */
		UInt16 refreshRate; /**< The refresh rate of the resolution. */
		char showStr[32]; /**< The string that is presented to the user. */
	};

	/**
	 * Communicates with the graphics device. Language of choice is OpenGL.
	 * Any function needing drawing can use a method of Visual Graphics class.
	 * That way only the header file of Visual Graphics needs to be included 
	 * and no strong binding to a specific graphics language is needed inside other classes than this one.
	 */
	class VisualGraphics {

	public:

		/**
		 * Returns a pointer to the instance. 
		 * Initializes the Visual Graphics class if neccessary. 
		 * The visual graphics class is of singleton type.
		 * @return A pointer to the initialized VisualGraphics.
		 */
		static VisualGraphics* getInstance(void);

		/**
		 * Cleans up the Visual Graphics singleton class.
		 */	
		static void dispose(void);

		/**
		 * Answers the question whether the Visual Graphics singleton class is initialized or not.
		 * @return True if VisualGraphics singleton class is initialized.\ False if not.
		 */	
		static bool isInitialized(void);
		
		/**
		 * Stores the port of the graphics device internally.
		 * @param thePort The drawing port of the graphics device.
		 */
		void setGraphicsDevicePort(const GRAPHICS_DEVICE thePort);

		/**
		 * Stores the canvas rect.
		 * The canvas rect defines the rectangle where the drawing takes place.
		 * The rectangle is passed in by iTunes and can be scaled in fullscreen mode.
		 * @param aCanvasRect Pointer to rectangle definition.
		 */
		void setCanvasRect(const ::Rect* const aCanvasRect);
		
		/**
		 * Stores whether the visualizer is expected to show in fullscreen mode or in windowed mode.
		 * @param isFullscreen True if fullscreen mode is expected. False if windowed mode is expected.
		 */
		void isSetupForFullScreenMode(const bool isFullscreen);

		/**
		 * Returns the draw rect relative to window rect.
		 * @param[out] aRect The draw rect. Coordinates are relative to window rect.
		 */
		void getCanvasRect(::Rect* aRect);

		/**
		 * Returns the current width of the screen in pixels.
		 * @return The current width of the screen in pixels.
		 */
		UInt16 getScreenWidth(void);

		/**
		 * Returns the current height of the screen in pixels.
		 * @return The current height of the screen in pixels.
		 */
		UInt16 getScreenHeight(void);

		/**
		 * Returns the current number of bits per pixel of the frame buffer.
		 * @return The current number of bits per pixel of the frame buffer.
		 */
		UInt16 getBitsPerPixelOfScreen(void);

		/**
		 * Returns the current refresh rate of the screen.
		 * Can return 0 if refresh rate can't be determined.
		 * @return The current refresh rate of the screen.
		 */
		UInt16 getRefreshRateOfScreen(void);

		/**
		 * Returns the width of the canvas in pixels.
		 * @return The width of the canvas in pixels.
		 */
		UInt16 getCanvasPixelWidth(void);

		/**
		 * Returns the height of the canvas in pixels.
		 * @return The height of the canvas in pixels.
		 */
		UInt16 getCanvasPixelHeight(void);
		
		/**
		 * Returns the width of the canvas in coords.
		 * @return The width of the canvas in coords.
		 */	
		double getCanvasCoordWidth(void);

		/**
		 * Returns the height of the canvas in coords.
		 * @return The height of the canvas in coords.
		 */	
		double getCanvasCoordHeight(void);

		/**
		 * Returns the bounding rectangle of the canvas rectangle.
		 * If in fullscreen mode, the screen rect is returned.
		 * If in windowed mode, the rect of the window is returned 
		 * (the position values are relative to the screen rectangle).
		 * @param[out] aRect The dimensions of the bounding rect.
		 */
		void getCanvasSurroundingRect(::Rect* aRect);

		/**
		 * Returns the OpenGL viewport.
		 * @param[out] aViewport The dimensions of the OpenGL viewport.
		 */
		void getViewportRect(::Rect* aViewport);

		/**
		 * Sets up an OpenGL context of the operating system.
		 * OS windowing system and OpenGL get connected.
		 * @return true on success, false on failure.
		 */
		bool setupContext();

		/**
		 * Tears down the OpenGL allocations.
		 */
		void disposeContext();

		/**
		 * Sets up an orthographic projection matrix of GL.
		 */
		void setOrthographicProjection();

		/**
		 * Sets up the perspective projection matrix of GL.
		 * @param maxNearCoord The maximum near position.\ With the maxNearCoord the position of the near clipping plane of the viewing volume is set.\ By setting maxNearCoord the perspective projection (the shape of the viewing volume) can be adjusted.\ The value of maxNearCoord must be positive.
		 * @remarks Default value for maxNearCoord is 1.0.
		 */
		void setPerspectiveProjection(double maxNearCoord = 1.0);

		/**
		 * Sets the value of the background color.
		 * @param aColorVal The RGBA values.
		 */	
		void setBackgroundColor(const RGBAColor& aColorVal);
		
		/**
		 * Returns the value of the background color.
		 * @return The RGBA values of the background color.
		 */	
		RGBAColor getBackgroundColor(void);

		/**
		 * Clears the screen with the background color.
		 */
		void clearBackground(void);
		
		/**
		 * Specifies the storage parameters for pixel packing and unpacking (copying from GPU to CPU and vice versa).
		 */
		void setPixelStorageParams(void);

		/**
		 * Resets the model view matrix.
		 */
		void resetModelViewMatrix(void);
		
		/**
		 * Call of glTranslatef().
		 * @param xNum Horizontal coord value.
		 * @param yNum Vertical coord value.
		 * @param zNum Z-coord value.
		 */	
		void translateMatrix(double xNum, double yNum, double zNum);

		/**
		 * Call of glRotatef().
		 * @param angle The angle of the rotation.
		 * @param xAmount The amount of the rotation around the horizontal axis.
		 * @param yAmount The amount of the rotation around the vertical axis.
		 * @param zAmount The amount of the rotation around the z-axis.
		 */	
		void rotateMatrix(float angle, float xAmount, float yAmount, float zAmount);

		/**
		 * Call of glScalef().
		 * @param xFactor The scale factor in the horizontal dimension.
		 * @param yFactor The scale factor in the vertical dimension.
		 * @param zFactor The scale factor in the z-dimension.
		 */	
		void scaleMatrix(float xFactor, float yFactor, float zFactor);

		/**
		 * Enables texturing by calling glEnable(GL_TEXTURE_2D) or glEnable(GL_TEXTURE_RECTANGLE_EXT).
		 * @param useRectExtension If true texturing is enabled with GL_TEXTURE_RECTANGLE_EXT, if false texturing is enabled with GL_TEXTURE_2D.
		 */
		void enableTexturing(bool useRectExtension);

		/**
		 * Disables texturing by calling glDisable(GL_TEXTURE_2D) or glDisable(GL_TEXTURE_RECTANGLE_EXT).
		 * @param useRectExtension If true texturing is disabled with GL_TEXTURE_RECTANGLE_EXT, if false texturing is disabled with GL_TEXTURE_2D.
		 */
		void disableTexturing(bool useRectExtension);

		/**
		 * Binds a texture (sets it current) by calling glBindTexture().
		 * @param textureName The name/number/id of the texture.
		 * @param useRectExtension If true texture is bound with GL_TEXTURE_RECTANGLE_EXT, if false texture is bound with GL_TEXTURE_2D.
		 */
		void bindTexture(UInt32 textureName, bool useRectExtension);

		/**
		 * Writes the pixels of the current texture (last one binded) into the provided buffer.
		 * @param useRectExtension If true texture has rectangular dimensions.
		 * @param format The format of the pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param type The data type of the pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 * @param[out] buffer Pointer to pointer to allocated buffer of 32bit chunks (one chunk is one pixel with four 8bit channels).
		 */
		void get32BitPixelsOfCurrentTexture(bool useRectExtension, UInt16 format, UInt16 type, UInt32** buffer);

		/**
		 * Writes the pixels of the current texture (last one binded) into the provided buffer.
		 * @param useRectExtension If true texture has rectangular dimensions.
		 * @param format The format of the pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param type The data type of the pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 * @param[out] buffer Pointer to pointer to allocated buffer of 8bit chunks (four chunks are one pixel with four channels).
		 */
		void get8BitPixelsOfCurrentTexture(bool useRectExtension, UInt16 format, UInt16 type, UInt8** buffer);

		/**
		 * Enables blending by calling glEnable(GL_BLEND) followed by function glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA).
		 * The alpha value of the source is taken and blended with the destination color values.
		 */
		void enableAlphaBlending(void);

		/**
		 * Disables blending by calling glDisable(GL_BLEND).
		 */
		void disableBlending(void);

		/**
		 * Sets a color value by calling glColor4f().
		 * @param theColor The color values to set.
		 */
		void setColor(const RGBAColor& theColor);
		
		/**
		 * Enables GL_DEPTH_TEST.
		 */
		void enableDepthTest(void);

		/**
		 * Disables GL_DEPTH_TEST.
		 */
		void disableDepthTest(void);

		/**
		 * Enables GL_CULL_FACE.
		 */
		void enableCullFace(void);

		/**
		 * Disables GL_CULL_FACE.
		 */
		void disableCullFace(void);
		
		/**
		 * Specifies that front-facing facets can be culled.
		 */
		void cullFaceFront(void);

		/**
		 * Specifies that back-facing facets can be culled.
		 */
		void cullFaceBack(void);

#if TARGET_OS_MAC
		/**
		 * Creates a bitmap context.
		 * @param pixelWidth The width of the bitmap in pixels.
		 * @param pixelHeight The height of the bitmap in pixels.
		 * @return A bitmap context.\ The caller has to issue a CGContextRelease() call on the context to release it.
		 */
		CGContextRef createBitmapContext(size_t pixelWidth, size_t pixelHeight);
#endif

		/**
		 * Returns an error string (if there was an error).
		 * @param[out] outErrorString Pointer to character buffer to receive the error string.
		 * @return Error number. 
		 */
		UInt8 getOpenGLError(char* outErrorString);

		/**
		 * Iterator function.
		 * @param[out] showStr The string that can be presented to the user.
		 * @param[out] isSelected 1 if current resolution is preferred by user (preference value), 0 otherwise.
		 * @return true if there are more DisplayResolutions; false if the last DisplayResolution has been processed.
		 */
		bool getNextAvailableDisplayResolution(char* showStr, UInt8* isSelected);

		/**
		 * Sets the values according to the showString.
		 * @param selectedShowStr The passed-in show string as it is presented on select control.
		 * @param[out] horizontalPixels The horizontal dimension of the resolution measured in pixels.
		 * @param[out] verticalPixels The vertical dimension of the resolution measured in pixels.
		 * @param[out] bitsPerPixel The number of bits per pixel of the resolution.
		 * @param[out] refreshRate The refresh rate of the resolution.
		 * @return Error state as SInt32 OSStatus.
		 */
		OSStatus matchDisplayResolutionShowStrWithPrefs(const char* const selectedShowStr, UInt16& horizontalPixels, UInt16& verticalPixels, UInt16& bitsPerPixel, UInt16& refreshRate);

		/**
		 * Returns the initially determimed preferred or best display resolution for full screen display.
		 * @return The initially determimed preferred or best display resolution for full screen display.
		 */
		DisplayResolution returnPreferredDisplayResolution(void);

		/**
		 * Returns a randomly picked horizontal position.
		 * @return Randomly picks possible position in horizontal dimension.
		 */
		double getRandXScreenPos(void);

		/**
		 * Returns a randomly picked vertical position.
		 * @return Randomly picks possible position in vertical dimension.
		 */
		double getRandYScreenPos(void);
		
		/**
		 * Sets the current context to the one that has been set up before with the setupContext() call.
		 * @return errNum: 0 if no error, > 0 error.
		 * @remarks The context must be set back to the one set up initially with each render action in case Cover Flow did change it in the meantime.
		 */
		UInt16 setCurrentContext(void);

		/**
		 * Ends the GL drawing by flushing, finishing and swaping the buffer content.
		 * Every executed drawing command should show its result on screen.
		 * The function is called at the end of each RenderMessage.
		 */
		void finishGLDrawing(void);
		
		/**
		 * Returns the current color buffer specification for reading pixels.
		 * @return The current color buffer specification for reading pixels.
		 */
		int getCurrentColorBufferForPixelReadingOperations(void);
		
		/**
		 * Returns the current color buffer specification for drawing pixels.
		 * @return The current color buffer specification for drawing pixels.
		 */
		int getCurrentColorBufferForPixelDrawingOperations(void);
		
		/**
		 * Sets the color buffer specification for reading pixels.
		 * @param colorBuffer The color buffer to use for reading pixels.
		 */
		void setColorBufferForPixelReadingOperations(int colorBuffer);

		/**
		 * Sets the color buffer specification for drawing pixels.
		 * @param colorBuffer The color buffer to use for drawing pixels.
		 */
		void setColorBufferForPixelDrawingOperations(int colorBuffer);

		/**
		 * Draws a texture on screen.
		 * If the texture is stored in texture memory, this function can be called to put the texture on screen.
		 * @param textureNumber The number/id/name of the texture.
		 * @param vertexChain A vector of vertices.
		 * @param canUseRectExtension True if graphics system does support rectangular texture extension.\ False otherwise.\ Default false.
		 * @param aBlendMode How to blend texture data with the framebuffer data.\ Default kBlend.
		 */
		void drawTexture(UInt32 textureNumber, ConstVertexChainRef const vertexChain, bool canUseRectExtension = false, BlendMode aBlendMode = kBlend);

		/**
		 * Draws image data to framebuffer.
		 * @param pixelData Pointer to pointer to image data.
		 * @param xCoord The bottom left x coord.
		 * @param yCoord The bottom top y coord.
		 * @param width The width of the rectangle.
		 * @param height The height of the rectangle.
		 * @param format The format of the pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param dataType The data type of the pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 * @param aConvolutionFilter An optional convolution filter that is used when drawing the pixels.
		 */	
		void drawPixels(UInt32** pixelData, double xCoord, double yCoord, UInt32 width, UInt32 height, UInt16 format, UInt16 dataType, const VisualConvolutionFilter* const aConvolutionFilter = NULL);

		/**
		 * Reads image data from framebuffer into processor memory.
		 * @param xCoord The bottom left x coord.
		 * @param yCoord The bottom top y coord.
		 * @param width The width of the rectangle.
		 * @param height The height of the rectangle.
		 * @param[out] pixelData Pointer to pointer of buffer to write image data to.
		 * @param format The format of the requested pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param dataType The data type of the requested pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 */	
		void readPixels(double xCoord, double yCoord, UInt32 width, UInt32 height, UInt32** pixelData, UInt16 format, UInt16 dataType);

		/**
		 * Creates a texture of the track title. The function uses a DIB section. 
		 * A custom font is loaded and the string is printed using OS specific funcionality. Windows-only.
		 * @param stringValue The string to plot to texture.
		 * @param stringValueLength The length of the string to plot to texture.
		 * @param textureNumber The texture number (texture name) of the generated texture.
		 * @param[out] textureWidth The width of the generated texture.
		 * @param[out] textureHeight The height of the generated texture.
		 * @param[out] imageWidth The width of the image data of the generated texture.
		 * @param[out] imageHeight The height of the image data of the generated texture.
		 * @param fontName The name of the font.
		 * @param fontSize The size of the font in points.\ The point size denotes the cell height of the font.
		 * @param red The red color component of the text font.
		 * @param green The green color component of the text font.
		 * @param blue The blue color component of the text font.
		 * @param alignment The horizontal alignment of the string.
		 * @param maxPixelWidth The requested maximum width of the texture.\ If 0 the argument is ignored.
		 * @param maxPixelHeight The requested maximum height of the texture.\ If 0 the argument is ignored.
		 * @param On Windows it is common to measure the point font height excluding internal leading.\ On Mac the point font size is measured including internal leading.
		 */
#if TARGET_OS_WIN
		OSStatus makeTextureOfStringWin(wchar_t* stringValue, int stringValueLength, UInt32 textureNumber, UInt32 &textureWidth, UInt32 &textureHeight, UInt32 &imageWidth, UInt32 &imageHeight, const char* const fontName, UInt16 fontSize, UInt8 red, UInt8 green, UInt8 blue, HorizontalAlignment alignment, UInt16 maxPixelWidth = 0, UInt16 maxPixelHeight = 0);
#endif

		/**
		 * Draws a single spot.
		 * @param xNum The horizontal position of the spot.
		 * @param yNum The vertical position of the spot.
		 * @param r The red component of the RGB-Color.
		 * @param g The green component of the RGB-Color.
		 * @param b The blue component of the RGB-Color.
		 * @param waveformIntensityVal The current radius of the spot.
		 * @param intensity The current intensity of the spot.
		 * @param tailSize The current tailSize of the spot.
		 */		
		void drawSpot(const float xNum, const float yNum, const float r, const float g, const float b, UInt16 waveformIntensityVal, float intensity, const UInt8 tailSize);

		/**
		 * Draws a triangle.
		 * For testing purposes.
		 */
		void drawTriangle(void);
		
		/**
		 * Draws a spot (for testing purpose).
		 * @param zPlane The z-Coord (optional parameter, default = 0.0).
		 */
		void spotGL(double zPlane = 0.0);

		/**
		 * Draws a perspective rectangle.
		 * @param textureNumber The number/id/name of the texture (if any).
		 */	
		void drawPerspectiveRect(UInt32 textureNumber = NULL);

		/**
		 * Draws a histogram of the beat values.
		 * In use?
		 * @param beatHistogram The beat histogram.
		 */
		void drawBeatHistogram(const UInt32* const beatHistogram);

		/**
		 * Draws a colorless GL primitive by calling glVertex() between glBegin() and glEnd(). 
		 * The color of the vertices can be set before calling this method.
		 * @param primitiveType Ten symbolic constants are accepted: GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, 
		 * GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, and GL_POLYGON.
		 * @param vertexChain A vector of vertices.
		 */
		void drawColorlessGLPrimitive(UInt16 primitiveType, ConstVertexChainRef const vertexChain);

		/**
		 * Draws a GL primitive by calling glVertex() between glBegin() and glEnd(). 
		 * The color of the vertices can be set before calling this method.
		 * @param primitiveType Ten symbolic constants are accepted: GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, 
		 * GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, and GL_POLYGON.
		 * @param vertexChain A vector of vertices.
		 */
		void drawGLPrimitive(UInt16 primitiveType, ConstVertexChainRef const vertexChain);

		/**
		 * The function queries the system and answers whether textures can have non power of 2 dimensions.
		 * @return true if graphics system does support rectangular texture extension. false otherwise.
		 */	
		bool canUseTextureRectExtension(void);

		/**
		 * Draws the waveform data.
		 * @param historyNum The index of the history the waveform data is taken from.
		 * @param maxNumberOfHistories Maximum number of histories stored in waveformDataMonoArray.
		 * @param numberOfWaveformEntries The number of waveform values.
		 * @param waveformDataMonoArray Pointer to waveform data of all histories. The array has two dimensions: history and index.
		 */		
		void drawWaveform(const SInt16 historyNum, const UInt16 maxNumberOfHistories, const UInt32 numberOfWaveformEntries, SInt16** waveformDataMonoArray);

		/**
		 * Draws the data of a vector.
		 * @param aVector Reference to a vextor that holds the data.
		 * @param baseIdx The current start index.
		 * @param minVal The (possible) minumum value.\ The value is used for calculating the vertical dimension of the diagram.
		 * @param maxVal The (possible) maximum value.\ The value is used for calculating the vertical dimension of the diagram.
		 * @remarks The vector can act as a ringbuffer by letting baseIdx point to the current start of the vector.
		 */	
		static void drawHistoryDiagram(std::vector<double>& aVector, size_t baseIdx, double minVal, double maxVal);
		
		/**
		 * Draws the waveform data in spiral form.
		 * @param currHistoryNum The current history of the waveform data.
		 * @param numberOfWaveformEntries The number of waveform values.
		 * @param waveformDataMonoArray Pointer to waveform data of all histories.
		 */
		void drawWaveformSpiral(const UInt16 currHistoryNum, const UInt16 numberOfWaveformEntries, const SInt16* const waveformDataMonoArray);

		/**
		 * Draws a graphical representation of the spectral data with subband bar graphs.
		 * @param currHistoryNum The index of the history the spectrum data is taken from.
		 * @param numberOfHistories The maximum number of histories.
		 * @param numberOfSpectrumEntries The number of spectrum data values.
		 * @param numberOfAudioChannels The number of audio channels.
		 * @param spectrumDataArray Pointer to spectrum data of all histories.
		 */
		void drawSpectrumAnalyzer(const SInt16 currHistoryNum, const UInt16 numberOfHistories, const UInt32 numberOfSpectrumEntries, const UInt16 numberOfAudioChannels, const UInt8*** const spectrumDataArray);

		/**
		 * Draws a graphical representation of the spectral data as spectrogram.
		 * @param currHistoryNum The index of the history the spectrum data is taken from.
		 * @param numberOfHistories The maximum number of histories.
		 * @param numberOfSpectrumEntries The number of spectrum data values.
		 * @param numberOfAudioChannels The number of audio channels.
		 * @param spectrumDataArray Pointer to spectrum data of all histories.
		 */
		void drawSpectrogram(const SInt16 currHistoryNum, const UInt16 numberOfHistories, const UInt32 numberOfSpectrumEntries, const UInt16 numberOfAudioChannels, const UInt8*** const spectrumDataArray);

		/**
		 * Draws a circle representing the waveform data.
		 * @param xNum The horizontal position.
		 * @param yNum The vertical position.
		 * @param rotVal The degree of rotation.
		 * @param radius Radius.
		 */
		void drawWaveformCircle(const float xNum, const float yNum, const float rotVal, const float radius);

		/**
		 * Draws a shape constucted by waveform data.
		 * @param waveformShapeArray Pointer to the waveform data.
		 * @param maxNumberOfWaveformShapeHistory Maximum of history.
		 */
		void drawWaveformShape(const SInt8* const waveformShapeArray, const UInt16 maxNumberOfWaveformShapeHistory);

		/**
		 * Draws a 7bit char buffer string to screen.
		 * Mac uses GLUT_BITMAP_9_BY_15. Win uses Arial font created by wglUseFontBitmaps.
		 */
		void drawCStringWithGL(const char* const cString, const UInt16 stringLength);

		/**
		 * The default implementation of the show() method of the VisualActor interface class.
		 * @param visualActorName The name of the VisualActor.
		 */
		void doFallbackActorShow(const char* const visualActorName);

		/**
		 * Returns cos(2.0 * _PI).
		 * @return cos(2.0 * _PI).
		 */		
		float getCosTwoTimesPi(void);

		/**
		 * Returns sin(2.0 * _PI).
		 * @return sin(2.0 * _PI).
		 */		
		float getSinTwoTimesPi(void);
		
		/**
		 * Returns a free texture name/index/number/id.
		 * @return A free texture name/index/number.
		 */
		UInt32 getNextFreeTextureName(void);

		/**
		 * Frees memory bound by OpenGL textures.
		 * @param numberOfTextures The number of textures to free.
		 * @param textureNames Pointer to texture names (texture numbers) to free.
		 */
		void deleteTextures(const UInt16 numberOfTextures, const UInt32* const textureNames);

		/**
		 * Shows a row of Process Monitor Info.
		 * The graphical operations involved in putting a row of textual process info on screen is encapsulated within this function.
		 * @param xNum Horizontal position.
		 * @param yNum Vertical position.
		 * @param textRowStr Pointer to character buffer with process info string (label and value).
		 */
		void showProcessInfoRow(double xNum, double yNum, const char* const textRowStr);

		/**
		 * Shows a note about Process Monitor.
		 */	
		void showProcessInfoNote(void);
		
		/**
		 * Creates an image buffer with 8-bit per channel ARGB pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @return A pointer to pointer to pixel data.
		 */
		UInt32* createARGBCheckPixels(UInt32 width, UInt32 height);

		/**
		 * Creates an image buffer with 8-bit per channel BGRA pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @return A pointer to pointer to pixel data.
		 */
		UInt32* createBGRACheckPixels(UInt32 width, UInt32 height);

		/**
		 * Creates an image buffer with 8-bit per channel RGBA pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @return A pointer to pointer to pixel data.
		 */
		UInt32* createRGBACheckPixels(UInt32 width, UInt32 height);

		/**
		 * Creates an image buffer with 8-bit per channel ABGR pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @return A pointer to pointer to pixel data.
		 */
		UInt32* createABGRCheckPixels(UInt32 width, UInt32 height);

		/**
		 * Creates a texture of squares. Useful for testing purposes.
		 * @param[out] textureNumber The texture number (texture name) of the generated texture.
		 * @param[out] textureWidth The width of the generated texture.
		 * @param[out] textureHeight The height of the generated texture.
		 * @param[out] imageWidth The width of the image data.
		 * @param[out] imageHeight The height of the image data.
		 */
		void createCheckTexture(UInt32& textureNumber, UInt32& textureWidth, UInt32& textureHeight, UInt32& imageWidth, UInt32& imageHeight);

		/**
		 * Copies ARGB bitmap data into texture.
		 * @param textureNumber The texture number (texture name) of the generated texture.
		 * @param width The width of the bitmap data (and therefore width of texture, too).
		 * @param height The height of the bitmap data (and therefore height of texture, too).
		 * @param canUseRectExtension True if the texture does support rectangular texture extension.\ False otherwise.
		 * @param format The format of the pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param dataType The data type of the pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 * @param bitmapData Pointer to pointer to the bitmap data.
		 */
		OSStatus copyARGBBitmapDataToTexture(UInt32 textureNumber, UInt32 width, UInt32 height, bool canUseRectExtension, UInt16 format, UInt16 dataType, const UInt32** bitmapData);

#if TARGET_OS_WIN
		/**
		 * The function queries the system to determine if the font is installed. Windows-only.
		 * @param currHdc Pointer to the current device.
		 * @param fontName The name of the font.
		 * @return 1 if font is installed, 0 if font is not installed.
		 */
		UInt8 fontIsInstalled(HDC currHdc, const char* const fontName);

		/**
		 * Builds the GL font.
		 * The built font can be used in OpenGL as a glList. Uses wglUseFontBitmaps to build font bitmap. Windows-only.
		 */
		void buildFont(void);

		/**
		 * Destroys the built GL font. Windows-only.
		 */
		void killFont(void);

		/**
		 * Prints the character buffer with GL bitmap font. Windows-only.
		 * @param fmt Pointer to character buffer.
		 */
		void glPrint(const char *fmt, ...);
#endif

		/**
		 * Stores the maximum top position internally.
		 * @param theMaxTopCoordOfGLView The maximum top position.
		 */
		void setMaxTopCoordOfCanvas(double theMaxTopCoordOfGLView);

		/**
		 * Stores the maximum left position internally.
		 * @param theMaxLeftCoordOfGLView The maximum left position.
		 */
		void setMaxLeftCoordOfCanvas(double theMaxLeftCoordOfGLView);

		/**
		 * Stores the maximum bottom position internally.
		 * @param theMaxBottomCoordOfGLView The maximum bottom position.
		 */
		void setMaxBottomCoordOfCanvas(double theMaxBottomCoordOfGLView);

		/**
		 * Stores the maximum right position internally.
		 * @param theMaxRightCoordOfGLView The maximum right position.
		 */
		void setMaxRightCoordOfCanvas(double theMaxRightCoordOfGLView);

		/**
		 * Stores the maximum near position internally.
		 * @param theMaxNearCoordOfGLView The maximum near position.
		 */
		void setMaxNearCoordOfCanvas(double theMaxNearCoordOfGLView);
		
		/**
		 * Stores the maximum far position internally.
		 * @param theMaxFarCoordOfGLView The maximum far position.
		 */
		void setMaxFarCoordOfCanvas(double theMaxFarCoordOfGLView);

		/**
		 * Returns the maximum top position.
		 * @return The maximum top position.
		 */
		double getMaxTopCoordOfCanvas(void);

		/**
		 * Returns the maximum left position.
		 * @return The maximum left position.
		 */
		double getMaxLeftCoordOfCanvas(void);

		/**
		 * Returns the maximum bottom position.
		 * @return The maximum bottom position.
		 */
		double getMaxBottomCoordOfCanvas(void);

		/**
		 * Returns the maximum right position.
		 * @return The maximum right position.
		 */
		double getMaxRightCoordOfCanvas(void);

		/**
		 * Returns the maximum near position.
		 * @return The maximum near position.
		 */
		double getMaxNearCoordOfCanvas(void);

		/**
		 * Returns the maximum far position.
		 * @return The maximum far position.
		 */
		double getMaxFarCoordOfCanvas(void);
		
		/**
		 * Returns the horizontal origin of the scissor rect.
		 * @return The horizontal origin of the scissor rect.
		 */
		int getCanvasXOriginOffset(void);

		/**
		 * Returns the vertical origin of the scissor rect.
		 * @return The vertical origin of the scissor rect.
		 */
		int getCanvasYOriginOffset(void);

		/**
		 * Returns the name of the renderer of the graphic card.
		 * @return The name of the renderer of the graphic card.
		 */
		const char* const getRendererName(void);

		/**
		 * Returns the name of the vendor of the graphic card.
		 * @return The name of the vendor of the graphic card.
		 */
		const char* const getRendererVendor(void);

		/**
		 * Returns the version of the renderer.
		 * @return The version of the renderer.
		 */
		const char* const getRendererVersion(void);

		/**
		 * Returns the version of OpenGL.
		 * @return The version of OpenGL.
		 */
		unsigned short getGLVersion(void);

		/**
		 * Returns the maximum texture size.
		 * @return the maximum texture size.
		 */
		long getMaxTextureSize(void);

		/**
		 * Converts a horizontal pixel position to a horizontal coordinate.
		 * @param pixelPos The canvas pixel position.
		 * @return The canvas coord position.
		 */	
		double xPixelToCoord(UInt32 pixelPos);

		/**
		 * Converts a vertical pixel position to a vertical coordinate.
		 * @param pixelPos The canvas pixel position.
		 * @return The canvas coord position.
		 */		
		double yPixelToCoord(UInt32 pixelPos);

		/**
		 * Converts a horizontal coordinate to a horizontal pixel position.
		 * @param coordPos The canvas coord position.
		 * @return The canvas pixel position.
		 */	
		UInt16 xCoordToPixel(double coordPos);

		/**
		 * Converts a vertical coordinate to a horizontal pixel position.
		 * @param coordPos The canvas coord position.
		 * @return The canvas pixel position.
		 */	
		UInt16 yCoordToPixel(double coordPos);

		/**
		 * Maps window coordinates to object coordinates.
		 * @param xPixelPos The horizontal pixel position.
		 * @param yPixelPos The vertical pixel position.
		 * @param zAxisPos The position on the z-axis.\ 0: near clipping plane, 1: far clipping plane.
		 * @param xCoordPos Returns: The x-CoordPosition.
		 * @param yCoordPos Returns: The y-CoordPosition.
		 * @param zCoordPos Returns: The z-CoordPosition.
		 * @return True on success, false on error.
		 */	
		bool getCoordsOfPixelPosition(
										double xPixelPos, 
										double yPixelPos, 
										double zAxisPos, 
										double* xCoordPos,
										double* yCoordPos,
										double* zCoordPos);

		/**
		 * Maps object coordinates to window coordinates.
		 * @param xCoordPos The x-CoordPosition.
		 * @param yCoordPos The y-CoordPosition.
		 * @param zCoordPos The z-CoordPosition.
		 * @param xPixelPos Returns: The horizontal pixel position.
		 * @param yPixelPos Returns: The vertical pixel position.
		 * @param zAxisPos Returns: The position on the z-axis.
		 * @return True on success, false on error.
		 */	
		bool getPixelsOfCoordPosition(
										double xCoordPos,
										double yCoordPos,
										double zCoordPos,
										double* xPixelPos, 
										double* yPixelPos, 
										double* zAxisPos);

		/**
		 * Determines the openGL capabilities and stores them internally.
		 */
		void gatherOpenGLCapabilities(void);

		/**
		 * Creates a NURBS object and returns a pointer to it.
		 * @param identifier Identifier of the NURBS.\ The Nurbs can later be addressed by that token.
		 * @param sNumKnots The number of knots in horizontal direction.
		 * @param sKnots An array of horizontal knot values.
		 * @param tNumKnots The number of knots in vertical direction.
		 * @param tKnots An array of vertical knot values.
		 * @param sStride The offset between successive control points in u direction.
		 * @param tStride The offset between successive control points in v direction.
		 * @param sOrder The order of the NURBS surface in u direction.
		 * @param tOrder The order of the NURBS surface in v direction.
		 * @return The newly created NURBS object with the identifierName.
		 */
		VisualNurbs* createNurbsObject(const VisualItemIdentifier& identifier, UInt8 sNumKnots, float* sKnots, UInt8 tNumKnots, float* tKnots, UInt16 sStride, UInt16 tStride, UInt16 sOrder, UInt16 tOrder);

		/**
		 * Deletes a NURBS object.
		 * @param identifier The identifier of the NURBS.
		 */
		void deleteNurbsObject(const VisualItemIdentifier& identifier);

		/**
		 * Returns a pointer to a NURBS object.
		 * @param identifier The identifier of the NURBS.
		 * @return The NURBS object with the identifier.
		 */		
		VisualNurbs* getNurbsObject(const VisualItemIdentifier& identifier);

		/**
		 * Copies the content of a screen rectangle into a texture buffer.
		 * @param textureNumber The number/id/name of the texture.
		 * @param canUseRectExtension True if graphics system does support rectangular texture extension.\ False otherwise.\ Default false.
		 * @param xPos The horizontal x position of the screen rect.
		 * @param yPos The vertical y position of the screen rect.
		 * @param width The width of the rectangle.
		 * @param height The height of the rectangle.
		 * @param pixelFormat The format of the pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param dataType The data type of the pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 * @remarks The pixels are copied from the framebuffer.
		 */	
		void copyFramebufferToTexture(UInt32 textureNumber, bool canUseRectExtension, UInt32 xPos, UInt32 yPos, UInt32 width, UInt32 height, UInt16 pixelFormat, UInt16 dataType);

		/**
		 * Returns the nearest power of 2 integer following the input integer.
		 * @param n The input integer.
		 * @return A power of 2 integer.
		 */
		UInt32 power2Ceiling(UInt32 n);

		/**
		 * Evaluates the preferred or best available display resolution for fullscreen display.
		 */		
		void evaluateFullscreenDisplayResolution(void);

	private:

		/** The constructor.\ VisualGraphics is a singleton class.\ The constructor is private.\ New instance of class can only be created internally. */
		VisualGraphics();

		/** The destructor.\ VisualGraphics is a singleton class.\ The destructor is private.\ Instance of class can only be destructed internally. */
		~VisualGraphics();

		/**
		 * Copy constructor.
		 * @param other Another VisualGraphics.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualGraphics(const VisualGraphics& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualGraphics& operator=(const VisualGraphics& other);

		/** VisualGraphics is a singleton class. Pointer to private instance is handled internally. */
		static VisualGraphics* theVisualGraphics;

		/**
		 * Returns the iModeNum for requested screen resolution settings.\ Windows-only.
		 * @param aDisplayResolution The requested screen resoltion values.
		 * @return The iModeNum for requested screen resolution settings.
		 */
#if TARGET_OS_WIN
		DWORD getBestModeForDisplayResolution(const DisplayResolution* const aDisplayResolution);
#endif

		/** The rectangle where the drawing takes place. */
		::Rect canvasRect;
		
		/** The rectangle of the window. */
		::Rect windowRect;
		
		/** True if set up on full screen, false if set up in windowed mode. */
		bool isSetUpOnFullscreen;

		/** The maximum top position. */
		double maxTopCoordOfGLView;
		
		/** The maximum left position. */
		double maxLeftCoordOfGLView;
		
		/** The maximum bottom position. */
		double maxBottomCoordOfGLView;
		
		/** The maximum right position. */
		double maxRightCoordOfGLView;
		
		/** The maximum near position. */
		double maxNearCoordOfGLView;
		
		/** The maximum far position. */
		double maxFarCoordOfGLView;
		
		/** The color of the background. */
		RGBAColor backgroundColor;

		/**
		 * A struct containing OpenGL capabilities.
		 * A collection of OpenGL capabilities. 
		 * The capabilities are not complete and can be extended or reduced as appropriate.
		 */
		typedef struct {

			// current (at time of look up) device geometry
			long deviceWidth; /**< Pixel width. */
			long deviceHeight; /**< Pixel height. */
			long deviceOriginX; /**< left location of device (relative to main device) */
			long deviceOriginY; /**< upper location of device (relative to main device) */
			short deviceDepth; /**< pixel depth in bits */
			short deviceRefresh; /**< integer refresh rate in Hz */

			// Renderer info
			long deviceVRAM; /**< video memory in bytes */
			long deviceTextureRAM; /**< uses current mode (geometry, pixel depth, etc.) */
			unsigned long rendererID; /**< renderer ID */
			char strRendererName [256]; /**< name of hardware renderer */
			char strRendererVendor [256]; /**< name of hardware renderer vendor */
			char strRendererVersion [256]; /**< string rep of hardware renderer version */
			bool fullScreenCapable; /**< does device support full screen */

			// Renderer Caps
			long textureUnits; /**< standard gl path max number of texture units */
			long maxTextureSize; /**< maximum 1D and 2D texture size supported */
			long max3DTextureSize; /**< maximum 3D texture size supported */
			long maxCubeMapTextureSize; /**< maximum cube map texture size supported */
			long maxRectTextureSize; /**< maximum rectangular texture size supported */

			// OpenGL version support
			unsigned short glVersion; /**< bcd gl version (ie. 1.4 is 0x0140) */

			// Functionality
			bool fSpecularVector; /**< GL_APPLE_specular_vector */
			bool fTransformHint; /**< GL_APPLE_transform_hint */
			bool fPackedPixels; /**< GL_APPLE_packed_pixels or 1.2+ */
			bool fClientStorage; /**< GL_APPLE_client_storage */
			bool fYCbCr; /**< GL_APPLE_ycbcr_422 (YUV texturing) */
			bool fTextureRange; /**< GL_APPLE_texture_range (AGP texturing) */
			bool fFence; /**< GL_APPLE_fence */
			bool fVAR; /**< GL_APPLE_vertex_array_range */
			bool fVAO; /**< GL_APPLE_vertex_array_object */
			bool fElementArray; /**< GL_APPLE_element_array */
			bool fVPEvals; /**< GL_APPLE_vertex_program_evaluators */
			bool fFloatPixels; /**< GL_APPLE_float_pixels */
			bool fFlushRenderer; /**< GL_APPLE_flush_render */
			bool fPixelBuffer; /**< GL_APPLE_pixel_buffer */
			bool fImaging; /**< GL_ARB_imaging  (not required in 1.2+) */
			bool fTransposeMatrix; /**< GL_ARB_transpose_matrix or 1.3+ */
			bool fMultitexture; /**< GL_ARB_multitexture or 1.3+ */
			bool fTexEnvAdd; /**< GL_ARB_texture_env_add, GL_EXT_texture_env_add or 1.3+ */
			bool fTexEnvCombine; /**< GL_ARB_texture_env_combine or 1.3+ */
			bool fTexEnvDot3; /**< GL_ARB_texture_env_dot3 or 1.3+ */
			bool fTexEnvCrossbar; /**< GL_ARB_texture_env_crossbar or 1.4+ */
			bool fTexCubeMap; /**< GL_ARB_texture_cube_map or 1.3+ */
			bool fTexCompress; /**< GL_ARB_texture_compression or 1.3+ */
			bool fMultisample; /**< GL_ARB_multisample or 1.3+ (Anti-aliasing) */
			bool fTexBorderClamp; /**< GL_ARB_texture_border_clamp or 1.3+ */
			bool fPointParam; /**< GL_ARB_point_parameters or 1.4+ */
			bool fVertexProg; /**< GL_ARB_vertex_program */
			bool fFragmentProg; /**< GL_ARB_fragment_program */
			bool fTexMirrorRepeat; /**< GL_ARB_texture_mirrored_repeat or 1.4+ */
			bool fDepthTex; /**< GL_ARB_depth_texture or 1.4+ */
			bool fShadow; /**< GL_ARB_shadow or 1.4+ */
			bool fShadowAmbient; /**< GL_ARB_shadow_ambient */
			bool fVertexBlend; /**< GL_ARB_vertex_blend */
			bool fWindowPos; /**< GL_ARB_window_pos or 1.4+ */
			bool fTex3D; /**< GL_EXT_texture3D or 1.2+ */
			bool fClipVolHint; /**< GL_EXT_clip_volume_hint */
			bool fRescaleNorm; /**< GL_EXT_rescale_normal or 1.2+ */
			bool fBlendColor; /**< GL_EXT_blend_color or GL_ARB_imaging */
			bool fBlendMinMax; /**< GL_EXT_blend_minmax or GL_ARB_imaging */
			bool fBlendSub; /**< GL_EXT_blend_subtract or GL_ARB_imaging */
			bool fCVA; /**< GL_EXT_compiled_vertex_array */
			bool fTexLODBias; /**< GL_EXT_texture_lod_bias or 1.4+ */
			bool fABGR; /**< GL_EXT_abgr */
			bool fBGRA; /**< GL_EXT_bgra or 1.2+ */
			bool fTexFilterAniso; /**< GL_EXT_texture_filter_anisotropic */
			bool fPaletteTex; /**< GL_EXT_paletted_texture */
			bool fShareTexPalette; /**< GL_EXT_shared_texture_palette */
			bool fSecColor; /**< GL_EXT_secondary_color or 1.4+ */
			bool fTexCompressS3TC; /**< GL_EXT_texture_compression_s3tc */
			bool fTexRect; /**< GL_EXT_texture_rectangle */
			bool fFogCoord; /**< GL_EXT_fog_coord */
			bool fDrawRangeElements; /**< GL_EXT_draw_range_elements */
			bool fStencilWrap; /**< GL_EXT_stencil_wrap or 1.4+ */
			bool fBlendFuncSep; /**< GL_EXT_blend_func_separate or 1.4+ */
			bool fMultiDrawArrays; /**< GL_EXT_multi_draw_arrays or 1.4+ */
			bool fShadowFunc; /**< GL_EXT_shadow_funcs */
			bool fStencil2Side; /**< GL_EXT_stencil_two_side */
			bool fColorSubtable; /**< GL_EXT_color_subtable or GL_ARB_imaging */
			bool fConvolution; /**< GL_EXT_convolution or GL_ARB_imaging */
			bool fHistogram; /**< GL_EXT_histogram or GL_ARB_imaging */
			bool fColorTable; /**< GL_SGI_color_table or GL_ARB_imaging */
			bool fColorMatrix; /**< GL_SGI_color_matrix */
			bool fTexEdgeClamp; /**< GL_SGIS_texture_edge_clamp or 1.2+ */
			bool fGenMipmap; /**< GL_SGIS_generate_mipmap or 1.4+ */
			bool fTexLOD; /**< GL_SGIS_texture_lod or 1.2+ */
			bool fPointCull; /**< GL_ATI_point_cull_mode */
			bool fTexMirrorOnce; /**< GL_ATI_texture_mirror_once */
			bool fPNtriangles; /**< GL_ATI_pn_triangles or GL_ATIX_pn_triangles */
			bool fTextFragShader; /**< GL_ATI_text_fragment_shader */
			bool fBlendEqSep; /**< GL_ATI_blend_equation_separate */
			bool fBlendWeightMinMax; /**< GL_ATI_blend_weighted_minmax */
			bool fCombine3; /**< GL_ATI_texture_env_combine3 */
			bool fSepStencil; /**< GL_ATI_separate_stencil */
			bool fArrayRevComps4Byte; /**< GL_ATI_array_rev_comps_in_4_bytes */
			bool fPointSprite; /**< GL_NV_point_sprite */
			bool fRegCombiners; /**< GL_NV_register_combiners */
			bool fRegCombiners2; /**< GL_NV_register_combiners2 */
			bool fTexEnvCombine4; /**< GL_NV_texture_env_combine4 */
			bool fBlendSquare; /**< GL_NV_blend_square or 1.4+ */
			bool fFogDist; /**< GL_NV_fog_distance */
			bool fMultisampleFilterHint; /**< GL_NV_multisample_filter_hint */
			bool fTexGenReflect; /**< GL_NV_texgen_reflection */
			bool fTexShader; /**< GL_NV_texture_shader */
			bool fTexShader2; /**< GL_NV_texture_shader2 */
			bool fTexShader3; /**< GL_NV_texture_shader3 */
			bool fDepthClamp; /**< GL_NV_depth_clamp */
			bool fLightMaxExp; /**< GL_NV_light_max_exponent */
			bool fConvBorderModes; /**< GL_HP_convolution_border_modes or GL_ARB_imaging */
			bool fRasterPosClip; /**< GL_IBM_rasterpos_clip */
		} GLCapabilities;
		
		/** The capabilities. */
		GLCapabilities capabilities;

		/**
		 * A struct containing the preferred or best available display resolution for fullscreen display.
		 */
		typedef struct {
			DisplayResolution theFullscreenDisplayResolution; /**< The preferred or best available display resolution for fullscreen display. */
			bool fullscreenDisplayResolutionIsKnown; /**< True if the preferred or best available display resolution for fullscreen display is known yet. */
		} FullscreenDisplayResolution;
		
		/** The preferred or best available display resolution for fullscreen display (incl.\ flag if the resolution has been evaluated yet). */
		FullscreenDisplayResolution fullscreenDisplayResolution;
		
		/**
		 * A DisplayResolutionVector is a vector of all display resolutions.
		 */
		typedef std::vector<DisplayResolution*> DisplayResolutionVector;
		
		/** The displayResolutionVector that contains all display resolutions. */
		DisplayResolutionVector displayResolutionVector;
		
		/** A DisplayResolutionVectorIterator is an iterator of a DisplayResolutionVector. */
		typedef DisplayResolutionVector::iterator DisplayResolutionVectorIterator;
		
		/** Current iterator index of display resolution vector. */
		UInt16 displayResolutionIterIndex;

		/**
		 * Gathers the available screen resolutions and stores them in displayResolutionVector.
		 */
		void gatherAvailableDisplayResolutions(void);

		/**
		 * Resets the iterator.
		 * Before iterating over the available screen resolutions gathered in displayResolutionVector
		 * the iterator has to be reset by calling this method.
		 */
		void resetDisplayResolutionIterIndex(void);

		/** A NurbsMap is a map of an identifier string and a pointer to a VisualNurbs. */
		typedef std::map<VisualItemIdentifier, VisualNurbs*> NurbsMap;
		
		/** The nurbsMap is the map of the VisualNurbs. */
		NurbsMap nurbsMap;
		
		/** A NurbsMapIterator is an iterator of a NurbsMap. */
		typedef NurbsMap::iterator NurbsMapIterator;

	};

}

#endif /* VisualGraphics_h */
