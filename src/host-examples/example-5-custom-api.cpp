//-----------------------------------------------------------------------------
// file: example-5-custom-api.cpp
// desc: Looking to integrate ChucK as a component inside another host program?
//       Here is an example of using ChucK as a library, with real-time audio,
//       and shows a C++ host can add a custom API that can be called from
//       the ChucK language side. This custom API bindings would be specific to
//       the c++ host, and is a powerful way to further integrate ChucK into a
//       a system.
//
//       INITIALIZATION
//         1. #include "chuck.h"
//         2. set up ChucK and real-time audio (see example-2-audio.cpp)
//         3. initialize ChucK, but don't start real-time audio just yet.
//            This is so ChucK's compiler and type system can be initialized;
//            we don't want to start real-time audio yet, since our next
//            step is not thread-safe, meaning it must be done before
//            starting real-time audio OR it must be done on the audio
//            thread; in this example, we will do the former.
//         4. provide a QUERY function to ChucK to call via ChucK::bind();
//            the function that a c++ host can use to programmatically add
//            custom classes to the ChucK type system
//            NOTE: this process is similar to defining a chugin (ChucK plugin)
//            -- and in fact uses the same Chuck_DL QUERY interface as chugins;
//            the only difference is that in this approach, the custom bindings
//            are statically compiled into the host, whereas chugins are
//            dynamically loaded libraries.
//         5. add code for ChucK to compile and run...
//            NOTE: this ChucK code can use the custom classes we've added
//            in the above step (#4).
//         6. start real-time audio
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
//   date: Winter 2025
//-----------------------------------------------------------------------------
#include "chuck.h"
#include "util_platforms.h" // for ck_usleep()
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
// custom API query function
DLL_QUERY host_query( Chuck_DL_Query * QUERY );




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

    return 0;
}


//-----------------------------------------------------------------------------
// error callback function (called by RtAudio on warning or error)
//-----------------------------------------------------------------------------
void error_cb( RtAudioErrorType type, const std::string & errorText )
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
    the_chuck->setParam( CHUCK_PARAM_IS_REALTIME_AUDIO_HINT, TRUE );
    // turn on logging to see what ChucK is up to; higher == more info
    // the_chuck->setLogLevel( 3 );

    // initialize real-time audio
    if( !init_realtime_audio( the_chuck->getParamInt( CHUCK_PARAM_SAMPLE_RATE ),
                              the_chuck->getParamInt( CHUCK_PARAM_INPUT_CHANNELS ),
                              the_chuck->getParamInt( CHUCK_PARAM_OUTPUT_CHANNELS ),
                              512 ) )
    { goto done; }

    // initialize ChucK, after the parameters are set
    the_chuck->init();

    //-------------------------------------------------------------------------
    // bind custom API here (must be after init())
    //-------------------------------------------------------------------------
    the_chuck->bind( host_query, "ExampleCustomAPI" );

    // start ChucK VM and synthesis engine
    the_chuck->start();

    // compile ChucK program from file
    // NOTE: this code can immediately use the custom types we just added
    // through using the query function
    if( !the_chuck->compileFile( "ck/test-5-custom.ck" ) )
    {
        // got error, baillng out...
        goto done;
    }

    // start real-time audio
    if( !start_realtime_audio() )
    { goto cleanup; }

    // print
    cerr << "chuck running...(press ctrl-c to quit)" << endl;
    
    // keep going as long as a shred is still running
    // (as long as CHUCK_PARAM_VM_HALT is set to TRUE)
    while( the_chuck->vm_running() )
    {
        // wait a bit
        ck_usleep( 10000 );
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


// function prototype for c++ hooks; these are the implementation for various functions
CK_DLL_CTOR( FooFoo_ctor );
CK_DLL_CTOR( FooFoo_ctor_value );
CK_DLL_DTOR( FooFoo_dtor );
CK_DLL_MFUN( FooFoo_getValue );
CK_DLL_MFUN( FooFoo_setValue );
CK_DLL_MFUN( FooFoo_increment );
CK_DLL_SFUN( FooFoo_add );

// offset for internal instanced data; this is typically used to associate
// a pointer of a c++ data structure to an instance of a ChucK object (see below)
static t_CKINT host_offset_data = 0;


//-----------------------------------------------------------------------------
// query fuction for adding custom API
//-----------------------------------------------------------------------------
DLL_QUERY host_query( Chuck_DL_Query * QUERY )
{
    // get global
    Chuck_Env * env = QUERY->env();
    // name (optional)
    QUERY->setname( QUERY, "OurCustomHostAPI" );

    // add a class, and specify its parent class
    QUERY->begin_class( QUERY, "FooFoo", "Object" );
    // add documentatiopn for the class
    QUERY->doc_class( QUERY, "created for the sake of this example, FooFoo is a custom class" );

    // add a default contructor
    QUERY->add_ctor( QUERY, FooFoo_ctor );
    // add destructor
    QUERY->add_dtor( QUERY, FooFoo_dtor );

    // add a overloaded contructor, with arguments
    QUERY->add_ctor( QUERY, FooFoo_ctor_value );
    // add arguments
    QUERY->add_arg( QUERY, "float", "value" );
    // add documentation for this function
    QUERY->doc_func( QUERY, "construct a FooFoo instance with value" );

    // add a *member* function (mfun) by providing a hook, return type, and name
    QUERY->add_mfun( QUERY, FooFoo_setValue, "float", "setValue" );
    // add arguments
    QUERY->add_arg( QUERY, "float", "value" );
    // add documentation for this function
    QUERY->doc_func( QUERY, "set FooFoo's value!" );

    // add a *member* function (mfun) by providing a hook, return type, and name
    QUERY->add_mfun( QUERY, FooFoo_getValue, "float", "getValue" );
    // add documentation for this function
    QUERY->doc_func( QUERY, "get FooFoo's value!" );

    // add a *member* function (mfun) by providing a hook, return type, and name
    QUERY->add_mfun( QUERY, FooFoo_increment, "float", "increment" );
    // add arguments
    QUERY->add_arg( QUERY, "float", "x" );
    // add documentation for this function
    QUERY->doc_func( QUERY, "increment FooFoo's value by x" );

    // add a *static* function (sfun) by providing a hook, return type, and name
    QUERY->add_sfun( QUERY, FooFoo_add, "float", "add" );
    // add arguments
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );
    // add documentation for this function
    QUERY->doc_func( QUERY, "add two floating point numbers" );

    // add member variable (not usable from chuck)
    host_offset_data = QUERY->add_mvar( QUERY, "int", "@host_data", FALSE );

    // NOTE: there are many additional kinds of things that can be added in
    // in an custom API query, including member variables, static variables,
    // Event that can triggered, and more; see various chugins for reference,
    // (e.g., ChuGL, which contains an extensive API)

    // end class definition
    QUERY->end_class( QUERY );

    // return success!
    return TRUE;
}

