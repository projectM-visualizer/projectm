/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualActorGraphics.cpp
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

#include "VisualActorGraphics.h"
#include "VisualDataStore.h"
#include "VisualImage.h"
#include "VisualGraphics.h"
#include "VisualActorState.h"
#include "VisualString.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"



using namespace VizKit;


VisualActorGraphics::VisualActorGraphics() {
	// null
}


VisualActorGraphics::~VisualActorGraphics() {
}


uint8 VisualActorGraphics::getError(char* errorString) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getOpenGLError(errorString);
}


void VisualActorGraphics::setBackgroundColor(const RGBAColor& aColorVal) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setCanvasBackgroundColor(aColorVal);
}


RGBAColor VisualActorGraphics::getBackgroundColor() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getBackgroundColor();
}


uint32 VisualActorGraphics::getNextFreeTextureName() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getNextFreeTextureName();
}


void VisualActorGraphics::deleteTextures(const uint16 numberOfTextures, const uint32* const textureNames) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->deleteTextures(numberOfTextures, textureNames);
}


void VisualActorGraphics::resetModelViewMatrix() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->resetModelViewMatrix();
}


void VisualActorGraphics::enableAlphaBlending() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->enableAlphaBlending();
}


void VisualActorGraphics::disableBlending() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->disableBlending();
}


bool VisualActorGraphics::getCoordsOfPixelPosition(
									double xPixelPos, 
									double yPixelPos, 
									double zAxisPos, 
									double* xCoordPos,
									double* yCoordPos,
									double* zCoordPos) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	Pixel bottomLeftViewportOrigin = theVisualGraphics->getViewportBottomLeftOrigin();
	return theVisualGraphics->getCoordsOfPixelPosition(xPixelPos + bottomLeftViewportOrigin.x, yPixelPos + bottomLeftViewportOrigin.y, zAxisPos, xCoordPos, yCoordPos, zCoordPos);
}


bool VisualActorGraphics::getPixelsOfCoordPosition(
								double xCoordPos,
								double yCoordPos,
								double zCoordPos,
								double* xPixelPos, 
								double* yPixelPos, 
								double* zAxisPos) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getPixelsOfCoordPosition(xCoordPos, yCoordPos, zCoordPos, xPixelPos, yPixelPos, zAxisPos);
}


uint32 VisualActorGraphics::getCanvasPixelWidth() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getCanvasPixelWidth();
}


uint32 VisualActorGraphics::getCanvasPixelHeight() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getCanvasPixelHeight();
}


RelationalRect VisualActorGraphics::getViewportOrientationAndAspectRatio() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getViewportOrientationAndAspectRatio();
}


uint16 VisualActorGraphics::xCoordToPixel(double coordPos, const VisualCamera& aCamera) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->xCoordToPixel(coordPos, aCamera);
}


uint16 VisualActorGraphics::yCoordToPixel(double coordPos, const VisualCamera& aCamera) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->yCoordToPixel(coordPos, aCamera);
}


double VisualActorGraphics::xPixelToCoord(const uint16 pixelPos, const VisualCamera& aCamera) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->xPixelToCoord(pixelPos, aCamera);
}


double VisualActorGraphics::yPixelToCoord(const uint16 pixelPos, const VisualCamera& aCamera) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->yPixelToCoord(pixelPos, aCamera);
}


Coord VisualActorGraphics::getCirclePoint(uint32 sliceIdx, uint32 slicesCount, double radius, Coord circleCenter) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getCirclePoint(sliceIdx, slicesCount, radius, circleCenter);
}


void VisualActorGraphics::spotGL(double zPlane) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->spotGL(zPlane);
}


void VisualActorGraphics::drawSpot(const double xNum, const double yNum, const double r, const double g, const double b, uint16 waveformIntensityVal, double intensity, const uint8 tailSize) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawSpot(xNum, yNum, r, g, b, waveformIntensityVal, intensity, tailSize);
}


void VisualActorGraphics::drawProjectionMetrics(const VisualCamera& aCamera) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawProjectionMetrics(aCamera);
}


void VisualActorGraphics::loadModelViewIdentityMatrix() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->loadModelViewIdentityMatrix();
}


void VisualActorGraphics::prepareProcessMonitorShow(RGBAColor& theColor) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setColor(theColor);
}


