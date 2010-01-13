/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAudioLab.h
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

#ifndef VisualAudioLab_h
#define VisualAudioLab_h


#include "VisualTypes.h"


namespace VizKit {

	/**
	 * A collection of routines dealing with processing, analyzing and interpretation of audio data.
	 */
	class VisualAudioLab {

	public:

		/**
		 * Returns the VisualAudioLab. The VisualAudiolab is initialized if required. 
		 * The VisualAudioLab is a singleton.
		 */
		static VisualAudioLab* getInstance(void);

		/**
		 * Disposes the VisualAudiolab.
		 */	
		static void dispose(void);
		
		/**
		 * Processes the audio data chunks.
		 * With each RenderMessage iTunes delivers waveform data and spectrum data. 
		 * The spectrum data is a fft (Fast Fourier Transform) of the waveform data.
		 * @param numWaveformChannels The number of waveform data channels.
		 * @param numWaveformEntries The number of waveform data entries per channel.
		 * @param currWaveformData A pointer to the waveform data.
		 * @param numSpectrumChannels The number of spectrum data channels.
		 * @param numSpectrumEntries The number of spectrum data entries per channel.
		 * @param currSpectrumData A pointer to the spectrum data.
		 */
		void processAudioData(uint8 numWaveformChannels, 
							uint16 numWaveformEntries, 
							const uint8* const currWaveformData, 
							uint8 numSpectrumChannels, 
							uint16 numSpectrumEntries, 
							const uint8* const currSpectrumData);

		/**
		 * Resets the stored values of the VisualAudioLab.
		 * The VisualAudioLab stores analyzed values of an audio track.
		 * Typically the values are resetted when a new track starts playing.
		 */	
		void resetData(void);

		/**
		 * Returns the last processed timestampID of the kVisualPluginRenderMessage.
		 * @return The last processed timestampID of the kVisualPluginRenderMessage.
		 */
		uint32 getCurrTimestampIDOfRenderData(void);
		
		/**
		 * Sets the last processed timestampID of the kVisualPluginRenderMessage.
		 * @param timestampID The last processed timestampID of the kVisualPluginRenderMessage.
		 */	
		void setCurrTimestampIDOfRenderData(const uint32 timestampID);
		
		/**
		 * Stores the total time of the current track.
		 * @param totalTimeInMS The total time of the currently playing audio track measured in milliseconds.
		 */
		void setTotalTimeOfCurrentTrack(const uint32 totalTimeInMS);

		/**
		 * Stores the start time of the current track.
		 * @param startTimeInMS The start time of the currently playing audio track measured in milliseconds.
		 */
		void setStartTimeOfCurrentTrack(const uint32 startTimeInMS);

		/**
		 * Stores the stop time of the current track.
		 * @param stopTimeInMS The stop time of the currently playing audio track measured in milliseconds.
		 */
		void setStopTimeOfCurrentTrack(const uint32 stopTimeInMS);

		/**
		 * Answers the question whether the remaining time of the current track is known.
		 * Streamed audio has no info about remaining time of current track.
		 * @return True if the remaining time of the current track is known, false if not.
		 */
		bool remainingTimeOfCurrentTrackIsKnown(void);
		
		/**
		 * Returns the remaining time of the currently playing audio track measured in milliseconds.
		 * @return The remaining time of the currently playing audio track measured in milliseconds.
		 */
		uint32 getRemainingTimeOfCurrentTrack(void);

		/**
		 * Returns the total time of the currently playing audio track measured in milliseconds.
		 * @return The total time of the currently playing audio track measured in milliseconds.
		 */	
		uint32 getTotalTimeOfCurrentTrack(void);

		/**
		 * Returns the current music data history.
		 * @return The current music data history.
		 */	
		sint16 getCurrMusicDataHistory(void);

		/**
		 * Returns the previous music data history. 
		 * The previous music data history is the one that was current 
		 * before the one that is now current.
		 * @return The previous music data history.
		 */	
		sint16 getPrevMusicDataHistory(void);

