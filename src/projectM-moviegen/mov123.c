//
// SlimServer Copyright (C) 2003-2004 Sean Adams, Slim Devices Inc.
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License,
// version 2.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
//  mov123 - A very basic Quicktime decoder command line application.
//
//  usage:  mov123 <srcfile>
//
//  opens and decodes the first audio track from a QuickTime compatible file.  This includes
//  Movie files, m4a AAC files, AIFF, WAV and other formats supported natively by quicktime.
//  Sends to standard out the raw uncompressed audio data in stereo 44.1kS/sec 16bit.
//  Output goes to stdout
//
//  Todo:  - extract channel, sample rate, and sample size information from the movie for
//           use in reencoding later
//         - CLI options for:
//             - specifying output file
//             - output files for AIFF and WAV
//             - changing sample rate, sample size, channel count, codec
//			   - usage
//	 	   - be graceful about failures
//
//  Portions based on Apple's ConvertMovieSndTrack sample code.
//

#include <stdio.h>
#include <fcntl.h>

//#include <io.h>


#ifdef WIN32
#include "stdafx.h"
#include "Movies.h"
#include "SoundComponents.h"
#include "QTML.h"
#else
#include <QuickTime/QuickTime.h>
#include <QuickTime/QTML.h>
#endif

#include <projectM.hpp>

#define kVideoTimeScale 600

extern projectM_t *globalPM;

int path2fss( FSSpec *fss, char *path ) {
    char buf[256];
    char *p = &buf[1];
    strcpy( p, path );
    buf[0] = strlen( p );

    return FSMakeFSSpec( 0, 0, (unsigned char *)buf, fss );
  }

#define BailErr(x) {err = x; if (err != noErr) { fprintf(stderr, "Failed at line: %d\n", __LINE__); goto bail; } }

const UInt32 kMaxBufferSize =  64 * 1024;  // max size of input buffer

// functions
OSErr ConvertMovieSndTrack( const char* inFileToConvert, Movie *movie,
                            Media *videoMedia, Media *audioMedia,
                            Track *videoTrack, Track *audioTrack );

typedef struct {
	ExtendedSoundComponentData	compData;
	Handle						hSource;			// source media buffer
	Media						sourceMedia;		// sound media identifier
	TimeValue					getMediaAtThisTime;
	TimeValue					sourceDuration;

	UInt32						maxBufferSize;
	Boolean						isThereMoreSource;
	Boolean						isSourceVBR;
} SCFillBufferData, *SCFillBufferDataPtr;

FILE* outFile;

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main123(int argc, char *argv[])
#endif
{
	
	FSSpec		theDestFSSpec;
	OSErr		result = 0;
    OSErr   err = noErr;
    short resRefNum = 0;
    short resId = movieInDataForkResID;
    Movie newMovie;
    Media audioMedia, videoMedia;
    Track audioTrack, videoTrack;
//    StringPtr fileName = QTUtils_ConvertCToPascalString( argv[2] );
	
	outFile = stderr;

#ifdef WIN32
	setmode(fileno(outFile), O_BINARY);	
	InitializeQTML(0);                        // Initialize QTML
#endif
	EnterMovies();

	if (argc > 2) 
        path2fss( &theDestFSSpec, argv[2] );

    /** Create the new output movie */
    err = 
        CreateMovieFile( &theDestFSSpec, FOUR_CHAR_CODE('TV0D'),
                         smSystemScript, 
                         createMovieFileDeleteCurFile | createMovieFileDontCreateResFile | newMovieActive,
                         &resRefNum, &newMovie );
    if ( err != noErr ) {
        printf( "failed to CreateMovieFile()\n" );
      } else {
        printf( "created movie ok\n" );
      }

    /** Create the video track */
    videoTrack =
        NewMovieTrack( newMovie, 512, 512, kNoVolume );

    videoMedia = 
        NewTrackMedia( videoTrack, VideoMediaType, kVideoTimeScale, nil,
                       0 );

    /** Establish a media edit session */
    err = BeginMediaEdits( videoMedia );
    if ( err != noErr ) {
        printf( "failed to BeginMediaEdits( video )\n" );
      } else {  
        printf( "beginning video media edit ok\n" );
      }

    /** Create the audio track */
    audioTrack = 
        NewMovieTrack( newMovie, 0, 0, kFullVolume );
    audioMedia =
        NewTrackMedia( audioTrack, SoundMediaType, 44000, nil, 0 );

    err = BeginMediaEdits( audioMedia );
    if ( err != noErr ) {
        printf( "failed to BeginMediaEdits( audio )\n" );
      } else {
        printf( "beginning audio media edits ok\n" );
      }

    /** Convert the input sound data into a movie file with video and audio */
	result = ConvertMovieSndTrack( argv[1], &newMovie, &videoMedia, &audioMedia,
	                               &videoTrack, &audioTrack );

    /** End media edits */
    err = EndMediaEdits( videoMedia );

    err = EndMediaEdits( audioMedia );

    /** Insert the audio track into the media */
    err = InsertMediaIntoTrack( audioTrack, 0, 0,
                                GetMediaDuration( audioMedia ), fixed1 );
    if ( err != noErr ) {
        printf( "failed to insert audio media into audio track\n" );
      } else {
        printf( "injected audio media into audio track ok\n" );
      }

    /** Add the movie resource to the file */
    err = AddMovieResource( newMovie, resRefNum, &resId, argv[2] );
    if ( err != noErr ) {
        printf( "failed to AddMovieResource()\n" );
      }
    if ( resRefNum ) {
        CloseMovieFile( resRefNum );
      }

//bail:
	if (result != 0) { fprintf(stderr, "Conversion failed with error: %d\n", result); }
	return result;
}


