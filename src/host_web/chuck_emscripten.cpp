//
//  chuck_emscripten.cpp
//  AudioPluginDemo
//
//  Created by Jack Atherton on 4/19/17.
//
//


#include "chuck_emscripten.h"
#include "chuck_globals.h"

#include <iostream>
#include <map>
#ifndef WIN32
#include <unistd.h>
#endif

extern "C"
{
    
    std::map< unsigned int, ChucK * > chuck_instances;
    std::string chuck_global_data_dir;

    void dummyFunction()
    {
        
    }
    
    void EMSCRIPTEN_KEEPALIVE dummyCall()
    {
        // call dummy function so Browser is included
        emscripten_async_call( (em_arg_callback_func) dummyFunction, NULL, 1000 );
    }


    // C# "string" corresponds to passing char *
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckCode( unsigned int chuckID, const char * code )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return -1; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );

        // compile it!
        if( chuck_instances[chuckID]->compileCode(
            std::string( code ), std::string("") ) )
        {
            // last shred shredded
            return chuck_instances[chuckID]->vm()->last_id();
            
        }
        // failure to compile
        return 0;
    }
    
    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckCodeWithReplacementDac(
        unsigned int chuckID, const char * code, const char * replacement_dac )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return -1; }

        // replace dac
        chuck_instances[chuckID]->compiler()->setReplaceDac( TRUE,
            std::string( replacement_dac ) );
        
        // compile it!
        bool result = chuck_instances[chuckID]->compileCode(
            std::string( code ), std::string("") );
        
        // don't replace dac for future compilations
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );
        
        if( result )
        {
            // last shred shredded
            return chuck_instances[chuckID]->vm()->last_id();
        }
        // failure to compile
        return 0;
    }
    
    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFile( unsigned int chuckID,
        const char * filename )
    {
        // run with empty args
        return runChuckFileWithArgs( chuckID, filename, "" );
    }
    
    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFileWithArgs( unsigned int chuckID,
        const char * filename, const char * args )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return -1; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );

        // compile it!
        if( chuck_instances[chuckID]->compileFile(
            std::string( filename ), std::string( args ) ) )
        {
            // last shred shredded
            return chuck_instances[chuckID]->vm()->last_id();
            
        }
        // failure to compile
        return 0;
    }
    
    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFileWithReplacementDac(
        unsigned int chuckID, const char * filename,
        const char * replacement_dac )
    {
        // run with empty args
        return runChuckFileWithArgsWithReplacementDac(
            chuckID, filename, "", replacement_dac
        );
    }
    
    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFileWithArgsWithReplacementDac(
        unsigned int chuckID, const char * filename, const char * args,
        const char * replacement_dac )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return -1; }

        // replace dac
        chuck_instances[chuckID]->compiler()->setReplaceDac( TRUE,
            std::string( replacement_dac ) );
        
        // compile it!
        bool result = chuck_instances[chuckID]->compileFile(
            std::string( filename ), std::string( args )
        );
        
        // don't replace dac for future compilations
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );
        
        if( result )
        {
            // last shred shredded
            return chuck_instances[chuckID]->vm()->last_id();
            
        }
        // failure to compile
        return 0;
    }
    


    t_CKUINT replaceCode( ChucK * chuck, t_CKUINT shredID,
        std::vector<std::string> * args )
    {
        Chuck_Msg * msg = new Chuck_Msg;
        msg->type = MSG_REPLACE;
        msg->code = chuck->compiler()->output();
        msg->param = shredID;
        // null reply so that VM will delete for us when it's done
        msg->reply = ( ck_msg_func )NULL;
        msg->args = args;
        
        // call the process_msg directly: we need to know the shred ID
        // and it's JS so we won't be pre-empted
        // also: return value is the shred ID!
        return chuck->vm()->process_msg( msg );
    }

    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckCode( unsigned int chuckID, unsigned int shredID, const char * code )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return -1; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );

        // compile it! and spork 0 times. (ONLY compile)
        if( chuck_instances[chuckID]->compileCode(
            std::string( code ), std::string(""), 0 ) )
        {
            // replace code
            return replaceCode( chuck_instances[chuckID], shredID, NULL );
        }
        // failure to compile
        return 0;
    }
    
    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckCodeWithReplacementDac( unsigned int chuckID, unsigned int shredID, const char * code, const char * replacement_dac )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return -1; }

        // replace dac
        chuck_instances[chuckID]->compiler()->setReplaceDac( TRUE,
            std::string( replacement_dac ) );
        
        // compile it! and spork 0 times. (ONLY compile)
        bool result = chuck_instances[chuckID]->compileCode(
            std::string( code ), std::string(""), 0 );
        
        // don't replace dac for future compilations
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );
        
        if( result )
        {
            // replace code
            return replaceCode( chuck_instances[chuckID], shredID, NULL );
        }
        // failure to compile
        return 0;
    }
    
    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFile( unsigned int chuckID, unsigned int shredID, const char * filename )
    {
        // replace with empty args
        return replaceChuckFileWithArgs( chuckID, shredID, filename, "" );
    }
    
    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFileWithReplacementDac( unsigned int chuckID, unsigned int shredID, const char * filename, const char * replacement_dac )
    {
        // replace with empty args
        return replaceChuckFileWithArgsWithReplacementDac( chuckID, shredID,
            filename, "", replacement_dac );
    }
    
    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFileWithArgs( unsigned int chuckID, unsigned int shredID, const char * filename, const char * args )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return -1; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );
        
        std::string filenameS( filename );
        std::string argsS( args );

        // compile it! and spork 0 times. (ONLY compile)
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
            return replaceCode( chuck_instances[chuckID], shredID, & found_args );
        }
        // failure to compile
        return 0;
    }
    
    
    
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFileWithArgsWithReplacementDac( unsigned int chuckID, unsigned int shredID, const char * filename, const char * args, const char * replacement_dac )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return -1; }

        // replace dac
        chuck_instances[chuckID]->compiler()->setReplaceDac( TRUE,
            std::string( replacement_dac ) );
        
        std::string filenameS( filename );
        std::string argsS( args );
        
        // compile it! and spork 0 times. (ONLY compile)
        bool result = chuck_instances[chuckID]->compileFile( filenameS, argsS, 0 );
        
        // don't replace dac for future compilations
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );
        
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
            return replaceCode( chuck_instances[chuckID], shredID, & found_args );
            
        }
        // failure to compile
        return 0;
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE isShredActive( unsigned int chuckID, unsigned int shredID )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        Chuck_VM_Shred * shred = chuck_instances[chuckID]->vm()->shreduler()->lookup( shredID );
        return ( shred != NULL );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE removeShred( unsigned int chuckID, unsigned int shredID )
    {
        if( chuck_instances.count( chuckID ) == 0 ||
            !isShredActive( chuckID, shredID ) )
        {
            return false;
        }
        
        // the chuck to clear
        ChucK * chuck = chuck_instances[chuckID];
    
        // create a msg asking to remove the shred
        Chuck_Msg * msg = new Chuck_Msg;
        msg->type = MSG_REMOVE;
        msg->param = shredID;
    
        // null reply so that VM will delete for us when it's done
        msg->reply = ( ck_msg_func )NULL;
    
        // tell the VM to clear
        chuck->globals()->execute_chuck_msg_with_globals( msg );
    
        return true;
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setChuckInt( unsigned int chuckID, const char * name, t_CKINT val )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->globals()->setGlobalInt( name, val );
    }
    
    
    
    t_CKINT EMSCRIPTEN_KEEPALIVE getChuckInt( unsigned int chuckID, const char * name )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->globals()->get_global_int_value( std::string( name ) );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setChuckFloat( unsigned int chuckID, const char * name, t_CKFLOAT val )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->globals()->setGlobalFloat( name, val );
    }
    
    
    
    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getChuckFloat( unsigned int chuckID, const char * name )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->globals()->get_global_float_value( std::string( name ) );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setChuckString( unsigned int chuckID, const char * name, const char * val )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->globals()->setGlobalString( name, val );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE getChuckString( unsigned int chuckID, const char * name, void (* callback)(const char *) )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->globals()->getGlobalString( name, callback );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE signalChuckEvent( unsigned int chuckID, const char * name )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->globals()->signalGlobalEvent( name );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE broadcastChuckEvent( unsigned int chuckID, const char * name )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->globals()->broadcastGlobalEvent( name );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE listenForChuckEventOnce( unsigned int chuckID, const char * name, void (* callback)(void) )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->listenForGlobalEvent(
            name, callback, FALSE );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE startListeningForChuckEvent( unsigned int chuckID, const char * name, void (* callback)(void) )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->listenForGlobalEvent(
            name, callback, TRUE );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE stopListeningForChuckEvent( unsigned int chuckID, const char * name, void (* callback)(void) )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->stopListeningForGlobalEvent(
            name, callback );
    }
    
    
    bool EMSCRIPTEN_KEEPALIVE getGlobalUGenSamples( unsigned int chuckID,
        const char * name, SAMPLE * buffer, int numSamples )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        if( !chuck_instances[chuckID]->globals()->getGlobalUGenSamples(
            name, buffer, numSamples ) )
        {
            // failed. fill with zeroes.
            memset( buffer, 0, sizeof( SAMPLE ) * numSamples );
            return false;
        }
        
        return true;
    }
    
    
    
    // int array methods
    bool EMSCRIPTEN_KEEPALIVE setGlobalIntArray( unsigned int chuckID,
        const char * name, t_CKINT arrayValues[], unsigned int numValues )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->setGlobalIntArray(
            name, arrayValues, numValues );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE getGlobalIntArray( unsigned int chuckID,
        const char * name, void (* callback)(t_CKINT[], t_CKUINT))
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->getGlobalIntArray(
            name, callback );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setGlobalIntArrayValue( unsigned int chuckID,
        const char * name, unsigned int index, t_CKINT value )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->setGlobalIntArrayValue(
            name, index, value );
    }
    
    
    
    t_CKINT EMSCRIPTEN_KEEPALIVE getGlobalIntArrayValue( unsigned int chuckID,
        const char * name, unsigned int index )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        
        return chuck_instances[chuckID]->globals()->get_global_int_array_value(
            std::string( name ), index );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setGlobalAssociativeIntArrayValue(
        unsigned int chuckID, const char * name, char * key, t_CKINT value )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->setGlobalAssociativeIntArrayValue(
            name, key, value );
    }
    
    
    
    t_CKINT EMSCRIPTEN_KEEPALIVE getGlobalAssociativeIntArrayValue(
        unsigned int chuckID, const char * name, char * key )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->get_global_associative_int_array_value(
            std::string( name ), std::string( key ) );
    }
    
    
    
    // float array methods
    bool EMSCRIPTEN_KEEPALIVE setGlobalFloatArray( unsigned int chuckID,
        const char * name, t_CKFLOAT arrayValues[], unsigned int numValues )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->setGlobalFloatArray(
            name, arrayValues, numValues );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE getGlobalFloatArray( unsigned int chuckID,
        const char * name, void (* callback)(t_CKFLOAT[], t_CKUINT))
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->getGlobalFloatArray(
            name, callback );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setGlobalFloatArrayValue( unsigned int chuckID,
        const char * name, unsigned int index, t_CKFLOAT value )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->setGlobalFloatArrayValue(
            name, index, value );
    }
    
    
    
    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getGlobalFloatArrayValue( unsigned int chuckID,
        const char * name, unsigned int index )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->get_global_float_array_value(
            std::string( name ), index );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setGlobalAssociativeFloatArrayValue(
        unsigned int chuckID, const char * name, char * key, t_CKFLOAT value )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->setGlobalAssociativeFloatArrayValue(
            name, key, value );
    }
    
    
    
    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getGlobalAssociativeFloatArrayValue(
        unsigned int chuckID, const char * name, char * key )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->globals()->get_global_associative_float_array_value(
            std::string( name) , std::string( key ) );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setChoutCallback( unsigned int chuckID, void (* callback)(const char *) )
    {
        return chuck_instances[chuckID]->setChoutCallback( callback );
    }



    bool EMSCRIPTEN_KEEPALIVE setCherrCallback( unsigned int chuckID, void (* callback)(const char *) )
    {
        return chuck_instances[chuckID]->setCherrCallback( callback );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setStdoutCallback( void (* callback)(const char *) )
    {
        return ChucK::setStdoutCallback( callback );
    }



    bool EMSCRIPTEN_KEEPALIVE setStderrCallback( void (* callback)(const char *) )
    {
        return ChucK::setStderrCallback( callback );
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setDataDir( const char * dir )
    {
        chuck_global_data_dir = std::string( dir );
        return true;
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE setLogLevel( unsigned int level )
    {
        EM_setlog( level );
        return true;
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE initChuckInstance( unsigned int chuckID, unsigned int sampleRate, unsigned int inChannels, unsigned int outChannels )
    {
        
        // proceed as normal
        if( chuck_instances.count( chuckID ) == 0 )
        {
            // if we aren't tracking a chuck vm on this ID, create a new one
            ChucK * chuck = new ChucK();
            
            // set params: sample rate, 2 in channels, 2 out channels,
            // don't halt the vm, and use our data directory
            chuck->setParam( CHUCK_PARAM_SAMPLE_RATE, (t_CKINT) sampleRate );
            chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT) inChannels );
            chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT) outChannels );
            chuck->setParam( CHUCK_PARAM_VM_HALT, (t_CKINT) 0 );
            chuck->setParam( CHUCK_PARAM_DUMP_INSTRUCTIONS, (t_CKINT) 0 );
            // directory for compiled.code
            chuck->setParam( CHUCK_PARAM_WORKING_DIRECTORY, chuck_global_data_dir );
            // directories to search for chugins and auto-run ck files
            std::list< std::string > chugin_search;
            chugin_search.push_back( chuck_global_data_dir + "/Chugins" );
            chugin_search.push_back( chuck_global_data_dir + "/ChuGins" );
            chugin_search.push_back( chuck_global_data_dir + "/chugins" );
            chuck->setParam( CHUCK_PARAM_USER_CHUGIN_DIRECTORIES, chugin_search );
            
            // initialize and start
            chuck->init();
            chuck->start();
            
            chuck_instances[chuckID] = chuck;
        }
        return true;
    }



    bool EMSCRIPTEN_KEEPALIVE clearChuckInstance( unsigned int chuckID )
    {
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // the chuck to clear
            ChucK * chuck = chuck_instances[chuckID];
            
            // create a msg asking to clear the VM
            Chuck_Msg * msg = new Chuck_Msg;
            msg->type = MSG_CLEARVM;
            
            // null reply so that VM will delete for us when it's done
            msg->reply = ( ck_msg_func )NULL;
            
            // tell the VM to clear
            chuck->vm()->globals_manager()->execute_chuck_msg_with_globals( msg );
            
            return true;
        }
        
        return false;
    }



    bool EMSCRIPTEN_KEEPALIVE clearGlobals( unsigned int chuckID )
    {
        if( chuck_instances.count( chuckID ) > 0 )
        {
            // the chuck to clear
            ChucK * chuck = chuck_instances[chuckID];
            
            // create a msg asking to clear the globals
            Chuck_Msg * msg = new Chuck_Msg;
            msg->type = MSG_CLEARGLOBALS;
            
            // null reply so that VM will delete for us when it's done
            msg->reply = ( ck_msg_func )NULL;
            
            // tell the VM to clear
            chuck->vm()->globals_manager()->execute_chuck_msg_with_globals( msg );
            
            return true;
        }
        
        return false;
    }


    bool EMSCRIPTEN_KEEPALIVE cleanupChuckInstance( unsigned int chuckID )
    {
        if( chuck_instances.count( chuckID ) > 0 )
        {
            ChucK * chuck = chuck_instances[chuckID];
            
            // don't track it anymore
            chuck_instances.erase( chuckID );

            // cleanup this chuck early
            delete chuck;

        }

        return true;
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE chuckManualAudioCallback( unsigned int chuckID, float * inBuffer, float * outBuffer, unsigned int numFrames, unsigned int inChannels, unsigned int outChannels )
    {
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
    void EMSCRIPTEN_KEEPALIVE cleanRegisteredChucks() {
        // delete chucks
        for( std::map< unsigned int, ChucK * >::iterator it =
             chuck_instances.begin(); it != chuck_instances.end(); it++ )
        {
            ChucK * chuck = it->second;
            delete chuck;
        }
        
        // delete stored chuck pointers
        chuck_instances.clear();
        
        // clear out callbacks also
        setStdoutCallback( NULL );
        setStderrCallback( NULL );
    }

    

}