		/**
		 * Returns the RMS (root-mean-square) intensity.
		 * The RMS intensity is the computed root-mean-square value.
		 * @return The RMS intensity.
		 */	
		uint8 getRMSIntensity(void);

		/**
		 * Returns the single waveform data of a specific channel at a specific index position.
		 * @param channel The channel.
		 * @param index The index of the waveform data.
		 * @return The single waveform data of a specific channel at a specific index position.
		 */	
		uint8 getWaveformDataOfChannelAtIndex(const uint16 channel, const uint16 index);

		/**
		 * Returns a pointer to the spectrum data array.
		 * @return A pointer to the spectrum data array.
		 */
		const uint8*** const getSpectrumDataArray(void);

		/**
		 * Returns a pointer to the mono waveform data array.
		 * The mono waveform data consists of the averaged values of the 
		 * waveform data of all available channels.
		 * @return A pointer to the mono waveform data array.
		 */
		const sint16** const getWaveformDataMonoArray(void);

		/**
		 * Returns the waveform data value of a specific index position.
		 * @param index The index of the requested waveform data.
		 * @return The mono waveform data at a specific index.
		 */
		uint8 getMonoWaveformDataAtIndex(const uint16 index);

		/**
		 * Returns the waveform data of a specific channel of a specific history at a specific index.
		 * @param channel The cannel of the requested waveform data.
		 * @param index The index of the requested waveform data.
		 * @param historyNum The history of the requested waveform data.
		 * @return The waveform data of a specific channel of a specific history at a specific index.
		 */
		uint8 getWaveformDataOfChannelAtIndexWithHistoryNum(const uint16 channel, const uint16 index, const uint16 historyNum);

		/**
		 * Returns the mono waveform data of a specific history at a specific index.
		 * @param index The index of the requested waveform data.
		 * @param historyNum The history of the requested waveform data.
		 * @return The mono waveform data of a specific history at a specific index.
		*/
		uint8 getMonoWaveformDataAtIndexWithHistoryNum(const uint16 index, const uint16 historyNum);

		/**
		 * Returns the DC intensity.
		 * The DC intensity is the intensity of the direct current.
		 * @return The DC intensity.
		 */
		uint8 getDCIntensity(void);

		/**
		 * Checks for the beat impulse and sends a notification in case a beat impulse is detected.
		 */
		void checkForBeatImpulse(void);

		/**
		 * Answers the question whether the current timestamp suggests a beat impulse.
		 * @return 1 if the current timestamp suggests a beat impulse, 0 if not.
		 */
		uint8 getBeatMeter(void);

		/**
		 * Processes the sample data with a Hann window.
		 * @param N The number of samples.
		 * @param TimeSamples The sample data.
		 */
		void HannWindow(const int N, double TimeSamples[]);

		/**
		 * Returns a pointer to the waveform shape array.
		 * @return A pointer to the waveform shape array.
		 */	
		const sint8* const getWaveformShapeArray(void);

		/**
		 * Sets the maximum number of audio data channels (2 channels means stereo).
		 * @param aNumberOfMaxAudioDataChannels The maximum number of audio data channels.
		 */
		static void setMaxNumberOfAudioDataChannels(uint16 aNumberOfMaxAudioDataChannels);

		/**
		 * Sets the number of audio waveform data entries.
		 * @param aNumberOfAudioWaveformDataEntries The number of audio waveform data entries.
		 */
		static void setNumberOfAudioWaveformDataEntries(uint32 aNumberOfAudioWaveformDataEntries);

		/**
		 * Sets the number of audio spectrum data entries.
		 * @param aNumberOfAudioSpectrumDataEntries The number of audio spectrum data entries.
		 */
		static void setNumberOfAudioSpectrumDataEntries(uint32 aNumberOfAudioSpectrumDataEntries);

		/**
		 * Returns the number of audio spectrum data entries.
		 * @return The number of audio spectrum data entries.
		 */
		uint32 getNumberOfAudioSpectrumDataEntries(void);

		/**
		 * Returns the number of waveform data entries.
		 * @return The number of waveform data entries.
		 */		
		uint32 getNumberOfWaveformEntries(void);