#ifndef AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
	// these didn't make it into the QT6 framework for 10.1.x so include
	// them here if we're not on 10.2 or later - if you have a newer framework
	// or are building a carbon CFM version you shouldn't need these
	enum {
	  scSoundVBRCompressionOK       = 'cvbr', /* pointer to Boolean*/
	  scSoundInputSampleRateType    = 'ssir', /* pointer to UnsignedFixed*/
	  scSoundSampleRateChangeOK     = 'rcok', /* pointer to Boolean*/
	  scAvailableCompressionListType = 'avai' /* pointer to OSType Handle*/
	};
#endif


// * ----------------------------
// SoundConverterFillBufferDataProc
//
// the callback routine that provides the SOURCE DATA for conversion - it provides data by setting
// outData to a pointer to a properly filled out ExtendedSoundComponentData structure
static pascal Boolean SoundConverterFillBufferDataProc(SoundComponentDataPtr *outData, void *inRefCon)
{
	int i, j;

	float nframes = 0;

	SCFillBufferDataPtr pFillData = (SCFillBufferDataPtr)inRefCon;
	
	OSErr err;
							
	// if after getting the last chunk of data the total time is over the duration, we're done
	if (pFillData->getMediaAtThisTime >= pFillData->sourceDuration) {
		pFillData->isThereMoreSource = false;
		pFillData->compData.desc.buffer = NULL;
		pFillData->compData.desc.sampleCount = 0;
		pFillData->compData.bufferSize = 0;		
		pFillData->compData.commonFrameSize = 0;
	}
	
	if (pFillData->isThereMoreSource) {
	
		long	  sourceBytesReturned;
		long	  numberOfSamples;
		TimeValue sourceReturnedTime, durationPerSample;
		
		// in calling GetMediaSample, we'll get a buffer that consists of equal sized frames - the
		// degenerate case is only 1 frame -- for non-self-framed vbr formats (like AAC in QT 6)
		// we need to provide some more framing information - either the frameCount, frameSizeArray pair or
		// the commonFrameSize field must be valid -- because we always get equal sized frames, we can use
		// commonFrameSize and set the kExtendedSoundCommonFrameSizeValid flag -- if there is
		// only 1 frame then (common frame size == media sample size), if there are multiple frames,
		// then (common frame size == media sample size / number of frames).
		
		err = GetMediaSample(pFillData->sourceMedia,		// specifies the media for this operation
							 pFillData->hSource,			// function returns the sample data into this handle
							 pFillData->maxBufferSize,		// maximum number of bytes of sample data to be returned
							 &sourceBytesReturned,			// the number of bytes of sample data returned
							 pFillData->getMediaAtThisTime,	// starting time of the sample to be retrieved (must be in Media's TimeScale)
							 &sourceReturnedTime,			// indicates the actual time of the returned sample data
							 &durationPerSample,			// duration of each sample in the media
							 NULL,							// sample description corresponding to the returned sample data (NULL to ignore)
							 NULL,							// index value to the sample description that corresponds to the returned sample data (NULL to ignore)
							 0,								// maximum number of samples to be returned (0 to use a value that is appropriate for the media)
							 &numberOfSamples,				// number of samples it actually returned
							 NULL);							// flags that describe the sample (NULL to ignore)

		if ((noErr != err) || (sourceBytesReturned == 0)) {
			pFillData->isThereMoreSource = false;
			pFillData->compData.desc.buffer = NULL;
			pFillData->compData.desc.sampleCount = 0;
			pFillData->compData.bufferSize = 0;		
			pFillData->compData.commonFrameSize = 0;
			
			if ((err != noErr) && (sourceBytesReturned > 0))
				printf("GetMediaSample - Failed in FillBufferDataProc");
		}

		pFillData->getMediaAtThisTime = sourceReturnedTime + (durationPerSample * numberOfSamples);
		
		// sampleCount is the number of PCM samples
		pFillData->compData.desc.sampleCount = numberOfSamples * durationPerSample;
		
		// kExtendedSoundBufferSizeValid was specified - make sure this field is filled in correctly
		pFillData->compData.bufferSize = sourceBytesReturned;
		
		// for VBR audio we specified the kExtendedSoundCommonFrameSizeValid flag - make sure this field is filled in correctly
		if (pFillData->isSourceVBR) pFillData->compData.commonFrameSize = sourceBytesReturned / numberOfSamples;
	}

	// set outData to a properly filled out ExtendedSoundComponentData struct
	*outData = (SoundComponentDataPtr)&pFillData->compData;
	
	return (pFillData->isThereMoreSource);
}

