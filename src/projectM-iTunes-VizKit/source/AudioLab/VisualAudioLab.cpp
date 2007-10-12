/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualAudioLab.cpp
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

#include "VisualAudioLab.h"
#include "VisualPlayerState.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"
#include "VisualTiming.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>



using namespace VizKit;


VisualAudioLab* VisualAudioLab::theVisualAudioLab = NULL; // singleton

UInt16 VisualAudioLab::maxNumberOfAudioDataChannels = 0;
UInt32 VisualAudioLab::numberOfAudioWaveformDataEntries = 0;
UInt32 VisualAudioLab::numberOfAudioSpectrumDataEntries = 0;

const UInt8 VisualAudioLab::beatTimeTolerance = 25;
const UInt16 VisualAudioLab::beatThreshold = 300;
const UInt16 VisualAudioLab::beatAnalyseTimeFrame = 10000;
const UInt16 VisualAudioLab::maximumBeatInterval = 2000;
const UInt16 VisualAudioLab::numberOfMillisecondsOfRMSBuffer = 3000;
const UInt16 VisualAudioLab::numberOfRMSBufferEntries = 2000;

// to-do: rework VisualAudioLab functionality (incl. beat detection)

VisualAudioLab::VisualAudioLab() {

    currMusicDataHistory = -1;
    rmsMonoAvg = 0.0;
    rmsMonoMax = 0.0;

	totalTimeOfTrack = 0;
	startTimeOfTrack = 0;
	stopTimeOfTrack = 0;
	elapsedTimeOfTrack = 0;
	currTimestampIDOfRenderData = 0;

	MAXNUMBEROFMUSICDATAHISTORY = 40;
	
}