		/**
		 * Returns the number of spectrum data entries.
		 * @return The number of spectrum data entries.
		 */	
		uint32 getNumberOfSpectrumEntries(void);

		/**
		 * Returns the maximum history number.
		 * @return The maximum history number.
		 */	
		uint16 getMaxNumberOfWaveformShapeHistory(void);

		/**
		 * Returns the number of data channels.
		 * @return The number of data channels.
		 */	
		uint16 getNumberOfDataChannels(void);

		/**
		 * Returns the maximum number of histories in waveform data.
		 * @return The maximum number of histories in waveform data.
		 */	
		uint16 getMaxNumberOfMusicDataHistories(void);

		/**
		 * Returns a pointer to the histogram of the gaps between detected beat events.
		 * @return A pointer to the histogram of the gaps between detected beat events.
		 */	
		const uint32* const getBeatHistogram(void);
		
	private:

		/** The constructor. VisualAudioLab is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualAudioLab();

		/** The destructor. VisualAudioLab is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualAudioLab();

		/**
		 * Copy constructor.
		 * @param other Another VisualAudioLab.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualAudioLab(const VisualAudioLab& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualAudioLab& operator=(const VisualAudioLab& other);

		/** VisualAudioLab is a singleton class. Pointer to private instance is handled internally. */
		static VisualAudioLab* theVisualAudioLab;

		/**
		 * Allocates and initializes the buffers of the VisualAudioLab.
		 */	
		void init(void);

		/**
		 * Processes the waveform data.
		 * @param numWaveformChannels The number of waveform data channels (e.g. two for stereo sound).
		 * @param numWaveformEntries The number of data points per channel.
		 * @param currWaveformData A pointer to the current waveform data.
		 */
		void processWaveformData(uint8 numWaveformChannels, uint16 numWaveformEntries, const uint8* const currWaveformData);

		/**
		 * Processes the spectrum data.
		 * @param numSpectrumChannels The number of spectrum data channels (e.g. two for stereo sound)..
		 * @param numSpectrumEntries The number of data points per channel.
		 * @param currSpectrumData A pointer to the current spectrum data.
		 */
		void processSpectrumData(const uint8 numSpectrumChannels, const uint16 numSpectrumEntries, const uint8* const currSpectrumData);
		
		/**
		 * Increments the current history number of music data.
		 * Music data is stored for a number of histories.
		 */		
		void incrementMusicDataHistory(void);

		/**
		 * Stores the beat impulse.
		 * @param beatImpulse True if the current waveform data indicates a beat impulse, false if not.
		 */
		void storeBeatImpulse(bool beatImpulse);

		/** The total time of the currently playing audio track measured in milliseconds. */
		uint32 totalTimeOfTrack;

		/** The accumulated play time of the current audio track measured in milliseconds. */
		uint32 elapsedTimeOfTrack;

		/** The start time of the currently playing audio track measured in milliseconds. iTunes allows to set a custom start time for a track. */
		uint32 startTimeOfTrack;

		/** The stop time of the currently playing audio track measured in milliseconds. iTunes allows to set a custom stop time for a track. */
		uint32 stopTimeOfTrack;

		/** The last processed timestampID of the kVisualPluginRenderMessage. */
		uint32 currTimestampIDOfRenderData;

		/** The number of music data history. */
		sint8 numberOfMusicDataHistoryInt;

		/** The maximum number of audio data channels (2 channels means stereo). */
		static uint16 maxNumberOfAudioDataChannels;
		
		/** The number of audio waveform data entries. */
		static uint32 numberOfAudioWaveformDataEntries;
		
		/** The number of audio spectrum data entries. */
		static uint32 numberOfAudioSpectrumDataEntries;

		/**
		 * Adds the minimum and maximum waveform data value to the waveform shape.
		 * @param minWaveformPoint The minimum waveform data value.
		 * @param maxWaveformPoint The maximum waveform data value.
		 */
		void addToWaveformShape(sint8 minWaveformPoint, sint8 maxWaveformPoint);
		
