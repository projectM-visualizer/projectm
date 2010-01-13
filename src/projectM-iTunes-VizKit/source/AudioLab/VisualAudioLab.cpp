/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAudioLab.cpp
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

#include "VisualAudioLab.h"
#include "VisualPlayerState.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"
#include "VisualTiming.h"
#include "VisualNotification.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>



using namespace VizKit;


VisualAudioLab* VisualAudioLab::theVisualAudioLab = NULL; // singleton

uint16 VisualAudioLab::maxNumberOfAudioDataChannels = 0;
uint32 VisualAudioLab::numberOfAudioWaveformDataEntries = 0;
uint32 VisualAudioLab::numberOfAudioSpectrumDataEntries = 0;

const uint8 VisualAudioLab::beatTimeTolerance = 25;
const uint16 VisualAudioLab::beatThreshold = 300;
const uint16 VisualAudioLab::beatAnalyseTimeFrame = 10000;
const uint16 VisualAudioLab::maximumBeatInterval = 2000;
const uint16 VisualAudioLab::numberOfMillisecondsOfRMSBuffer = 3000;
const uint16 VisualAudioLab::numberOfRMSBufferEntries = 2000;
const uint16 VisualAudioLab::maxNumberOfWaveformShapeHistory = 1400;
const uint16 VisualAudioLab::maxNumberOfMusicDataHistory = 40;
const uint16 VisualAudioLab::tappingBufferLength = 4096;
const uint16 VisualAudioLab::maxNumberOfBeatHistogramBins = 100;
const uint16 VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe = 200;


VisualAudioLab::VisualAudioLab() {

    currMusicDataHistory = -1;
    rmsMonoAvg = 0.0;
    rmsMonoMax = 0.0;

	totalTimeOfTrack = 0;
	startTimeOfTrack = 0;
	stopTimeOfTrack = 0;
	elapsedTimeOfTrack = 0;
	currTimestampIDOfRenderData = 0;
	
}


VisualAudioLab::~VisualAudioLab() {
	uint32 i, k;
    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
			free(waveformData[i][k]);
        }
		free(waveformData[i]);
    }
	free(waveformData);

    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
		free(waveformDataMono[i]);
    }
	free(waveformDataMono);

    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
			free(spectrumData[i][k]);
        }
		free(spectrumData[i]);
    }
	free(spectrumData);
	
	free(amplitudeEnvelope);

	free(mean);
	
	free(tappingBuffer);

    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
		free(rms[i]);
    }
	free(rms);
	
	free(rmsMono);
	
	free(beatGapHistogram);
	
	free(minSpectrumLevel);
	
	free(maxSpectrumLevel);
	
	free(minWaveformVal);
	
	free(maxWaveformVal);
	
	free(waveformValSum);
	
	free(peakWaveformValSum);
	
	free(prevWaveformValSum);
	
	free(avgWaveformValSum);
	
	free(avgWaveformValSumCounter);
	
	free(beatsRecordedInBeatAnalyseTimeFrame);

}


VisualAudioLab* VisualAudioLab::getInstance() {
    if (theVisualAudioLab == NULL) {
		theVisualAudioLab = new VisualAudioLab;
		if (theVisualAudioLab != NULL) {
			theVisualAudioLab->init();
		}
    }
	if (theVisualAudioLab == NULL) {
		writeLog("ERR init VisualAudioLab failed");
	}
	return theVisualAudioLab;
}


void VisualAudioLab::dispose() {
    if (theVisualAudioLab != 0) {
        delete theVisualAudioLab;
        theVisualAudioLab = 0;
    }
}


