/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualActorGraphics.h
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

#ifndef VisualActorGraphics_h
#define VisualActorGraphics_h

#include "VisualGraphicTypes.h" // RGBAColor

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class VisualImage; // Forward declaration (to avoid include of header file).
	class VisualString; // Forward declaration (to avoid include of header file).
	
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
		static UInt8 getError(char* errorString);
		
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
		 * Creates an image of the cover.
		 * @return The image of the album cover artwork.\ Returns NULL if cover image is not available.
		 * @remarks The caller has to release the allocated memory by calling VisualActorGraphics::releaseCoverArtImage().
		 */
		static VisualImage* createCoverArtImage(void);

		/**
		 * Releases the allocated memory of the coverArtImage.
		 * @param coverArtImage The image of the album cover artwork.
		 */
		static void releaseCoverArtImage(VisualImage** coverArtImage);
		
		/**
		 * Creates a texture of squares. Useful for testing purposes.
		 * @param[out] textureNumber The texture number (texture name) of the generated texture.
		 * @param[out] textureWidth The width of the generated texture.
		 * @param[out] textureHeight The height of the generated texture.
		 * @param[out] imageWidth The width of the image data.
		 * @param[out] imageHeight The height of the image data.
		 */
		static void createCheckTexture(UInt32 &textureNumber, UInt32 &textureWidth, UInt32 &textureHeight, UInt32 &imageWidth, UInt32 &imageHeight);

		/**
		 * Returns a free texture name/index/number/id.
		 * @return A free texture name/index/number.
		 */
		static UInt32 getNextFreeTextureName(void);

		/**
		 * Frees memory bound by OpenGL textures.
		 * @param numberOfTextures The number of textures to free.
		 * @param textureNames Pointer to texture names (texture numbers) to free.
		 */
		static void deleteTextures(const UInt16 numberOfTextures, const UInt32* const textureNames);

		/**
		 * Draws a perspective square.
		 * Used by template actor.
		 * @param textureNumber The number/id/name of the texture (if any).
		 */	
		static void drawPerspectiveSquare(UInt32 textureNumber = NULL);
		
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
		static UInt16 getCanvasPixelWidth(void);

		/**
		 * Returns the height of the canvas in pixels.
		 * @return The height of the canvas in pixels.
		 */
		static UInt16 getCanvasPixelHeight(void);
		
		/**
		 * Returns the width of the canvas in coords.
		 * @return The width of the canvas in coords.
		 */	
		static double getCanvasCoordWidth(void);

		/**
		 * Returns the height of the canvas in coords.
		 * @return The height of the canvas in coords.
		 */	
		static double getCanvasCoordHeight(void);

		/**
		 * Returns the maximum top position.
		 * @return The maximum top position.
		 */
		static double getMaxTopCoordOfCanvas(void);

		/**
		 * Returns the maximum left position.
		 * @return The maximum left position.
		 */
		static double getMaxLeftCoordOfCanvas(void);

		/**
		 * Returns the maximum bottom position.
		 * @return The maximum bottom position.
		 */
		static double getMaxBottomCoordOfCanvas(void);

		/**
		 * Returns the maximum right position.
		 * @return The maximum right position.
		 */
		static double getMaxRightCoordOfCanvas(void);

		/**
		 * Returns the maximum near position.
		 * @return The maximum near position.
		 */
		static double getMaxNearCoordOfCanvas(void);

		/**
		 * Returns the maximum far position.
		 * @return The maximum far position.
		 */
		static double getMaxFarCoordOfCanvas(void);

		/**
		 * Converts a horizontal coordinate to a horizontal pixel position.
		 * @param coordPos The canvas coord position.
		 * @return The canvas pixel position.
		 */	
		static UInt16 xCoordToPixel(double coordPos);

		/**
		 * Converts a vertical coordinate to a horizontal pixel position.
		 * @param coordPos The canvas coord position.
		 * @return The canvas pixel position.
		 */	
		static UInt16 yCoordToPixel(double coordPos);

		/**
		 * Converts a horizontal pixel position to a horizontal coordinate.
		 * @param pixelPos The canvas pixel position.
		 * @return The canvas coord position.
		 */	
		static double xPixelToCoord(const UInt16 pixelPos);

		/**
		 * Converts a vertical pixel position to a vertical coordinate.
		 * @param pixelPos The canvas pixel position.
		 * @return The canvas coord position.
		 */		
		static double yPixelToCoord(const UInt16 pixelPos);

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
		static void drawSpot(const float xNum, const float yNum, const float r, const float g, const float b, UInt16 waveformIntensityVal, float intensity, const UInt8 tailSize);

		/**
		 * Prepares the action of the cover art by setting up perspective projection and enabling depth testing.
		 */	
		static void prepareCoverArtAction(void);

		/**
		 * Finishes the action of the cover art.
		 */			
		static void finishCoverArtAction(void);

		/**
		 * Sets up a orthographic projection.
		 */	
		static void setOrthographicProjection(void);

		/**
		 * Sets up a perspective projection.
		 * @param maxNearCoord The maximum near position.\ With the maxNearCoord the position of the near clipping plane of the viewing volume is set.\ By setting maxNearCoord the perspective projection (the shape of the viewing volume) can be adjusted.\ The value of maxNearCoord must be positive.
		 */	
		static void setPerspectiveProjection(double maxNearCoord);

		/**
		 * Prepares the show of the Process Monitor.
		 * @param theColor The requested color.
		 */
		static void prepareProcessMonitorShow(RGBAColor& theColor);

		/**
		 * Shows a row of Process Monitor Info.
		 * The graphical operations involved in putting a row of textual process info on screen is encapsulated within this function.
		 * @param xNum Horizontal position.
		 * @param yNum Vertical position.
		 * @param textRowStr Pointer to character buffer with process info string (label and value).
		 */
		static void showProcessInfoRow(double xNum, double yNum, const char* const textRowStr);

		/**
		 * Shows a note about Process Monitor.
		 */	
		static void showProcessInfoNote(void);

		/**
		 * Draws the waveform data.
		 * @param historyNum The index of the history the waveform data is taken from.
		 * @param maxNumberOfHistories Maximum number of histories stored in waveformDataMonoArray.
		 * @param numberOfWaveformEntries The number of waveform values.
		 * @param waveformDataMonoArray Pointer to waveform data of all histories. The array has two dimensions: history and index.
		 */		
		static void drawWaveform(const SInt16 historyNum, const UInt16 maxNumberOfHistories, const UInt32 numberOfWaveformEntries, const SInt16** const waveformDataMonoArray);

		/**
		 * Draws a graphical representation of the spectral data with subband bar graphs.
		 * @param currHistoryNum The index of the history the spectrum data is taken from.
		 * @param numberOfHistories The maximum number of histories.
		 * @param numberOfSpectrumEntries The number of spectrum data values.
		 * @param numberOfAudioChannels The number of audio channels.
		 * @param spectrumDataArray Pointer to spectrum data of all histories.
		 */
		static void drawSpectrumAnalyzer(const SInt16 currHistoryNum, const UInt16 numberOfHistories, const UInt32 numberOfSpectrumEntries, const UInt16 numberOfAudioChannels, const UInt8*** const spectrumDataArray);

		/**
		 * Draws a graphical representation of the spectral data as spectrogram.
		 * @param currHistoryNum The index of the history the spectrum data is taken from.
		 * @param numberOfHistories The maximum number of histories.
		 * @param numberOfSpectrumEntries The number of spectrum data values.
		 * @param numberOfAudioChannels The number of audio channels.
		 * @param spectrumDataArray Pointer to spectrum data of all histories.
		 */
		static void drawSpectrogram(const SInt16 currHistoryNum, const UInt16 numberOfHistories, const UInt32 numberOfSpectrumEntries, const UInt16 numberOfAudioChannels, const UInt8*** const spectrumDataArray);

		/**
		 * Draws a histogram of the beat values.
		 * @param beatHistogram The beat histogram.
		 */
		static void drawBeatHistogram(const UInt32* const beatHistogram);

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
		 * Call of glScalef().
		 * @param xFactor The scale factor in the horizontal dimension.
		 * @param yFactor The scale factor in the vertical dimension.
		 * @param zFactor The scale factor in the z-dimension.
		 */
		static void scaleMatrix(float xFactor, float yFactor, float zFactor);

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