// * ----------------------------
// GetMovieMedia
//
// returns a Media identifier - if the file is a System 7 Sound a non-in-place import is done and
// a handle to the data is passed back to the caller who is responsible for disposing of it
static OSErr GetMovieMedia(const char* inFile, Movie *outMovie, Media *outMedia)
{
	Movie theMovie = 0;
	Track theTrack;
	short theRefNum;
	short theResID = 0;	// we want the first movie
	OSErr err = noErr;

	BailErr(err);
		
	Boolean wasChanged;
	
	// open the movie file
	if (strncmp(inFile, "http:", strlen("http:")) &&
		strncmp(inFile, "rtsp:", strlen("rtsp:")) &&
		strncmp(inFile, "ftp:", strlen("ftp:") )) {
		FSSpec		theFSSpec;
#ifdef WIN32
		OSErr result = NativePathNameToFSSpec((char*)inFile, &theFSSpec, 0 /* flags */);
#else
                FSRef ref; // intermediate struct
                FSPathMakeRef( (const UInt8*)inFile, &ref, NULL );
                OSErr result = FSGetCatalogInfo( &ref, kFSCatInfoNone , NULL, NULL, &theFSSpec, NULL);
#endif
		if (result) {printf("NativePathNameToFSSpec failed on source file %s with %d\n", inFile, result); goto bail; }
		err = OpenMovieFile(&theFSSpec, &theRefNum, fsRdPerm);
		// instantiate the movie
		BailErr(err);
		err = NewMovieFromFile(&theMovie, theRefNum, &theResID, NULL, newMovieActive, &wasChanged);
		CloseMovieFile(theRefNum);
	} else {

		Handle urlDataRef; 
		
		urlDataRef = NewHandle(strlen(inFile) + 1); 
		if ( ( err = MemError()) != noErr) goto bail; 
		
		BlockMoveData(inFile, *urlDataRef, strlen(inFile) + 1); 
		
		err = NewMovieFromDataRef(&theMovie, newMovieActive, nil, urlDataRef, URLDataHandlerSubType); 
		if (err) {printf("NewMovieFrom Data Ref failed on source file %s with %d\n", inFile, err); goto bail; }

		DisposeHandle(urlDataRef); 
		
	}

	BailErr(err);
	
	// get the first sound track
	theTrack = GetMovieIndTrackType(theMovie, 1, SoundMediaType, movieTrackMediaType);
	if (NULL == theTrack ) BailErr(invalidTrack);

	// get and return the sound track media
	*outMedia = GetTrackMedia(theTrack);
	if (NULL == *outMedia) err = invalidMedia;
	
	*outMovie = theMovie;
	
bail:
	return err;
}