void VisualAudioLab::init() {
	uint32 i;
    uint32 k;

	// waveformData
	waveformData = (sint8***)malloc(VisualAudioLab::maxNumberOfMusicDataHistory * sizeof(sint8**));
    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
		waveformData[i] = (sint8**)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(sint8*));
    }
    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
			waveformData[i][k] = (sint8*)malloc(VisualAudioLab::numberOfAudioWaveformDataEntries * sizeof(sint8));
			for (uint32 m = 0; m < VisualAudioLab::numberOfAudioWaveformDataEntries; m++) {
				waveformData[i][k][m] = 0;
			}
		}
    }
	
	// waveformDataMono
	waveformDataMono = (sint16**)malloc(VisualAudioLab::maxNumberOfMusicDataHistory * sizeof(sint16*));
    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
		waveformDataMono[i] = (sint16*)malloc(VisualAudioLab::numberOfAudioWaveformDataEntries * sizeof(sint16));
		for (k = 0; k < VisualAudioLab::numberOfAudioWaveformDataEntries; k++) {
			waveformDataMono[i][k] = 0;
		}
	}

	// spectrumData
	spectrumData = (uint8***)malloc(VisualAudioLab::maxNumberOfMusicDataHistory * sizeof(uint8**));
    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
		spectrumData[i] = (uint8**)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(uint8*));
    }
    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
			spectrumData[i][k] = (uint8*)malloc(VisualAudioLab::numberOfAudioSpectrumDataEntries * sizeof(uint8));
			for (uint32 m = 0; m < VisualAudioLab::numberOfAudioSpectrumDataEntries; m++) {
				spectrumData[i][k][m] = 0;
			}
		}
    }

	// amplitudeEnvelope
	amplitudeEnvelope = (double*)malloc(VisualAudioLab::numberOfAudioWaveformDataEntries * sizeof(double));
	
	// mean
	mean = (sint32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(sint32));
	
	// tappingBuffer
	tappingBuffer = (double*)malloc(tappingBufferLength * sizeof(double));
	
	// rms
	rms = (double**)malloc(VisualAudioLab::maxNumberOfMusicDataHistory * sizeof(double*));
    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
		rms[i] = (double*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(double));
    }
	
	// rmsMono
	rmsMono = (double*)malloc(VisualAudioLab::maxNumberOfMusicDataHistory * sizeof(double));
	
	// beatGapHistogram
	beatGapHistogram = (uint32*)malloc(VisualAudioLab::maxNumberOfBeatHistogramBins * sizeof(uint32));

	// minSpectrumLevel
	minSpectrumLevel = (uint8*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(uint8));
	
	// maxSpectrumLevel
	maxSpectrumLevel = (uint8*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(uint8));
	
	// minWaveformVal
	minWaveformVal = (sint8*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(sint8));

	// maxWaveformVal
	maxWaveformVal = (sint8*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(sint8));
	
	// waveformValSum
	waveformValSum = (uint32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(uint32));
	
	// peakWaveformValSum
	peakWaveformValSum = (uint32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(uint32));

	// prevWaveformValSum
	prevWaveformValSum = (uint32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(uint32));

	// avgWaveformValSum
	avgWaveformValSum = (uint32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(uint32));

	// avgWaveformValSumCounter
	avgWaveformValSumCounter = (uint32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(uint32));
	
	// beatsRecordedInBeatAnalyseTimeFrame
	beatsRecordedInBeatAnalyseTimeFrame = (BeatGuess*)malloc(VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe * sizeof(BeatGuess));

    for (i = 0; i < VisualAudioLab::maxNumberOfAudioDataChannels; i++) {
        avgWaveformValSumCounter[i] = 0;
        minWaveformVal[i] = 0;
        maxWaveformVal[i] = 0;
        waveformValSum[i] = 0;
    }
    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
            rms[i][k] = 0;
        }
    }
}


void VisualAudioLab::resetData(void) {
	
	uint16 i,k,m;

	//theVisualAudioLab->totalTimeOfTrack = 0;
	//theVisualAudioLab->startTimeOfTrack = 0;
	//theVisualAudioLab->stopTimeOfTrack = 0;
	theVisualAudioLab->currTimestampIDOfRenderData = 0;

    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
            for (m = 0; m < VisualAudioLab::numberOfAudioWaveformDataEntries; m++) {
                theVisualAudioLab->waveformData[i][k][m] = 0;
            }
        }
    }

    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
		for (m = 0; m < VisualAudioLab::numberOfAudioWaveformDataEntries; m++) {
			theVisualAudioLab->waveformDataMono[i][m] = 0;
		}
    }

    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
            for (m = 0; m < VisualAudioLab::numberOfAudioSpectrumDataEntries; m++) {
                theVisualAudioLab->spectrumData[i][k][m] = 0;
            }
        }
    }

    // other statistics
    for (i = 0; i < VisualAudioLab::maxNumberOfMusicDataHistory; i++) {
		for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
			theVisualAudioLab->rms[i][k] = 0;
		}
		theVisualAudioLab->rmsMono[i] = 0;
    }
	
	for (i = 0; i < 100; i++) {
		theVisualAudioLab->beatGapHistogram[i] = 0;
	}

	for (i = 0; i < 100; i++) {
		theVisualAudioLab->beatGapHistogram[i] = 0;
	}

	for (i = 0; i < VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe; i++) {
		theVisualAudioLab->beatsRecordedInBeatAnalyseTimeFrame[i].timeInMillisecondsSinceStartOfTrack = 0;
		theVisualAudioLab->beatsRecordedInBeatAnalyseTimeFrame[i].confidence = 0;
	}
	for (i = 0; i < numberOfRMSBufferEntries; i++) {
		theVisualAudioLab->rmsBuffer[i].rmsValue = 0.0f;
		theVisualAudioLab->rmsBuffer[i].timeInMillisecondsSinceStartOfTrack = 0;
	}
	theVisualAudioLab->currRMSValue = 0;

	VisualTiming::resetTimestamp("timeSinceStartOfTrack");
	
	theVisualAudioLab->currRecordedBeat = 0;

	theVisualAudioLab->currMusicDataHistory = -1;
	theVisualAudioLab->maxWaveformValMono = 0;
	theVisualAudioLab->minWaveformValMono = 0;
	theVisualAudioLab->rmsMonoMax = 0;
    
    // zero tappingBuffer
    for(i=0; i<tappingBufferLength;i++) {
        theVisualAudioLab->tappingBuffer[i] = 0;
    }

}


