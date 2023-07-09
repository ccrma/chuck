//-----------------------------------------------------------------------------
// file: example-1-minimal.cpp
// desc: Looking to integrate ChucK as a component inside another host program?
//       Here is a minimal example of using ChucK as a library!
//
//       FYI generally speaking, this is how ChucK core is integrated
//       into various host systems in the ChucK ecosystem -- including:
//       command-line chuck, miniAudicle, webchuck, chunity, chunreal, etc.
//
//       INITIALIZATION
//         1. #include "chuck.h"
//         2. make a new ChucK instance, e.g., the_chuck = new ChucK();
//         3. set parameters
//         4. initialize and start the ChucK instance
//         5. allocate audio buffers
//
//       RUNTIME
//         1. add code for ChucK to compile and run...
//            using the_chuck->compileCode() and/or the_chuck->compileFile()
//         2. set up a simple run-loop and instruct ChucK to run
//            one audio buffer size at time (corresponding to a chunk of time)
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
//   date: Summer 2023
//-----------------------------------------------------------------------------
#include "chuck.h"

#include <string>
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


//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
// allocate global audio buffers
void alloc_global_buffers( t_CKINT bufferSize );
// de-allocate global audio buffers
void cleanup_global_buffers();




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
    the_chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, 2 );
    // number of output channels
    the_chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, 2 );
    // whether to halt the VM when there is no more shred running
    the_chuck->setParam( CHUCK_PARAM_VM_HALT, TRUE );
    // set hint so internally can advise things like async data writes etc.
    the_chuck->setParam( CHUCK_PARAM_HINT_IS_REALTIME_AUDIO, FALSE );
    // turn on logging to see what ChucK is up to; higher == more info
    // chuck->setLogLevel( 3 );

    // allocate audio buffers, also after parameters are set
    alloc_global_buffers( 1024 );

    // initialize ChucK, after the parameters are set
    the_chuck->init();

    //-------------------------------------------------------------------------
    // ChucK runtime
    //-------------------------------------------------------------------------
    // start ChucK VM and synthesis engine
    the_chuck->start();

    // compile some ChucK code from string (this can be called on-the-fly and repeatedly)
    if( !the_chuck->compileCode( "repeat(10) { <<< \"hello ChucK! random integer:\", Math.random2(1,100) >>>; }", "", 1 ) )
    {
        // got error, baillng out...
        exit( 1 );
    }

    // keep going as long as a shred is still running
    // (as long as CHUCK_PARAM_VM_HALT is set to TRUE)
    while( the_chuck->vm_running() )
    {
        // run ChucK for the next `bufferSize`
        // pre-condition: `input` constains any audio input
        // post-condition: `output` constains any synthesized audio
        the_chuck->run( g_inputBuffer, g_outputBuffer, g_bufferSize );
    }

    // deallocate
    cleanup_global_buffers();
    
    // clean up ChucK
    CK_SAFE_DELETE( the_chuck );

    return 0;
}




//-----------------------------------------------------------------------------
// initialize global audio buffers
//-----------------------------------------------------------------------------
void alloc_global_buffers( t_CKINT bufferSize )
{
    // good practice to clean, in case this function is called more than once
    cleanup_global_buffers();

    // allocate audio buffers as expected by ChucK's run() function below
    g_inputBuffer = new SAMPLE[bufferSize * the_chuck->getParamInt(CHUCK_PARAM_INPUT_CHANNELS)];
    g_outputBuffer = new SAMPLE[bufferSize * the_chuck->getParamInt(CHUCK_PARAM_OUTPUT_CHANNELS)];
    // zero out
    memset( g_inputBuffer, 0, bufferSize * the_chuck->getParamInt(CHUCK_PARAM_INPUT_CHANNELS) );
    memset( g_outputBuffer, 0, bufferSize * the_chuck->getParamInt(CHUCK_PARAM_OUTPUT_CHANNELS) );
    // set the buffer size
    g_bufferSize = bufferSize;
}




//-----------------------------------------------------------------------------
// cleanup global audio buffers
//-----------------------------------------------------------------------------
void cleanup_global_buffers()
{
    // reclaim memory
    CK_SAFE_DELETE_ARRAY( g_inputBuffer );
    CK_SAFE_DELETE_ARRAY( g_outputBuffer );
    // reset
    g_bufferSize = 0;
}
