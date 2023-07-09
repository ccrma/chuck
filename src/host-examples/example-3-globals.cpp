//-----------------------------------------------------------------------------
// file: example-3-globals.cpp
// desc: Looking to integrate ChucK as a component inside another host program?
//       Here is an example of using ChucK as a library, with real-time audio,
//       and communication betweeh C++ and ChucK through ChucK global variables!
//
//       NOTE: the Chuck_Globals_Manager API has TWO variants for communicating
//       between C++ and ChucK; (see chuck_globals.h for more information)
//       -- CamelCase named functions in C++ are to be used from outside the
//          audio thread (e.g., setGlobalInt())
//       -- lower_case named functions in C++ are to be used from within the
//          audio thread (e.g., get_global_int_value())
//
//       INITIALIZATION
//         1. #include "chuck.h"
//         2. make a new ChucK instance, e.g., the_chuck = new ChucK();
//         3. set up real-time audio (e.g., using RtAudio or another way)
//            -- this also sets up an audio callback function that will
//            be automatically called each time the system needs a new
//            chunk of audio to process / playback
//         4. set ChucK parameters
//         5. initialize and start the ChucK instance
//         6. add code for ChucK to compile and run...
//            using the_chuck->compileCode() or the_chuck->compileFile()
//            (after starting real-time audio, this should only be done
//            from the audio thread (same thread as the_chuck->run())
//         7. start real-time audio
//
//       RUNTIME
//         1. from within the audio callback function -- audio_cb(...), set up
//            in step #3 above -- call the_chuck->run() with the audio buffers
//            and the I/O frame size
//         2. the main thread waits for user input and then use
//            the_chuck()->globals() to signal a ChucK Event from C++
//         3. the audio thread get the value of a ChucK global variable
//            and if it changed from last time, prints its value
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
//   date: Summer 2023
//-----------------------------------------------------------------------------
#include "chuck.h"
#include "chuck_globals.h" // for globals
#include "RtAudio.h"

#include <iostream>
using namespace std;


//-----------------------------------------------------------------------------
// global things for this example
//-----------------------------------------------------------------------------
// our ChucK instance
ChucK * the_chuck = NULL;
// audio input buffer
SAMPLE * g_inputBuffer = NULL;
// audio output buffer
SAMPLE * g_outputBuffer = NULL;
// our audio buffer size
t_CKINT g_bufferSize = 0;

// real-time audio
RtAudio * the_rtaudio = NULL;

// last frequency
t_CKFLOAT g_last_frequency = 0;


//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
// initialize real-time audio
t_CKBOOL init_realtime_audio( t_CKINT sampleRate,
                              t_CKINT inputChannels,
                              t_CKINT outputChannels,
                              t_CKINT preferredBufferSize );
// start real-time audio
t_CKBOOL start_realtime_audio();
// clean up real-time audio
t_CKBOOL cleanup_realtime_audio();




