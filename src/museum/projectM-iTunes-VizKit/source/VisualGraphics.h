/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualGraphics.h
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

#ifndef VisualGraphics_h
#define VisualGraphics_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"

#include <map>
#include <vector>

#if TARGET_OS_MAC
#include <ApplicationServices/../Frameworks/QD.framework/Headers/Quickdraw.h> // CGrafPtr
#endif

#if TARGET_OS_WIN
#include <windows.h> // HWND
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
	class VisualCamera; // Forward declaration (to avoid include of header file).
	
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
		 * @return True if VisualGraphics singleton class is initialized. False if not.
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
		 * @param aCanvasRect Rectangle dimensions.
		 * @remarks The values of the TopLeftPositionedPixelRect are converted and stored internally as BottomLeftPositionedPixelRect values.
		 */
		void setCanvasRect(const TopLeftPositionedPixelRect& aCanvasRect);
		
		/**
		 * Sets the dimensions of the total visualizer rect.
		 * @param aRect The dimensions and position of the total visualizer rect.
		 */
		void setTotalVisualizerRect(TopLeftPositionedPixelRect aRect);
		
		/**
		 * Stores the vertical offset of the surface rect from the bottom of the iTunes window.
		 * @param top The top pixel position of the total visualizer rect.
		 * @param height The height of the total visualizer rect.
		 */	
		void setTotalVisualizerRectVerticalOffsetFromBottom(int top, int height);
		
		/**
		 * Stores whether the visualizer is expected to show in fullscreen mode or in windowed mode.
		 * @param isFullscreen True if fullscreen mode is expected. False if windowed mode is expected.
		 */
		void isSetupForFullScreenMode(const bool isFullscreen);
		
		/**
		 * Returns the draw rect positioned relative to window rect.
		 */
		BottomLeftPositionedPixelRect getCanvasRect(void);
		
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
		 * Returns the width of the canvas in pixels.
		 * @return The width of the canvas in pixels.
		 */
		uint32 getCanvasPixelWidth(void);
		
		/**
		 * Returns the height of the canvas in pixels.
		 * @return The height of the canvas in pixels.
		 */
		uint32 getCanvasPixelHeight(void);
		
		/**
		 * Returns the dimensions and position of the current OpenGL viewport.
		 * @return The dimensions and position of the current OpenGL viewport.
		 */
		BottomLeftPositionedPixelRect getViewportRect(void);
		
		/**
		 * Returns the bottom-left positioned origin of the viewport rect.
		 * @return The bottom-left positioned origin of the viewport rect.
		 */
		Pixel getViewportBottomLeftOrigin(void);
		
		/**
		 * Returns the current orientation and aspect ratio of the viewport.
		 * @return The current orientation and aspect ratio of the viewport.
		 */
		RelationalRect getViewportOrientationAndAspectRatio(void);
		
#if TARGET_OS_WIN
		/**
		 * Initializes Gdiplus.
		 */
		void initGdiplus(void);
		
		/**
		 * Terminates Gdiplus.
		 */
		void terminateGdiplus(void);

		/**
		 * Returns a gdiplus encoder.
		 * @param format The requested data format of the encoder.
		 * @param[out] pClsid The CLSID of the encoder.
		 * @return True on success, false on failure.
		 */
		static bool getGdiplusEncoderClsid(const WCHAR* format, CLSID* pClsid);