		/**
		 * Adds a value to beatsRecordedInBeatAnalyseTimeFrame.
		 */
		void addToBeatRecords(void);
		
		/**
		 * Adds a value to beatGapHistogram.
		 * @param gapInMilliseconds The gap to previous beat detection in milliseconds.
		 */
		void addToBeatGapHistogram(uint16 gapInMilliseconds);
		
		/**
		 * Resets the beatGapHistogram.
		 */
		void resetBeatGapHistogram(void);
		
		/**
		 * Calculates the confidence of the beat estimation.
		 */
		void calcBeatConfidence(void);
		
		/**
		 * Returns the best guess for the beat interval in milliseconds.
		 * The best guess for the beat interval in milliseconds.
		 */
		uint32 getBestBeatMSInterval(void);
		
		/** 
		 * The waveform data array. 
		 * The waveform data array is 3-dimensional with data values, channels and history.
		 */
		//sint8 waveformData[maxNumberOfMusicDataHistory][kVisualMaxDataChannels][kVisualNumWaveformEntries];
		sint8*** waveformData;
		
		/** 
		 * The mono waveform data array. 
		 * The mono waveform data array is 2-dimensional with data values, and history.
		 */
		//sint16 waveformDataMono[maxNumberOfMusicDataHistory][kVisualNumWaveformEntries];
		sint16** waveformDataMono;
		
		/** 
		 * The spectrum data array. 
		 * The spectrum data array is 3-dimensional with data values, channels and history.
		 */
		//uint8 spectrumData[maxNumberOfMusicDataHistory][kVisualMaxDataChannels][kVisualNumSpectrumEntries];
		uint8*** spectrumData;
		
		/** The amplitude envelope array. */
		//double amplitudeEnvelope[kVisualNumWaveformEntries];
		double* amplitudeEnvelope;
		
		/** The mean values. */
		//SInt32 mean[kVisualMaxDataChannels];
		sint32* mean;
		
		/** A special buffer for beat detection. */
		//double tappingBuffer[tappingBufferLength];
		double* tappingBuffer;
		
		/** The rms values. */
		//double rms[maxNumberOfMusicDataHistory][kVisualMaxDataChannels];
		double** rms;
		
		/** The mono rms values. */
		//double rmsMono[maxNumberOfMusicDataHistory];
		double* rmsMono;
		
		/** The average rms value. */
		double rmsMonoAvg;
		
		/** A histogram of the gaps between detected beat events. */
		//uint32 beatGapHistogram[maxNumberOfBeatHistogramBins];
		uint32* beatGapHistogram;
		
		/** The delta between beats. */
		uint32 beatTimeDeltaInMS;
		
		/** The maximum mono rms value. Timeframe numberOfMillisecondsOfRMSBuffer. */
		double rmsMonoMax;
		
		/** The current history number. */
		sint16 currMusicDataHistory;
		
		/** The minimum spectrum level. 0-128. */
		//uint8 minSpectrumLevel[kVisualMaxDataChannels];
		uint8* minSpectrumLevel;
		
		/** The maximum spectrum level. 0-128. */
		//uint8 maxSpectrumLevel[kVisualMaxDataChannels];
		uint8* maxSpectrumLevel;
		
		/** The spectrum DC level. 0-256 (?). Direct Current of FFT (mono). */
		uint8 spectrumDCLevel;
		
		/** The average spectrum DC level. */
		float avgSpectrumDCLevel;
		
		/** The sum of the spectrum DC level. sum of data (for calc of avg). */
		float spectrumDCLevelSum;
		
		/** The minimum waveform data value. 0-255. -128 ...127. */
		//sint8 minWaveformVal[kVisualMaxDataChannels];
		sint8* minWaveformVal;
		
		/** The maximum waveform data value. 0-255. -128 ...127. */
		//sint8 maxWaveformVal[kVisualMaxDataChannels];
		sint8* maxWaveformVal;
		
		/** The minimum mono waveform data value. */
		sint8 minWaveformValMono;
		
		/** The maximum mono waveform data value. */
		sint8 maxWaveformValMono;
		
