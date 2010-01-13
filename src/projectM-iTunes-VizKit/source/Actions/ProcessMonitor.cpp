/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: ProcessMonitor.cpp
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

#include "ProcessMonitor.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"
#include "VisualAudioLab.h"
#include "VisualGraphicTypes.h"
#include "VisualVertex.h"
#include "VisualAsset.h"
#include "VisualCamera.h"

using namespace VizKit;


ProcessMonitor::ProcessMonitor() {
	processMonitorAsset = new VisualAsset;
	setupProgressMeterVertices();
}


ProcessMonitor::~ProcessMonitor() {
    cleanupProgressMeterVertices();
	delete processMonitorAsset;
}


void ProcessMonitor::prepareProcessMonitorShow() {
	VisualCamera aCamera;
	aCamera.setOrthographicProjection();
	processMonitorAsset->setCamera(aCamera);
	RGBAColor theColor;
	theColor.r = 0.0;
	theColor.g = 1.0;
	theColor.b = 0.0;
	theColor.a = 1.0;
	VisualActorGraphics::prepareProcessMonitorShow(theColor);
}


void ProcessMonitor::showInfoStrings() {

	ProcessInfoMapIterator it;

    char finalTextString[255];
    double yNum;
    double xNum;
	uint8 charCountMax = 0; // in one line per column
	double rowHeight, characterWidth;
	
	VisualCamera aCamera;
	aCamera.setOrthographicProjection();
	CoordSize3D size = aCamera.getSize();
	rowHeight = size.height * (15.0f / VisualActorGraphics::getCanvasPixelHeight());
	characterWidth = size.width * (9.0f / VisualActorGraphics::getCanvasPixelWidth());

	xNum = aCamera.getMaxLeftCoord() + characterWidth;
	yNum = aCamera.getMaxTopCoord();
	
	Coord coord;

	for (it = this->processInfoMap.begin(); it != this->processInfoMap.end(); it++) {
	
		yNum = yNum - rowHeight;
		if ((yNum - rowHeight) < aCamera.getMaxBottomCoord()) {
			// we reached the bottom -> go to next column to the right
			yNum = aCamera.getMaxTopCoord() - rowHeight;
			xNum = xNum + characterWidth + (charCountMax * characterWidth);
			charCountMax = 0;
		}
		strcpy (finalTextString, (*it).first.c_str());
		strcat (finalTextString, ": ");
		strcat (finalTextString, (*it).second.c_str());
		if (strlen(finalTextString) > charCountMax) {
			charCountMax = strlen(finalTextString);
		}
		coord.x = xNum;
		coord.y = yNum;
		coord.z = 0.0;
		VisualActorGraphics::showProcessInfoRow(coord, finalTextString); // constant height of font
		
    }
	
	VisualActorGraphics::showProcessInfoNote();
    
}