#endif
		
		/**
		 * Sets up an OpenGL context of the operating system.
		 * OS windowing system and OpenGL get connected.
		 * @return true on success, false on failure.
		 */
		bool setupContext();
		
		/**
		 * Tears down the OpenGL allocations.
		 */
		void disposeContext(void);
		
		/**
		 * Sets up the viewport according to current dimensions of canvasRect.
		 */
		void setCanvasViewport(void);
		
		/**
		 * Sets up the viewport according to current dimensions of surface rect.
		 */
		void setSurfaceViewport(void);
		
		/**
		 * Sets up an orthographic projection mode.
		 * @param maxLeftCoord Maximum left coord position.
		 * @param maxRightCoord Maximum right coord position.
		 * @param maxBottomCoord Maximum bottom coord position.
		 * @param maxTopCoord Maximum top coord position.
		 * @param maxNearPos Maximum near position (position of near clipping plane).
		 * @param maxFarPos Maximum far position (position of far clipping plane).
		 * @param zoomFactor Optional zoom factor.
		 */
		void setOrthographicProjection(double maxLeftCoord, double maxRightCoord, double maxBottomCoord, double maxTopCoord, double maxNearPos, double maxFarPos, double zoomFactor = 1.0);
		
		/**
		 * Sets up a perspective projection mode.
		 * @param maxLeftCoord Maximum left coord position.
		 * @param maxRightCoord Maximum right coord position.
		 * @param maxBottomCoord Maximum bottom coord position.
		 * @param maxTopCoord Maximum top coord position.
		 * @param maxNearPos Maximum near position (position of near clipping plane).
		 * @param maxFarPos Maximum far position (position of far clipping plane).
		 * @param zoomFactor Optional zoom factor.
		 */
		void setPerspectiveProjection(double maxLeftCoord, double maxRightCoord, double maxBottomCoord, double maxTopCoord, double maxNearPos, double maxFarPos, double zoomFactor = 1.0);
		
		/**
		 * Loads the identity matrix of the model view. The model view transformation are reset
		 * @remarks It is assumed the model view is the currently active matrix mode.
		 */
		void loadModelViewIdentityMatrix(void);
		
		/**
		 * Performs a viewing transformation.
		 * @param eye The position of the eye point.
		 * @param center The position of the reference point.
		 * @param up the direction of the up vector.
		 */	
		void lookAt(Point3D eye, Point3D center, Vector up);
		
		/**
		 * Sets the value of the canvas background color.
		 * @param aColorVal The RGBA values.
		 */	
		void setCanvasBackgroundColor(const RGBAColor& aColorVal);
		
		/**
		 * Returns the value of the canvas background color.
		 * @return The RGBA values of the canvas background color.
		 */	
		RGBAColor getBackgroundColor(void);
		
		/**
		 * Clears the canvas region with the background color.
		 */
		void clearCanvasBackground(void);
		
		/**
		 * Clears the surface region with the background color.
		 * @remarks The surface region is the canvas surrounding region.
		 */
		void clearSurfaceBackground(void);
		
		/**
		 * Specifies the storage parameters for pixel packing and unpacking (copying from GPU to CPU and vice versa).
		 */
		void setPixelStorageParams(void);
		
		/**
		 * Resets the model view matrix.
		 */
		void resetModelViewMatrix(void);
		
		/**
		 * Call of glTranslate().
		 * @param xNum Horizontal coord value.
		 * @param yNum Vertical coord value.
		 * @param zNum Z-coord value.
		 */	
		void translateMatrix(double xNum, double yNum, double zNum);
		
		/**
		 * Call of glRotate().
		 * @param angle The angle of the rotation.
		 * @param xAmount The amount of the rotation around the horizontal axis.
		 * @param yAmount The amount of the rotation around the vertical axis.
		 * @param zAmount The amount of the rotation around the z-axis.
		 */	
		void rotateMatrix(double angle, double xAmount, double yAmount, double zAmount);
		
		/**
		 * Call of glScale().
		 * @param xFactor The scale factor in the horizontal dimension.
		 * @param yFactor The scale factor in the vertical dimension.
		 * @param zFactor The scale factor in the z-dimension.
		 */	
		void scaleMatrix(double xFactor, double yFactor, double zFactor);
		
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
		void bindTexture(uint32 textureName, bool useRectExtension);
		
		/**
		 * Writes the pixels of the current texture (last one binded) into the provided buffer.
		 * @param useRectExtension If true texture has rectangular dimensions.
		 * @param format The format of the pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param type The data type of the pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 * @param[out] buffer Pointer to pointer to allocated buffer of 32bit chunks (one chunk is one pixel with four 8bit channels).
		 */
		void getPixelsOfCurrentTexture(bool useRectExtension, uint16 format, uint16 type, uint32** buffer);
		
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
		 * Sets a color value by calling glColor().
		 * @param theColor The color values to set.
		 */
		void setColor(const RGBAColor& theColor);
		
		/**
		 * Sets the line width for subsequent line drawing operations.
		 * @param width The requested width for subsequent line drawing operations.
		 */
		void setLineWidth(double width);
		
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
		 * @return A bitmap context. The caller has to issue a CGContextRelease() call on the context to release it.
		 */
		CGContextRef createBitmapContext(size_t pixelWidth, size_t pixelHeight);
