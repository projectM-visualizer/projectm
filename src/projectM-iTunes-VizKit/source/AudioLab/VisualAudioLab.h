/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualAudioLab.h
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

#ifndef VisualAudioLab_h
#define VisualAudioLab_h

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


#define MAXNUMBEROFWAVEFORMSHAPEHISTORY 1400
#define tappingBufferLength 4096
#define MAXNUMBEROFBEATHISTOGRAMBINS 100
#define MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME 200


namespace VizKit {

	/**
	 * A collection of routines 
	 * dealing with processing, analyzing and interpreting audio data.
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
		 * Processes the audio data chunk.
		 * With each RenderMessage iTunes delivers waveform data and spectrum data. 
		 * The spectrum data is a fft (Fast Fourier Transform) of the waveform data.
		 * @param numWaveformChannels The number of waveform data channels.
		 * @param numWaveformEntries The number of waveform data entries per channel.
		 * @param currWaveformData A pointer to the waveform data.
		 * @param numSpectrumChannels The number of spectrum data channels.
		 * @param numSpectrumEntries The number of spectrum data entries per channel.
		 * @param currSpectrumData A pointer to the spectrum data.
		 */
		void processAudioData(const UInt8 numWaveformChannels, 
							const UInt16 numWaveformEntries, 
							const UInt8* const currWaveformData, 
							const UInt8 numSpectrumChannels, 
							const UInt16 numSpectrumEntries, 
							const UInt8* const currSpectrumData);

		/**
		 * Processes the waveform data.
		 * @param numWaveformChannels The number of waveform data channels (e.g.\ two for stereo sound).
		 * @param numWaveformEntries The number of data points per channel.
		 * @param currWaveformData A pointer to the current waveform data.
		 */
		void processWaveformData(const UInt8 numWaveformChannels, const UInt16 numWaveformEntries, const UInt8* const currWaveformData);

		/**
		 * Processes the spectrum data.
		 * @param numSpectrumChannels The number of spectrum data channels (e.g.\ two for stereo sound)..
		 * @param numSpectrumEntries The number of data points per channel.
		 * @param currSpectrumData A pointer to the current spectrum data.
		 */
		void processSpectrumData(const UInt8 numSpectrumChannels, const UInt16 numSpectrumEntries, const UInt8* const currSpectrumData);

		/**
		 * Resets the stored values of the VisualAudioLab.
		 * The VisualAudioLab stores analyzed values of an audio track.
		 * Typically the values are resetted when a new track starts playing.
		 */	
		void resetVisualAudioLabData(void);

		/**
		 * Returns the last processed timestampID of the kVisualPluginRenderMessage.
		 * @return The last processed timestampID of the kVisualPluginRenderMessage.
		 */
		UInt32 getCurrTimestampIDOfRenderData(void);
		
		/**
		 * Sets the last processed timestampID of the kVisualPluginRenderMessage.
		 * @param timestampID The last processed timestampID of the kVisualPluginRenderMessage.
		 */	
		void setCurrTimestampIDOfRenderData(const UInt32 timestampID);
		
		/**
		 * Stores the total time of the current track.
		 * @param totalTimeInMS The total time of the currently playing audio track measured in milliseconds.
		 */
		void setTotalTimeOfCurrentTrack(const UInt32 totalTimeInMS);

		/**
		 * Stores the start time of the current track.
		 * @param startTimeInMS The start time of the currently playing audio track measured in milliseconds.
		 */
		void setStartTimeOfCurrentTrack(const UInt32 startTimeInMS);

		/**
		 * Stores the stop time of the current track.
		 * @param stopTimeInMS The stop time of the currently playing audio track measured in milliseconds.
		 */
		void setStopTimeOfCurrentTrack(const UInt32 stopTimeInMS);

		/**
		 * Answers the question whether the remaining time of the current track is known.
		 * Streamed audio has no info about remaining time of current track.
		 * @return True if the remaining time of the current track is known, false if not.
		 */
		bool remainingTimeOfCurrentTrackIsKnown(void);
		