void ProcessMonitor::showAudioInfo(const uint32 elapsedAudioTime, const uint32 remainingAudioTime) {

	VisualAudioLab* theVisualAudioLab;

	uint16 numberOfAudioDataChannels;
	uint32 numberOfSpectrumEntries;
	uint16 maxNumberOfMusicDataHistories;

	const sint16** waveformDataMonoArray;
	sint16 currMusicDataHistory;
	uint16 prevMusicDataHistory;
	uint32 numberOfWaveformEntries;

	const uint8*** spectrumDataArray;
	//const uint32* beatHistogram;

	theVisualAudioLab = VisualAudioLab::getInstance();

	currMusicDataHistory = theVisualAudioLab->getCurrMusicDataHistory();
	prevMusicDataHistory = theVisualAudioLab->getPrevMusicDataHistory();
	numberOfWaveformEntries = theVisualAudioLab->getNumberOfWaveformEntries();
	maxNumberOfMusicDataHistories = theVisualAudioLab->getMaxNumberOfMusicDataHistories();
	waveformDataMonoArray = theVisualAudioLab->getWaveformDataMonoArray();
	
	numberOfAudioDataChannels = theVisualAudioLab->getNumberOfDataChannels();
	numberOfSpectrumEntries = theVisualAudioLab->getNumberOfSpectrumEntries();
	spectrumDataArray = theVisualAudioLab->getSpectrumDataArray();

    VisualActorGraphics::drawWaveform(currMusicDataHistory, maxNumberOfMusicDataHistories, numberOfWaveformEntries, waveformDataMonoArray, processMonitorAsset->getCamera());
	VisualActorGraphics::resetModelViewMatrix();

	//theVisualGraphics->drawWaveform(currMusicDataHistory, 1, numberOfWaveformEntries, waveformDataMonoArray);
	VisualActorGraphics::drawSpectrumAnalyzer(currMusicDataHistory, maxNumberOfMusicDataHistories, numberOfSpectrumEntries, numberOfAudioDataChannels, spectrumDataArray, processMonitorAsset->getCamera());
	VisualActorGraphics::resetModelViewMatrix();
	VisualActorGraphics::drawSpectrogram(currMusicDataHistory, maxNumberOfMusicDataHistories, numberOfSpectrumEntries, numberOfAudioDataChannels, spectrumDataArray, processMonitorAsset->getCamera());
    VisualActorGraphics::resetModelViewMatrix();

	//theVisualGraphics->drawWaveformSpiral(currMusicDataHistory, numberOfWaveformEntries, waveformDataMonoArray);
	//beatHistogram = theVisualAudioLab->getBeatHistogram();
	//VisualActorGraphics::drawBeatHistogram(beatHistogram, processMonitorAsset->getCamera());
	//VisualActorGraphics::drawTrackProgressMeter(elapsedAudioTime, remainingAudioTime);
	
	//this->updateProgressMeterVertices();
	this->calcTrackProgressMeterVertices(elapsedAudioTime, remainingAudioTime);
	VisualActorGraphics::drawTrackProgressMeter(&this->progressMeterBackgroundVertices, &this->progressMeterVertices, &this->progressMeterOutlineVertices);
    
}


void ProcessMonitor::registerProcessMonitorInfoMap(const std::map<std::string, std::string>* const processMonitorInfoMap) {
	this->processInfoMap = *processMonitorInfoMap;
}


void ProcessMonitor::finishProcessMonitorShow() {
	// null
}