void VisualAudioLab::setTotalTimeOfCurrentTrack(const uint32 totalTimeInMS) {
	theVisualAudioLab->totalTimeOfTrack = totalTimeInMS;
}


void VisualAudioLab::setStartTimeOfCurrentTrack(const uint32 startTimeInMS) {
	theVisualAudioLab->startTimeOfTrack = startTimeInMS;
}


void VisualAudioLab::setStopTimeOfCurrentTrack(const uint32 stopTimeInMS) {
	theVisualAudioLab->stopTimeOfTrack = stopTimeInMS;
}


uint32 VisualAudioLab::getCurrTimestampIDOfRenderData() {
	return theVisualAudioLab->currTimestampIDOfRenderData;
}


void VisualAudioLab::setCurrTimestampIDOfRenderData(const uint32 timestampID) {
	char timestampHitStr[64];
	if (timestampID > theVisualAudioLab->currTimestampIDOfRenderData) {
		if (timestampID - theVisualAudioLab->currTimestampIDOfRenderData > 1) {
			sprintf(timestampHitStr, "%d", timestampID - theVisualAudioLab->currTimestampIDOfRenderData);
			setProcessInfo("AudioDataHit", timestampHitStr);
		} else {
			setProcessInfo("AudioDataHit", "1");
		}
	}
	theVisualAudioLab->currTimestampIDOfRenderData = timestampID;
}


bool VisualAudioLab::remainingTimeOfCurrentTrackIsKnown() {
	if (theVisualAudioLab->totalTimeOfTrack == 0) {
		return false;
	} else {
		return true;
	}
}


uint32 VisualAudioLab::getRemainingTimeOfCurrentTrack() {
	if (theVisualAudioLab->totalTimeOfTrack == 0) {
		return 0;
	}

	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	uint32 elapsedTime = theVisualPlayerState->getElapsedAudioTime();
	if (theVisualAudioLab->totalTimeOfTrack < elapsedTime) {
		return 0;
	}
	return theVisualAudioLab->totalTimeOfTrack - elapsedTime;
}


uint32 VisualAudioLab::getTotalTimeOfCurrentTrack() {
	return theVisualAudioLab->totalTimeOfTrack;
}


sint16 VisualAudioLab::getCurrMusicDataHistory() {
    return theVisualAudioLab->currMusicDataHistory;
}


sint16 VisualAudioLab::getPrevMusicDataHistory() {
	if ((theVisualAudioLab->currMusicDataHistory - 1) < 0) {
		return VisualAudioLab::maxNumberOfMusicDataHistory - 1;
	} else {
		return theVisualAudioLab->currMusicDataHistory - 1;
	}
}


uint32 VisualAudioLab::getNumberOfWaveformEntries() {
	return VisualAudioLab::numberOfAudioWaveformDataEntries;
}


uint32 VisualAudioLab::getNumberOfSpectrumEntries() {
	return VisualAudioLab::numberOfAudioSpectrumDataEntries;
}


uint16 VisualAudioLab::getNumberOfDataChannels() {
	return VisualAudioLab::maxNumberOfAudioDataChannels;
}


uint16 VisualAudioLab::getMaxNumberOfMusicDataHistories() {
	return VisualAudioLab::maxNumberOfMusicDataHistory;
}


void VisualAudioLab::incrementMusicDataHistory() {
    theVisualAudioLab->currMusicDataHistory++;
    if (theVisualAudioLab->currMusicDataHistory == VisualAudioLab::maxNumberOfMusicDataHistory) {
		// cycled
        theVisualAudioLab->currMusicDataHistory = 0;
    }
}


void VisualAudioLab::processAudioData(uint8 numWaveformChannels, 
										uint16 numWaveformEntries, 
										const uint8* const currWaveformData, 
										uint8 numSpectrumChannels, 
										uint16 numSpectrumEntries, 
										const uint8* const currSpectrumData) {

	theVisualAudioLab->incrementMusicDataHistory();
    theVisualAudioLab->processWaveformData(numWaveformChannels, numWaveformEntries, currWaveformData);
    theVisualAudioLab->processSpectrumData(numSpectrumChannels, numSpectrumEntries, currSpectrumData);
	
}


