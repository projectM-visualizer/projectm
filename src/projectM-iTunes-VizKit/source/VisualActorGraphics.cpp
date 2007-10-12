/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualActorGraphics.cpp
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

#include "VisualActorGraphics.h"
#include "VisualDataStore.h"
#include "VisualImage.h"
#include "VisualGraphics.h"
#include "VisualActorState.h"
#include "VisualString.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"

#if TARGET_OS_WIN
#include <QT/macmemory.h> // HLock
#endif



using namespace VizKit;


VisualActorGraphics::VisualActorGraphics() {
	// null
}


VisualActorGraphics::~VisualActorGraphics() {
}


UInt8 VisualActorGraphics::getError(char* errorString) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getOpenGLError(errorString);
}


void VisualActorGraphics::setBackgroundColor(const RGBAColor& aColorVal) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setBackgroundColor(aColorVal);
}


RGBAColor VisualActorGraphics::getBackgroundColor() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getBackgroundColor();
}


VisualImage* VisualActorGraphics::createCoverArtImage() {

	Handle coverArtDataHdl = VisualDataStore::getHandleOfCoverArt();
	HLock(coverArtDataHdl);
	
	OSType coverArtFileType = VisualDataStore::getCoverArtFileType();

	char coverArtFileTypeCStr[5];
	VisualDataStore::OSTypeToString(coverArtFileType, coverArtFileTypeCStr);
	setProcessInfo("CoverArtFileType", coverArtFileTypeCStr);
	
	VisualImage* anImage = new VisualImage;
	OSStatus osStatus = anImage->initWithDataHandle(coverArtDataHdl, coverArtFileType);

	HUnlock(coverArtDataHdl);
	VisualDataStore::disposeHandleOfCoverArt();
	
	if (osStatus != noErr) {
		return NULL;
	} else if (anImage->isEmpty()) {
		delete anImage;
		return NULL;
	}
	
	return anImage;
}


void VisualActorGraphics::releaseCoverArtImage(VisualImage** coverArtImage) {
	delete *coverArtImage;
	*coverArtImage = NULL;
}


void VisualActorGraphics::createCheckTexture(UInt32& textureNumber, UInt32& textureWidth, UInt32& textureHeight, UInt32& imageWidth, UInt32& imageHeight) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->createCheckTexture(textureNumber, textureWidth, textureHeight, imageWidth, imageHeight);
}


UInt32 VisualActorGraphics::getNextFreeTextureName() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getNextFreeTextureName();
}


void VisualActorGraphics::deleteTextures(const UInt16 numberOfTextures, const UInt32* const textureNames) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->deleteTextures(numberOfTextures, textureNames);
}


void VisualActorGraphics::drawPerspectiveSquare(UInt32 textureNumber) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawPerspectiveRect(textureNumber);
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
	return theVisualGraphics->getCoordsOfPixelPosition(xPixelPos + theVisualGraphics->getCanvasXOriginOffset(), yPixelPos + theVisualGraphics->getCanvasYOriginOffset(), zAxisPos, xCoordPos, yCoordPos, zCoordPos);
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


UInt16 VisualActorGraphics::getCanvasPixelWidth() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getCanvasPixelWidth();
}


UInt16 VisualActorGraphics::getCanvasPixelHeight() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getCanvasPixelHeight();
}


double VisualActorGraphics::getCanvasCoordWidth() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getCanvasCoordWidth();
}


double VisualActorGraphics::getCanvasCoordHeight() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getCanvasCoordHeight();
}
	

double VisualActorGraphics::getMaxTopCoordOfCanvas() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getMaxTopCoordOfCanvas();
}


double VisualActorGraphics::getMaxLeftCoordOfCanvas() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getMaxLeftCoordOfCanvas();
}


double VisualActorGraphics::getMaxBottomCoordOfCanvas() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getMaxBottomCoordOfCanvas();
}


double VisualActorGraphics::getMaxRightCoordOfCanvas() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getMaxRightCoordOfCanvas();
}


double VisualActorGraphics::getMaxNearCoordOfCanvas() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getMaxNearCoordOfCanvas();
}


double VisualActorGraphics::getMaxFarCoordOfCanvas() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->getMaxFarCoordOfCanvas();
}


UInt16 VisualActorGraphics::xCoordToPixel(double coordPos) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->xCoordToPixel(coordPos);
}


UInt16 VisualActorGraphics::yCoordToPixel(double coordPos) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->yCoordToPixel(coordPos);
}


double VisualActorGraphics::xPixelToCoord(const UInt16 pixelPos) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->xPixelToCoord(pixelPos);
}