void ProcessMonitor::updateProgressMeterVertices() {

	double topPos, leftPos, bottomPos, rightPos, rightProgressPos;
	
	VisualCamera aCamera;
	aCamera.setOrthographicProjection();
	topPos = aCamera.getMaxBottomCoord() + VisualActorGraphics::yPixelToCoord(10, aCamera);
	leftPos = aCamera.getMaxLeftCoord();
	bottomPos = aCamera.getMaxBottomCoord();
	rightPos = aCamera.getMaxRightCoord();
	rightProgressPos = aCamera.getMaxLeftCoord();
	RGBAColor backgroundColor = VisualActorGraphics::getBackgroundColor();
	
	progressMeterBackgroundVertices[0]->vertexPosition.coord.x = leftPos;
	progressMeterBackgroundVertices[0]->vertexPosition.coord.y = bottomPos;
	progressMeterBackgroundVertices[0]->vertexPosition.coord.z = 0.0f;
	progressMeterBackgroundVertices[0]->vertexColor.r = backgroundColor.r;
	progressMeterBackgroundVertices[0]->vertexColor.g = backgroundColor.g;
	progressMeterBackgroundVertices[0]->vertexColor.b = backgroundColor.b;
	progressMeterBackgroundVertices[0]->vertexColor.a = backgroundColor.a;

	progressMeterBackgroundVertices[1]->vertexPosition.coord.x = rightPos;
	progressMeterBackgroundVertices[1]->vertexPosition.coord.y = bottomPos;
	progressMeterBackgroundVertices[1]->vertexPosition.coord.z = 0.0f;
	progressMeterBackgroundVertices[1]->vertexColor.r = backgroundColor.r;
	progressMeterBackgroundVertices[1]->vertexColor.g = backgroundColor.g;
	progressMeterBackgroundVertices[1]->vertexColor.b = backgroundColor.b;
	progressMeterBackgroundVertices[1]->vertexColor.a = backgroundColor.a;

	progressMeterBackgroundVertices[2]->vertexPosition.coord.x = rightPos;
	progressMeterBackgroundVertices[2]->vertexPosition.coord.y = topPos;
	progressMeterBackgroundVertices[2]->vertexPosition.coord.z = 0.0f;
	progressMeterBackgroundVertices[2]->vertexColor.r = backgroundColor.r;
	progressMeterBackgroundVertices[2]->vertexColor.g = backgroundColor.g;
	progressMeterBackgroundVertices[2]->vertexColor.b = backgroundColor.b;
	progressMeterBackgroundVertices[2]->vertexColor.a = backgroundColor.a;

	progressMeterBackgroundVertices[3]->vertexPosition.coord.x = leftPos;
	progressMeterBackgroundVertices[3]->vertexPosition.coord.y = topPos;
	progressMeterBackgroundVertices[3]->vertexPosition.coord.z = 0.0f;
	progressMeterBackgroundVertices[3]->vertexColor.r = backgroundColor.r;
	progressMeterBackgroundVertices[3]->vertexColor.g = backgroundColor.g;
	progressMeterBackgroundVertices[3]->vertexColor.b = backgroundColor.b;
	progressMeterBackgroundVertices[3]->vertexColor.a = backgroundColor.a;
	
	progressMeterVertices[0]->vertexPosition.coord.x = leftPos;
	progressMeterVertices[0]->vertexPosition.coord.y = bottomPos;
	progressMeterVertices[0]->vertexPosition.coord.z = 0.0f;
	progressMeterVertices[0]->vertexColor.r = 1.0f;
	progressMeterVertices[0]->vertexColor.g = 1.0f;
	progressMeterVertices[0]->vertexColor.b = 1.0f;
	progressMeterVertices[0]->vertexColor.a = 1.0f;

	progressMeterVertices[1]->vertexPosition.coord.x = rightProgressPos;
	progressMeterVertices[1]->vertexPosition.coord.y = bottomPos;
	progressMeterVertices[1]->vertexPosition.coord.z = 0.0f;
	progressMeterVertices[1]->vertexColor.r = 1.0f;
	progressMeterVertices[1]->vertexColor.g = 1.0f;
	progressMeterVertices[1]->vertexColor.b = 1.0f;
	progressMeterVertices[1]->vertexColor.a = 1.0f;

	progressMeterVertices[2]->vertexPosition.coord.x = rightProgressPos;
	progressMeterVertices[2]->vertexPosition.coord.y = topPos;
	progressMeterVertices[2]->vertexPosition.coord.z = 0.0f;
	progressMeterVertices[2]->vertexColor.r = 1.0f;
	progressMeterVertices[2]->vertexColor.g = 1.0f;
	progressMeterVertices[2]->vertexColor.b = 1.0f;
	progressMeterVertices[2]->vertexColor.a = 1.0f;

	progressMeterVertices[3]->vertexPosition.coord.x = leftPos;
	progressMeterVertices[3]->vertexPosition.coord.y = topPos;
	progressMeterVertices[3]->vertexPosition.coord.z = 0.0f;
	progressMeterVertices[3]->vertexColor.r = 1.0f;
	progressMeterVertices[3]->vertexColor.g = 1.0f;
	progressMeterVertices[3]->vertexColor.b = 1.0f;
	progressMeterVertices[3]->vertexColor.a = 1.0f;

	progressMeterOutlineVertices[0]->vertexPosition.coord.x = leftPos;
	progressMeterOutlineVertices[0]->vertexPosition.coord.y = bottomPos;
	progressMeterOutlineVertices[0]->vertexPosition.coord.z = 0.0f;
	/*
	progressMeterOutlineVertices[0]->vertexColor.r = 0.0f;
	progressMeterOutlineVertices[0]->vertexColor.g = 0.0f;
	progressMeterOutlineVertices[0]->vertexColor.b = 0.0f;
	progressMeterOutlineVertices[0]->vertexColor.a = 1.0f;
	*/
	

	progressMeterOutlineVertices[1]->vertexPosition.coord.x = rightPos;
	progressMeterOutlineVertices[1]->vertexPosition.coord.y = bottomPos;
	progressMeterOutlineVertices[1]->vertexPosition.coord.z = 0.0f;
	/*
	progressMeterOutlineVertices[1]->vertexColor.r = 0.0f;
	progressMeterOutlineVertices[1]->vertexColor.g = 0.0f;
	progressMeterOutlineVertices[1]->vertexColor.b = 0.0f;
	progressMeterOutlineVertices[1]->vertexColor.a = 1.0f;
	*/

	progressMeterOutlineVertices[2]->vertexPosition.coord.x = rightPos;
	progressMeterOutlineVertices[2]->vertexPosition.coord.y = topPos;
	progressMeterOutlineVertices[2]->vertexPosition.coord.z = 0.0f;
	/*
	progressMeterOutlineVertices[2]->vertexColor.r = 0.0f;
	progressMeterOutlineVertices[2]->vertexColor.g = 0.0f;
	progressMeterOutlineVertices[2]->vertexColor.b = 0.0f;
	progressMeterOutlineVertices[2]->vertexColor.a = 1.0f;
	*/

	progressMeterOutlineVertices[3]->vertexPosition.coord.x = leftPos;
	progressMeterOutlineVertices[3]->vertexPosition.coord.y = topPos;
	progressMeterOutlineVertices[3]->vertexPosition.coord.z = 0.0f;
	/*
	progressMeterOutlineVertices[3]->vertexColor.r = 0.0f;
	progressMeterOutlineVertices[3]->vertexColor.g = 0.0f;
	progressMeterOutlineVertices[3]->vertexColor.b = 0.0f;
	progressMeterOutlineVertices[3]->vertexColor.a = 1.0f;
	*/
}