void VisualAudioLab::processWaveformData(uint8 numWaveformChannels, uint16 numWaveformEntries, const uint8* const currWaveformData) {

    uint16 index;
    uint8 channel;
    uint16 tappingIdx;
	uint32 elapsedMillisecondsSinceStartOfTrack;
    uint32 elapsedMilliseconds;
    uint32 sampleRate;
    uint16 numberOfSamplesToAnalyse;
    char infoStr[32];
    char labelStr[32];
    bool hasBeatImpulse = false;
    sint8 waveformVal; // -128 ..0.. 127
    static uint16 tappingBufferHighWatermarkIdx = 0;
    uint16 numberOfTappingSamplesProcessed = 0;
    uint16 tappingStartSampleNum;
    uint16 numberOfSamplesToInterpolate;
    double lastWaveVal;
    double interpolStepVal;
	uint32 milliSecsForBeat;
	static double deviance = 2.1;
	double aRMSMonoValSum;
	uint16 aRMSCount;
	
	elapsedMillisecondsSinceStartOfTrack = VisualTiming::getElapsedMilliSecsSinceReset("timeSinceStartOfTrack");

    elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceLastCall("processWaveformData");

    sampleRate = static_cast<uint32>(static_cast<float>(VisualAudioLab::numberOfAudioWaveformDataEntries) * (1000.0 / static_cast<float>(elapsedMilliseconds)));
    sprintf(infoStr, "%d",sampleRate);
    setProcessInfo ("Audio sampleRate", infoStr);
    

    // mean

    for (channel = 0; channel < numWaveformChannels; channel++) {
        for (index = 0; index < numWaveformEntries; index++) {
			theVisualAudioLab->mean[channel] += (abs)(currWaveformData[channel * numWaveformEntries + index] - 128);
        }
        theVisualAudioLab->mean[channel] = theVisualAudioLab->mean[channel] / numWaveformEntries;
    }

    
    theVisualAudioLab->minWaveformValMono = 0;
    theVisualAudioLab->maxWaveformValMono = 0;

    
    for (channel = 0; channel < numWaveformChannels; channel++) {

        tappingIdx = 0;

        theVisualAudioLab->minWaveformVal[channel] = 0;
        theVisualAudioLab->maxWaveformVal[channel] = 0;
        theVisualAudioLab->waveformValSum[channel] = 0;
        theVisualAudioLab->rms[theVisualAudioLab->currMusicDataHistory][channel] = 0.0;

        theVisualAudioLab->avgWaveformValSumCounter[channel] ++;
        
        // data
        for (index = 0; index < numWaveformEntries; index++) {
        
			waveformVal = currWaveformData[channel * numWaveformEntries + index] - 128;

            theVisualAudioLab->waveformData[theVisualAudioLab->currMusicDataHistory][channel][index] = waveformVal;
            
            if (channel == 0) {
                theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index] = waveformVal;
            } else {
                theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index] += waveformVal;
            }
            
            theVisualAudioLab->waveformValSum[channel] += abs(waveformVal);
            theVisualAudioLab->rms[theVisualAudioLab->currMusicDataHistory][channel] += (double)pow(static_cast<float>(abs(waveformVal)) - theVisualAudioLab->mean[channel], 2);

            if (waveformVal < theVisualAudioLab->minWaveformVal[channel]) {
                theVisualAudioLab->minWaveformVal[channel] = waveformVal;
            }
            if (waveformVal > theVisualAudioLab->maxWaveformVal[channel]) {
                theVisualAudioLab->maxWaveformVal[channel] = waveformVal;
            }

        }
        
        theVisualAudioLab->rms[theVisualAudioLab->currMusicDataHistory][channel] /= numWaveformEntries;
        theVisualAudioLab->rms[theVisualAudioLab->currMusicDataHistory][channel] = sqrt(theVisualAudioLab->rms[theVisualAudioLab->currMusicDataHistory][channel]);

		sprintf(labelStr, "RMS channel %u", channel);
        sprintf(infoStr, "%10.3lf", theVisualAudioLab->rms[theVisualAudioLab->currMusicDataHistory][channel]);
        setProcessInfo (labelStr, infoStr);

		sprintf(labelStr, "MAX waveform channel %u", channel);
		sprintf(infoStr, "%d", theVisualAudioLab->maxWaveformVal[channel]);
        setProcessInfo (labelStr, infoStr);
		
        theVisualAudioLab->avgWaveformValSum[channel] = \
            ((theVisualAudioLab->avgWaveformValSum[channel] * ((theVisualAudioLab->avgWaveformValSumCounter[channel])-1) + \
              theVisualAudioLab->waveformValSum[channel]) / theVisualAudioLab->avgWaveformValSumCounter[channel]);
    }

    
    if (theVisualAudioLab->waveformValSum[0] > theVisualAudioLab->peakWaveformValSum[0]) {
        theVisualAudioLab->peakWaveformValSum[0] = theVisualAudioLab->waveformValSum[0];
    }

    theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory] = 0;

    for (index = 0; index < numWaveformEntries; index++) {
    
        theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index] /= numWaveformChannels;
        theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory] += (double)pow(static_cast<float>(abs(theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index])), 2);
        if (theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index] < theVisualAudioLab->minWaveformValMono) {
            theVisualAudioLab->minWaveformValMono = (sint8)theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index];
        }
        if (theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index] > theVisualAudioLab->maxWaveformValMono) {
            theVisualAudioLab->maxWaveformValMono = (sint8)theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index];
        }
    }

    theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory] /= numWaveformEntries;
    theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory] = sqrt(theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory]);
    //sprintf(infoStr, "%4.4f",theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory]);
    //setProcessInfo ("rms_mono", infoStr);

	/*
    if (theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory] > theVisualAudioLab->rmsMonoMax) {
        theVisualAudioLab->rmsMonoMax = theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory];
    }
	*/


	theVisualAudioLab->rmsBuffer[theVisualAudioLab->currRMSValue].rmsValue = (float)theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory];
	theVisualAudioLab->rmsBuffer[theVisualAudioLab->currRMSValue].timeInMillisecondsSinceStartOfTrack = elapsedMillisecondsSinceStartOfTrack;
	theVisualAudioLab->currRMSValue++;
	if (theVisualAudioLab->currRMSValue == numberOfRMSBufferEntries) {
		theVisualAudioLab->currRMSValue = 0;
	}
	theVisualAudioLab->rmsMonoMax = 0.0;
	aRMSMonoValSum = 0.0;
	aRMSCount = 0;
	for (index = 0; index < numberOfRMSBufferEntries; index++) {
		if ((theVisualAudioLab->rmsBuffer[index].timeInMillisecondsSinceStartOfTrack > 0.0) && (theVisualAudioLab->rmsBuffer[index].timeInMillisecondsSinceStartOfTrack > (elapsedMillisecondsSinceStartOfTrack - numberOfMillisecondsOfRMSBuffer))) {
			if (theVisualAudioLab->rmsBuffer[index].rmsValue > theVisualAudioLab->rmsMonoMax) {
				theVisualAudioLab->rmsMonoMax = theVisualAudioLab->rmsBuffer[index].rmsValue;
			}
			aRMSMonoValSum += theVisualAudioLab->rmsBuffer[index].rmsValue;
			aRMSCount++;
		}
	}
	theVisualAudioLab->rmsMonoAvg = (aRMSMonoValSum / aRMSCount);
	
    //theVisualAudioLab->rmsMonoAvg = (theVisualAudioLab->rmsMonoAvgSum + theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory]) / theVisualAudioLab->rmsMonoAvgCount;
	//sprintf(infoStr, "%10.3lf", theVisualAudioLab->rmsMonoAvg);
	//setProcessInfo ("rmsMono avg", infoStr);	
    //theVisualAudioLab->rmsMonoAvgSum = theVisualAudioLab->rmsMonoAvgSum + theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory];
    //sprintf(infoStr, "%f",theVisualAudioLab->rmsMonoAvg);
    //setProcessInfo ("rms_mono_avg", infoStr);
    
    //theVisualAudioLab->amplitudeEnvelope[tappingIdx] += double(waveformVal);

    // tapping
    tappingStartSampleNum = tappingBufferHighWatermarkIdx;
    for (index = 0; index < numWaveformEntries; index++) {
        theVisualAudioLab->tappingBuffer[tappingBufferHighWatermarkIdx] = static_cast<double>(theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index]);
        tappingBufferHighWatermarkIdx ++;
        numberOfTappingSamplesProcessed ++;
        if (tappingBufferHighWatermarkIdx == tappingBufferLength) {
            tappingBufferHighWatermarkIdx = 0;
        }
    }
    

    // sampleRate = ((float)VisualAudioLab::numberOfAudioWaveformDataEntries/(float)elapsedMilliseconds * 1000.0);
    //sampleRate = (float)VisualAudioLab::numberOfAudioWaveformDataEntries * (1000.0 / (float)elapsedMilliseconds);

    // try constant sampleRate 4000 (unsure)
    if (sampleRate > 4000) {
        numberOfSamplesToAnalyse = (uint16)((float)numWaveformEntries/((float)sampleRate/4000.0));
    } else {
        if (elapsedMilliseconds > 1000) {
            elapsedMilliseconds = 1000; // Maximum an Interpolation
        }
        numberOfSamplesToInterpolate =  (uint16)((4000.0 * ((float)elapsedMilliseconds) / 1000.0) - numWaveformEntries);
        
        //cout << "elapsedMilliseconds: " << elapsedMilliseconds << endl;
        //cout << numberOfSamplesToInterpolate << " samples fehlen zur konstanten sample rate 6000" << endl;

        // add lost samples

        // linear interpoliert (?)
        lastWaveVal = static_cast<double>(theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][numWaveformEntries-1]);
        interpolStepVal = (abs)((int)(lastWaveVal / numberOfSamplesToInterpolate));
        if (lastWaveVal > 0) {
            interpolStepVal = interpolStepVal*-1;
        }
        
        for (index = 0; index < numberOfSamplesToInterpolate; index++) {
            theVisualAudioLab->tappingBuffer[tappingBufferHighWatermarkIdx] = lastWaveVal + (interpolStepVal*(index+1));
            tappingBufferHighWatermarkIdx ++;
            numberOfTappingSamplesProcessed ++;
            if (tappingBufferHighWatermarkIdx == tappingBufferLength) {
                tappingBufferHighWatermarkIdx = 0;
            }
            
        }
        
        numberOfSamplesToAnalyse = numberOfTappingSamplesProcessed;
        
        //cout << "elapsed: " << elapsedMilliseconds << " nulls: " << numberOfSamplesToInterpolate << " count: " << numberOfSamplesToAnalyse << endl;
    }
    
    
    //numberOfSamplesToAnalyse = numberOfSamplesToAnalyse / 2; // pauschale Reduzierung der Datenmenge (?)
    
	double devianceNow;
	devianceNow = theVisualAudioLab->rmsMonoMax - theVisualAudioLab->rmsMonoAvg;
	//deviance /= 1.6;
	devianceNow = devianceNow / deviance;
	//sprintf(infoStr, "%lf", theVisualAudioLab->rmsMonoMax);
	//setProcessInfo ("rmsMonoMax", infoStr);
	//sprintf(infoStr, "%lf", theVisualAudioLab->rmsMonoAvg);
	//setProcessInfo ("rmsMonoAvg", infoStr);
	//if (theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory] > theVisualAudioLab->rmsMonoAvg) {
	milliSecsForBeat = VisualTiming::getElapsedMilliSecsSinceReset("BeatPulse");
	if (theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory] > (theVisualAudioLab->rmsMonoAvg + deviance)) {
		if (milliSecsForBeat > beatThreshold) {
			// maximum bpm: 1000 / beatThreshold * 60
			// so we wait beatThreshold milliseconds until we are free to detect a new beat
			hasBeatImpulse = true;
			deviance += 0.05;
			VisualTiming::resetTimestamp("BeatPulse");
			theVisualAudioLab->addToBeatRecords();
		}
	} else {
		if (milliSecsForBeat > 800) {
			// we must have missed a beat, so lower deviance
			deviance -= 0.05;
		}
	}
	
    storeBeatImpulse(hasBeatImpulse);
	
	theVisualAudioLab->calcBeatConfidence();
	
	
	sprintf(infoStr, "%d", theVisualAudioLab->getBestBeatMSInterval());
    setProcessInfo ("BestBeatGuess", infoStr);

}