// * ----------------------------
// GetSoundDescriptionInfo
//
// this function will extract the information needed to decompress the sound file, this includes 
// retrieving the sample description and the decompression atom saved as a Sample Description Extention
static OSErr GetSoundDescriptionInfo(Media inMedia, Ptr *outAudioAtom, SoundDescriptionPtr outSoundDesc)
{
	OSErr err = noErr;
			
	Size size;
	Handle extension = NULL;
	SoundDescriptionHandle hSoundDescription = (SoundDescriptionHandle)NewHandle(0);
	
	// get the description of the sample data
	GetMediaSampleDescription(inMedia, 1, (SampleDescriptionHandle)hSoundDescription);
	BailErr(GetMoviesError());

	extension = NewHandle(0);
	BailErr(MemError());
	
	// get the "magic" decompression atom
	// This extension to the SoundDescription information stores data specific to a given audio decompressor.
	// Some audio decompression algorithms require a set of out-of-stream values to configure the decompressor
	// which are stored in a siDecompressionParams atom. The contents of the siDecompressionParams atom are dependent
	// on the sound decompressor.
	err = GetSoundDescriptionExtension(hSoundDescription, &extension, siDecompressionParams);
	if (noErr == err) {
		size = GetHandleSize(extension);
		HLock(extension);
		*outAudioAtom = NewPtr(size);
		BailErr(MemError());
		// copy the atom data to our Ptr...
		BlockMoveData(*extension, *outAudioAtom, size);
		HUnlock(extension);
	} else {
		// if it doesn't have an atom, that's ok
		err = noErr;
	}
	
	// set up our sound header
	outSoundDesc->dataFormat = (*hSoundDescription)->dataFormat;
	outSoundDesc->numChannels = (*hSoundDescription)->numChannels;
	outSoundDesc->sampleSize = (*hSoundDescription)->sampleSize;
	outSoundDesc->sampleRate = (*hSoundDescription)->sampleRate;
	outSoundDesc->compressionID = (*hSoundDescription)->compressionID;
	
bail:
	if (extension) DisposeHandle(extension);
	if (hSoundDescription) DisposeHandle((Handle)hSoundDescription);
	
	return err;
}

