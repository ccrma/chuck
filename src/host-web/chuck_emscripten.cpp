//-----------------------------------------------------------------------------
// name: chuck_emscripten.cpp
// desc: emscripten binding agent for web assembly compilation
//       e.g., used for WebChucK and Chunity web target
//
// author: Jack Atherton
// date: created 4/19/17
//-----------------------------------------------------------------------------
#include "chuck_emscripten.h"
#include "chuck_globals.h"

#include <iostream>
#include <map>

// #ifndef __PLATFORM_WINDOWS__
// #include <unistd.h>
// #endif

// c linkage
extern "C"
{
    // chuck instances
    std::map< unsigned int, ChucK * > chuck_instances;
    // global data dir
    std::string chuck_global_data_dir;

    // something to do nothing
    void dummyFunction()
    { }

    // a call to do nothing
    void EMSCRIPTEN_KEEPALIVE dummyCall()
    {
        // call dummy function so Browser is included
        emscripten_async_call( (em_arg_callback_func) dummyFunction, NULL, 1000 );
    }


    // C# "string" corresponds to passing char *
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckCode( unsigned int chuckID, const char * code )
    {
        // check to see id has instances
        if( chuck_instances.count(chuckID) == 0 )
        { return 0; } // 1.5.0.8 (ge) | was: { return -1; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );

        // compile it!
        // 1.5.0.8 (ge) explicitly set count=1, immediate=TRUE
        if( chuck_instances[chuckID]->compileCode( code, "", 1, TRUE ) )
        {
            // last shred shredded
            return chuck_instances[chuckID]->vm()->last_id();
        }

        // failure to compile
        return 0;
    }


    // run code with dac replacement
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckCodeWithReplacementDac(
        unsigned int chuckID, const char * code, const char * replacement_dac )
    {
        // check to see id has instances
        if( chuck_instances.count(chuckID) == 0 )
        { return 0; } // 1.5.0.8 (ge) | was: { return -1; }

        // replace dac
        chuck_instances[chuckID]->compiler()->setReplaceDac( TRUE,
            std::string( replacement_dac ) );
        // compile it!
        // 1.5.0.8 (ge) explicitly set count=1, immediate=TRUE
        t_CKBOOL result = chuck_instances[chuckID]->compileCode( code, "", 1, TRUE );
        // don't replace dac for future compilations
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );

        // check result
        if( result ) {
            // last shred shredded
            return chuck_instances[chuckID]->vm()->last_id();
        }

        // failure to compile
        return 0;
    }


    // run chuck from file
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFile( unsigned int chuckID, const char * filename )
    {
        // run with empty args
        return runChuckFileWithArgs( chuckID, filename, "" );
    }


    // run chuck from file with args
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFileWithArgs( unsigned int chuckID,
        const char * filename, const char * args )
    {
        // check to see id has instances
        if( chuck_instances.count(chuckID) == 0 )
        { return 0; } // 1.5.0.8 (ge) | was: { return -1; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );

        // compile it!
        // 1.5.0.8 (ge) explicitly set count=1, immediate=TRUE
        if( chuck_instances[chuckID]->compileFile( filename, args, 1, TRUE ) )
        {
            // last shred shredded
            return chuck_instances[chuckID]->vm()->last_id();
        }

        // failure to compile
        return 0;
    }


    // run chuck from file with replacement dac
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFileWithReplacementDac(
        unsigned int chuckID, const char * filename,
        const char * replacement_dac )
    {
        // run with empty args
        return runChuckFileWithArgsWithReplacementDac(
            chuckID, filename, "", replacement_dac
        );
    }


    // run chuck from file with replacement dac with args
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFileWithArgsWithReplacementDac(
        unsigned int chuckID, const char * filename, const char * args,
        const char * replacement_dac )
    {
        // check to see id has instances
        if( chuck_instances.count(chuckID) == 0 )
        { return 0; } // 1.5.0.8 (ge) | was: { return -1; }

        // replace dac
        chuck_instances[chuckID]->compiler()->setReplaceDac( TRUE,
            std::string( replacement_dac ) );
        // compile it!
        // 1.5.0.8 (ge) explicitly set count=1, immediate=TRUE
        t_CKBOOL result = chuck_instances[chuckID]->compileFile( filename, args, 1, TRUE );
        // don't replace dac for future compilations
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );

        // check result
        if( result ) {
            // last shred shredded
            return chuck_instances[chuckID]->vm()->last_id();
        }

        // failure to compile
        return 0;
    }


    // replace code given a chuck instance
    t_CKUINT replaceCode( ChucK * chuck, t_CKUINT shredID, std::vector<std::string> * args )
    {
        Chuck_Msg * msg = new Chuck_Msg;
        msg->type = CK_MSG_REPLACE;
        msg->code = chuck->compiler()->output();
        msg->param = shredID;
        // set so that VM will delete for us when it's done
        msg->reply_queue = FALSE;
        // 1.5.0.8 use set for proper memory management
        msg->set( args ); // msg->args = args;

        // call the process_msg directly: we need to know the shred ID
        // and it's JS so we won't be pre-empted
        // also: return value is the shred ID!
        return chuck->vm()->process_msg( msg );
    }


    // replace chuck code
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckCode( unsigned int chuckID, unsigned int shredID, const char * code )
    {
        // check to see id has instances
        if( chuck_instances.count(chuckID) == 0 )
        { return 0; } // 1.5.0.8 (ge) | was: { return -1; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );

        // compile it! and spork 0 times (ONLY compile)
        if( chuck_instances[chuckID]->compileCode( code, "", 0 ) )
        {
            // replace code
            return replaceCode( chuck_instances[chuckID], shredID, NULL );
        }

        // failure to compile
        return 0;
    }


    // replace chuck code with replacement dac
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckCodeWithReplacementDac( unsigned int chuckID, unsigned int shredID,
                                                                      const char * code, const char * replacement_dac )
    {
        // check to see id has instances
        if( chuck_instances.count(chuckID) == 0 )
        { return 0; } // 1.5.0.8 (ge) | was: { return -1; }

        // replace dac
        chuck_instances[chuckID]->compiler()->setReplaceDac( TRUE, replacement_dac );
        // compile it! and spork 0 times. (ONLY compile)
        t_CKBOOL result = chuck_instances[chuckID]->compileCode( code, "", 0 );
        // don't replace dac for future compilations
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );
        // check result
        if( result )
        {
            // replace code
            return replaceCode( chuck_instances[chuckID], shredID, NULL );
        }

        // failure to compile
        return 0;
    }


    // replace chuck file
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFile( unsigned int chuckID, unsigned int shredID, const char * filename )
    {
        // replace with empty args
        return replaceChuckFileWithArgs( chuckID, shredID, filename, "" );
    }


    // replace chuck file with replacement dac
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFileWithReplacementDac( unsigned int chuckID, unsigned int shredID,
                                                                      const char * filename, const char * replacement_dac )
    {
        // replace with empty args
        return replaceChuckFileWithArgsWithReplacementDac( chuckID, shredID, filename, "", replacement_dac );
    }


    // replace chuck file with args
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFileWithArgs( unsigned int chuckID, unsigned int shredID,
                                                            const char * filename, const char * args )
    {
        // check to see id has instances
        if( chuck_instances.count(chuckID) == 0 )
        { return 0; } // 1.5.0.8 (ge) | was: { return -1; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );
        
        std::string filenameS( filename );
        std::string argsS( args );

        // compile it! and spork 0 times (ONLY compile)
        if( chuck_instances[chuckID]->compileFile( filenameS, argsS, 0 ) )
        {
            // find args
            std::string found_filename;
            std::vector<std::string> found_args;
            std::string together = filenameS + ":" + argsS;
            // parse out command line arguments
            if( !extract_args( together, found_filename, found_args ) )
            {
                // error
                CK_FPRINTF_STDERR( "[chuck]: malformed filename with argument list...\n" );
                CK_FPRINTF_STDERR( "    -->  '%s'", together.c_str() );
                return 0;
            }
            
            // replace code
            return replaceCode( chuck_instances[chuckID], shredID, &found_args );
        }

        // failure to compile
        return 0;
    }


    // replace chuck by file with args and with replacement dac
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFileWithArgsWithReplacementDac(
        unsigned int chuckID, unsigned int shredID, const char * filename,
        const char * args, const char * replacement_dac )
    {
        // check to see id has instances
        if( chuck_instances.count(chuckID) == 0 )
        { return 0; } // 1.5.0.8 (ge) | was: { return -1; }

        // replace dac
        chuck_instances[chuckID]->compiler()->setReplaceDac( TRUE, replacement_dac );

        // file name
        std::string filenameS( filename );
        // args
        std::string argsS( args );
        // compile it! and spork 0 times (ONLY compile)
        t_CKBOOL result = chuck_instances[chuckID]->compileFile( filenameS, argsS, 0 );

        // don't replace dac for future compilations
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );
        // check result
        if( result )
        {
            // find args
            std::string found_filename;
            std::vector<std::string> found_args;
            std::string together = filenameS + ":" + argsS;
            // parse out command line arguments
            if( !extract_args( together, found_filename, found_args ) )
            {
                // error
                CK_FPRINTF_STDERR( "[chuck]: malformed filename with argument list...\n" );
                CK_FPRINTF_STDERR( "    -->  '%s'", together.c_str() );
                return 0;
            }
            
            // replace code
            return replaceCode( chuck_instances[chuckID], shredID, &found_args );
        }

        // failure to compile
        return 0;
    }


    // is shred alive
    bool EMSCRIPTEN_KEEPALIVE isShredActive( unsigned int chuckID, unsigned int shredID )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // look up shred by id
        Chuck_VM_Shred * shred = chuck_instances[chuckID]->vm()->shreduler()->lookup(shredID);
        // return
        return (shred != NULL);
    }


    // remove shred
    bool EMSCRIPTEN_KEEPALIVE removeShred( unsigned int chuckID, unsigned int shredID )
    {
        // check for chuck and shred id
        if( chuck_instances.count( chuckID ) == 0 || !isShredActive( chuckID, shredID ) )
        { return false; }
        
        // the chuck to clear
        ChucK * chuck = chuck_instances[chuckID];
    
        // create a msg asking to remove the shred
        Chuck_Msg * msg = new Chuck_Msg;
        msg->type = CK_MSG_REMOVE;
        msg->param = shredID;
        // set so that VM will delete for us when it's done
        msg->reply_queue = FALSE;

        // tell the VM to clear
        chuck->globals()->execute_chuck_msg_with_globals( msg );
    
        return true;
    }


    // set chuck global int
    bool EMSCRIPTEN_KEEPALIVE setChuckInt( unsigned int chuckID, const char * name, t_CKINT val )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // set global int value
        return chuck_instances[chuckID]->globals()->setGlobalInt( name, val );
    }


    // get chuck global int
    t_CKINT EMSCRIPTEN_KEEPALIVE getChuckInt( unsigned int chuckID, const char * name )
    {
        // check if chuck id has instances
        if( chuck_instances.count(chuckID) == 0 ) { return false; }
        // set global value
        return chuck_instances[chuckID]->globals()->get_global_int_value(name);
    }


    // set chuck global float
    bool EMSCRIPTEN_KEEPALIVE setChuckFloat( unsigned int chuckID, const char * name, t_CKFLOAT val )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // set global value
        return chuck_instances[chuckID]->globals()->setGlobalFloat( name, val );
    }


    // get chuck global float
    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getChuckFloat( unsigned int chuckID, const char * name )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // get global value
        return chuck_instances[chuckID]->globals()->get_global_float_value( name );
    }


    // set chuck global string
    bool EMSCRIPTEN_KEEPALIVE setChuckString( unsigned int chuckID, const char * name, const char * val )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // set global val
        return chuck_instances[chuckID]->globals()->setGlobalString( name, val );
    }


    // get chuck global string by callbackk
    bool EMSCRIPTEN_KEEPALIVE getChuckString( unsigned int chuckID, const char * name, void (* callback)(const char *) )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // get global val via callback
        return chuck_instances[chuckID]->globals()->getGlobalString( name, callback );
    }


    // signal chuck event
    bool EMSCRIPTEN_KEEPALIVE signalChuckEvent( unsigned int chuckID, const char * name )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // signal global event
        return chuck_instances[chuckID]->globals()->signalGlobalEvent( name );
    }


    // broadcast chuck event
    bool EMSCRIPTEN_KEEPALIVE broadcastChuckEvent( unsigned int chuckID, const char * name )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // broadcast global event
        return chuck_instances[chuckID]->globals()->broadcastGlobalEvent( name );
    }


    // listen for chuck event once
    bool EMSCRIPTEN_KEEPALIVE listenForChuckEventOnce( unsigned int chuckID, const char * name, void (* callback)(void) )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // listen for global event
        return chuck_instances[chuckID]->globals()->listenForGlobalEvent( name, callback, FALSE );
    }


    // start listening for chuck event
    bool EMSCRIPTEN_KEEPALIVE startListeningForChuckEvent( unsigned int chuckID, const char * name, void (* callback)(void) )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // listen for global event
        return chuck_instances[chuckID]->globals()->listenForGlobalEvent( name, callback, TRUE );
    }


    // stop listening for chuck event
    bool EMSCRIPTEN_KEEPALIVE stopListeningForChuckEvent( unsigned int chuckID, const char * name, void (* callback)(void) )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // stop listening for event
        return chuck_instances[chuckID]->globals()->stopListeningForGlobalEvent( name, callback );
    }
    

    // get global ugen samples
    bool EMSCRIPTEN_KEEPALIVE getGlobalUGenSamples( unsigned int chuckID,
        const char * name, SAMPLE * buffer, int numSamples )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // get global ugen samples
        if( !chuck_instances[chuckID]->globals()->getGlobalUGenSamples( name, buffer, numSamples ) )
        {
            // failed; fill with zeroes
            memset( buffer, 0, sizeof( SAMPLE ) * numSamples );
            return false;
        }
        return true;
    }


    // int array methods
    bool EMSCRIPTEN_KEEPALIVE setGlobalIntArray( unsigned int chuckID,
        const char * name, t_CKINT arrayValues[], unsigned int numValues )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // set global int array
        return chuck_instances[chuckID]->globals()->setGlobalIntArray(
            name, arrayValues, numValues );
    }


    // get global int array
    bool EMSCRIPTEN_KEEPALIVE getGlobalIntArray( unsigned int chuckID,
        const char * name, void (* callback)(t_CKINT[], t_CKUINT))
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // get global int array
        return chuck_instances[chuckID]->globals()->getGlobalIntArray(
            name, callback );
    }


    // set global int array
    bool EMSCRIPTEN_KEEPALIVE setGlobalIntArrayValue( unsigned int chuckID,
        const char * name, unsigned int index, t_CKINT value )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // set global int array
        return chuck_instances[chuckID]->globals()->setGlobalIntArrayValue(
            name, index, value );
    }


    // get global int array
    t_CKINT EMSCRIPTEN_KEEPALIVE getGlobalIntArrayValue( unsigned int chuckID,
        const char * name, unsigned int index )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // get global int array
        return chuck_instances[chuckID]->globals()->get_global_int_array_value(
            name, index );
    }


    // set global associative int array
    bool EMSCRIPTEN_KEEPALIVE setGlobalAssociativeIntArrayValue(
        unsigned int chuckID, const char * name, char * key, t_CKINT value )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // set global associative int array
        return chuck_instances[chuckID]->globals()->setGlobalAssociativeIntArrayValue(
            name, key, value );
    }


    // get global associative int array
    t_CKINT EMSCRIPTEN_KEEPALIVE getGlobalAssociativeIntArrayValue(
        unsigned int chuckID, const char * name, char * key )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // get global associative int array
        return chuck_instances[chuckID]->globals()->get_global_associative_int_array_value(
            std::string( name ), std::string( key ) );
    }


    // set float array methods
    bool EMSCRIPTEN_KEEPALIVE setGlobalFloatArray( unsigned int chuckID,
        const char * name, t_CKFLOAT arrayValues[], unsigned int numValues )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // set global float array
        return chuck_instances[chuckID]->globals()->setGlobalFloatArray(
            name, arrayValues, numValues );
    }


    // get global float array
    bool EMSCRIPTEN_KEEPALIVE getGlobalFloatArray( unsigned int chuckID,
        const char * name, void (* callback)(t_CKFLOAT[], t_CKUINT))
    {
        // check if chuck id has instances
        if( chuck_instances.count(chuckID) == 0 ) { return false; }
        // get global float array
        return chuck_instances[chuckID]->globals()->getGlobalFloatArray(
            name, callback );
    }


    // set global float array
    bool EMSCRIPTEN_KEEPALIVE setGlobalFloatArrayValue( unsigned int chuckID,
        const char * name, unsigned int index, t_CKFLOAT value )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // set global float array
        return chuck_instances[chuckID]->globals()->setGlobalFloatArrayValue(
            name, index, value );
    }


    // get global float array
    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getGlobalFloatArrayValue( unsigned int chuckID,
        const char * name, unsigned int index )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // get global float array
        return chuck_instances[chuckID]->globals()->get_global_float_array_value(
            name, index );
    }


    // set global associative float array
    bool EMSCRIPTEN_KEEPALIVE setGlobalAssociativeFloatArrayValue(
        unsigned int chuckID, const char * name, char * key, t_CKFLOAT value )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // set global associative float array
        return chuck_instances[chuckID]->globals()->setGlobalAssociativeFloatArrayValue(
            name, key, value );
    }


    // get global associative float array
    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getGlobalAssociativeFloatArrayValue(
        unsigned int chuckID, const char * name, char * key )
    {
        // check if chuck id has instances
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        // get global associative float array
        return chuck_instances[chuckID]->globals()->get_global_associative_float_array_value(
            std::string( name) , std::string( key ) );
    }


    // set chout callback
    bool EMSCRIPTEN_KEEPALIVE setChoutCallback( unsigned int chuckID, void (* callback)(const char *) )
    {
        // set
        return chuck_instances[chuckID]->setChoutCallback( callback );
    }


    // set cherr callback
    bool EMSCRIPTEN_KEEPALIVE setCherrCallback( unsigned int chuckID, void (* callback)(const char *) )
    {
        // set
        return chuck_instances[chuckID]->setCherrCallback( callback );
    }


    // set stdout callback
    bool EMSCRIPTEN_KEEPALIVE setStdoutCallback( void (* callback)(const char *) )
    {
        // set
        return ChucK::setStdoutCallback( callback );
    }


    // set stderr callback
    bool EMSCRIPTEN_KEEPALIVE setStderrCallback( void (* callback)(const char *) )
    {
        // set
        return ChucK::setStderrCallback( callback );
    }


    // set data dir
    bool EMSCRIPTEN_KEEPALIVE setDataDir( const char * dir )
    {
        // set
        chuck_global_data_dir = dir;
        return true;
    }


    // set log level
    bool EMSCRIPTEN_KEEPALIVE setLogLevel( unsigned int level )
    {
        // set log
        EM_setlog( level );
        return true;
    }


    // init chuck instance
    bool EMSCRIPTEN_KEEPALIVE initChuckInstance( unsigned int chuckID, unsigned int sampleRate,
                                                 unsigned int inChannels, unsigned int outChannels )
    {
        // proceed as normal
        if( chuck_instances.count( chuckID ) == 0 )
        {
            // if we aren't tracking a chuck vm on this ID, create a new one
            ChucK * chuck = new ChucK();
            
            // set params: sample rate, 2 in channels, 2 out channels,
            // don't halt the vm, and use our data directory
            chuck->setParam( CHUCK_PARAM_SAMPLE_RATE, (t_CKINT)sampleRate );
            chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT)inChannels );
            chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT)outChannels );
            chuck->setParam( CHUCK_PARAM_VM_HALT, (t_CKINT)0 );
            chuck->setParam( CHUCK_PARAM_DUMP_INSTRUCTIONS, (t_CKINT)0 );
            // directory for compiled.code
            chuck->setParam( CHUCK_PARAM_WORKING_DIRECTORY, chuck_global_data_dir );
            // directories to search for chugins and auto-run ck files
            std::list< std::string > chugin_search;
            // chugin_search.push_back( chuck_global_data_dir + "/Chugins" );
            // chugin_search.push_back( chuck_global_data_dir + "/ChuGins" );
            chugin_search.push_back( chuck_global_data_dir + "chugins" );
            chuck->setParam( CHUCK_PARAM_IMPORT_PATH_SYSTEM, chugin_search );
 
            // default real-time audio is true | chuck-1.4.2.1 (ge) added
            // set hint, so internally can advise things like async data writes etc.
            chuck->setParam( CHUCK_PARAM_IS_REALTIME_AUDIO_HINT, (t_CKINT)TRUE );
            
            // initialize and start
            chuck->init();
            chuck->start();

            // put into map
            chuck_instances[chuckID] = chuck;
        }

        return true;
    }


    // clear chuck instance
    bool EMSCRIPTEN_KEEPALIVE clearChuckInstance( unsigned int chuckID )
    {
        // check
        if( chuck_instances.count(chuckID) > 0 )
        {
            // the chuck to clear
            ChucK * chuck = chuck_instances[chuckID];

            // create a msg asking to clear the VM
            Chuck_Msg * msg = new Chuck_Msg;
            // set message type
            msg->type = CK_MSG_CLEARVM;
            // set so that VM will delete for us when it's done
            msg->reply_queue = FALSE;
            // tell the VM to clear
            chuck->vm()->globals_manager()->execute_chuck_msg_with_globals( msg );

            return true;
        }
        
        return false;
    }


    // clear globals
    bool EMSCRIPTEN_KEEPALIVE clearGlobals( unsigned int chuckID )
    {
        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // the chuck to clear
            ChucK * chuck = chuck_instances[chuckID];
            
            // create a msg asking to clear the globals
            Chuck_Msg * msg = new Chuck_Msg;
            // set message type
            msg->type = CK_MSG_CLEARGLOBALS;
            // set so that VM will delete for us when it's done
            msg->reply_queue = FALSE;
            // tell the VM to clear
            chuck->vm()->globals_manager()->execute_chuck_msg_with_globals( msg );

            return true;
        }

        return false;
    }


    // clean up chuck instance
    bool EMSCRIPTEN_KEEPALIVE cleanupChuckInstance( unsigned int chuckID )
    {
        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // get
            ChucK * chuck = chuck_instances[chuckID];
            // don't track it anymore
            chuck_instances.erase( chuckID );

            // cleanup this chuck early
            CK_SAFE_DELETE( chuck );
        }

        return true;
    }


    // chuck manual audio callback
    bool EMSCRIPTEN_KEEPALIVE chuckManualAudioCallback( unsigned int chuckID,
        float * inBuffer, float * outBuffer, unsigned int numFrames,
        unsigned int inChannels, unsigned int outChannels )
    {
        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // zero out the output buffer, in case chuck isn't running
            for( unsigned int n = 0; n < numFrames * outChannels; n++ )
            {
                outBuffer[n] = 0;
            }
            
            // call callback
            // TODO: check inChannels, outChannels
            chuck_instances[chuckID]->run( inBuffer, outBuffer, numFrames );
        }
        
        return true;
    }


    // on launch, reset all ids (necessary when relaunching a lot in unity editor)
    void EMSCRIPTEN_KEEPALIVE cleanRegisteredChucks()
    {
        // delete chucks
        for( std::map< unsigned int, ChucK * >::iterator it =
             chuck_instances.begin(); it != chuck_instances.end(); it++ )
        {
            ChucK * chuck = it->second;
            CK_SAFE_DELETE( chuck );
        }

        // delete stored chuck pointers
        chuck_instances.clear();

        // clear out callbacks also
        setStdoutCallback( NULL );
        setStderrCallback( NULL );
    }


    // set param
    bool EMSCRIPTEN_KEEPALIVE setParamInt( unsigned int chuckID, const char * key, t_CKINT val )
    {
        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // call
            return chuck_instances[chuckID]->setParam( key, val );
        }

        return false;
    }

    bool EMSCRIPTEN_KEEPALIVE setParamFloat( unsigned int chuckID, const char * key, t_CKFLOAT val )
    {
        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // call
            return chuck_instances[chuckID]->setParam( key, val );
        }
        return false;
    }

    bool EMSCRIPTEN_KEEPALIVE setParamString( unsigned int chuckID, const char * key, const char * val )
    {
        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // call
            return chuck_instances[chuckID]->setParam( key, val );
        }
        return false;
    }

    // get param
    t_CKINT EMSCRIPTEN_KEEPALIVE getParamInt( unsigned int chuckID, const char * key )
    {
        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // call
            return chuck_instances[chuckID]->getParamInt( key );
        }
        return false;
    }

    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getParamFloat( unsigned int chuckID, const char * key )
    {
        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // call
            return chuck_instances[chuckID]->getParamFloat( key );
        }
        return false;
    }

    const char * EMSCRIPTEN_KEEPALIVE getParamString( unsigned int chuckID, const char * key )
    {
        // hopefully this is ok | assume not reentrant code
        static std::string theStr;

        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // call
            theStr = chuck_instances[chuckID]->getParamString( key );
            // return persistent storage | the recipient should not hold on to the char *
            return theStr.c_str();
        }
        return "";
    }

    // get chuck time
    t_CKTIME EMSCRIPTEN_KEEPALIVE getChuckNow( unsigned int chuckID )
    {
        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // call
            return chuck_instances[chuckID]->now();
        }
        return 0;
    }

    // runtime import API (TBD)
    t_CKBOOL EMSCRIPTEN_KEEPALIVE importModule( unsigned int chuckID, const char * path )
    {
        // check
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // call
            // return chuck_instances[chuckID]->compiler()->loadModule( path );
        }
        return FALSE;
    }

} // extern "C"