void VisualAudioLab::addToBeatRecords() {
	uint32 elapsedMilliseconds;
    
    elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset("timeSinceStartOfTrack");
	theVisualAudioLab->beatsRecordedInBeatAnalyseTimeFrame[theVisualAudioLab->currRecordedBeat].timeInMillisecondsSinceStartOfTrack = elapsedMilliseconds;
	theVisualAudioLab->beatsRecordedInBeatAnalyseTimeFrame[theVisualAudioLab->currRecordedBeat].confidence = 0;
	theVisualAudioLab->currRecordedBeat++;
	if (theVisualAudioLab->currRecordedBeat == VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe) {
		theVisualAudioLab->currRecordedBeat = 0;
	}

}


void VisualAudioLab::resetBeatGapHistogram() {
	uint16 i;
	for (i = 0; i < VisualAudioLab::maxNumberOfBeatHistogramBins; i++) {
		theVisualAudioLab->beatGapHistogram[i] = 0;
	}
}


void VisualAudioLab::addToBeatGapHistogram(uint16 gapInMilliseconds) {
	uint8 beatGapHistogramIdx;
	
	beatGapHistogramIdx = (gapInMilliseconds / theVisualAudioLab->beatTimeTolerance);
	if (beatGapHistogramIdx > (VisualAudioLab::maxNumberOfBeatHistogramBins - 1)) return;
	theVisualAudioLab->beatGapHistogram[beatGapHistogramIdx]++;
}