void ProcessMonitor::setupProgressMeterVertices() {

	VisualVertex* aVertex;
	double topPos;
	double leftPos;
	double bottomPos;
	double rightPos;
	double rightProgressPos;
	RGBAColor backgroundColor;
	
	VisualCamera aCamera;
	aCamera.setOrthographicProjection();
	topPos = aCamera.getMaxBottomCoord() + VisualActorGraphics::yPixelToCoord(10, aCamera);
	leftPos = aCamera.getMaxLeftCoord();
	bottomPos = aCamera.getMaxBottomCoord();
	rightPos = aCamera.getMaxRightCoord();
	rightProgressPos = aCamera.getMaxLeftCoord();
	backgroundColor = VisualActorGraphics::getBackgroundColor();
	
	Coord coordPoint;
	coordPoint.x = leftPos;
	coordPoint.y = bottomPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(leftPos, bottomPos, 0.0, backgroundColor);
	aVertex = new VisualVertex(coordPoint, backgroundColor);
	/*
	aVertex->vertexPosition.coord.x = leftPos;
	aVertex->vertexPosition.coord.y = bottomPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = backgroundColor.r;
	aVertex->vertexColor.g = backgroundColor.g;
	aVertex->vertexColor.b = backgroundColor.b;
	aVertex->vertexColor.a = backgroundColor.a;
	*/
	progressMeterBackgroundVertices.push_back(aVertex);

	coordPoint.x = rightPos;
	coordPoint.y = bottomPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(rightPos, bottomPos, 0.0, backgroundColor);
	aVertex = new VisualVertex(coordPoint, backgroundColor);
	/*
	aVertex->vertexPosition.coord.x = rightPos;
	aVertex->vertexPosition.coord.y = bottomPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = backgroundColor.r;
	aVertex->vertexColor.g = backgroundColor.g;
	aVertex->vertexColor.b = backgroundColor.b;
	aVertex->vertexColor.a = backgroundColor.a;
	*/
	progressMeterBackgroundVertices.push_back(aVertex);

	coordPoint.x = rightPos;
	coordPoint.y = topPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(rightPos, topPos, 0.0, backgroundColor);
	aVertex = new VisualVertex(coordPoint, backgroundColor);
	/*
	aVertex->vertexPosition.coord.x = rightPos;
	aVertex->vertexPosition.coord.y = topPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = backgroundColor.r;
	aVertex->vertexColor.g = backgroundColor.g;
	aVertex->vertexColor.b = backgroundColor.b;
	aVertex->vertexColor.a = backgroundColor.a;
	*/
	progressMeterBackgroundVertices.push_back(aVertex);

	coordPoint.x = leftPos;
	coordPoint.y = topPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(leftPos, topPos, 0.0, backgroundColor);
	aVertex = new VisualVertex(coordPoint, backgroundColor);
	/*
	aVertex->vertexPosition.coord.x = leftPos;
	aVertex->vertexPosition.coord.y = topPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = backgroundColor.r;
	aVertex->vertexColor.g = backgroundColor.g;
	aVertex->vertexColor.b = backgroundColor.b;
	aVertex->vertexColor.a = backgroundColor.a;
	*/
	progressMeterBackgroundVertices.push_back(aVertex);

	coordPoint.x = leftPos;
	coordPoint.y = bottomPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(leftPos, bottomPos, 0.0, white);
	aVertex = new VisualVertex(coordPoint, white);
	/*
	aVertex->vertexPosition.coord.x = leftPos;
	aVertex->vertexPosition.coord.y = bottomPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = 1.0f;
	aVertex->vertexColor.g = 1.0f;
	aVertex->vertexColor.b = 1.0f;
	aVertex->vertexColor.a = 1.0f;
	*/
	progressMeterVertices.push_back(aVertex);

	coordPoint.x = rightProgressPos;
	coordPoint.y = bottomPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(rightProgressPos, bottomPos, 0.0, white);
	aVertex = new VisualVertex(coordPoint, white);
	/*
	aVertex->vertexPosition.coord.x = rightProgressPos;
	aVertex->vertexPosition.coord.y = bottomPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = 1.0f;
	aVertex->vertexColor.g = 1.0f;
	aVertex->vertexColor.b = 1.0f;
	aVertex->vertexColor.a = 1.0f;
	*/
	progressMeterVertices.push_back(aVertex);

	coordPoint.x = rightProgressPos;
	coordPoint.y = topPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(rightProgressPos, topPos, 0.0, white);
	aVertex = new VisualVertex(coordPoint, white);
	/*
	aVertex->vertexPosition.coord.x = rightProgressPos;
	aVertex->vertexPosition.coord.y = topPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = 1.0f;
	aVertex->vertexColor.g = 1.0f;
	aVertex->vertexColor.b = 1.0f;
	aVertex->vertexColor.a = 1.0f;
	*/
	progressMeterVertices.push_back(aVertex);

	coordPoint.x = rightProgressPos;
	coordPoint.y = topPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(leftPos, topPos, 0.0, white);
	aVertex = new VisualVertex(coordPoint, white);
	/*
	aVertex->vertexPosition.coord.x = leftPos;
	aVertex->vertexPosition.coord.y = topPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = 1.0f;
	aVertex->vertexColor.g = 1.0f;
	aVertex->vertexColor.b = 1.0f;
	aVertex->vertexColor.a = 1.0f;
	*/
	progressMeterVertices.push_back(aVertex);

	coordPoint.x = leftPos;
	coordPoint.y = bottomPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(leftPos, bottomPos, 0.0, black);
	//aVertex = new VisualVertex(coordPoint, black);
	aVertex = new VisualVertex(coordPoint);
	/*
	aVertex->vertexPosition.coord.x = leftPos;
	aVertex->vertexPosition.coord.y = bottomPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = 0.0f;
	aVertex->vertexColor.g = 0.0f;
	aVertex->vertexColor.b = 0.0f;
	aVertex->vertexColor.a = 1.0f;
	*/
	progressMeterOutlineVertices.push_back(aVertex);

	coordPoint.x = rightPos;
	coordPoint.y = bottomPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(rightPos, bottomPos, 0.0, black);
	//aVertex = new VisualVertex(coordPoint, black);
	aVertex = new VisualVertex(coordPoint);
	/*
	aVertex->vertexPosition.coord.x = rightPos;
	aVertex->vertexPosition.coord.y = bottomPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = 0.0f;
	aVertex->vertexColor.g = 0.0f;
	aVertex->vertexColor.b = 0.0f;
	aVertex->vertexColor.a = 1.0f;
	*/
	progressMeterOutlineVertices.push_back(aVertex);

	coordPoint.x = rightPos;
	coordPoint.y = topPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(rightPos, topPos, 0.0, black);
	//aVertex = new VisualVertex(coordPoint, black);
	aVertex = new VisualVertex(coordPoint);
	/*
	aVertex->vertexPosition.coord.x = rightPos;
	aVertex->vertexPosition.coord.y = topPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = 0.0f;
	aVertex->vertexColor.g = 0.0f;
	aVertex->vertexColor.b = 0.0f;
	aVertex->vertexColor.a = 1.0f;
	*/
	progressMeterOutlineVertices.push_back(aVertex);

	coordPoint.x = leftPos;
	coordPoint.y = topPos;
	coordPoint.z = 0.0;
	//aVertex = new VisualVertex(leftPos, topPos, 0.0, black);
	//aVertex = new VisualVertex(coordPoint, black);
	aVertex = new VisualVertex(coordPoint);
	/*
	aVertex->vertexPosition.coord.x = leftPos;
	aVertex->vertexPosition.coord.y = topPos;
	aVertex->vertexPosition.coord.z = 0.0f;
	aVertex->vertexColor.r = 0.0f;
	aVertex->vertexColor.g = 0.0f;
	aVertex->vertexColor.b = 0.0f;
	aVertex->vertexColor.a = 1.0f;
	*/
	progressMeterOutlineVertices.push_back(aVertex);

}