		/** The sum of the absolute waveform data values that are higher or lower than 128. */
		//uint32 waveformValSum[kVisualMaxDataChannels];
		uint32* waveformValSum;

		/** The peak values of the sum of the waveform data values. */
		//uint32 peakWaveformValSum[kVisualMaxDataChannels];
		uint32* peakWaveformValSum;

		/** The previous sum of the waveform data values. */
		//uint32 prevWaveformValSum[kVisualMaxDataChannels];
		uint32* prevWaveformValSum;

		/** The average value of the sum of the waveform data values. */
		//uint32 avgWaveformValSum[kVisualMaxDataChannels];
		uint32* avgWaveformValSum;

		/** The counter for the calculation o the average value of the sum of the waveform data values. */
		//uint32 avgWaveformValSumCounter[kVisualMaxDataChannels];
		uint32* avgWaveformValSumCounter;

		/** 1 if the current waveform data indicates a beat impulse, 0 if not. */
		bool isBeatImpulse;

		/** The smallest time a beat is considered to be the same. Measured in milliseconds. */
		static const uint8 beatTimeTolerance;
		
		/** The threshold after which we are free to detect a new beat. Measured in milliseconds. */
		static const uint16 beatThreshold;
		
		/** The time frame in which beats are recorded and analysed. Measured in milliseconds. */
		static const uint16 beatAnalyseTimeFrame;
		
		/** The maximum beat interval. Measured in milliseconds. */
		static const uint16 maximumBeatInterval;
		
		/** The number of milliseconds rms values are collected. */
		static const uint16 numberOfMillisecondsOfRMSBuffer;
		
		/** 
		 * The number of records in rmsBuffer. 
		 * Must be greater than: Audio sample rate (44100) / kVisualNumWaveformEntries * (numberOfMillisecondsOfRMSBuffer / 1000).
		 */
		static const uint16 numberOfRMSBufferEntries;
		
		static const uint16 maxNumberOfWaveformShapeHistory;
		
		static const uint16 maxNumberOfMusicDataHistory;
		
		static const uint16 tappingBufferLength;
		
		static const uint16 maxNumberOfBeatHistogramBins;
		
		static const uint16 maxNumberOfBeatsRecordedInAnalyseTimeframe;
		
		/**
		 * Struct with values concerning rms (root mean square) values.
		 */
		typedef struct {
			float rmsValue; /**< The rms value. */
			uint32 timeInMillisecondsSinceStartOfTrack; /**< The number of milliseconds elapsed since start of audio track. */
		} RMSValue;
		
		/** 
		 * Array containg rms mono values for the last numberOfMillisecondsOfRMSBuffer milliseconds. Ringbuffer.
		 * RMSValues with ((currentMSSinceStartOfTrack - timeInMillisecondsSinceStartOfTrack) > numberOfMillisecondsOfRMSBuffer) are discarded.
		 */
		RMSValue rmsBuffer[2000];

		/** The current index of array rmsBuffer. */
		uint16 currRMSValue;

		/** The current index of array beatsRecordedInBeatAnalyseTimeFrame. */
		uint8 currRecordedBeat;

		/**
		 * Struct with values concerning beat estimation.
		 */
		typedef struct {
			uint32 timeInMillisecondsSinceStartOfTrack; /**< The number of milliseconds elapsed since start of audio track. */
			uint16 confidence; /**< The strength of the beatGuess. */
		} BeatGuess;

		/** Array in which the detected beats are stored with their elapsed milliseconds timestamp since start of track. Ringbuffer. */
		//BeatGuess beatsRecordedInBeatAnalyseTimeFrame[maxNumberOfBeatsRecordedInAnalyseTimeframe];
		BeatGuess* beatsRecordedInBeatAnalyseTimeFrame;

		/**
		 * 
		 * @param elapsedMSSinceStartOfTrack The number of milliseconds elapsed since start of audio track.
		 * @return 1 if time is in frame of beat meter, 0 if not.
		 */
		uint8 isBeatMeter(uint32 elapsedMSSinceStartOfTrack);
		
	};

}

#endif /* VisualAudioLab_h */