// example of host-side c++ data strcture
// these serve as internal implementation to the ChucK types we are binding
struct HostData
{
    // some state
    float theValue;
    // add any other state and behaviors here...

    // constructor
    HostData() { theValue = 0; }
};

// implementation for FooFoo's default constructor
CK_DLL_CTOR( FooFoo_ctor )
{
    // create internal host-side data structure
    HostData * data = new HostData;
    // set into chuck object's data field
    OBJ_MEMBER_INT( SELF, host_offset_data ) = (t_CKINT)data;
}

// implementation for FooFoo's overloaded constructor FooFoo( float )
CK_DLL_CTOR( FooFoo_ctor_value )
{
    // create internal host-side data structure
    HostData * data = new HostData;
    // retrieve argument
    t_CKFLOAT arg = GET_NEXT_FLOAT( ARGS );
    // set into chuck object's data field
    OBJ_MEMBER_INT( SELF, host_offset_data ) = (t_CKINT)data;
    // set the value
    data->theValue = arg;
}

// implementation for FooFoo's destructor
CK_DLL_DTOR( FooFoo_dtor )
{
    // retrieve our c++/host-side data structure
    HostData * data = (HostData *)OBJ_MEMBER_INT( SELF, host_offset_data );
    // release the memory
    CK_SAFE_DELETE( data );
    // set the data field to zero, in the corresponding chuck object instance
    OBJ_MEMBER_INT( SELF, host_offset_data ) = 0;
}

// implementation of FooFoo.getValue()
CK_DLL_MFUN( FooFoo_getValue )
{
    // retrieve our c++/host-side data structure
    HostData * data = (HostData *)OBJ_MEMBER_INT( SELF, host_offset_data );
    // set the return value
    RETURN->v_float = data->theValue;
}

// implementation of FooFoo.getValue( float )
CK_DLL_MFUN( FooFoo_setValue )
{
    // retrieve our c++/host-side data structure
    HostData * data = (HostData *)OBJ_MEMBER_INT( SELF, host_offset_data );
    // retrieve argument
    t_CKFLOAT arg = GET_NEXT_FLOAT( ARGS );
    // set the value from the argument
    data->theValue = arg;
    // set the return value
    RETURN->v_float = data->theValue;
}

// implementation of FooFoo.increment( float )
CK_DLL_MFUN( FooFoo_increment )
{
    // retrieve our c++/host-side data structure
    HostData * data = (HostData *)OBJ_MEMBER_INT( SELF, host_offset_data );
    // retrieve argument
    t_CKFLOAT arg = GET_NEXT_FLOAT( ARGS );
    // set the value from the argument
    data->theValue += arg;
    // set the return value
    RETURN->v_float = data->theValue;
}

// implementation of STATIC function FooFoo.add( float, float )
CK_DLL_SFUN( FooFoo_add )
{
    // retrieve arguments
    t_CKFLOAT x = GET_NEXT_FLOAT( ARGS );
    t_CKFLOAT y = GET_NEXT_FLOAT( ARGS );
    // set sum to be returned as a float
    RETURN->v_float = x + y;
}