void VisualAudioLab::calcBeatConfidence() {

	uint16 j;
	uint32 currBeatMSInterval, checkedBeatMSInterval;
	sint16 i, prevBeatIndex;

	for (i = 0; i < VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe; i++) {
		beatsRecordedInBeatAnalyseTimeFrame[i].confidence = 0;
	}

	for (i = 0; i < VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe; i++) {
		prevBeatIndex = i - 1;
		if (prevBeatIndex < 0) {
			prevBeatIndex = VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe;
		}
		currBeatMSInterval = beatsRecordedInBeatAnalyseTimeFrame[i].timeInMillisecondsSinceStartOfTrack - beatsRecordedInBeatAnalyseTimeFrame[prevBeatIndex].timeInMillisecondsSinceStartOfTrack;
		if ((currBeatMSInterval < maximumBeatInterval) && (currBeatMSInterval > beatTimeTolerance)) {
			for (j = 0; j < VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe; j++) {
				prevBeatIndex = j  - 1;
				if (prevBeatIndex < 0) {
					prevBeatIndex = VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe;
				}
				checkedBeatMSInterval = beatsRecordedInBeatAnalyseTimeFrame[j].timeInMillisecondsSinceStartOfTrack - beatsRecordedInBeatAnalyseTimeFrame[prevBeatIndex].timeInMillisecondsSinceStartOfTrack;
				if ((checkedBeatMSInterval < (currBeatMSInterval + beatTimeTolerance)) && (checkedBeatMSInterval > (currBeatMSInterval - beatTimeTolerance))) {

					beatsRecordedInBeatAnalyseTimeFrame[i].confidence++;

				}
			}
		}
	}

}