void VisualActorGraphics::showProcessInfoRow(Coord coord, const char* const textRowStr) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->showProcessInfoRow(coord, textRowStr);
}


void VisualActorGraphics::showProcessInfoNote() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->showProcessInfoNote();
}


void VisualActorGraphics::drawVertexChain(const VertexChain& vertexChain, int drawMode, BlendMode aBlendMode) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawVertexChain(vertexChain, drawMode, aBlendMode);
}


void VisualActorGraphics::drawWaveform(const sint16 historyNum, const uint16 maxNumberOfHistories, const uint32 numberOfWaveformEntries, const sint16** const waveformDataMonoArray, const VisualCamera& aCamera) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawWaveform(historyNum, maxNumberOfHistories, numberOfWaveformEntries, const_cast<sint16**>(waveformDataMonoArray), aCamera);
}


void VisualActorGraphics::drawSpectrumAnalyzer(const sint16 currHistoryNum, const uint16 numberOfHistories, const uint32 numberOfSpectrumEntries, const uint16 numberOfAudioChannels, const uint8*** const spectrumDataArray, const VisualCamera& aCamera) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawSpectrumAnalyzer(currHistoryNum, numberOfHistories, numberOfSpectrumEntries, numberOfAudioChannels, spectrumDataArray, aCamera);
}


void VisualActorGraphics::drawSpectrogram(const sint16 currHistoryNum, const uint16 numberOfHistories, const uint32 numberOfSpectrumEntries, const uint16 numberOfAudioChannels, const uint8*** const spectrumDataArray, const VisualCamera& aCamera) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawSpectrogram(currHistoryNum, numberOfHistories, numberOfSpectrumEntries, numberOfAudioChannels, spectrumDataArray, aCamera);
}


void VisualActorGraphics::drawBeatHistogram(const uint32* const beatHistogram, const VisualCamera& aCamera) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawBeatHistogram(beatHistogram, aCamera);
}


void VisualActorGraphics::drawTrackProgressMeter(ConstVertexChainRef const progressMeterBackgroundVertices, ConstVertexChainRef const progressMeterVertices, ConstVertexChainRef const progressMeterOutlineVertices) {
	
	RGBAColor verticesColor;
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	RGBAColor backgroundColor = theVisualGraphics->getBackgroundColor();
	theVisualGraphics->setColor(backgroundColor);
	
	theVisualGraphics->drawVertexChain(*progressMeterBackgroundVertices, kGL_QUADS);
	
	verticesColor.r = 0.8;
	verticesColor.g = 0.8;
	verticesColor.b = 0.8;
	verticesColor.a = 1.0;
	theVisualGraphics->setColor(verticesColor);

	theVisualGraphics->drawVertexChain(*progressMeterVertices, kGL_QUADS);

	verticesColor.r = 1.0;
	verticesColor.g = 1.0;
	verticesColor.b = 1.0;
	verticesColor.a = 1.0;
	theVisualGraphics->setColor(verticesColor);
	theVisualGraphics->setLineWidth(1.0);
	
	theVisualGraphics->drawVertexChain(*progressMeterOutlineVertices, kGL_LINE_LOOP);

}


void VisualActorGraphics::doFallbackActorShow(const char* const visualActorName) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->doFallbackActorShow(visualActorName);
}


void VisualActorGraphics::translateMatrix(double xNum, double yNum, double zNum) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->translateMatrix(xNum, yNum, zNum);
}


void VisualActorGraphics::rotateMatrix(double angle, double xAmount, double yAmount, double zAmount) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->rotateMatrix(angle, xAmount, yAmount, zAmount);
}


void VisualActorGraphics::scaleMatrix(double xFactor, double yFactor, double zFactor) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->scaleMatrix(xFactor, yFactor, zFactor);
}


void VisualActorGraphics::enableDepthTest() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->enableDepthTest();
}


void VisualActorGraphics::disableDepthTest() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->disableDepthTest();
}


void VisualActorGraphics::enableCullFace() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->enableCullFace();
}


void VisualActorGraphics::disableCullFace() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->disableCullFace();
}


void VisualActorGraphics::cullFaceFront() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->cullFaceFront();
}


void VisualActorGraphics::cullFaceBack() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->cullFaceBack();
}