//-----------------------------------------------------------------------------
// audio callback function (called by RtAudio)
//-----------------------------------------------------------------------------
int audio_cb( void * outputBuffer, void * inputBuffer, unsigned int nBufferFrames,
              double streamTime, RtAudioStreamStatus status, void * data )
{
    SAMPLE * input = (SAMPLE *)inputBuffer;
    SAMPLE * output = (SAMPLE *)outputBuffer;
    
    // run ChucK for # of frames
    the_chuck->run( input, output, nBufferFrames );
    
    // get global variable from chuck
    // NOTE: this is using lower_case version (calling from audio thread)
    t_CKFLOAT f = the_chuck->globals()->get_global_float_value( "the_frequency" );
    // if different
    if( f != g_last_frequency )
    {
        // print
        cerr << "getting global value from ChucK: " << f << endl;
        // set
        g_last_frequency = f;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// error callback function (called by RtAudio on warning or error)
//-----------------------------------------------------------------------------
void error_cb( RtAudioErrorType type, const std::string &errorText )
{
    // print the type and error text from RtAudio
    cerr << "[real-time audio](error " << (int)type << "): " << errorText << endl;
}




//-----------------------------------------------------------------------------
// host program entry point
//-----------------------------------------------------------------------------
int main( int argc, char ** argv )
{
    //-------------------------------------------------------------------------
    // ChucK initialization
    //-------------------------------------------------------------------------
    // instantiate a ChucK instance
    the_chuck = new ChucK();

    // set some parameters: sample rate
    the_chuck->setParam( CHUCK_PARAM_SAMPLE_RATE, 44100 );
    // number of input channels
    the_chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, 0 );
    // number of output channels
    the_chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, 2 );
    // whether to halt the VM when there is no more shred running
    the_chuck->setParam( CHUCK_PARAM_VM_HALT, TRUE );
    // set hint so internally can advise things like async data writes etc.
    the_chuck->setParam( CHUCK_PARAM_HINT_IS_REALTIME_AUDIO, TRUE );
    // turn on logging to see what ChucK is up to; higher == more info
    // chuck->setLogLevel( 3 );

    // a char
    char c;

    // initialize real-time audio
    if( !init_realtime_audio( the_chuck->getParamInt( CHUCK_PARAM_SAMPLE_RATE ),
                              the_chuck->getParamInt( CHUCK_PARAM_INPUT_CHANNELS ),
                              the_chuck->getParamInt( CHUCK_PARAM_OUTPUT_CHANNELS ),
                              512 ) )
    { goto done; }

    // initialize ChucK, after the parameters are set
    the_chuck->init();

    //-------------------------------------------------------------------------
    // ChucK runtime
    //-------------------------------------------------------------------------
    // start ChucK VM and synthesis engine
    the_chuck->start();

    // compile ChucK program from file (this can be called on-the-fly and repeatedly)
    if( !the_chuck->compileFile( "ck/test-3-globals.ck", "", 1 ) )
    {
        // got error, baillng out...
        goto done;
    }

    // start real-time audio
    if( !start_realtime_audio() )
    { goto cleanup; }

    // keep going as long as a shred is still running
    // (as long as CHUCK_PARAM_VM_HALT is set to TRUE)
    while( the_chuck->vm_running() )
    {
        // print
        cerr << "press [CTRL-C] to quit" << endl;
        cerr << "press [ENTER] to signal next note" << endl;;
        // get a char
        cin.get(c);

        // signal the event in ChucK
        // NOTE: this is using CamelCase version (calling from outside of audio thread)
        the_chuck->globals()->broadcastGlobalEvent( "the_nextNote" );
    }

cleanup:
    // clean up real-time audio
    cleanup_realtime_audio();

done:
    // clean up ChucK
    CK_SAFE_DELETE( the_chuck );

    return 0;
}


//-----------------------------------------------------------------------------
// initialize real-time audio
//-----------------------------------------------------------------------------
t_CKBOOL init_realtime_audio( t_CKINT sampleRate,
                              t_CKINT inputChannels,
                              t_CKINT outputChannels,
                              t_CKINT preferredBufferSize )
{
    // instantiate a new RtAudio
    the_rtaudio = new RtAudio();

    // check device count
    if( the_rtaudio->getDeviceCount() < 1 )
    {
        cerr << "[real-time audio]: no audio device found!" << endl;
        return FALSE;
    }
    
    // set error callback
    the_rtaudio->setErrorCallback( error_cb );

    // input params
    RtAudio::StreamParameters iParams;
    iParams.deviceId = the_rtaudio->getDefaultInputDevice();
    iParams.nChannels = inputChannels;

    // output params
    RtAudio::StreamParameters oParams;
    oParams.deviceId = the_rtaudio->getDefaultOutputDevice();
    oParams.nChannels = outputChannels;

    // buffer frames variable
    unsigned int bufferFrames = preferredBufferSize;
    // audio data type
    t_CKUINT datatype = RTAUDIO_FLOAT32;
    if( sizeof(SAMPLE) == 8 ) datatype = RTAUDIO_FLOAT64;

    // open audio stream
    RtAudioErrorType retval = the_rtaudio->openStream( &oParams,
                                                       inputChannels > 0 ? &iParams : NULL,
                                                       datatype, sampleRate, &bufferFrames,
                                                       &audio_cb, NULL );
    // check
    return retval == RTAUDIO_NO_ERROR;
}


//-----------------------------------------------------------------------------
// start real-time audio
//-----------------------------------------------------------------------------
t_CKBOOL start_realtime_audio()
{
    // check
    if( !the_rtaudio )
    {
        cerr << "[real-time audio]: uninitialized..." << endl;
        return FALSE;
    }

    // start the stream
    RtAudioErrorType retval = the_rtaudio->startStream();
    // check
    return retval == RTAUDIO_NO_ERROR;
}


//-----------------------------------------------------------------------------
// clean up real-time audio
//-----------------------------------------------------------------------------
t_CKBOOL cleanup_realtime_audio()
{
    // check
    if( !the_rtaudio )
    {
        cerr << "[real-time audio]: uninitialized..." << endl;
        return FALSE;
    }

    // stop the stream
    RtAudioErrorType retval = the_rtaudio->stopStream();
    // clean up
    if( the_rtaudio->isStreamOpen() ) the_rtaudio->closeStream();
    
    // done
    return retval == RTAUDIO_NO_ERROR;
}