		/**
		 * Returns the remaining time of the currently playing audio track measured in milliseconds.
		 * @param audioIsPlaying True if audio is playing, false if not.
		 * @return The remaining time of the currently playing audio track measured in milliseconds.
		 */
		UInt32 getRemainingTimeOfCurrentTrack(bool audioIsPlaying);

		/**
		 * Returns the total time of the currently playing audio track measured in milliseconds.
		 * @return The total time of the currently playing audio track measured in milliseconds.
		 */	
		UInt32 getTotalTimeOfCurrentTrack(void);

		/**
		 * Increments the current history number of music data.
		 * Music data is stored for a number of histories.
		 */		
		void incrementMusicDataHistory(void);

		/**
		 * Returns the current music data history.
		 * @return The current music data history.
		 */	
		SInt16 getCurrMusicDataHistory(void);

		/**
		 * Returns the previous music data history. 
		 * The previous music data history is the one that was current 
		 * before the one that is now current.
		 * @return The previous music data history.
		 */	
		UInt8 getPrevMusicDataHistory(void);

		/**
		 * Returns the minimum spectrum level.
		 * @return The minum sprectrum level.
		 */	
		UInt8 getMinSpectrumLevel(const int channel);

		/**
		 * Returns the maximum spectrum level.
		 * @return The maximum sprectrum level.
		 */	
		UInt8 getMaxSpectrumLevel(const int channel);

		/**
		 * Returns the sum of all waveform data values.
		 * @return The sum of all waveform data values.
		 */	
		UInt32 getWaveformValSum(void);

		/**
		 * Returns the RMS (root-mean-square) intensity.
		 * The RMS intensity is the computed root-mean-square value.
		 * @return The RMS intensity.
		 */	
		UInt8 getRMSIntensity(void);

		/**
		 * Returns the single waveform data of a specific channel at a specific index position.
		 * @param channel The channel.
		 * @param index The index of the waveform data.
		 * @return The single waveform data of a specific channel at a specific index position.
		 */	
		UInt8 getWaveformDataOfChannelAtIndex(const UInt16 channel, const UInt16 index);

		/**
		 * Returns a pointer to the spectrum data array.
		 * @return A pointer to the spectrum data array.
		 */
		const UInt8*** const getSpectrumDataArray(void);

		/**
		 * Returns a pointer to the mono waveform data array.
		 * The mono waveform data consists of the averaged values of the 
		 * waveform data of all available channels.
		 * @return A pointer to the mono waveform data array.
		 */
		const SInt16** const getWaveformDataMonoArray(void);

		/**
		 * Returns the waveform data value of a specific index position.
		 * @param index The index of the requested waveform data.
		 * @return The mono waveform data at a specific index.
		 */
		UInt8 getMonoWaveformDataAtIndex(const UInt16 index);

		/**
		 * Returns the waveform data of a specific channel of a specific history at a specific index.
		 * @param channel The cannel of the requested waveform data.
		 * @param index The index of the requested waveform data.
		 * @param historyNum The history of the requested waveform data.
		 * @return The waveform data of a specific channel of a specific history at a specific index.
		 */
		UInt8 getWaveformDataOfChannelAtIndexWithHistoryNum(const UInt16 channel, const UInt16 index, const UInt16 historyNum);

		/**
		 * Returns the mono waveform data of a specific history at a specific index.
		 * @param index The index of the requested waveform data.
		 * @param historyNum The history of the requested waveform data.
		 * @return The mono waveform data of a specific history at a specific index.
		*/
		UInt8 getMonoWaveformDataAtIndexWithHistoryNum(const UInt16 index, const UInt16 historyNum);

		/**
		 * Returns a pointer to the waveform envelope.
		 * @param[out] waveformEnvelope A pointer to the waveform envelope.
		 */
		void getAmplitudeEnvelope(double* waveformEnvelope);

		/**
		 * Returns the sum of the average values of the waveform data.
		 * @return The sum of the average values of the waveform data.
		 */
		UInt32 getWaveformAvgValSum(void);

		/**
		 * Returns the DC intensity.
		 * The DC intensity is the intensity of the direct current.
		 * @return The DC intensity.
		 */
		UInt8 getDCIntensity(void);