#endif
		
		/**
		 * Returns an error string (if there was an error).
		 * @param[out] outErrorString Pointer to character buffer to receive the error string.
		 * @return Error number. 
		 */
		uint8 getOpenGLError(char* outErrorString);
		
		/**
		 * Sets the current context to the one that has been set up before with the setupContext() call.
		 * @return errNum: 0 if no error, > 0 error.
		 * @remarks The context must be set back to the one set up initially with each render action in case Cover Flow did change it in the meantime.
		 */
		uint16 setCurrentContext(void);
		
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
		 * @param canUseRectExtension True if graphics system does support rectangular texture extension. False otherwise.
		 * @param aBlendMode How to blend texture data with the framebuffer data. Default kBlend.
		 * @param debug If true the position numbers are displayed.
		 */
		void drawTexture(uint32 textureNumber, const VertexChain* const vertexChain, bool canUseRectExtension, BlendMode aBlendMode = kBlend, bool debug = false);		
		
		/**
		 * Draws a vertex chain. Useful for debugging, monitoring or diagnistic purposes.
		 * @param vertexChain A vector of vertices.
		 * @param drawMode The requested draw mode (kGL_POINTS, kGL_LINES, kGL_LINE_STRIP, kGL_LINE_LOOP, kGL_TRIANGLES, kGL_TRIANGLE_STRIP, kGL_TRIANGLE_FAN, kGL_QUADS, kGL_QUAD_STRIP, kGL_POLYGON).
		 * @param aBlendMode How to blend vertex chain drawing with the framebuffer data. Default kReplace.
		 */
		void drawVertexChain(const VertexChain& vertexChain, int drawMode = kGL_LINE_LOOP, BlendMode aBlendMode = kReplace);
		
		/**
		 * Draws debug information of a vertex chain. Useful for debugging, monitoring or diagnistic purposes.
		 * @param vertexChain A vector of vertices.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */
		void drawDebugVertexChain(const VertexChain& vertexChain, const VisualCamera& aCamera);
		
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
		void drawPixels(PixelColor** pixelData, double xCoord, double yCoord, uint32 width, uint32 height, uint16 format, uint16 dataType, const VisualConvolutionFilter* const aConvolutionFilter = NULL);
		
		/**
		 * Resamples (resizes) pixel data according to the passed-in dimensions.
		 * @param formatIn The format of the incoming pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param widthIn The width of the incoming pixels.
		 * @param heightIn The width of the incoming pixels.
		 * @param dataTypeIn The data type of the incoming pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 * @param pixelDataIn A pointer to the incoming pixel data.
		 * @param widthOut The requested width of the outgoing pixels.
		 * @param heightOut The requested height of the outgoing pixels.
		 * @param dataTypeOut The requested data type of the outgoing pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 * @param pixelDataOut A pointer to a pointer to the outgoing pixel data. The memory has to be allocated an freed by the caller.
		 */	
		void resample(uint16 formatIn, uint32 widthIn, uint32 heightIn, uint16 dataTypeIn, PixelColor* pixelDataIn, uint32 widthOut, uint32 heightOut, uint16 dataTypeOut, PixelColor** pixelDataOut);
		
		
		/**
		 * Reads image data from framebuffer into processor memory.
		 * @param xCoord The bottom left x coord.
		 * @param yCoord The bottom top y coord.
		 * @param width The width of the rectangle.
		 * @param height The height of the rectangle.
		 * @param[out] pixelData Pointer to pointer of buffer to write image data to.
		 * @param format The format of the requested pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param dataType The data type of the requested pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */	
		void readPixels(double xCoord, double yCoord, uint32 width, uint32 height, uint32** pixelData, uint16 format, uint16 dataType, const VisualCamera& aCamera);
		