uint32 VisualAudioLab::getBestBeatMSInterval() {
	uint16 maxConfidence = 0;
	uint16 beatIndex = 0;
	uint32 bestBeatMSInterval;
	uint16 i;
	for (i = 0; i < VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe; i++) {
		if (beatsRecordedInBeatAnalyseTimeFrame[i].confidence > maxConfidence) {
			maxConfidence = beatsRecordedInBeatAnalyseTimeFrame[i].confidence;
			beatIndex = i;
		}
	}
	if (beatIndex == 0) {
		bestBeatMSInterval = beatsRecordedInBeatAnalyseTimeFrame[beatIndex].timeInMillisecondsSinceStartOfTrack - beatsRecordedInBeatAnalyseTimeFrame[VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe].timeInMillisecondsSinceStartOfTrack;
	} else {
		bestBeatMSInterval = beatsRecordedInBeatAnalyseTimeFrame[beatIndex].timeInMillisecondsSinceStartOfTrack - beatsRecordedInBeatAnalyseTimeFrame[beatIndex - 1].timeInMillisecondsSinceStartOfTrack;
	}
	return bestBeatMSInterval;
}


uint8 VisualAudioLab::isBeatMeter(uint32 elapsedMSSinceStartOfTrack) {
	uint16 confidenceVal = 0;
	uint16 maxConfidence = 0;
	uint16 minConfidence = 65000;
	for (uint8 index = 0; index < VisualAudioLab::maxNumberOfBeatsRecordedInAnalyseTimeframe; index++) {
		if (beatsRecordedInBeatAnalyseTimeFrame[index].timeInMillisecondsSinceStartOfTrack < (elapsedMSSinceStartOfTrack + 100)) {
			if (beatsRecordedInBeatAnalyseTimeFrame[index].timeInMillisecondsSinceStartOfTrack > (elapsedMSSinceStartOfTrack - 100)) {
				confidenceVal = beatsRecordedInBeatAnalyseTimeFrame[index].confidence;
			}
		}
		if (beatsRecordedInBeatAnalyseTimeFrame[index].confidence > maxConfidence) {
			maxConfidence = beatsRecordedInBeatAnalyseTimeFrame[index].confidence;
		}
		if (beatsRecordedInBeatAnalyseTimeFrame[index].confidence < minConfidence) {
			minConfidence = beatsRecordedInBeatAnalyseTimeFrame[index].confidence;
		}
	}
	//printf("confidenceVal: %u\n", confidenceVal);
	//printf("b: %f\n", ((float)(maxConfidence - minConfidence) / 2.0f));
	if (confidenceVal > (minConfidence + ((float)(maxConfidence - minConfidence) / 2.0f))) {
		return 1;
	} else {
		return 0;
	}
}


const uint32* const VisualAudioLab::getBeatHistogram() {
	return theVisualAudioLab->beatGapHistogram;
}


uint16 VisualAudioLab::getMaxNumberOfWaveformShapeHistory() {
	return VisualAudioLab::maxNumberOfWaveformShapeHistory;
}


void VisualAudioLab::setMaxNumberOfAudioDataChannels(uint16 aNumberOfMaxAudioDataChannels) {
	VisualAudioLab::maxNumberOfAudioDataChannels = aNumberOfMaxAudioDataChannels;
}


void VisualAudioLab::setNumberOfAudioWaveformDataEntries(uint32 aNumberOfAudioWaveformDataEntries) {
	VisualAudioLab::numberOfAudioWaveformDataEntries = aNumberOfAudioWaveformDataEntries;
}


void VisualAudioLab::setNumberOfAudioSpectrumDataEntries(uint32 aNumberOfAudioSpectrumDataEntries) {
	VisualAudioLab::numberOfAudioSpectrumDataEntries = aNumberOfAudioSpectrumDataEntries;
}


const sint16** const VisualAudioLab::getWaveformDataMonoArray() {
	return const_cast<const sint16** const>(theVisualAudioLab->waveformDataMono);
}


const uint8*** const VisualAudioLab::getSpectrumDataArray() {
	return const_cast<const uint8*** const>(theVisualAudioLab->spectrumData);
}


