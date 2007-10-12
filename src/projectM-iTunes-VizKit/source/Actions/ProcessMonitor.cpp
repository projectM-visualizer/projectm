/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: ProcessMonitor.cpp
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

#include "ProcessMonitor.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"
#include "VisualAudioLab.h"



using namespace VizKit;


ProcessMonitor::ProcessMonitor() {
	setupProgressMeterVertices();
}


ProcessMonitor::~ProcessMonitor() {
    cleanupProgressMeterVertices();
}


void ProcessMonitor::prepareProcessMonitorShow() {
	RGBAColor theColor;
	theColor.r = 0.0f;
	theColor.g = 1.0f;
	theColor.b = 0.0f;
	theColor.a = 1.0f;
	VisualActorGraphics::prepareProcessMonitorShow(theColor);
}


void ProcessMonitor::showInfoStrings() {

	ProcessInfoMapIterator it;

    char finalTextString[255];
    double yNum;
    double xNum;
	UInt8 charCountMax = 0; // in one line per column
	double rowHeight, characterWidth;

	rowHeight = VisualActorGraphics::getCanvasCoordHeight() * (15.0f / VisualActorGraphics::getCanvasPixelHeight());
	characterWidth = VisualActorGraphics::getCanvasCoordWidth() * (9.0f / VisualActorGraphics::getCanvasPixelWidth());

	xNum = VisualActorGraphics::getMaxLeftCoordOfCanvas() + characterWidth;
	yNum = VisualActorGraphics::getMaxTopCoordOfCanvas();

	for (it = this->processInfoMap.begin(); it != this->processInfoMap.end(); it++) {
	
		yNum = yNum - rowHeight;
		if ((yNum - rowHeight) < VisualActorGraphics::getMaxBottomCoordOfCanvas()) {
			// we reached the bottom -> go to next column to the right
			yNum = VisualActorGraphics::getMaxTopCoordOfCanvas() - rowHeight;
			xNum = xNum + characterWidth + (charCountMax * characterWidth);
			charCountMax = 0;
		}
		strcpy (finalTextString, (*it).first.c_str());
		strcat (finalTextString, ": ");
		strcat (finalTextString, (*it).second.c_str());
		if (strlen(finalTextString) > charCountMax) {
			charCountMax = strlen(finalTextString);
		}
		VisualActorGraphics::showProcessInfoRow(xNum, yNum, finalTextString); // constant height of font
		
    }
	
	VisualActorGraphics::showProcessInfoNote();
    
}


