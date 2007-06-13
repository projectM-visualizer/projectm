/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id: projectMmovie.c,v 1.1 2004/10/19 20:28:46 cvs Exp $
 *
 * Converts a list of audio files supplied as arguments into MOV files
 * with embedded audio and video tracks
 *
 */

#include <stdio.h>
#include <QuickTime.h>

//path2fss makes an FSSpec from a path with or without a filename
int path2fss( FSSpec *fss, char *path ) {
    char buf[1024];
    char *p = &buf[1];
    strcpy( p, path );
    buf[0] = strlen( p );
    return ( FSMakeFSSpec( 0, 0, (unsigned char *)buf, fss ) );
  }

#ifdef PANTS
int main( int argc, char **argv ) {

    Movie movie;
    Track track;
    Media media;
    short refNum;
    short resID = 0;
    Boolean wasChanged;
    OSErr err = noErr;
    FSSpec fsspec;
    AudioFormatAtomPtr outAudioAtom;
    CmpSoundHeader outSoundInfo;
    SoundComponentData theInputFormat,
                       theOutputFormat;
    SoundConverter mySoundConverter = NULL;
//    SCFillBufferData scFillBufferData = { NULL };
    Ptr pDecomBuffer0 = NULL,
        pDecomBuffer1 = NULL;
    long kMaxOutputBuffer = 64 * 1024;
    long noFrames = 0,
                  niFrames = 0,
                  noBytes = 0,
                  noSamples = 0;
#define MAX_BUFFER_SIZE 256 * 1024 * 1024


    /** Initialise MovieToolbox */
    EnterMovies();

    /** Open the movie file from the first argument */
    printf( "opening audio file: '%s'\n", argv[1] );
    path2fss( &fsspec, argv[1] );
    err = OpenMovieFile( &fsspec, &refNum, fsRdPerm );
    if ( err != noErr ) {
        printf( "failed to open audio: %d\n", GetMoviesError() );
        exit( -1 );
      }

    /** Instantiate the movie */
    err = NewMovieFromFile( &movie, refNum, &resID, NULL,
                            newMovieActive, &wasChanged );
    if ( err ) {
        printf( "failed to instantiate movie\n" );
        exit( -1 );
      }

    CloseMovieFile( refNum );
    refNum = 0;

    /** Get the first sound track */
    track = GetMovieIndTrackType( movie, 1, SoundMediaType,
                                  movieTrackMediaType );
    if ( track == NULL ) {
        printf( "failed to get sound track\n" );
        exit( -1 );
      }

    /** Get the sound track media */
    media = GetTrackMedia( track );
    if ( media == NULL ) {
        printf( "failed to get media from audio track\n" );
        exit( -1 );
      }

    Size size;
    Handle extension;
    SoundDescriptionHandle sourceSoundDescription;

    sourceSoundDescription = (SoundDescriptionHandle)NewHandle(0);

    /** Get the description of the sample data */
    GetMediaSampleDescription( media, 1,
                               (SampleDescriptionHandle)sourceSoundDescription );
    err = GetMoviesError();
    if ( err ) {
        printf( "failed to get description of sample data\n" );
        exit( -1 );
      }

    extension = NewHandle( 0 );

    // get the "magic" decompression atom
    // This extension to the SoundDescription information stores
    // data specific to a given audio decompressor. Some audio
    // decompression algorithms require a set of out-of-stream
    // values to configure the decompressor.
    err = 
        GetSoundDescriptionExtension( (SoundDescriptionHandle)sourceSoundDescription,
                                      &extension, siDecompressionParams );
    if ( noErr == err ) {
        size = GetHandleSize( extension );
        printf( "transferring data to audio buffer: %d bytes\n", size );
        HLock( extension );
        outAudioAtom = (AudioFormatAtom*)NewPtr( size );
        err = MemError();
        // copy the atom data to our buffer...
        BlockMoveData( *extension, outAudioAtom, size );
        HUnlock( extension );
      } else {
        // if it doesn't have an atom, that's ok
        outAudioAtom = NULL;
        err = noErr;
      }

    /** Setup our sound header */
    outSoundInfo.format = (*sourceSoundDescription)->dataFormat;
    outSoundInfo.numChannels = (*sourceSoundDescription)->numChannels;
    outSoundInfo.sampleSize = (*sourceSoundDescription)->sampleSize;
    outSoundInfo.sampleRate = (*sourceSoundDescription)->sampleRate;
    outSoundInfo.compressionID = (*sourceSoundDescription)->compressionID;

    float db = ((float)outSoundInfo.sampleRate)/(1<<16);

    printf( "sample: %d\tchannels: %d\tsample size: %d\tsample rate: %f\tcompressionID: %d\n",
            outSoundInfo.format, outSoundInfo.numChannels, outSoundInfo.sampleSize,
            db, outSoundInfo.compressionID );

    DisposeHandle( extension );
    DisposeHandle( (Handle)sourceSoundDescription );

    /** 
     * Now that we've figured out what the audio file is, allocate buffers
     * and so on for conversion and playback
     */

    printf( "initialising input/output conversion buffers\n" );

    /** setup input/output format for sound converter */
    theInputFormat.flags = 0;
    theInputFormat.format = outSoundInfo.format;
    theInputFormat.numChannels = outSoundInfo.numChannels;
    theInputFormat.sampleSize = outSoundInfo.sampleSize;
    theInputFormat.sampleRate = outSoundInfo. sampleRate;
    theInputFormat.sampleCount = 0;
    theInputFormat.buffer = NULL;
    theInputFormat.reserved = 0;

    theOutputFormat.flags = 0;
    theOutputFormat.format = kSoundNotCompressed;
    theOutputFormat.numChannels = theInputFormat.numChannels;
    theOutputFormat.sampleSize = theInputFormat.sampleSize;
    theOutputFormat.sampleRate = theInputFormat.sampleRate;
    theOutputFormat.sampleCount = 0;
    theOutputFormat.buffer = NULL;
    theOutputFormat.reserved = 0;

    // variableCompression means we're going to use the commonFrameSize field and the kExtendedSoundCommonFrameSizeValid flag
//    scFillBufferData.isSourceVBR = (outSoundInfo.compressionID == variableCompression );

    err = SoundConverterOpen( &theInputFormat, &theOutputFormat, 
                              &mySoundConverter );
    if ( err != noErr ) {
        printf( "failed to open sound converter\n" );
        exit( -1 );
      } else {
        printf( "opened sound converter ok\n" );
      }

    // this isn't crucial or even required for decompression only, but it does tell
    // the sound converter that we're cool with VBR audio
    Ptr tptr = NewPtr( 1 );
    tptr[0] = 1;
    SoundConverterSetInfo( mySoundConverter, siClientAcceptsVBR, tptr );
    free( tptr );

    /**
     * Set up the sound converters decompresson 'environment' by passing
     * in the 'magic' decompression atom
     */
    err = 
        SoundConverterSetInfo( mySoundConverter, siDecompressionParams,
                               outAudioAtom );
    if ( err != noErr ) {
        printf( "failed to set sound converter info\n" );
        exit( -1 );
      } else {
        printf( "set sound converter info ok\n" );
      }

    if ( outAudioAtom ) {
        DisposePtr( (Ptr)outAudioAtom );
      }

    if ( siUnknownInfoType == err ) {
        // clear this error, the decompressor didn't
        // need the decompression atom and that's OK
        err = noErr;
      } else {
//        BailErr(err);
      }

    /** 
     * The input buffer has to be large enough so GetMediaSample isn't
     * going to fail, your mileage may vary
     */
    Handle inputBuffer = NewHandle( MAX_BUFFER_SIZE );
//    HLock( inputBuffer );

    /** Start the sound conversion */
    err = SoundConverterBeginConversion(mySoundConverter);
//    BailErr(err);

    /** Extract compressed audio from media track */
    TimeValue tperSample = 0;
    err = 
        GetMediaSample( media, inputBuffer, 0, 
                        &noBytes, 0, NULL, &tperSample, NULL, NULL, 0, &noSamples, NULL );
    if ( err != noErr ) {
        printf( "failed to fetch media sample data: %d\n", GetMoviesError() );
        exit( -1 );
      } else {
        printf( "media sample: %d (%d) bytes / %ld samples / %d per sample\n", 
                noBytes, GetHandleSize( inputBuffer ), noSamples, tperSample );
      }

    unsigned long niBytes = 0;
    SoundConverterGetBufferSizes( mySoundConverter, noBytes * noSamples,
                                  &niFrames, &niBytes, &noBytes );

    printf( "buffer sizes: frames: %d\tibytes: %d\tobytes: %d\n",
            niFrames, niBytes, noBytes );

    /** Convert into uncompressed audio */
    Ptr outputBuffer = NewPtr( noBytes * 1.2 );
    SoundConverterConvertBuffer( mySoundConverter, inputBuffer, noSamples /* niFrames */, 
                                 outputBuffer, &noFrames, &noBytes );

    printf( "converted: %d frames / %d bytes\n", noFrames, noBytes );

    /** Shutdown the sound converter */
    err = SoundConverterEndConversion( mySoundConverter, outputBuffer, &noFrames, &noBytes );

    printf( "converted final: %d frames / %d bytes\n", noFrames, noBytes );

//    HUnlock( inputBuffer );

    /** We now should have decompressed audio for the input file */
    /** 
     * So, generate visuals using a sliding sample grid at the
     * given framerate
     */

    /** Create a new movie clip with audio and video tracks */

    /** PROJECTM CRAP HERE -- stuff frames into QuickTime */

    /** Close movie file */

    /** Shutdown MovieToolbox */
    ExitMovies();

    return 0;
  }
#else
extern int main123( int argc, char **argv );
int main( int argc, char **argv ) {
    main123( argc, argv );
  }
#endif