VisualAudioLab::~VisualAudioLab() {
	UInt32 i, k;
    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
			free(waveformData[i][k]);
        }
		free(waveformData[i]);
    }
	free(waveformData);

    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
		free(waveformDataMono[i]);
    }
	free(waveformDataMono);

    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
			free(spectrumData[i][k]);
        }
		free(spectrumData[i]);
    }
	free(spectrumData);

    for (i = 0; i < MAXNUMBEROFWAVEFORMSHAPEHISTORY; i++) {
		free(waveformShape[i]);
    }
	free(waveformShape);
	
	free(amplitudeEnvelope);

	free(mean);
	
	free(tappingBuffer);

    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
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
	UInt32 i;
    UInt32 k;

	// waveformData
	waveformData = (SInt8***)malloc(MAXNUMBEROFMUSICDATAHISTORY * sizeof(SInt8**));
    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
		waveformData[i] = (SInt8**)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(SInt8*));
    }
    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
			waveformData[i][k] = (SInt8*)malloc(VisualAudioLab::numberOfAudioWaveformDataEntries * sizeof(SInt8));
			for (UInt32 m = 0; m < VisualAudioLab::numberOfAudioWaveformDataEntries; m++) {
				waveformData[i][k][m] = 0;
			}
		}
    }
	
	// waveformDataMono
	waveformDataMono = (SInt16**)malloc(MAXNUMBEROFMUSICDATAHISTORY * sizeof(SInt16*));
    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
		waveformDataMono[i] = (SInt16*)malloc(VisualAudioLab::numberOfAudioWaveformDataEntries * sizeof(SInt16));
		for (k = 0; k < VisualAudioLab::numberOfAudioWaveformDataEntries; k++) {
			waveformDataMono[i][k] = 0;
		}
	}

	// spectrumData
	spectrumData = (UInt8***)malloc(MAXNUMBEROFMUSICDATAHISTORY * sizeof(UInt8**));
    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
		spectrumData[i] = (UInt8**)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(UInt8*));
    }
    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
			spectrumData[i][k] = (UInt8*)malloc(VisualAudioLab::numberOfAudioSpectrumDataEntries * sizeof(UInt8));
			for (UInt32 m = 0; m < VisualAudioLab::numberOfAudioSpectrumDataEntries; m++) {
				spectrumData[i][k][m] = 0;
			}
		}
    }

	// waveformShape
	waveformShape = (SInt8**)malloc(MAXNUMBEROFWAVEFORMSHAPEHISTORY * 2 * sizeof(SInt8*));
    for (i = 0; i < MAXNUMBEROFWAVEFORMSHAPEHISTORY; i++) {
		waveformShape[i] = (SInt8*)malloc(2 * sizeof(SInt8));
    }

	// amplitudeEnvelope
	amplitudeEnvelope = (double*)malloc(VisualAudioLab::numberOfAudioWaveformDataEntries * sizeof(double));
	
	// mean
	mean = (SInt32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(SInt32));
	
	// tappingBuffer
	tappingBuffer = (double*)malloc(tappingBufferLength * sizeof(double));
	
	// rms
	rms = (double**)malloc(MAXNUMBEROFMUSICDATAHISTORY * sizeof(double*));
    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
		rms[i] = (double*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(double));
    }
	
	// rmsMono
	rmsMono = (double*)malloc(MAXNUMBEROFMUSICDATAHISTORY * sizeof(double));
	
	// beatGapHistogram
	beatGapHistogram = (UInt32*)malloc(MAXNUMBEROFBEATHISTOGRAMBINS * sizeof(UInt32));

	// minSpectrumLevel
	minSpectrumLevel = (UInt8*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(UInt8));
	
	// maxSpectrumLevel
	maxSpectrumLevel = (UInt8*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(UInt8));
	
	// minWaveformVal
	minWaveformVal = (SInt8*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(SInt8));

	// maxWaveformVal
	maxWaveformVal = (SInt8*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(SInt8));
	
	// waveformValSum
	waveformValSum = (UInt32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(UInt32));
	
	// peakWaveformValSum
	peakWaveformValSum = (UInt32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(UInt32));

	// prevWaveformValSum
	prevWaveformValSum = (UInt32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(UInt32));

	// avgWaveformValSum
	avgWaveformValSum = (UInt32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(UInt32));

	// avgWaveformValSumCounter
	avgWaveformValSumCounter = (UInt32*)malloc(VisualAudioLab::maxNumberOfAudioDataChannels * sizeof(UInt32));

    for (i = 0; i < VisualAudioLab::maxNumberOfAudioDataChannels; i++) {
        avgWaveformValSumCounter[i] = 0;
        minWaveformVal[i] = 0;
        maxWaveformVal[i] = 0;
        waveformValSum[i] = 0;
    }
    for (i = 0; i < (UInt32)MAXNUMBEROFMUSICDATAHISTORY; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
            rms[i][k] = 0;
        }
    }
}