// * ----------------------------
// ConvertMovieSndTrack
//
// this function does the actual work
OSErr ConvertMovieSndTrack(const char* inFileToConvert, Movie *movie,
                           Media *videoMedia, Media *audioMedia,
                           Track *videoTrack, Track *audioTrack ) {

	SoundConverter			 mySoundConverter = NULL;
	
	Movie					 theSrcMovie = 0;
	Media					 theSrcMedia = 0;
							
	Ptr						 theDecompressionParams = NULL;
	Handle 					 theCompressionParams = NULL;

	SoundDescription		 theSrcInputFormatInfo;
	SoundDescriptionV1Handle hSoundDescription = NULL;
	UnsignedFixed 			 theOutputSampleRate;
	SoundComponentData		 theInputFormat,
							 theOutputFormat;

	SCFillBufferData 		 scFillBufferData = { NULL };
	Ptr						 pDecomBuffer = NULL;
										
	Boolean					 isSoundDone = false;
	
	OSErr 					 err = noErr;
	
    int i, j;
    int sindex = 0;
    unsigned short pcmdata[2][512];
    unsigned short *dptr;

	// *********** SOURCE: Get sound data info from the first source movie sound track
	
	err = GetMovieMedia(inFileToConvert, &theSrcMovie, &theSrcMedia);
	BailErr(err);
	
	err = GetSoundDescriptionInfo(theSrcMedia, (Ptr *)&theDecompressionParams, &theSrcInputFormatInfo);
	if (noErr == err) {		
		// setup input format for sound converter
		theInputFormat.flags = 0;
		theInputFormat.format = theSrcInputFormatInfo.dataFormat;
		theInputFormat.numChannels = theSrcInputFormatInfo.numChannels;
		theInputFormat.sampleSize = theSrcInputFormatInfo.sampleSize;
		theInputFormat.sampleRate = theSrcInputFormatInfo. sampleRate;
		theInputFormat.sampleCount = 0;
		theInputFormat.buffer = NULL;
		theInputFormat.reserved = 0;

		theOutputFormat.flags = kNoRealtimeProcessing;
		theOutputFormat.format = k16BitBigEndianFormat;
		theOutputFormat.numChannels = 2; // theInputFormat.numChannels;
		theOutputFormat.sampleSize = 16;
		theOutputFormat.sampleRate = 44100 << 16; //theInputFormat.sampleRate;
		theOutputFormat.sampleCount = 0;
		theOutputFormat.buffer = NULL;
		theOutputFormat.reserved = 0;

	// *********** SOUND CONVERTER: Open converter and prepare for buffer conversion...captain!

		err = SoundConverterOpen(&theInputFormat, &theOutputFormat, &mySoundConverter);
		BailErr(err);
		
		// tell the sound converter we're cool with VBR formats
//		SoundConverterSetInfo(mySoundConverter, siClientAcceptsVBR, Ptr(true));															

		// set up the sound converters compression environment
		// pass down siCompressionSampleRate, siCompressionChannels then siCompressionParams
		SoundConverterSetInfo(mySoundConverter, siCompressionSampleRate, &theOutputFormat.sampleRate); // ignore errors
		SoundConverterSetInfo(mySoundConverter, siCompressionChannels, &theOutputFormat.numChannels);

		// set up the compression environment by passing in the 'magic' compression params aquired from
		// standard sound compression eariler
		if (theCompressionParams) {
			HLock(theCompressionParams);
			err = SoundConverterSetInfo(mySoundConverter, siCompressionParams, *theCompressionParams);
			BailErr(err);
			HUnlock(theCompressionParams);
		}

		// set up the decompresson environment by passing in the 'magic' decompression params
		if (theDecompressionParams) {
			// don't check for an error, if the decompressor didn't need the
			// decompression atom for whatever reason we should still be ok
			SoundConverterSetInfo(mySoundConverter, siDecompressionParams, theDecompressionParams);
		}
		
		// we need to know if the output sample rate was changed so we can write it in the image description
		// few codecs (but some) will implement this - MPEG4 for example may change the output sample rate if
		// the user selects a low bit rate -  ignore errors
	 	theOutputSampleRate = theOutputFormat.sampleRate;
		SoundConverterGetInfo(mySoundConverter, siCompressionOutputSampleRate, &theOutputSampleRate);
		
		err = SoundConverterBeginConversion(mySoundConverter);
		BailErr(err);
		
		// we need to get info about data/frame sizes 
		// good practice to fill in the size of this structure
		CompressionInfo compressionFactor = { sizeof(compressionFactor), 0 };
		
		hSoundDescription = (SoundDescriptionV1Handle)NewHandleClear(sizeof(SoundDescriptionV1));	
		BailErr(MemError());
		
		err = SoundConverterGetInfo(mySoundConverter, siCompressionFactor, &compressionFactor);				
		BailErr(err);

		HLock((Handle)hSoundDescription);
		
		(*hSoundDescription)->desc.descSize		 = sizeof(SoundDescriptionV1);
		(*hSoundDescription)->desc.dataFormat	 = (long)theOutputFormat.format;	   // compression format
		(*hSoundDescription)->desc.resvd1		 = 0;								   // must be 0
		(*hSoundDescription)->desc.resvd2		 = 0;							       // must be 0
		(*hSoundDescription)->desc.dataRefIndex	 = 0;								   // 0 - we'll let AddMediaXXX determine the index
		(*hSoundDescription)->desc.version		 = 1;								   // set to 1
		(*hSoundDescription)->desc.revlevel		 = 0;								   // set to 0
		(*hSoundDescription)->desc.vendor		 = 0;
		(*hSoundDescription)->desc.numChannels	 = theOutputFormat.numChannels;		   // number of channels
		(*hSoundDescription)->desc.sampleSize	 = theOutputFormat.sampleSize;		   // bits per sample - everything but 8 bit can be set to 16
		(*hSoundDescription)->desc.compressionID = compressionFactor.compressionID;    // the compression ID (eg. variableCompression)
		(*hSoundDescription)->desc.packetSize	 = 0;								   // set to 0
		(*hSoundDescription)->desc.sampleRate	 = theOutputSampleRate;		   		   // the sample rate
		// version 1 stuff
		(*hSoundDescription)->samplesPerPacket 	 = compressionFactor.samplesPerPacket; // the samples per packet holds the PCM sample count per audio frame (packet)
		(*hSoundDescription)->bytesPerPacket 	 = compressionFactor.bytesPerPacket;   // the bytes per packet
		
		// bytesPerFrame isn't necessarily calculated for us and returned as part of the CompressionFactor - not all codecs that
		// implement siCompressionFactor fill out bytesPerFrame - so we do it here - note that VBR doesn't deserve this treatment
		// but it's not harmful, the Sound Manager would do calculations itself as part of GetCompressionInfo()
		// It should be noted that GetCompressionInfo() doesn't work for codecs that need configuration with 'magic' settings.
		// This requires explicit opening of the codec and the siCompressionFactor selector for SoundComponentGetInfo()
		(*hSoundDescription)->bytesPerFrame 	 = compressionFactor.bytesPerPacket * theOutputFormat.numChannels;
		(*hSoundDescription)->bytesPerSample 	 = compressionFactor.bytesPerSample;							
       
        // the theCompressionParams are not necessarily present
		if (theCompressionParams) {
			// a Sound Description can't be locked when calling AddSoundDescriptionExtension so make sure it's unlocked
			HUnlock((Handle)hSoundDescription);
			err = AddSoundDescriptionExtension((SoundDescriptionHandle)hSoundDescription, theCompressionParams, siDecompressionParams);	
			BailErr(err);
			HLock((Handle)hSoundDescription);
		}
        
        // VBR implies a different media layout, this will affect how AddMediaSample() is called below
        Boolean outputFormatIsVBR = ((*hSoundDescription)->desc.compressionID == variableCompression);
        
	// *********** SOUND CONVERTER: Create buffers and Convert Data

		// figure out sizes for the input and output buffers
		// the input buffer has to be large enough so GetMediaSample isn't going to fail
		// start with some rough numbers which should work well
		UInt32 inputBytes = ((1000 + (theInputFormat.sampleRate >> 16)) * theInputFormat.numChannels) * 4,
			   outputBytes = 0,
			   maxPacketSize = 0;
		
		// ask about maximum packet size (or worst case packet size) so we don't allocate a destination (output)
		// buffer that's too small - an output buffer smaller than MaxPacketSize would be really bad - init maxPacketSize
		// to 0 so if the request isn't understood we can create a number (some multiple of maxPacketSize) and go from there
		// this is likely only implemented by VBR codecs so don't get anxious about it not being implemented
		SoundConverterGetInfo(mySoundConverter, siCompressionMaxPacketSize, &maxPacketSize);
		
		// start with this - you don't really need to use GetBufferSizes just as long as the output buffer is larger than
		// the MaxPacketSize if implemented - we use kMaxBufferSize which is 64k as a minimum
		SoundConverterGetBufferSizes(mySoundConverter, kMaxBufferSize, NULL, NULL, &outputBytes);
		
		if (0 == maxPacketSize)
			maxPacketSize = kMaxBufferSize;   // kMaxBufferSize is 64k
											 								  
		if (inputBytes < kMaxBufferSize)	  // kMaxBufferSize is 64k
			inputBytes = kMaxBufferSize;	  // note this is still too small for DV (NTSC=120000, PAL=144000)
			
		if (outputBytes < maxPacketSize)	  
			outputBytes = maxPacketSize;

		// allocate conversion buffer
		pDecomBuffer = NewPtr(outputBytes);
		BailErr(MemError());
		
		// fill in struct that gets passed to SoundConverterFillBufferDataProc via the refcon
		// this includes the ExtendedSoundComponentData information		
		scFillBufferData.sourceMedia = theSrcMedia;
		scFillBufferData.getMediaAtThisTime = 0;		
		scFillBufferData.sourceDuration = GetMediaDuration(theSrcMedia);
		scFillBufferData.isThereMoreSource = true;
		scFillBufferData.maxBufferSize = inputBytes;
		
		// if the source is VBR it means we're going to set the kExtendedSoundCommonFrameSizeValid
		// flag and use the commonFrameSize field in the FillBuffer callback
		scFillBufferData.isSourceVBR = (theSrcInputFormatInfo.compressionID == variableCompression);
		
		scFillBufferData.hSource = NewHandle((long)scFillBufferData.maxBufferSize);	// allocate source media buffer
		BailErr(MemError());
		HLockHi((Handle)scFillBufferData.hSource);
		
		scFillBufferData.compData.desc = theInputFormat;
		scFillBufferData.compData.desc.buffer = (BytePtr)*scFillBufferData.hSource;
		scFillBufferData.compData.desc.flags = kExtendedSoundData;
		scFillBufferData.compData.recordSize = sizeof(ExtendedSoundComponentData);
		scFillBufferData.compData.extendedFlags = kExtendedSoundBufferSizeValid;
		if (scFillBufferData.isSourceVBR) scFillBufferData.compData.extendedFlags |= kExtendedSoundCommonFrameSizeValid;
		scFillBufferData.compData.bufferSize = 0;	// filled in during FillBuffer callback

		if (err == noErr) {	
			
			UInt32 outputFrames,
				   actualOutputBytes,
				   outputFlags,
				   durationPerMediaSample,
				   numberOfMediaSamples;
				   			
			SoundConverterFillBufferDataUPP theFillBufferDataUPP = NewSoundConverterFillBufferDataUPP(SoundConverterFillBufferDataProc);	

			while (!isSoundDone) {
				printf( ".");
				err = SoundConverterFillBuffer(mySoundConverter,		// a sound converter
											   theFillBufferDataUPP,	// the callback UPP
											   &scFillBufferData,		// refCon passed to FillDataProc
											   pDecomBuffer,			// the destination data  buffer
											   outputBytes,				// size of the destination buffer
											   &actualOutputBytes,		// number of output bytes
											   &outputFrames,			// number of output frames
											   &outputFlags);			// FillBuffer retured advisor flags
				if (err) break;
				if((outputFlags & kSoundConverterHasLeftOverData) == false) {
					isSoundDone = true;
				}
				
				// see if output buffer is filled so we can write some data	
				if (actualOutputBytes > 0) {					
					// so, what are we going to pass to AddMediaSample?
					// 
					// for variableCompression, a media sample == an audio packet (compressed), this is also true for uncompressed audio
					// for fixedCompression, a media sample is a portion of an audio packet - it is 1 / compInfo.samplesPerPacket worth
					// of data, there's no way to access just a portion of the samples
					// therefore, we need to know if our compression format is VBR or Fixed and make the correct calculations for
					// either VBR or not - Fixed and uncompressed are treated the same
					if (outputFormatIsVBR) {
						numberOfMediaSamples = outputFrames;
						durationPerMediaSample = compressionFactor.samplesPerPacket;
					} else {		
						numberOfMediaSamples = outputFrames * compressionFactor.samplesPerPacket;
						durationPerMediaSample = 1;
					}

                    i = 0;
                    sindex = 0;
                    dptr = (unsigned short *)pDecomBuffer;
                    while ( i < numberOfMediaSamples ) {
                        for ( j = 0 ; j < 512 ; j++ ) {
                            pcmdata[0][j] = *dptr;
                            *dptr++;
                            pcmdata[1][j] = *dptr;
                            *dptr++;
                            i++;
                          }

                        /** Render the frame */
                        renderLoop( globalPM, pcmdata );

                        /** Stuff frame into movie */

                        /** Stuff audio into movie */
                        err = 
                            AddMediaSample( *audioMedia, pDecomBuffer,
                                            0, actualOutputBytes,
                                            durationPerMediaSample,
                                            nil, numberOfMediaSamples,
                                            0, nil );

                      }

//                    printf( "samples: %d (%d bytes)\n", numberOfMediaSamples, actualOutputBytes );

//					if (!fwrite(pDecomBuffer, actualOutputBytes, 1, outFile)) goto bail;
				}

			} // while
			
			SoundConverterEndConversion(mySoundConverter, pDecomBuffer, &outputFrames, &actualOutputBytes);

			// if there's any left over data write it out
			if (noErr == err && actualOutputBytes > 0) {
				// see above comments regarding these calculations
				if (outputFormatIsVBR) {
					numberOfMediaSamples = outputFrames;
					durationPerMediaSample = compressionFactor.samplesPerPacket;
				} else {		
					numberOfMediaSamples = outputFrames * compressionFactor.samplesPerPacket;
					durationPerMediaSample = 1;
				}

//				if (!fwrite(pDecomBuffer, actualOutputBytes, 1, outFile)) goto bail;

				BailErr(err);
			}
			
			if (theFillBufferDataUPP) {
				DisposeSoundConverterFillBufferDataUPP(theFillBufferDataUPP);
			}
		}
						
	}
						
bail:
	if (mySoundConverter)
		SoundConverterClose(mySoundConverter);

	if (scFillBufferData.hSource)
		DisposeHandle(scFillBufferData.hSource);
		
	if (pDecomBuffer)
		DisposePtr(pDecomBuffer);
		
	if (theCompressionParams)
		DisposeHandle(theCompressionParams);
		
	if (theDecompressionParams)
		DisposePtr((Ptr)theDecompressionParams);
		
	if (hSoundDescription)
		DisposeHandle((Handle)hSoundDescription);
		
	if (theSrcMovie)
		DisposeMovie(theSrcMovie);

	return err;
}