		/**
		 * Stores the beat impulse.
		 * @param beatImpulse 1 if the current waveform data indicates a beat impulse, 0 if not.
		 */
		void storeBeatImpulse(const UInt8 beatImpulse);

		/**
		 * Answers the question whether the current waveform data indicates a beat impulse.
		 * @return 1 if the current waveform data indicates a beat impulse, 0 if not.
		 */
		UInt8 getBeatImpulse(void);

		/**
		 * Answers the question whether the current timestamp suggests a beat impulse.
		 * @return 1 if the current timestamp suggests a beat impulse, 0 if not.
		 */
		UInt8 getBeatMeter(void);

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
		const SInt8* const getWaveformShapeArray(void);

		/**
		 * Sets the maximum number of audio data channels (2 channels means stereo).
		 * @param aNumberOfMaxAudioDataChannels The maximum number of audio data channels.
		 */
		static void setMaxNumberOfAudioDataChannels(UInt16 aNumberOfMaxAudioDataChannels);

		/**
		 * Sets the number of audio waveform data entries.
		 * @param aNumberOfAudioWaveformDataEntries The number of audio waveform data entries.
		 */
		static void setNumberOfAudioWaveformDataEntries(UInt32 aNumberOfAudioWaveformDataEntries);

		/**
		 * Sets the number of audio spectrum data entries.
		 * @param aNumberOfAudioSpectrumDataEntries The number of audio spectrum data entries.
		 */
		static void setNumberOfAudioSpectrumDataEntries(UInt32 aNumberOfAudioSpectrumDataEntries);

		/**
		 * Returns the number of audio spectrum data entries.
		 * @return The number of audio spectrum data entries.
		 */
		UInt32 getNumberOfAudioSpectrumDataEntries(void);

		/**
		 * Returns the number of waveform data entries.
		 * @return The number of waveform data entries.
		 */		
		UInt32 getNumberOfWaveformEntries(void);

		/**
		 * Returns the number of spectrum data entries.
		 * @return The number of spectrum data entries.
		 */	
		UInt32 getNumberOfSpectrumEntries(void);

		/**
		 * Returns the maximum history number.
		 * @return The maximum history number.
		 */	
		UInt16 getMaxNumberOfWaveformShapeHistory(void);

		/**
		 * Returns the number of data channels.
		 * @return The number of data channels.
		 */	
		UInt16 getNumberOfDataChannels(void);

		/**
		 * Returns the maximum number of histories in waveform data.
		 * @return The maximum number of histories in waveform data.
		 */	
		UInt16 getMaxNumberOfMusicDataHistories(void);

		/**
		 * Returns a pointer to the histogram of the gaps between detected beat events.
		 * @return A pointer to the histogram of the gaps between detected beat events.
		 */	
		const UInt32* const getBeatHistogram(void);
		
	private:

		/** The constructor.\ VisualAudioLab is a singleton class.\ The constructor is private.\ New instance of class can only be created internally. */
		VisualAudioLab();

		/** The destructor.\ VisualAudioLab is a singleton class.\ The destructor is private.\ Instance of class can only be destructed internally. */
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

		/** The total time of the currently playing audio track measured in milliseconds. */
		UInt32 totalTimeOfTrack;

		/** The accumulated play time of the current audio track measured in milliseconds. */
		UInt32 elapsedTimeOfTrack;

		/** The start time of the currently playing audio track measured in milliseconds. iTunes allows to set a custom start time for a track. */
		UInt32 startTimeOfTrack;

		/** The stop time of the currently playing audio track measured in milliseconds. iTunes allows to set a custom stop time for a track. */
		UInt32 stopTimeOfTrack;

		/** The last processed timestampID of the kVisualPluginRenderMessage. */
		UInt32 currTimestampIDOfRenderData;

		/** The number of music data history. */
		SInt8 numberOfMusicDataHistoryInt;

		/** The maximum number of audio data channels (2 channels means stereo). */
		static UInt16 maxNumberOfAudioDataChannels;
		
		/** The number of audio waveform data entries. */
		static UInt32 numberOfAudioWaveformDataEntries;
		
		/** The number of audio spectrum data entries. */
		static UInt32 numberOfAudioSpectrumDataEntries;