void VisualAudioLab::processSpectrumData(const uint8 numSpectrumChannels, const uint16 numSpectrumEntries, const uint8* const currSpectrumData) {
    uint16 index;
    sint32 channel;
    uint8 spectrumVal;

    for (channel = 0; channel < numSpectrumChannels; channel++) {

        for (index = 0; index < numSpectrumEntries; index++) {
                    
			spectrumVal = currSpectrumData[channel * numSpectrumEntries + index];

            theVisualAudioLab->spectrumData[theVisualAudioLab->currMusicDataHistory][channel][index] = spectrumVal;

            if (spectrumVal < theVisualAudioLab->minSpectrumLevel[channel]) {
                theVisualAudioLab->minSpectrumLevel[channel] = spectrumVal;
            } else if (spectrumVal > theVisualAudioLab->maxSpectrumLevel[channel]) {
                theVisualAudioLab->maxSpectrumLevel[channel] = spectrumVal;
            }
        }
    }

}


uint8 VisualAudioLab::getDCIntensity() {
    uint8 intensity;
    intensity = (uint8)(((float)theVisualAudioLab->spectrumDCLevel/255.0)*100.0);
    return intensity;
}


uint8 VisualAudioLab::getRMSIntensity() {
    uint8 intensity;
    intensity = (uint8)((theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory] / theVisualAudioLab->rmsMonoMax) * 100.0);
    return intensity;
}


uint8 VisualAudioLab::getWaveformDataOfChannelAtIndex(const uint16 channel, const uint16 index) {
    return theVisualAudioLab->waveformData[theVisualAudioLab->currMusicDataHistory][channel][index] + 128;
}


uint8 VisualAudioLab::getMonoWaveformDataAtIndex(const uint16 index) {
    return theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index] + 128;
}

/*
void VisualAudioLab::getMonoWaveformData(const uint8* const waveformArr, const uint8 numberOfHistories) {
    uint8 historyIndex;
    uint16 waveDataIndex;
    sint8 aHistory;
	if (numberOfHistories > VisualAudioLab::maxNumberOfMusicDataHistory) return;
    aHistory = theVisualAudioLab->currMusicDataHistory;
    for (historyIndex = 0; historyIndex < numberOfHistories; historyIndex++) {
        aHistory++;
        if (aHistory == VisualAudioLab::maxNumberOfMusicDataHistory) {
            aHistory = 0;
        }
        for (waveDataIndex = 0; waveDataIndex < VisualAudioLab::numberOfAudioWaveformDataEntries; waveDataIndex++) {
			waveformArr[aHistory * numberOfHistories + waveDataIndex] = theVisualAudioLab->waveformDataMono[aHistory][waveDataIndex];
			waveformArr[aHistory * numberOfHistories + waveDataIndex] += 128;
            // + 128 (?)
        }
    }
}
*/

uint8 VisualAudioLab::getWaveformDataOfChannelAtIndexWithHistoryNum(const uint16 channel, const uint16 index, const uint16 historyNum) {
    return theVisualAudioLab->waveformData[historyNum][channel][index]+128;
}


uint8 VisualAudioLab::getMonoWaveformDataAtIndexWithHistoryNum(const uint16 index, const uint16 historyNum) {
    return theVisualAudioLab->waveformDataMono[historyNum][index]+128;
}


void VisualAudioLab::HannWindow(const int pNumberOfWaveformEntries, double TimeSamples[]) {
    int i;
    //double PI = 4 * arctan(1);
    for (i = 0; i< pNumberOfWaveformEntries; i++) {
        TimeSamples[i] = TimeSamples[i] * 0.5 *(1 - cos((2*3.1415926535*i) / (pNumberOfWaveformEntries-1)));
    }
}


void VisualAudioLab::storeBeatImpulse(bool beatImpulse) {
    theVisualAudioLab->isBeatImpulse = beatImpulse;
	if (beatImpulse) {
		theVisualAudioLab->beatTimeDeltaInMS = VisualTiming::getElapsedMilliSecsSinceReset("beatTimestamp");
		VisualTiming::resetTimestamp("beatTimestamp");
	}
}


void VisualAudioLab::checkForBeatImpulse() {
	//if (theVisualAudioLab->isBeatImpulse) {
	if (theVisualAudioLab->getBeatMeter() == 1) {
		VisualNotification::post(kBeatImpulseEvt);
	}
}


uint8 VisualAudioLab::getBeatMeter() {
	uint32 elapsedMilliseconds;
	uint32 elapsedMillisecondsSinceLastBeatConfirmed;
	uint32 bestGuess;
	//char infoStr[32];
	elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset("timeSinceStartOfTrack");
	elapsedMillisecondsSinceLastBeatConfirmed = VisualTiming::getElapsedMilliSecsSinceReset("prevPositiveMeter");
	if (theVisualAudioLab->isBeatMeter(elapsedMilliseconds) != 0) {
		if (elapsedMillisecondsSinceLastBeatConfirmed > beatThreshold) {
			VisualTiming::resetTimestamp("prevPositiveMeter");
			return 1;
		}
	} else {
		bestGuess = theVisualAudioLab->getBestBeatMSInterval();
		if (elapsedMillisecondsSinceLastBeatConfirmed > bestGuess) {
			VisualTiming::resetTimestamp("prevPositiveMeter");
			return 1;
		}
	}
	return 0;
}