void VisualAudioLab::resetVisualAudioLabData(void) {
	
	UInt16 i,k,m;

	theVisualAudioLab->totalTimeOfTrack = 0;
	theVisualAudioLab->startTimeOfTrack = 0;
	theVisualAudioLab->stopTimeOfTrack = 0;
	theVisualAudioLab->currTimestampIDOfRenderData = 0;

    for (i = 0; i < MAXNUMBEROFMUSICDATAHISTORY; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
            for (m = 0; m < VisualAudioLab::numberOfAudioWaveformDataEntries; m++) {
                theVisualAudioLab->waveformData[i][k][m] = 0;
            }
        }
    }

    for (i = 0; i < MAXNUMBEROFMUSICDATAHISTORY; i++) {
		for (m = 0; m < VisualAudioLab::numberOfAudioWaveformDataEntries; m++) {
			theVisualAudioLab->waveformDataMono[i][m] = 0;
		}
    }

    for (i = 0; i < MAXNUMBEROFMUSICDATAHISTORY; i++) {
        for (k = 0; k < VisualAudioLab::maxNumberOfAudioDataChannels; k++) {
            for (m = 0; m < VisualAudioLab::numberOfAudioSpectrumDataEntries; m++) {
                theVisualAudioLab->spectrumData[i][k][m] = 0;
            }
        }
    }
    
    // zero waveformShape
    for (i = 0; i < MAXNUMBEROFWAVEFORMSHAPEHISTORY; i++) {
        theVisualAudioLab->waveformShape[i][0] = 0;
        theVisualAudioLab->waveformShape[i][1] = 0;
    }

    // other statistics
    for (i = 0; i < MAXNUMBEROFMUSICDATAHISTORY; i++) {
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

	for (i = 0; i < MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME; i++) {
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


void VisualAudioLab::setTotalTimeOfCurrentTrack(const UInt32 totalTimeInMS) {
	theVisualAudioLab->totalTimeOfTrack = totalTimeInMS;
}


void VisualAudioLab::setStartTimeOfCurrentTrack(const UInt32 startTimeInMS) {
	theVisualAudioLab->startTimeOfTrack = startTimeInMS;
}


void VisualAudioLab::setStopTimeOfCurrentTrack(const UInt32 stopTimeInMS) {
	theVisualAudioLab->stopTimeOfTrack = stopTimeInMS;
}


UInt32 VisualAudioLab::getCurrTimestampIDOfRenderData() {
	return theVisualAudioLab->currTimestampIDOfRenderData;
}


void VisualAudioLab::setCurrTimestampIDOfRenderData(const UInt32 timestampID) {
	char timestampHitStr[64];
	if (timestampID > theVisualAudioLab->currTimestampIDOfRenderData) {
		if (timestampID - theVisualAudioLab->currTimestampIDOfRenderData > 1) {
			sprintf(timestampHitStr, "%ld", timestampID - theVisualAudioLab->currTimestampIDOfRenderData);
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


UInt32 VisualAudioLab::getRemainingTimeOfCurrentTrack(bool audioIsPlaying) {
	if (theVisualAudioLab->totalTimeOfTrack == 0) {
		return 0;
	}

	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	UInt32 elapsedTime = theVisualPlayerState->getElapsedAudioTime();
	if (theVisualAudioLab->totalTimeOfTrack < elapsedTime) {
		return 0;
	}
	return theVisualAudioLab->totalTimeOfTrack - elapsedTime;
}


UInt32 VisualAudioLab::getTotalTimeOfCurrentTrack() {
	return theVisualAudioLab->totalTimeOfTrack;
}


SInt16 VisualAudioLab::getCurrMusicDataHistory() {
    return theVisualAudioLab->currMusicDataHistory;
}


UInt8 VisualAudioLab::getPrevMusicDataHistory() {
	if ((theVisualAudioLab->currMusicDataHistory - 1) < 0) {
		return MAXNUMBEROFMUSICDATAHISTORY - 1;
	} else {
		return theVisualAudioLab->currMusicDataHistory - 1;
	};
}


UInt32 VisualAudioLab::getNumberOfWaveformEntries() {
	return VisualAudioLab::numberOfAudioWaveformDataEntries;
}


UInt32 VisualAudioLab::getNumberOfSpectrumEntries() {
	return VisualAudioLab::numberOfAudioSpectrumDataEntries;
}


UInt16 VisualAudioLab::getNumberOfDataChannels() {
	return VisualAudioLab::maxNumberOfAudioDataChannels;
}


UInt16 VisualAudioLab::getMaxNumberOfMusicDataHistories() {
	return (UInt16)MAXNUMBEROFMUSICDATAHISTORY;
}


void VisualAudioLab::incrementMusicDataHistory() {
    theVisualAudioLab->currMusicDataHistory++;
    if (theVisualAudioLab->currMusicDataHistory == MAXNUMBEROFMUSICDATAHISTORY) {
		// cycled
        theVisualAudioLab->currMusicDataHistory = 0;
    }
}


void VisualAudioLab::processAudioData(const UInt8 numWaveformChannels, 
										const UInt16 numWaveformEntries, 
										const UInt8* const currWaveformData, 
										const UInt8 numSpectrumChannels, 
										const UInt16 numSpectrumEntries, 
										const UInt8* const currSpectrumData) {

	theVisualAudioLab->incrementMusicDataHistory();
    theVisualAudioLab->processWaveformData(numWaveformChannels, numWaveformEntries, currWaveformData);
    theVisualAudioLab->processSpectrumData(numSpectrumChannels, numSpectrumEntries, currSpectrumData);
	
}


void VisualAudioLab::processWaveformData(const UInt8 numWaveformChannels, const UInt16 numWaveformEntries, const UInt8* const currWaveformData) {

    UInt16 index;
    UInt8 channel;
    UInt16 tappingIdx;
	UInt32 elapsedMillisecondsSinceStartOfTrack;
    UInt32 elapsedMilliseconds;
    UInt32 sampleRate;
    UInt16 numberOfSamplesToAnalyse;
    char infoStr[32];
    //char labelStr[32];
    UInt8 hasBeatImpulse = 0;
    SInt8 waveformVal; // -128 ..0.. 127
    static UInt16 tappingBufferHighWatermarkIdx = 0;
    UInt16 numberOfTappingSamplesProcessed = 0;
    UInt16 tappingStartSampleNum;
    UInt16 numberOfSamplesToInterpolate;
    double lastWaveVal;
    double interpolStepVal;
	UInt32 milliSecsForBeat;
	static double deviance = 2.1;
	double aRMSMonoValSum;
	UInt16 aRMSCount;
	
	elapsedMillisecondsSinceStartOfTrack = VisualTiming::getElapsedMilliSecsSinceReset("timeSinceStartOfTrack");

    elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceLastCall("processWaveformData");

    sampleRate = static_cast<UInt32>(static_cast<float>(VisualAudioLab::numberOfAudioWaveformDataEntries) * (1000.0 / static_cast<float>(elapsedMilliseconds)));
    sprintf(infoStr, "%ld",sampleRate);
    setProcessInfo ("Audio sampleRate", infoStr);
    

    // mean

    for (channel = 0; channel < numWaveformChannels; channel++) {
        for (index = 0; index < numWaveformEntries; index++) {
			theVisualAudioLab->mean[channel] += (abs)(currWaveformData[channel * numWaveformChannels + index] - 128);
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
        
			waveformVal = currWaveformData[channel * numWaveformChannels + index] - 128;

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

        //sprintf(infoStr, "%10.3lf", theVisualAudioLab->rms[theVisualAudioLab->currMusicDataHistory][channel]);
        //sprintf(labelStr, "rms_channel%u", channel);
        //setProcessInfo (labelStr, infoStr);
                
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
            theVisualAudioLab->minWaveformValMono = (SInt8)theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index];
        }
        if (theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index] > theVisualAudioLab->maxWaveformValMono) {
            theVisualAudioLab->maxWaveformValMono = (SInt8)theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index];
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

    theVisualAudioLab->addToWaveformShape(theVisualAudioLab->minWaveformValMono, theVisualAudioLab->maxWaveformValMono);
    
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
        numberOfSamplesToAnalyse = (UInt16)((float)numWaveformEntries/((float)sampleRate/4000.0));
    } else {
        if (elapsedMilliseconds > 1000) {
            elapsedMilliseconds = 1000; // Maximum an Interpolation
        }
        numberOfSamplesToInterpolate =  (UInt16)((4000.0 * ((float)elapsedMilliseconds) / 1000.0) - numWaveformEntries);
        
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
			hasBeatImpulse = 1;
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
	
	
	sprintf(infoStr, "%ld", theVisualAudioLab->getBestBeatMSInterval());
    setProcessInfo ("BestBeatGuess", infoStr);

}


void VisualAudioLab::addToBeatRecords() {
	UInt32 elapsedMilliseconds;
    
    elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset("timeSinceStartOfTrack");
	theVisualAudioLab->beatsRecordedInBeatAnalyseTimeFrame[theVisualAudioLab->currRecordedBeat].timeInMillisecondsSinceStartOfTrack = elapsedMilliseconds;
	theVisualAudioLab->beatsRecordedInBeatAnalyseTimeFrame[theVisualAudioLab->currRecordedBeat].confidence = 0;
	theVisualAudioLab->currRecordedBeat++;
	if (theVisualAudioLab->currRecordedBeat == MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME) {
		theVisualAudioLab->currRecordedBeat = 0;
	}

}


void VisualAudioLab::resetBeatGapHistogram() {
	UInt16 i;
	for (i = 0; i < MAXNUMBEROFBEATHISTOGRAMBINS; i++) {
		theVisualAudioLab->beatGapHistogram[i] = 0;
	}
}


void VisualAudioLab::addToBeatGapHistogram(UInt16 gapInMilliseconds) {
	UInt8 beatGapHistogramIdx;
	
	beatGapHistogramIdx = (gapInMilliseconds / theVisualAudioLab->beatTimeTolerance);
	if (beatGapHistogramIdx > (MAXNUMBEROFBEATHISTOGRAMBINS - 1)) return;
	theVisualAudioLab->beatGapHistogram[beatGapHistogramIdx]++;
}


void VisualAudioLab::calcBeatConfidence() {

	UInt16 j;
	UInt32 currBeatMSInterval, checkedBeatMSInterval;
	SInt16 i, prevBeatIndex;

	for (i = 0; i < MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME; i++) {
		beatsRecordedInBeatAnalyseTimeFrame[i].confidence = 0;
	}

	for (i = 0; i < MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME; i++) {
		prevBeatIndex = i - 1;
		if (prevBeatIndex < 0) {
			prevBeatIndex = MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME;
		}
		currBeatMSInterval = beatsRecordedInBeatAnalyseTimeFrame[i].timeInMillisecondsSinceStartOfTrack - beatsRecordedInBeatAnalyseTimeFrame[prevBeatIndex].timeInMillisecondsSinceStartOfTrack;
		if ((currBeatMSInterval < maximumBeatInterval) && (currBeatMSInterval > beatTimeTolerance)) {
			for (j = 0; j < MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME; j++) {
				prevBeatIndex = j  - 1;
				if (prevBeatIndex < 0) {
					prevBeatIndex = MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME;
				}
				checkedBeatMSInterval = beatsRecordedInBeatAnalyseTimeFrame[j].timeInMillisecondsSinceStartOfTrack - beatsRecordedInBeatAnalyseTimeFrame[prevBeatIndex].timeInMillisecondsSinceStartOfTrack;
				if ((checkedBeatMSInterval < (currBeatMSInterval + beatTimeTolerance)) && (checkedBeatMSInterval > (currBeatMSInterval - beatTimeTolerance))) {

					beatsRecordedInBeatAnalyseTimeFrame[i].confidence++;

				}
			}
		}
	}

}


UInt32 VisualAudioLab::getBestBeatMSInterval() {
	UInt16 maxConfidence = 0;
	UInt16 beatIndex = 0;
	UInt32 bestBeatMSInterval;
	UInt16 i;
	for (i = 0; i < MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME; i++) {
		if (beatsRecordedInBeatAnalyseTimeFrame[i].confidence > maxConfidence) {
			maxConfidence = beatsRecordedInBeatAnalyseTimeFrame[i].confidence;
			beatIndex = i;
		}
	}
	if (beatIndex == 0) {
		bestBeatMSInterval = beatsRecordedInBeatAnalyseTimeFrame[beatIndex].timeInMillisecondsSinceStartOfTrack - beatsRecordedInBeatAnalyseTimeFrame[MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME].timeInMillisecondsSinceStartOfTrack;
	} else {
		bestBeatMSInterval = beatsRecordedInBeatAnalyseTimeFrame[beatIndex].timeInMillisecondsSinceStartOfTrack - beatsRecordedInBeatAnalyseTimeFrame[beatIndex - 1].timeInMillisecondsSinceStartOfTrack;
	}
	return bestBeatMSInterval;
}


UInt8 VisualAudioLab::isBeatMeter(UInt32 elapsedMSSinceStartOfTrack) {
	UInt16 confidenceVal = 0;
	UInt16 maxConfidence = 0;
	UInt16 minConfidence = 65000;
	for (UInt8 index = 0; index < MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME; index++) {
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


const UInt32* const VisualAudioLab::getBeatHistogram() {
	return theVisualAudioLab->beatGapHistogram;
}


void VisualAudioLab::addToWaveformShape(SInt8 minWaveformPoint, SInt8 maxWaveformPoint) {
    static SInt16 currWaveformShapePoint = -1;

	currWaveformShapePoint++;
	if (currWaveformShapePoint == MAXNUMBEROFWAVEFORMSHAPEHISTORY) {
		currWaveformShapePoint = 0;
	}
	theVisualAudioLab->waveformShape[currWaveformShapePoint][0] = minWaveformPoint; // minWaveformPoint
	theVisualAudioLab->waveformShape[currWaveformShapePoint][1] = maxWaveformPoint; // maxWaveformPoint

}


UInt16 VisualAudioLab::getMaxNumberOfWaveformShapeHistory() {
	return MAXNUMBEROFWAVEFORMSHAPEHISTORY;
}


const SInt8* const VisualAudioLab::getWaveformShapeArray() {
	return *theVisualAudioLab->waveformShape;
}


void VisualAudioLab::setMaxNumberOfAudioDataChannels(UInt16 aNumberOfMaxAudioDataChannels) {
	VisualAudioLab::maxNumberOfAudioDataChannels = aNumberOfMaxAudioDataChannels;
}


void VisualAudioLab::setNumberOfAudioWaveformDataEntries(UInt32 aNumberOfAudioWaveformDataEntries) {
	VisualAudioLab::numberOfAudioWaveformDataEntries = aNumberOfAudioWaveformDataEntries;
}


void VisualAudioLab::setNumberOfAudioSpectrumDataEntries(UInt32 aNumberOfAudioSpectrumDataEntries) {
	VisualAudioLab::numberOfAudioSpectrumDataEntries = aNumberOfAudioSpectrumDataEntries;
}


const SInt16** const VisualAudioLab::getWaveformDataMonoArray() {
	return const_cast<const SInt16** const>(theVisualAudioLab->waveformDataMono);
}


const UInt8*** const VisualAudioLab::getSpectrumDataArray() {
	return const_cast<const UInt8*** const>(theVisualAudioLab->spectrumData);
}


void VisualAudioLab::processSpectrumData(const UInt8 numSpectrumChannels, const UInt16 numSpectrumEntries, const UInt8* const currSpectrumData) {
    UInt16 index;
    SInt32 channel;
    UInt8 spectrumVal;

    for (channel = 0; channel < numSpectrumChannels; channel++) {

        for (index = 0; index < numSpectrumEntries; index++) {
                    
			spectrumVal = currSpectrumData[channel * numSpectrumChannels + index];

            theVisualAudioLab->spectrumData[theVisualAudioLab->currMusicDataHistory][channel][index] = spectrumVal;

            if (spectrumVal < theVisualAudioLab->minSpectrumLevel[channel]) {
                theVisualAudioLab->minSpectrumLevel[channel] = spectrumVal;
            } else if (spectrumVal > theVisualAudioLab->maxSpectrumLevel[channel]) {
                theVisualAudioLab->maxSpectrumLevel[channel] = spectrumVal;
            }
        }
    }

}


UInt8 VisualAudioLab::getMinSpectrumLevel(const int channel) {
	return theVisualAudioLab->minSpectrumLevel[channel];
}


UInt8 VisualAudioLab::getMaxSpectrumLevel(const int channel) {
	return theVisualAudioLab->maxSpectrumLevel[channel];
}


UInt32 VisualAudioLab::getWaveformAvgValSum() {
    return theVisualAudioLab->avgWaveformValSum[0];
}


UInt32 VisualAudioLab::getWaveformValSum() {
    return theVisualAudioLab->waveformValSum[0];
}


UInt8 VisualAudioLab::getDCIntensity() {
    UInt8 intensity;
    intensity = (UInt8)(((float)theVisualAudioLab->spectrumDCLevel/255.0)*100.0);
    return intensity;
}


UInt8 VisualAudioLab::getRMSIntensity() {
    UInt8 intensity;
    intensity = (UInt8)((theVisualAudioLab->rmsMono[theVisualAudioLab->currMusicDataHistory] / theVisualAudioLab->rmsMonoMax) * 100.0);
    return intensity;
}


UInt8 VisualAudioLab::getWaveformDataOfChannelAtIndex(const UInt16 channel, const UInt16 index) {
    return theVisualAudioLab->waveformData[theVisualAudioLab->currMusicDataHistory][channel][index] + 128;
}


UInt8 VisualAudioLab::getMonoWaveformDataAtIndex(const UInt16 index) {
    return theVisualAudioLab->waveformDataMono[theVisualAudioLab->currMusicDataHistory][index] + 128;
}

/*
void VisualAudioLab::getMonoWaveformData(const UInt8* const waveformArr, const UInt8 numberOfHistories) {
    UInt8 historyIndex;
    UInt16 waveDataIndex;
    SInt8 aHistory;
	if (numberOfHistories > MAXNUMBEROFMUSICDATAHISTORY) return;
    aHistory = theVisualAudioLab->currMusicDataHistory;
    for (historyIndex = 0; historyIndex < numberOfHistories; historyIndex++) {
        aHistory++;
        if (aHistory == MAXNUMBEROFMUSICDATAHISTORY) {
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

UInt8 VisualAudioLab::getWaveformDataOfChannelAtIndexWithHistoryNum(const UInt16 channel, const UInt16 index, const UInt16 historyNum) {
    return theVisualAudioLab->waveformData[historyNum][channel][index]+128;
}


UInt8 VisualAudioLab::getMonoWaveformDataAtIndexWithHistoryNum(const UInt16 index, const UInt16 historyNum) {
    return theVisualAudioLab->waveformDataMono[historyNum][index]+128;
}


void VisualAudioLab::getAmplitudeEnvelope(double* waveformEnvelope) {
    UInt16 index;

    for (index = 0; index < VisualAudioLab::numberOfAudioWaveformDataEntries; index++) {
        waveformEnvelope[index] = theVisualAudioLab->amplitudeEnvelope[index];
    }
    
    //waveformEnvelope = theVisualAudioLab->amplitudeEnvelope;

}


void VisualAudioLab::HannWindow(const int pNumberOfWaveformEntries, double TimeSamples[]) {
    int i;
    //double PI = 4 * arctan(1);
    for (i = 0; i< pNumberOfWaveformEntries; i++) {
        TimeSamples[i] = TimeSamples[i] * 0.5 *(1 - cos((2*3.1415926535*i) / (pNumberOfWaveformEntries-1)));
    }
}


void VisualAudioLab::storeBeatImpulse(const UInt8 beatImpulse) {
    theVisualAudioLab->isBeatImpulse = beatImpulse;
	if (beatImpulse != 0) {
		theVisualAudioLab->beatTimeDeltaInMS = VisualTiming::getElapsedMilliSecsSinceReset("beatTimestamp");
		VisualTiming::resetTimestamp("beatTimestamp");
	}
}


UInt8 VisualAudioLab::getBeatImpulse() {
    return theVisualAudioLab->isBeatImpulse;
}


UInt8 VisualAudioLab::getBeatMeter() {
	UInt32 elapsedMilliseconds;
	UInt32 elapsedMillisecondsSinceLastBeatConfirmed;
	UInt32 bestGuess;
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