#if TARGET_OS_WIN
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
		 * @param fontSize The size of the font in points. The point size denotes the cell height of the font.
		 * @param red The red color component of the text font.
		 * @param green The green color component of the text font.
		 * @param blue The blue color component of the text font.
		 * @param alignment The horizontal alignment of the string.
		 * @param maxPixelWidth The requested maximum width of the texture. If 0 the argument is ignored.
		 * @param maxPixelHeight The requested maximum height of the texture. If 0 the argument is ignored.
		 * @param On Windows it is common to measure the point font height excluding internal leading. On Mac the point font size is measured including internal leading.
		 * @return True on success, false on failure.
		 */
		bool makeTextureOfStringWin(wchar_t* stringValue, int stringValueLength, uint32 textureNumber, uint32 &textureWidth, uint32 &textureHeight, uint32 &imageWidth, uint32 &imageHeight, const char* const fontName, uint16 fontSize, uint8 red, uint8 green, uint8 blue, HorizontalAlignment alignment, uint32 maxPixelWidth = 0, uint32 maxPixelHeight = 0);
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
		void drawSpot(const double xNum, const double yNum, const double r, const double g, const double b, uint16 waveformIntensityVal, double intensity, const uint8 tailSize);
		
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
		 * Draws some metrics in the current scene.
		 * @remarks For diagnostical puposes.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */
		void drawProjectionMetrics(const VisualCamera& aCamera);
		
		/**
		 * Returns a point on the outline of a circle.
		 * @param sliceIdx The index of the requested slice.
		 * @param slicesCount The number of slices.
		 * @param radius The length of the radius. Default = 1.0.
		 * @param circleCenter The center of the circle. Default is x = 0.0 and y = 0.0.
		 * @remarks The center of the circle is at 0.0, 0.0. The number of slices determine how close the circle points are located to each other. The circle points start at the rightmost position in the middle of the circle (at 3 o'clock) and go in counter clockwise direction.
		 */	
		Coord getCirclePoint(uint32 sliceIdx, uint32 slicesCount, double radius = 1.0, Coord circleCenter = zeroCoord);
		
		/**
		 * Draws a histogram of the beat values.
		 * @param beatHistogram The beat histogram.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */
		void drawBeatHistogram(const uint32* const beatHistogram, const VisualCamera& aCamera);
		
		/**
		 * The function queries the system and answers whether textures can have non power of 2 dimensions.
		 * @return true if graphics system does support rectangular texture extension, false otherwise.
		 */	
		bool canUseTextureRectExtension(void);

		/**
		 * The function queries the system and answers whether convolution filter can be performed with OpenGL system.
		 * @return true if graphics system does support convolution filter, false otherwise.
		 */	
		bool doesSupportGLConvolutionFilter(void);
		
		/**
		 * Draws the waveform data.
		 * @param historyNum The index of the history the waveform data is taken from.
		 * @param maxNumberOfHistories Maximum number of histories stored in waveformDataMonoArray.
		 * @param numberOfWaveformEntries The number of waveform values.
		 * @param waveformDataMonoArray Pointer to waveform data of all histories. The array has two dimensions: history and index.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */		
		void drawWaveform(const sint16 historyNum, const uint16 maxNumberOfHistories, const uint32 numberOfWaveformEntries, sint16** waveformDataMonoArray, const VisualCamera& aCamera);
		
		/**
		 * Draws the data of a vector.
		 * @param aVector Reference to a vextor that holds the data.
		 * @param baseIdx The current start index.
		 * @param minVal The (possible) minumum value. The value is used for calculating the vertical dimension of the diagram.
		 * @param maxVal The (possible) maximum value. The value is used for calculating the vertical dimension of the diagram.
		 * @remarks The vector can act as a ringbuffer by letting baseIdx point to the current start of the vector.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */	
		static void drawHistoryDiagram(std::vector<double>& aVector, size_t baseIdx, double minVal, double maxVal, const VisualCamera& aCamera);
		
		/**
		 * Draws the waveform data in spiral form.
		 * @param currHistoryNum The current history of the waveform data.
		 * @param numberOfWaveformEntries The number of waveform values.
		 * @param waveformDataMonoArray Pointer to waveform data of all histories.
		 */
		void drawWaveformSpiral(const uint16 currHistoryNum, const uint16 numberOfWaveformEntries, const sint16* const waveformDataMonoArray);
		
		/**
		 * Draws a graphical representation of the spectral data with subband bar graphs.
		 * @param currHistoryNum The index of the history the spectrum data is taken from.
		 * @param numberOfHistories The maximum number of histories.
		 * @param numberOfSpectrumEntries The number of spectrum data values.
		 * @param numberOfAudioChannels The number of audio channels.
		 * @param spectrumDataArray Pointer to spectrum data of all histories.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */
		void drawSpectrumAnalyzer(const sint16 currHistoryNum, const uint16 numberOfHistories, const uint32 numberOfSpectrumEntries, const uint16 numberOfAudioChannels, const uint8*** const spectrumDataArray, const VisualCamera& aCamera);
		
		/**
		 * Draws a graphical representation of the spectral data as spectrogram.
		 * @param currHistoryNum The index of the history the spectrum data is taken from.
		 * @param numberOfHistories The maximum number of histories.
		 * @param numberOfSpectrumEntries The number of spectrum data values.
		 * @param numberOfAudioChannels The number of audio channels.
		 * @param spectrumDataArray Pointer to spectrum data of all histories.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */
		void drawSpectrogram(const sint16 currHistoryNum, const uint16 numberOfHistories, const uint32 numberOfSpectrumEntries, const uint16 numberOfAudioChannels, const uint8*** const spectrumDataArray, const VisualCamera& aCamera);
		
		/**
		 * Draws a circle representing the waveform data.
		 * @param xNum The horizontal position.
		 * @param yNum The vertical position.
		 * @param rotVal The degree of rotation.
		 * @param radius Radius.
		 */
		void drawWaveformCircle(const float xNum, const float yNum, const float rotVal, const float radius);
		
		/**
		 * Draws a 7bit char buffer string to screen.
		 * Mac uses GLUT_BITMAP_9_BY_15. Win uses Arial font created by wglUseFontBitmaps.
		 */
		void drawCStringWithGL(const char* const cString, const uint16 stringLength);
		
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
		uint32 getNextFreeTextureName(void);
		
		/**
		 * Frees memory bound by OpenGL textures.
		 * @param numberOfTextures The number of textures to free.
		 * @param textureNames Pointer to texture names (texture numbers) to free.
		 */
		void deleteTextures(const uint16 numberOfTextures, const uint32* const textureNames);
		
		/**
		 * Shows a row of Process Monitor Info.
		 * The graphical operations involved in putting a row of textual process info on screen is encapsulated within this function.
		 * @param coord The coord where to place the text (left start position).
		 * @param textRowStr Pointer to character buffer with process info string (label and value).
		 */
		void showProcessInfoRow(Coord coord, const char* const textRowStr);
		
		/**
		 * Shows a note about Process Monitor.
		 */	
		void showProcessInfoNote(void);

		/**
		 * Creates clipped pixel data.
		 * @param inPixelData Pixel data to be clipped.
		 * @param inPixelRect Dimensions of pixel data.
		 * @param clipRect Dimensions of the clip rect.
		 * @returns The clipped pixel data.
		 * @remarks The caller has to deallocate the returned pixel data with free().
		 */
		PixelColor* clipPixelData(const PixelColor* inPixelData, const PixelRect& inPixelRect, const TopLeftPositionedPixelRect& clipRect);

		/**
		 * Copies ARGB bitmap data into texture.
		 * @param textureNumber The texture number (texture name) of the generated texture.
		 * @param imageWidth The width of the bitmap data (width of texture might differ because of normalization to power-of-two value).
		 * @param imageHeight The height of the bitmap data (height of texture might differ because of normalization to power-of-two value).
		 * @param canUseRectExtension True if the texture does support rectangular texture extension. False otherwise.
		 * @param bitmapData Pointer to pointer to the bitmap data.
		 * @param debug If true, prebuilt pixel data is used.
		 * @return True on success, false on failure.
		 */
		bool copyARGBBitmapDataToTexture(uint32 textureNumber, uint32 imageWidth, uint32 imageHeight, bool canUseRectExtension, const PixelColor** bitmapData, bool debug = false);
		
