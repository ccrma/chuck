//-----------------------------------------------------------------------------
// file: example-4-shreds.cpp
// desc: Looking to integrate ChucK as a component inside another host program?
//       Here is an example of using ChucK as a library, with real-time audio
//       and various shred-related operations from c++, including adding,
//       removing last, and getting the current VM status including info about
//       shreds
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
//         6. start real-time audio
//         7. use various parts of the API (see below) to work with
//            ChucK shreds: adding (implictly done with compileCode() and
//            compileFile()), removing previously added shreds, getting info
//            on all shreds in the VM, and printing status info
//
//            NOTE: in general, this example uses thread-safe versions of
//            the shreds API, which means these functions can be called from
//            a thread other than the audio thread / the thread that calls
//            ChucK::run(...).
//
//       RUNTIME
//         ** from within the audio callback function -- audio_cb(...), set up
//            in step #3 above -- call the_chuck->run() with the audio buffers
//            and the I/O frame size
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
//   date: Summer 2023
//-----------------------------------------------------------------------------
#include "chuck.h"
#include "chuck_vm.h" // since we will reaching deeper into engine
#include "util_platforms.h" // for ck_usleep() and ck_isatty()
#include "RtAudio.h"

#include <vector>
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
// VM message callback function (called by chuck VM in response to queue_msg())
//-----------------------------------------------------------------------------
void vm_msg_cb( const Chuck_Msg * msg )
{
    // check the type
    switch( msg->type )
    {
        // cases we'd like to handle
        case CK_MSG_STATUS:
        {
            // get status info
            Chuck_VM_Status * status = msg->status;
            // verify
            if( status )
            {
                // number shreds
                cerr << "\n-------------------------------" << endl;
                cerr << "printing from callback function" << endl;
                cerr << "-------------------------------" << endl;
                cerr << "# of shreds in VM: " << status->list.size() << endl;
                // chuck time
                cerr << "chuck time: " << status->now_system << "::samp ("
                     << status->t_hour << "h"
                     << status->t_minute << "m"
                     << status->t_second << "s)" << endl;

                // print status
                if( status->list.size() ) cerr << "--------"  << endl;
                for( t_CKUINT i = 0; i < status->list.size(); i++ )
                {
                    // get shred info
                    Chuck_VM_Shred_Status * info = status->list[i];
                    // print shred info
                    cerr << "[shred] id: " << info->xid
                         << " source: " << info->name.c_str()
                         << " spork time: " << (status->now_system - info->start) / status->srate
                         << " state: " << (!info->has_event ? "ACTIVE" : "(waiting on event)") << endl;
                }
                if( status->list.size() ) cerr << "--------"  << endl;

                // clean up
                CK_SAFE_DELETE( status );
            }
        }
        break;
    }
    
    // our responsibility to delete
    CK_SAFE_DELETE( msg );
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
// print usage for the simple command prompt in this example
//-----------------------------------------------------------------------------
void usage()
{
    // print
    cerr << "-------------------" << endl;
    cerr << "type '+' or 'add' to add a shred" << endl;
    cerr << "type '^' or 'status' to get VM info and print shred status" << endl;
    cerr << "type '--' or 'remove.last' to remove last shred" << endl;
    cerr << "type 'remove.all' to remove all shreds" << endl;
    cerr << "type 'time' to print time" << endl;
    cerr << "type 'exit' to quit this program" << endl;
    cerr << "-------------------" << endl << endl;
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
    // set to false so a VM without shreds will keep running
    the_chuck->setParam( CHUCK_PARAM_VM_HALT, FALSE );
    // set hint so internally can advise things like async data writes etc.
    the_chuck->setParam( CHUCK_PARAM_IS_REALTIME_AUDIO_HINT, TRUE );
    // turn on color output mode
    the_chuck->setParam( CHUCK_PARAM_TTY_COLOR, ck_isatty() );

    // shred id
    t_CKUINT shredID = 0;
    // our shred ID stack
    vector<t_CKUINT> shredIDs;
    // filename
    string filename = "ck/test-4-shreds.ck";
    // chuck msg pointer; for communicating with VM
    Chuck_Msg * msg = NULL;

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

    // start real-time audio
    if( !start_realtime_audio() )
    { goto cleanup; }
    
    // print
    cerr << "chuck running...(press ctrl-c or type 'exit' to quit)" << endl;

    // wait a bit to give audio callback a chance to start up
    ck_usleep( 200000 );

    // keep going as long as a shred is still running
    // (as long as CHUCK_PARAM_VM_HALT is set to TRUE)
    while( the_chuck->vm_running() )
    {
        // wait a bit to give audio callback function a chance to receive
        // message reply from chuck VM and print before we print usage
        ck_usleep( 100000 );
        // print usage
        usage();

        // prompt
        cerr << "enter a command > ";
        // command
        string command; getline( cin, command );

        // check
        if( command == "+" || command == "add" )
        {
            // compile file; FALSE means deferred spork -- thread-safe since
            // we are on a different thread as the audio thread that is calling
            // the_chuck->run(...)
            the_chuck->compileFile( filename, "", 1, FALSE );
            // get the id of the previously sporked shred
            shredID = the_chuck->vm()->last_id();
            // print out the last VM shred id (should be the shred we just compiled)
            cerr << "adding shred '" << filename << "' with ID: " << shredID  << endl;
            // push on stack
            shredIDs.push_back( shredID );
        }
        else if( command == "--" || command == "remove" )
        {
            // if already added
            if( !shredIDs.size() )
            {
                cerr << "no shred to remove!" << endl;
                continue;
            }
            // create a message; VM will delete
            msg = new Chuck_Msg;
            // message type
            msg->type = CK_MSG_REMOVE;
            // signify no value (will remove last)
            msg->param = shredIDs.back();
            // queue on VM
            the_chuck->vm()->queue_msg( msg );
            // pop
            shredIDs.pop_back();
        }
        else if( command == "^" || command == "status" )
        {
            // create a message, we will delete later on
            msg = new Chuck_Msg;
            // message type
            msg->type = CK_MSG_STATUS;
            // create a status info
            msg->status = new Chuck_VM_Status();
            // add callback function
            msg->reply_cb = vm_msg_cb;
            // queue msg in VM; reply will be received by the callback
            the_chuck->vm()->queue_msg( msg );
        }
        else if( command == "status-2" )
        {
            // create a message; VM will delete
            msg = new Chuck_Msg;
            // message type
            msg->type = CK_MSG_STATUS;
            // is passed in without further argument
            // this will cause VM to print shred status
            // queue on VM
            the_chuck->vm()->queue_msg( msg );
        }
        else if( command == "remove.all" )
        {
            // create a message; VM will delete
            msg = new Chuck_Msg;
            // message type
            msg->type = CK_MSG_REMOVEALL;
            // queue on VM
            the_chuck->vm()->queue_msg( msg );
        }
        else if( command == "time" )
        {
            // create a message; VM will delete
            msg = new Chuck_Msg;
            // message type
            msg->type = CK_MSG_TIME;
            // queue on VM
            the_chuck->vm()->queue_msg( msg );
        }
        else if( command == "exit" )
        {
            break;
        }
        else
        {
            // what
            cerr << "*** unrecognized command: " << command << " ***" << endl;
        }
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