void ProcessMonitor::showAudioInfo(const UInt32 elapsedAudioTime, const UInt32 remainingAudioTime) {

	VisualAudioLab* theVisualAudioLab;

	UInt16 numberOfAudioDataChannels;
	UInt32 numberOfSpectrumEntries;
	UInt16 maxNumberOfMusicDataHistories;
	const SInt8* waveformShapeArray;
	UInt16 maxNumberOfWaveformShapeHistory;

	const SInt16** waveformDataMonoArray;
	SInt16 currMusicDataHistory;
	UInt8 prevMusicDataHistory;
	UInt32 numberOfWaveformEntries;

	const UInt8*** spectrumDataArray;
	const UInt32* beatHistogram;

	theVisualAudioLab = VisualAudioLab::getInstance();

	currMusicDataHistory = theVisualAudioLab->getCurrMusicDataHistory();
	prevMusicDataHistory = theVisualAudioLab->getPrevMusicDataHistory();
	numberOfWaveformEntries = theVisualAudioLab->getNumberOfWaveformEntries();
	maxNumberOfMusicDataHistories = theVisualAudioLab->getMaxNumberOfMusicDataHistories();
	waveformDataMonoArray = theVisualAudioLab->getWaveformDataMonoArray();

	maxNumberOfWaveformShapeHistory = theVisualAudioLab->getMaxNumberOfWaveformShapeHistory();
	waveformShapeArray = theVisualAudioLab->getWaveformShapeArray();
	
	numberOfAudioDataChannels = theVisualAudioLab->getNumberOfDataChannels();
	numberOfSpectrumEntries = theVisualAudioLab->getNumberOfSpectrumEntries();
	spectrumDataArray = theVisualAudioLab->getSpectrumDataArray();

    VisualActorGraphics::drawWaveform(currMusicDataHistory, maxNumberOfMusicDataHistories, numberOfWaveformEntries, waveformDataMonoArray);
	VisualActorGraphics::resetModelViewMatrix();

	//theVisualGraphics->drawWaveform(currMusicDataHistory, 1, numberOfWaveformEntries, waveformDataMonoArray);
	VisualActorGraphics::drawSpectrumAnalyzer(currMusicDataHistory, maxNumberOfMusicDataHistories, numberOfSpectrumEntries, numberOfAudioDataChannels, spectrumDataArray);
	VisualActorGraphics::resetModelViewMatrix();
	VisualActorGraphics::drawSpectrogram(currMusicDataHistory, maxNumberOfMusicDataHistories, numberOfSpectrumEntries, numberOfAudioDataChannels, spectrumDataArray);
    VisualActorGraphics::resetModelViewMatrix();

	//theVisualGraphics->drawWaveformShape(waveformShapeArray, maxNumberOfWaveformShapeHistory);
	//theVisualGraphics->drawWaveformSpiral(currMusicDataHistory, numberOfWaveformEntries, waveformDataMonoArray);
	beatHistogram = theVisualAudioLab->getBeatHistogram();
	VisualActorGraphics::drawBeatHistogram(beatHistogram);
	//VisualActorGraphics::drawTrackProgressMeter(elapsedAudioTime, remainingAudioTime);
	
	//this->updateProgressMeterVertices();
	//printf("elapsedAudioTime: %ld, remainingAudioTime: %ld\n", elapsedAudioTime, remainingAudioTime);
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
	
	topPos = VisualActorGraphics::getMaxBottomCoordOfCanvas() + VisualActorGraphics::yPixelToCoord(10);
	leftPos = VisualActorGraphics::getMaxLeftCoordOfCanvas();
	bottomPos = VisualActorGraphics::getMaxBottomCoordOfCanvas();
	rightPos = VisualActorGraphics::getMaxRightCoordOfCanvas();
	rightProgressPos = VisualActorGraphics::getMaxLeftCoordOfCanvas();
	RGBAColor backgroundColor = VisualActorGraphics::getBackgroundColor();
	
	progressMeterBackgroundVertices[0]->vertexPosition.xPos = leftPos;
	progressMeterBackgroundVertices[0]->vertexPosition.yPos = bottomPos;
	progressMeterBackgroundVertices[0]->vertexPosition.zPos = 0.0f;
	progressMeterBackgroundVertices[0]->vertexColor.r = backgroundColor.r;
	progressMeterBackgroundVertices[0]->vertexColor.g = backgroundColor.g;
	progressMeterBackgroundVertices[0]->vertexColor.b = backgroundColor.b;
	progressMeterBackgroundVertices[0]->vertexColor.a = backgroundColor.a;

	progressMeterBackgroundVertices[1]->vertexPosition.xPos = rightPos;
	progressMeterBackgroundVertices[1]->vertexPosition.yPos = bottomPos;
	progressMeterBackgroundVertices[1]->vertexPosition.zPos = 0.0f;
	progressMeterBackgroundVertices[1]->vertexColor.r = backgroundColor.r;
	progressMeterBackgroundVertices[1]->vertexColor.g = backgroundColor.g;
	progressMeterBackgroundVertices[1]->vertexColor.b = backgroundColor.b;
	progressMeterBackgroundVertices[1]->vertexColor.a = backgroundColor.a;

	progressMeterBackgroundVertices[2]->vertexPosition.xPos = rightPos;
	progressMeterBackgroundVertices[2]->vertexPosition.yPos = topPos;
	progressMeterBackgroundVertices[2]->vertexPosition.zPos = 0.0f;
	progressMeterBackgroundVertices[2]->vertexColor.r = backgroundColor.r;
	progressMeterBackgroundVertices[2]->vertexColor.g = backgroundColor.g;
	progressMeterBackgroundVertices[2]->vertexColor.b = backgroundColor.b;
	progressMeterBackgroundVertices[2]->vertexColor.a = backgroundColor.a;

	progressMeterBackgroundVertices[3]->vertexPosition.xPos = leftPos;
	progressMeterBackgroundVertices[3]->vertexPosition.yPos = topPos;
	progressMeterBackgroundVertices[3]->vertexPosition.zPos = 0.0f;
	progressMeterBackgroundVertices[3]->vertexColor.r = backgroundColor.r;
	progressMeterBackgroundVertices[3]->vertexColor.g = backgroundColor.g;
	progressMeterBackgroundVertices[3]->vertexColor.b = backgroundColor.b;
	progressMeterBackgroundVertices[3]->vertexColor.a = backgroundColor.a;
	
	progressMeterVertices[0]->vertexPosition.xPos = leftPos;
	progressMeterVertices[0]->vertexPosition.yPos = bottomPos;
	progressMeterVertices[0]->vertexPosition.zPos = 0.0f;
	progressMeterVertices[0]->vertexColor.r = 1.0f;
	progressMeterVertices[0]->vertexColor.g = 1.0f;
	progressMeterVertices[0]->vertexColor.b = 1.0f;
	progressMeterVertices[0]->vertexColor.a = 1.0f;

	progressMeterVertices[1]->vertexPosition.xPos = rightProgressPos;
	progressMeterVertices[1]->vertexPosition.yPos = bottomPos;
	progressMeterVertices[1]->vertexPosition.zPos = 0.0f;
	progressMeterVertices[1]->vertexColor.r = 1.0f;
	progressMeterVertices[1]->vertexColor.g = 1.0f;
	progressMeterVertices[1]->vertexColor.b = 1.0f;
	progressMeterVertices[1]->vertexColor.a = 1.0f;

	progressMeterVertices[2]->vertexPosition.xPos = rightProgressPos;
	progressMeterVertices[2]->vertexPosition.yPos = topPos;
	progressMeterVertices[2]->vertexPosition.zPos = 0.0f;
	progressMeterVertices[2]->vertexColor.r = 1.0f;
	progressMeterVertices[2]->vertexColor.g = 1.0f;
	progressMeterVertices[2]->vertexColor.b = 1.0f;
	progressMeterVertices[2]->vertexColor.a = 1.0f;

	progressMeterVertices[3]->vertexPosition.xPos = leftPos;
	progressMeterVertices[3]->vertexPosition.yPos = topPos;
	progressMeterVertices[3]->vertexPosition.zPos = 0.0f;
	progressMeterVertices[3]->vertexColor.r = 1.0f;
	progressMeterVertices[3]->vertexColor.g = 1.0f;
	progressMeterVertices[3]->vertexColor.b = 1.0f;
	progressMeterVertices[3]->vertexColor.a = 1.0f;

	progressMeterOutlineVertices[0]->vertexPosition.xPos = leftPos;
	progressMeterOutlineVertices[0]->vertexPosition.yPos = bottomPos;
	progressMeterOutlineVertices[0]->vertexPosition.zPos = 0.0f;
	progressMeterOutlineVertices[0]->vertexColor.r = 0.0f;
	progressMeterOutlineVertices[0]->vertexColor.g = 0.0f;
	progressMeterOutlineVertices[0]->vertexColor.b = 0.0f;
	progressMeterOutlineVertices[0]->vertexColor.a = 1.0f;

	progressMeterOutlineVertices[1]->vertexPosition.xPos = rightPos;
	progressMeterOutlineVertices[1]->vertexPosition.yPos = bottomPos;
	progressMeterOutlineVertices[1]->vertexPosition.zPos = 0.0f;
	progressMeterOutlineVertices[1]->vertexColor.r = 0.0f;
	progressMeterOutlineVertices[1]->vertexColor.g = 0.0f;
	progressMeterOutlineVertices[1]->vertexColor.b = 0.0f;
	progressMeterOutlineVertices[1]->vertexColor.a = 1.0f;

	progressMeterOutlineVertices[2]->vertexPosition.xPos = rightPos;
	progressMeterOutlineVertices[2]->vertexPosition.yPos = topPos;
	progressMeterOutlineVertices[2]->vertexPosition.zPos = 0.0f;
	progressMeterOutlineVertices[2]->vertexColor.r = 0.0f;
	progressMeterOutlineVertices[2]->vertexColor.g = 0.0f;
	progressMeterOutlineVertices[2]->vertexColor.b = 0.0f;
	progressMeterOutlineVertices[2]->vertexColor.a = 1.0f;

	progressMeterOutlineVertices[3]->vertexPosition.xPos = leftPos;
	progressMeterOutlineVertices[3]->vertexPosition.yPos = topPos;
	progressMeterOutlineVertices[3]->vertexPosition.zPos = 0.0f;
	progressMeterOutlineVertices[3]->vertexColor.r = 0.0f;
	progressMeterOutlineVertices[3]->vertexColor.g = 0.0f;
	progressMeterOutlineVertices[3]->vertexColor.b = 0.0f;
	progressMeterOutlineVertices[3]->vertexColor.a = 1.0f;
}