#if TARGET_OS_WIN
		/**
		 * The function queries the system to determine if the font is installed. Windows-only.
		 * @param currHdc Pointer to the current device.
		 * @param fontName The name of the font.
		 * @return 1 if font is installed, 0 if font is not installed.
		 */
		//uint8 fontIsInstalled(HDC currHdc, const char* const fontName);
		
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
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */	
		double xPixelToCoord(uint32 pixelPos, const VisualCamera& aCamera);
		
		/**
		 * Converts a vertical pixel position to a vertical coordinate.
		 * @param pixelPos The canvas pixel position.
		 * @return The canvas coord position.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */		
		double yPixelToCoord(uint32 pixelPos, const VisualCamera& aCamera);
		
		/**
		 * Converts a horizontal coordinate to a horizontal pixel position.
		 * @param coordPos The canvas coord position.
		 * @return The canvas pixel position.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */	
		uint16 xCoordToPixel(double coordPos, const VisualCamera& aCamera);
		
		/**
		 * Converts a vertical coordinate to a horizontal pixel position.
		 * @param coordPos The canvas coord position.
		 * @return The canvas pixel position.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */	
		uint16 yCoordToPixel(double coordPos, const VisualCamera& aCamera);
		
		/**
		 * Maps window coordinates to object coordinates.
		 * @param xPixelPos The horizontal pixel position.
		 * @param yPixelPos The vertical pixel position.
		 * @param zAxisPos The position on the z-axis. 0: near clipping plane, 1: far clipping plane.
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
		 * @param identifier Identifier of the NURBS. The Nurbs can later be addressed by that token.
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
		VisualNurbs* createNurbsObject(const VisualItemIdentifier& identifier, uint8 sNumKnots, float* sKnots, uint8 tNumKnots, float* tKnots, uint16 sStride, uint16 tStride, uint16 sOrder, uint16 tOrder);
		
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
		 * @param canUseRectExtension True if graphics system does support rectangular texture extension. False otherwise. Default false.
		 * @param clipRect The dimensions and position of the clip rect.
		 * @param pixelFormat The format of the pixels (kGL_RGBA, kGL_LUMINANCE).
		 * @param dataType The data type of the pixels (kGL_UNSIGNED_BYTE, kGL_FLOAT).
		 * @remarks The pixels are copied from the framebuffer.
		 */	
		void copyFramebufferToTexture(uint32 textureNumber, bool canUseRectExtension, const BottomLeftPositionedPixelRect& clipRect, uint16 pixelFormat, uint16 dataType);
		
		/**
		 * Returns the nearest power of 2 integer following the input integer.
		 * @param n The input integer.
		 * @return A power of 2 integer.
		 */
		uint32 power2Ceiling(uint32 n);
		
		/*
		 #if TARGET_OS_WIN
		 static int CALLBACK EnumFontFamExProc2(
		 ENUMLOGFONTEX *lpelfe,    // logical-font data
		 NEWTEXTMETRICEX *lpntme,  // physical-font data
		 DWORD FontType,           // type of font
		 LPARAM lParam             // application-defined data
		 );
		 void resetFontVectorIterIndex(void);
		 void gatherFonts(void);
		 bool getNextFontName(char* fontName);
		 #endif
		 */
		
	private:
		
		/** The constructor. VisualGraphics is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualGraphics();
		
		/** The destructor. VisualGraphics is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
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
		
		/** The rectangle where the drawing takes place. */
		BottomLeftPositionedPixelRect canvasRect;
		
		/** The dimensions of the complete drawing surface which is reserved for visualizer action. */
		BottomLeftPositionedPixelRect surfaceRect;
		
		/** The vertical offset of the bottom-positioned surface rect. */
		uint32 surfaceRectVerticalOffsetFromBottom;
		
		/** True if set up on full screen, false if set up in windowed mode. */
		bool isSetUpOnFullscreen;
		
		/** The color of the canvas background. */
		RGBAColor canvasBackgroundColor;
		
		/** The color of the surface background. */
		RGBAColor surfaceBackgroundColor;
		
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
		
		/** A NurbsMap is a map of an identifier string and a pointer to a VisualNurbs. */
		typedef std::map<VisualItemIdentifier, VisualNurbs*> NurbsMap;
		
		/** The nurbsMap is the map of the VisualNurbs. */
		NurbsMap nurbsMap;
		
		/** A NurbsMapIterator is an iterator of a NurbsMap. */
		typedef NurbsMap::iterator NurbsMapIterator;
		
#if TARGET_OS_WIN
		ULONG_PTR gdiplusToken; /**< Token used for starting and terminating Gdiplus */
#endif
		
	};
	
}

#endif /* VisualGraphics_h */