double VisualActorGraphics::yPixelToCoord(const UInt16 pixelPos) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	return theVisualGraphics->yPixelToCoord(pixelPos);
}


void VisualActorGraphics::spotGL(double zPlane) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->spotGL(zPlane);
}


void VisualActorGraphics::drawSpot(const float xNum, const float yNum, const float r, const float g, const float b, UInt16 waveformIntensityVal, float intensity, const UInt8 tailSize) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawSpot(xNum, yNum, r, g, b, waveformIntensityVal, intensity, tailSize);
}


void VisualActorGraphics::setOrthographicProjection() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setOrthographicProjection();
}


void VisualActorGraphics::setPerspectiveProjection(double maxNearCoord) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setPerspectiveProjection(maxNearCoord);
}


void VisualActorGraphics::prepareProcessMonitorShow(RGBAColor& theColor) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setColor(theColor);
}


void VisualActorGraphics::showProcessInfoRow(double xNum, double yNum, const char* const textRowStr) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->showProcessInfoRow(xNum, yNum, textRowStr);
}


void VisualActorGraphics::showProcessInfoNote() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->showProcessInfoNote();
}


void VisualActorGraphics::drawWaveform(const SInt16 historyNum, const UInt16 maxNumberOfHistories, const UInt32 numberOfWaveformEntries, const SInt16** const waveformDataMonoArray) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawWaveform(historyNum, maxNumberOfHistories, numberOfWaveformEntries, const_cast<SInt16**>(waveformDataMonoArray));
}


void VisualActorGraphics::drawSpectrumAnalyzer(const SInt16 currHistoryNum, const UInt16 numberOfHistories, const UInt32 numberOfSpectrumEntries, const UInt16 numberOfAudioChannels, const UInt8*** const spectrumDataArray) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawSpectrumAnalyzer(currHistoryNum, numberOfHistories, numberOfSpectrumEntries, numberOfAudioChannels, spectrumDataArray);
}


void VisualActorGraphics::drawSpectrogram(const SInt16 currHistoryNum, const UInt16 numberOfHistories, const UInt32 numberOfSpectrumEntries, const UInt16 numberOfAudioChannels, const UInt8*** const spectrumDataArray) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawSpectrogram(currHistoryNum, numberOfHistories, numberOfSpectrumEntries, numberOfAudioChannels, spectrumDataArray);
}


void VisualActorGraphics::drawBeatHistogram(const UInt32* const beatHistogram) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawBeatHistogram(beatHistogram);
}


void VisualActorGraphics::drawTrackProgressMeter(ConstVertexChainRef const progressMeterBackgroundVertices, ConstVertexChainRef const progressMeterVertices, ConstVertexChainRef const progressMeterOutlineVertices) {
	
	RGBAColor verticesColor;
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	RGBAColor backgroundColor = theVisualGraphics->getBackgroundColor();
	theVisualGraphics->setColor(backgroundColor);
	
	theVisualGraphics->drawColorlessGLPrimitive(kGL_QUADS, progressMeterBackgroundVertices);
	
	verticesColor.r = 0.8f;
	verticesColor.g = 0.8f;
	verticesColor.b = 0.8f;
	verticesColor.a = 1.0f;
	theVisualGraphics->setColor(verticesColor);
	
	theVisualGraphics->drawColorlessGLPrimitive(kGL_QUADS, progressMeterVertices);

	verticesColor.r = 0.0f;
	verticesColor.g = 0.0f;
	verticesColor.b = 0.0f;
	verticesColor.a = 1.0f;
	theVisualGraphics->setColor(verticesColor);
	
	theVisualGraphics->drawColorlessGLPrimitive(kGL_LINE_LOOP, progressMeterOutlineVertices);
}


void VisualActorGraphics::doFallbackActorShow(const char* const visualActorName) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->doFallbackActorShow(visualActorName);
}


void VisualActorGraphics::prepareCoverArtAction() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	VisualActorGraphics::setPerspectiveProjection(3.0f);
	theVisualGraphics->enableDepthTest();
	theVisualGraphics->translateMatrix(0.0, 0.0, (theVisualGraphics->getMaxFarCoordOfCanvas() - theVisualGraphics->getMaxNearCoordOfCanvas()) / 2.0 * -1.0 - theVisualGraphics->getMaxNearCoordOfCanvas());
}


void VisualActorGraphics::finishCoverArtAction() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	theVisualGraphics->disableDepthTest();
	VisualActorGraphics::setOrthographicProjection();
}


void VisualActorGraphics::scaleMatrix(float xFactor, float yFactor, float zFactor) {
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