void ProcessMonitor::calcTrackProgressMeterVertices(const uint32 elapsedAudioTime, const uint32 remainingAudioTime) {
	double rightProgressPos;
	
	VisualCamera aCamera;
	aCamera.setOrthographicProjection();
	rightProgressPos = aCamera.getMaxLeftCoord() + ((aCamera.getMaxRightCoord() - aCamera.getMaxLeftCoord()) * ((float)elapsedAudioTime / (float)(remainingAudioTime + elapsedAudioTime)));

	this->progressMeterVertices[1]->vertexPosition.coord.x = rightProgressPos;
	this->progressMeterVertices[2]->vertexPosition.coord.x = rightProgressPos;

}


void ProcessMonitor::cleanupProgressMeterVertices() {
	VertexChainIterator it;
	for (it = progressMeterBackgroundVertices.begin(); it != progressMeterBackgroundVertices.end(); it++) {
		delete *it;
		*it = NULL;
	}
	progressMeterBackgroundVertices.clear();
	
	for (it = progressMeterVertices.begin(); it != progressMeterVertices.end(); it++) {
		delete *it;
		*it = NULL;
	}
	progressMeterVertices.clear();

	for (it = progressMeterOutlineVertices.begin(); it != progressMeterOutlineVertices.end(); it++) {
		delete *it;
		*it = NULL;
	}
	progressMeterOutlineVertices.clear();
}
