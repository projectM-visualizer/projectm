/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualActorGraphics.h
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

#ifndef VisualActorGraphics_h
#define VisualActorGraphics_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h" // RGBAColor


namespace VizKit {

	class VisualImage; // Forward declaration (to avoid include of header file).
	class VisualString; // Forward declaration (to avoid include of header file).
	class VisualCamera; // Forward declaration (to avoid include of header file).
	
	/**
	 * The graphics related static functions for the Visual Actors.
	 * By gathering the graphics related calls of the Visual Actors, it is easier to track and isolate possible problems.
	 * All functions of VisualActorGraphics are static.
	 * Most of the time, functions of VisualActorGraphics are executed by VisualGraphics.
	 * The static functions of VisualActorGraphics are expected to change more often than the methods of the VisualGraphics class.
	 * The static functions of VisualActorGraphics are aggregates of graphical operations.
	 */
	class VisualActorGraphics {

	public:

		/**
		 * Returns an error string (if there was an error).
		 * @param[out] errorString Pointer to character buffer to receive the error string.
		 * @return Error Number.
		 */
		static uint8 getError(char* errorString);
		
		/**
		 * Sets the value of the background color.
		 * @param aColorVal The RGBA values.
		 */	
		static void setBackgroundColor(const RGBAColor& aColorVal);

		/**
		 * Returns the value of the background color.
		 * @return The RGBA values of the background color.
		 */	
		static RGBAColor getBackgroundColor(void);

		/**
		 * Creates an image buffer with 8-bit per channel ARGB pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @return A pointer to pointer to pixel data.
		 */
		static uint32* createARGBCheckPixels(uint32 width, uint32 height);

		/**
		 * Creates an image buffer with 8-bit per channel BGRA pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @return A pointer to pointer to pixel data.
		 */
		static uint32* createBGRACheckPixels(uint32 width, uint32 height);

		/**
		 * Creates an image buffer with 8-bit per channel RGBA pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @return A pointer to pointer to pixel data.
		 */
		static uint32* createRGBACheckPixels(uint32 width, uint32 height);

		/**
		 * Creates an image buffer with 8-bit per channel ABGR pixel data.
		 * @param width The width of the bitmap data.
		 * @param height The height of the bitmap data.
		 * @return A pointer to pointer to pixel data.
		 */
		static uint32* createABGRCheckPixels(uint32 width, uint32 height);

		/**
		 * Returns a free texture name/index/number/id.
		 * @return A free texture name/index/number.
		 */
		static uint32 getNextFreeTextureName(void);

		/**
		 * Frees memory bound by OpenGL textures.
		 * @param numberOfTextures The number of textures to free.
		 * @param textureNames Pointer to texture names (texture numbers) to free.
		 */
		static void deleteTextures(const uint16 numberOfTextures, const uint32* const textureNames);
		
		/**
		 * Resets the model view matrix.
		 */
		static void resetModelViewMatrix(void);
		
		/**
		 * Enables alpha blending.
		 */		
		static void enableAlphaBlending(void);

		/**
		 * Disables blending.
		 */
		static void disableBlending(void);