void ProcessMonitor::setupProgressMeterVertices() {

	Vertex* aVertex;
	double topPos;
	double leftPos;
	double bottomPos;
	double rightPos;
	double rightProgressPos;
	RGBAColor backgroundColor;
	
	topPos = VisualActorGraphics::getMaxBottomCoordOfCanvas() + VisualActorGraphics::yPixelToCoord(10);
	leftPos = VisualActorGraphics::getMaxLeftCoordOfCanvas();
	bottomPos = VisualActorGraphics::getMaxBottomCoordOfCanvas();
	rightPos = VisualActorGraphics::getMaxRightCoordOfCanvas();
	rightProgressPos = VisualActorGraphics::getMaxLeftCoordOfCanvas();
	backgroundColor = VisualActorGraphics::getBackgroundColor();
	
	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = leftPos;
	aVertex->vertexPosition.yPos = bottomPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = backgroundColor.r;
	aVertex->vertexColor.g = backgroundColor.g;
	aVertex->vertexColor.b = backgroundColor.b;
	aVertex->vertexColor.a = backgroundColor.a;
	progressMeterBackgroundVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = rightPos;
	aVertex->vertexPosition.yPos = bottomPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = backgroundColor.r;
	aVertex->vertexColor.g = backgroundColor.g;
	aVertex->vertexColor.b = backgroundColor.b;
	aVertex->vertexColor.a = backgroundColor.a;
	progressMeterBackgroundVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = rightPos;
	aVertex->vertexPosition.yPos = topPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = backgroundColor.r;
	aVertex->vertexColor.g = backgroundColor.g;
	aVertex->vertexColor.b = backgroundColor.b;
	aVertex->vertexColor.a = backgroundColor.a;
	progressMeterBackgroundVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = leftPos;
	aVertex->vertexPosition.yPos = topPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = backgroundColor.r;
	aVertex->vertexColor.g = backgroundColor.g;
	aVertex->vertexColor.b = backgroundColor.b;
	aVertex->vertexColor.a = backgroundColor.a;
	progressMeterBackgroundVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = leftPos;
	aVertex->vertexPosition.yPos = bottomPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = 1.0f;
	aVertex->vertexColor.g = 1.0f;
	aVertex->vertexColor.b = 1.0f;
	aVertex->vertexColor.a = 1.0f;
	progressMeterVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = rightProgressPos;
	aVertex->vertexPosition.yPos = bottomPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = 1.0f;
	aVertex->vertexColor.g = 1.0f;
	aVertex->vertexColor.b = 1.0f;
	aVertex->vertexColor.a = 1.0f;
	progressMeterVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = rightProgressPos;
	aVertex->vertexPosition.yPos = topPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = 1.0f;
	aVertex->vertexColor.g = 1.0f;
	aVertex->vertexColor.b = 1.0f;
	aVertex->vertexColor.a = 1.0f;
	progressMeterVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = leftPos;
	aVertex->vertexPosition.yPos = topPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = 1.0f;
	aVertex->vertexColor.g = 1.0f;
	aVertex->vertexColor.b = 1.0f;
	aVertex->vertexColor.a = 1.0f;
	progressMeterVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = leftPos;
	aVertex->vertexPosition.yPos = bottomPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = 0.0f;
	aVertex->vertexColor.g = 0.0f;
	aVertex->vertexColor.b = 0.0f;
	aVertex->vertexColor.a = 1.0f;
	progressMeterOutlineVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = rightPos;
	aVertex->vertexPosition.yPos = bottomPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = 0.0f;
	aVertex->vertexColor.g = 0.0f;
	aVertex->vertexColor.b = 0.0f;
	aVertex->vertexColor.a = 1.0f;
	progressMeterOutlineVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = rightPos;
	aVertex->vertexPosition.yPos = topPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = 0.0f;
	aVertex->vertexColor.g = 0.0f;
	aVertex->vertexColor.b = 0.0f;
	aVertex->vertexColor.a = 1.0f;
	progressMeterOutlineVertices.push_back(aVertex);

	aVertex = new Vertex;
	aVertex->vertexPosition.xPos = leftPos;
	aVertex->vertexPosition.yPos = topPos;
	aVertex->vertexPosition.zPos = 0.0f;
	aVertex->vertexColor.r = 0.0f;
	aVertex->vertexColor.g = 0.0f;
	aVertex->vertexColor.b = 0.0f;
	aVertex->vertexColor.a = 1.0f;
	progressMeterOutlineVertices.push_back(aVertex);

}


void ProcessMonitor::calcTrackProgressMeterVertices(const UInt32 elapsedAudioTime, const UInt32 remainingAudioTime) {
	double rightProgressPos;
	
	rightProgressPos = VisualActorGraphics::getMaxLeftCoordOfCanvas() + ((VisualActorGraphics::getMaxRightCoordOfCanvas() - VisualActorGraphics::getMaxLeftCoordOfCanvas()) * ((float)elapsedAudioTime / (float)(remainingAudioTime + elapsedAudioTime)));

	this->progressMeterVertices[1]->vertexPosition.xPos = rightProgressPos;
	this->progressMeterVertices[2]->vertexPosition.xPos = rightProgressPos;

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