		/**
		 * Adds the minimum and maximum waveform data value to the waveform shape.
		 * @param minWaveformPoint The minimum waveform data value.
		 * @param maxWaveformPoint The maximum waveform data value.
		 */
		void addToWaveformShape(SInt8 minWaveformPoint, SInt8 maxWaveformPoint);
		
		/**
		 * Adds a value to beatsRecordedInBeatAnalyseTimeFrame.
		 */
		void addToBeatRecords(void);
		
		/**
		 * Adds a value to beatGapHistogram.
		 * @param gapInMilliseconds The gap to previous beat detection in milliseconds.
		 */
		void addToBeatGapHistogram(UInt16 gapInMilliseconds);
		
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
		UInt32 getBestBeatMSInterval(void);
		
		/** 
		 * The waveform data array. 
		 * The waveform data array is 3-dimensional with data values, channels and history.
		 */
		//SInt8 waveformData[MAXNUMBEROFMUSICDATAHISTORY][kVisualMaxDataChannels][kVisualNumWaveformEntries];
		SInt8*** waveformData;
		
		/** 
		 * The mono waveform data array. 
		 * The mono waveform data array is 2-dimensional with data values, and history.
		 */
		//SInt16 waveformDataMono[MAXNUMBEROFMUSICDATAHISTORY][kVisualNumWaveformEntries];
		SInt16** waveformDataMono;
		
		/** 
		 * The spectrum data array. 
		 * The spectrum data array is 3-dimensional with data values, channels and history.
		 */
		//UInt8 spectrumData[MAXNUMBEROFMUSICDATAHISTORY][kVisualMaxDataChannels][kVisualNumSpectrumEntries];
		UInt8*** spectrumData;
		
		/**
		 * The waveform shape array. 
		 * The spectrum data array is 2-dimensional with two data values (min and max), and history.
		 */
		//SInt8 waveformShape[MAXNUMBEROFWAVEFORMSHAPEHISTORY][2]; // min und max
		SInt8** waveformShape; // min und max
		
		/** The amplitude envelope array. */
		//double amplitudeEnvelope[kVisualNumWaveformEntries];
		double* amplitudeEnvelope;
		
		/** The mean values. */
		//SInt32 mean[kVisualMaxDataChannels];
		SInt32* mean;
		
		/** A special buffer for beat detection. */
		//double tappingBuffer[tappingBufferLength];
		double* tappingBuffer;
		
		/** The rms values. */
		//double rms[MAXNUMBEROFMUSICDATAHISTORY][kVisualMaxDataChannels];
		double** rms;
		
		/** The mono rms values. */
		//double rmsMono[MAXNUMBEROFMUSICDATAHISTORY];
		double* rmsMono;
		
		/** The average rms value. */
		double rmsMonoAvg;
		
		/** A histogram of the gaps between detected beat events. */
		//UInt32 beatGapHistogram[MAXNUMBEROFBEATHISTOGRAMBINS];
		UInt32* beatGapHistogram;
		
		/** The delta between beats. */
		UInt32 beatTimeDeltaInMS;
		
		/** The maximum mono rms value. Timeframe numberOfMillisecondsOfRMSBuffer. */
		double rmsMonoMax;
		
		/** The current history number. */
		SInt16 currMusicDataHistory;

		SInt16 MAXNUMBEROFMUSICDATAHISTORY;
		
		/** The minimum spectrum level.\ 0-128. */
		//UInt8 minSpectrumLevel[kVisualMaxDataChannels];
		UInt8* minSpectrumLevel;
		
		/** The maximum spectrum level.\ 0-128. */
		//UInt8 maxSpectrumLevel[kVisualMaxDataChannels];
		UInt8* maxSpectrumLevel;
		
		/** The spectrum DC level. 0-256 (?). Direct Current of FFT (mono). */
		UInt8 spectrumDCLevel;
		
		/** The average spectrum DC level. */
		float avgSpectrumDCLevel;
		
		/** The sum of the spectrum DC level.\ sum of data (for calc of avg). */
		float spectrumDCLevelSum;
		
		/** The minimum waveform data value.\ 0-255.\ -128 ...127. */
		//SInt8 minWaveformVal[kVisualMaxDataChannels];
		SInt8* minWaveformVal;
		