		/**
		 * Maps window coordinates to object coordinates.
		 * @param xPixelPos The horizontal pixel position.
		 * @param yPixelPos The vertical pixel position.
		 * @param zAxisPos The position on the z-axis.
		 * @param xCoordPos Returns: The x-CoordPosition.
		 * @param yCoordPos Returns: The y-CoordPosition.
		 * @param zCoordPos Returns: The z-CoordPosition.
		 * @return True on success, false on error.
		 */	
		static bool getCoordsOfPixelPosition(
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
		static bool getPixelsOfCoordPosition(
										double xCoordPos,
										double yCoordPos,
										double zCoordPos,
										double* xPixelPos, 
										double* yPixelPos, 
										double* zAxisPos);

		/**
		 * Returns the width of the canvas in pixels.
		 * @return The width of the canvas in pixels.
		 */
		static uint32 getCanvasPixelWidth(void);

		/**
		 * Returns the height of the canvas in pixels.
		 * @return The height of the canvas in pixels.
		 */
		static uint32 getCanvasPixelHeight(void);
		
		/**
		 * Returns the current orientation and aspect ratio of the viewport.
		 * @return The current orientation and aspect ratio of the viewport.
		 */
		static RelationalRect getViewportOrientationAndAspectRatio(void);

		/**
		 * Converts a horizontal coordinate to a horizontal pixel position.
		 * @param coordPos The canvas coord position.
		 * @return The canvas pixel position.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */	
		static uint16 xCoordToPixel(double coordPos, const VisualCamera& aCamera);

		/**
		 * Converts a vertical coordinate to a horizontal pixel position.
		 * @param coordPos The canvas coord position.
		 * @return The canvas pixel position.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */	
		static uint16 yCoordToPixel(double coordPos, const VisualCamera& aCamera);

		/**
		 * Converts a horizontal pixel position to a horizontal coordinate.
		 * @param pixelPos The canvas pixel position.
		 * @return The canvas coord position.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */	
		static double xPixelToCoord(const uint16 pixelPos, const VisualCamera& aCamera);

		/**
		 * Converts a vertical pixel position to a vertical coordinate.
		 * @param pixelPos The canvas pixel position.
		 * @return The canvas coord position.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */		
		static double yPixelToCoord(const uint16 pixelPos, const VisualCamera& aCamera);

		/**
		 * Returns a point on the outline of a circle.
		 * @param sliceIdx The index of the requested slice.
		 * @param slicesCount The number of slices.
		 * @param radius The length of the radius. Default = 1.0.
		 * @param circleCenter The center of the circle. Default is x = 0.0 and y = 0.0.
		 * @remarks The center of the circle is at 0.0, 0.0. The number of slices determine how close the circle points are located to each other. The circle points start at the rightmost position in the middle of the circle (at 3 o'clock) and go in counter clockwise direction.
		 */	
		static Coord getCirclePoint(uint32 sliceIdx, uint32 slicesCount, double radius = 1.0, Coord circleCenter = zeroCoord);

		/**
		 * Draws a spot (for testing purpose).
		 * @param zPlane The z-Coord (optional parameter, default = 0.0).
		 */
		static void spotGL(double zPlane = 0.0);
		
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
		static void drawSpot(const double xNum, const double yNum, const double r, const double g, const double b, uint16 waveformIntensityVal, double intensity, const uint8 tailSize);

		/**
		 * Draws some metrics in the current scene.
		 * @remarks For diagnostical puposes.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */
		static void drawProjectionMetrics(const VisualCamera& aCamera);

		/**
		 * Loads the identity matrix of the model view. The model view transformation are reset
		 * @remarks It is assumed the model view is the currently active matrix mode.
		 */
		static void loadModelViewIdentityMatrix(void);

		/**
		 * Prepares the show of the Process Monitor.
		 * @param theColor The requested color.
		 */
		static void prepareProcessMonitorShow(RGBAColor& theColor);

		/**
		 * Shows a row of Process Monitor Info.
		 * The graphical operations involved in putting a row of textual process info on screen is encapsulated within this function.
		 * @param coord The coord where to place the text (left start position).
		 * @param textRowStr Pointer to character buffer with process info string (label and value).
		 */
		static void showProcessInfoRow(Coord coord, const char* const textRowStr);

		/**
		 * Shows a note about Process Monitor.
		 */	
		static void showProcessInfoNote(void);

		/**
		 * Draws a vertex chain. Useful for debugging, monitoring or diagnistic purposes.
		 * @param vertexChain A vector of vertices.
		 * @param drawMode The requested draw mode (kGL_POINTS, kGL_LINES, kGL_LINE_STRIP, kGL_LINE_LOOP, kGL_TRIANGLES, kGL_TRIANGLE_STRIP, kGL_TRIANGLE_FAN, kGL_QUADS, kGL_QUAD_STRIP, kGL_POLYGON).
		 * @param aBlendMode How to blend vertex chain drawing with the framebuffer data. Default kReplace.
		 */
		static void drawVertexChain(const VertexChain& vertexChain, int drawMode = kGL_LINE_LOOP, BlendMode aBlendMode = kReplace);

		/**
		 * Draws the waveform data.
		 * @param historyNum The index of the history the waveform data is taken from.
		 * @param maxNumberOfHistories Maximum number of histories stored in waveformDataMonoArray.
		 * @param numberOfWaveformEntries The number of waveform values.
		 * @param waveformDataMonoArray Pointer to waveform data of all histories. The array has two dimensions: history and index.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */		
		static void drawWaveform(const sint16 historyNum, const uint16 maxNumberOfHistories, const uint32 numberOfWaveformEntries, const sint16** const waveformDataMonoArray, const VisualCamera& aCamera);

		/**
		 * Draws a graphical representation of the spectral data with subband bar graphs.
		 * @param currHistoryNum The index of the history the spectrum data is taken from.
		 * @param numberOfHistories The maximum number of histories.
		 * @param numberOfSpectrumEntries The number of spectrum data values.
		 * @param numberOfAudioChannels The number of audio channels.
		 * @param spectrumDataArray Pointer to spectrum data of all histories.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */
		static void drawSpectrumAnalyzer(const sint16 currHistoryNum, const uint16 numberOfHistories, const uint32 numberOfSpectrumEntries, const uint16 numberOfAudioChannels, const uint8*** const spectrumDataArray, const VisualCamera& aCamera);

		/**
		 * Draws a graphical representation of the spectral data as spectrogram.
		 * @param currHistoryNum The index of the history the spectrum data is taken from.
		 * @param numberOfHistories The maximum number of histories.
		 * @param numberOfSpectrumEntries The number of spectrum data values.
		 * @param numberOfAudioChannels The number of audio channels.
		 * @param spectrumDataArray Pointer to spectrum data of all histories.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */
		static void drawSpectrogram(const sint16 currHistoryNum, const uint16 numberOfHistories, const uint32 numberOfSpectrumEntries, const uint16 numberOfAudioChannels, const uint8*** const spectrumDataArray, const VisualCamera& aCamera);

		/**
		 * Draws a histogram of the beat values.
		 * @param beatHistogram The beat histogram.
		 * @param aCamera Camera with dimensions, position, perspective projection, and orientation of stage view.
		 */
		static void drawBeatHistogram(const uint32* const beatHistogram, const VisualCamera& aCamera);

		/**
		 * Draws a progress bar that advances with the progress of the currently playing audio track.
		 * @param progressMeterBackgroundVertices The vertices of the background of the progress meter.
		 * @param progressMeterVertices The vertices of the actual progress meter.
		 * @param progressMeterOutlineVertices The vertices of the outline of the progress meter.
		 */
		static void drawTrackProgressMeter(ConstVertexChainRef const progressMeterBackgroundVertices, ConstVertexChainRef const progressMeterVertices, ConstVertexChainRef const progressMeterOutlineVertices);

		/**
		 * The default implementation of the show() method of the VisualActor interface class.
		 * @param visualActorName The name of the VisualActor.
		 */
		static void doFallbackActorShow(const char* const visualActorName);

		/**
		 * Call of glTranslate().
		 * @param xNum Horizontal coord value.
		 * @param yNum Vertical coord value.
		 * @param zNum Z-coord value.
		 */	
		static void translateMatrix(double xNum, double yNum, double zNum);

		/**
		 * Call of glRotate().
		 * @param angle The angle of the rotation.
		 * @param xAmount The amount of the rotation around the horizontal axis.
		 * @param yAmount The amount of the rotation around the vertical axis.
		 * @param zAmount The amount of the rotation around the z-axis.
		 */	
		static void rotateMatrix(double angle, double xAmount, double yAmount, double zAmount);

		/**
		 * Call of glScale().
		 * @param xFactor The scale factor in the horizontal dimension.
		 * @param yFactor The scale factor in the vertical dimension.
		 * @param zFactor The scale factor in the z-dimension.
		 */
		static void scaleMatrix(double xFactor, double yFactor, double zFactor);

		/**
		 * Enables GL_DEPTH_TEST.
		 */
		static void enableDepthTest(void);

		/**
		 * Disables GL_DEPTH_TEST.
		 */
		static void disableDepthTest(void);
		
		/**
		 * Enables GL_CULL_FACE.
		 */
		static void enableCullFace(void);

		/**
		 * Disables GL_CULL_FACE.
		 */
		static void disableCullFace(void);

		/**
		 * Specifies that front-facing facets can be culled.
		 */
		static void cullFaceFront(void);

		/**
		 * Specifies that back-facing facets can be culled.
		 */
		static void cullFaceBack(void);

	private:

		/**
		 * The constructor.
		 */
		VisualActorGraphics();
		
		/**
		 * The destructor.
		 */
		~VisualActorGraphics();
		
	};

}

#endif /* VisualActorGraphics_h */