		/** The maximum waveform data value.\ 0-255.\ -128 ...127. */
		//SInt8 maxWaveformVal[kVisualMaxDataChannels];
		SInt8* maxWaveformVal;
		
		/** The minimum mono waveform data value. */
		SInt8 minWaveformValMono;
		
		/** The maximum mono waveform data value. */
		SInt8 maxWaveformValMono;
		
		/** The sum of the absolute waveform data values that are higher or lower than 128. */
		//UInt32 waveformValSum[kVisualMaxDataChannels];
		UInt32* waveformValSum;

		/** The peak values of the sum of the waveform data values. */
		//UInt32 peakWaveformValSum[kVisualMaxDataChannels];
		UInt32* peakWaveformValSum;

		/** The previous sum of the waveform data values. */
		//UInt32 prevWaveformValSum[kVisualMaxDataChannels];
		UInt32* prevWaveformValSum;

		/** The average value of the sum of the waveform data values. */
		//UInt32 avgWaveformValSum[kVisualMaxDataChannels];
		UInt32* avgWaveformValSum;

		/** The counter for the calculation o the average value of the sum of the waveform data values. */
		//UInt32 avgWaveformValSumCounter[kVisualMaxDataChannels];
		UInt32* avgWaveformValSumCounter;

		/** 1 if the current waveform data indicates a beat impulse, 0 if not. */
		UInt8 isBeatImpulse;

		/** The smallest time a beat is considered to be the same. Measured in milliseconds. */
		static const UInt8 beatTimeTolerance;
		
		/** The threshold after which we are free to detect a new beat. Measured in milliseconds. */
		static const UInt16 beatThreshold;
		
		/** The time frame in which beats are recorded and analysed. Measured in milliseconds. */
		static const UInt16 beatAnalyseTimeFrame;
		
		/** The maximum beat interval. Measured in milliseconds. */
		static const UInt16 maximumBeatInterval;
		
		/** The number of milliseconds rms values are collected. */
		static const UInt16 numberOfMillisecondsOfRMSBuffer;
		
		/** 
		 * The number of records in rmsBuffer. 
		 * Must be greater than: Audio sample rate (44100) / kVisualNumWaveformEntries * (numberOfMillisecondsOfRMSBuffer / 1000).
		 */
		static const UInt16 numberOfRMSBufferEntries;
		
		/**
		 * Struct with values concerning rms (root mean square) values.
		 */
		typedef struct {
			float rmsValue; /**< The rms value. */
			UInt32 timeInMillisecondsSinceStartOfTrack; /**< The number of milliseconds elapsed since start of audio track. */
		} RMSValue;
		
		/** 
		 * Array containg rms mono values for the last numberOfMillisecondsOfRMSBuffer milliseconds.\ Ringbuffer.
		 * RMSValues with ((currentMSSinceStartOfTrack - timeInMillisecondsSinceStartOfTrack) > numberOfMillisecondsOfRMSBuffer) are discarded.
		 */
		RMSValue rmsBuffer[2000];

		/** The current index of array rmsBuffer. */
		UInt16 currRMSValue;

		/** The current index of array beatsRecordedInBeatAnalyseTimeFrame. */
		UInt8 currRecordedBeat;

		/**
		 * Struct with values concerning beat estmation.
		 */
		typedef struct {
			UInt32 timeInMillisecondsSinceStartOfTrack; /**< The number of milliseconds elapsed since start of audio track. */
			UInt16 confidence; /**< The strength of the beatGuess. */
		} BeatGuess;

		/** Array in which the detected beats are stored with their elapsed milliseconds timestamp since start of track. Ringbuffer. */
		BeatGuess beatsRecordedInBeatAnalyseTimeFrame[MAXNUMBEROFBEATSRECORDEDINANALYSETIMEFRAME];

		/**
		 * 
		 * @param elapsedMSSinceStartOfTrack The number of milliseconds elapsed since start of audio track.
		 * @return 1 if time is in frame of beat meter, 0 if not.
		 */
		UInt8 isBeatMeter(UInt32 elapsedMSSinceStartOfTrack);
		
	};

}

#endif /* VisualAudioLab_h */


