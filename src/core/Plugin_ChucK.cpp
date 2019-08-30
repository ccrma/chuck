//
//  Plugin_ChucK.cpp
//  AudioPluginDemo
//
//  Created by Jack Atherton on 4/19/17.
//
//


#include "Plugin_ChucK.h"

#include <iostream>
#include <map>
#ifndef WIN32
#include <unistd.h>
#endif

extern "C"
{
    
    std::map< unsigned int, ChucK * > chuck_instances;
    std::string chuck_global_data_dir;



    // C# "string" corresponds to passing char *
    bool EMSCRIPTEN_KEEPALIVE runChuckCode( unsigned int chuckID, const char * code )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );

        // compile it!
        return chuck_instances[chuckID]->compileCode(
            std::string( code ), std::string("") );
    }
    
    
    
    bool runChuckCodeWithReplacementDac(
        unsigned int chuckID, const char * code, const char * replacement_dac )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        // replace dac
        chuck_instances[chuckID]->compiler()->setReplaceDac( TRUE,
            std::string( replacement_dac ) );
        
        // compile it!
        bool ret = chuck_instances[chuckID]->compileCode(
            std::string( code ), std::string("") );
        
        // don't replace dac for future compilations
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );
        
        return ret;
    }
    
    
    
    bool runChuckFile( unsigned int chuckID,
        const char * filename )
    {
        // run with empty args
        return runChuckFileWithArgs( chuckID, filename, "" );
    }
    
    
    
    bool runChuckFileWithArgs( unsigned int chuckID,
        const char * filename, const char * args )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        // don't want to replace dac
        // (a safeguard in case compiler got interrupted while replacing dac)
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );

        // compile it!
        return chuck_instances[chuckID]->compileFile(
            std::string( filename ), std::string( args )
        );
    }
    
    
    
    bool runChuckFileWithReplacementDac(
        unsigned int chuckID, const char * filename,
        const char * replacement_dac )
    {
        // run with empty args
        return runChuckFileWithArgsWithReplacementDac(
            chuckID, filename, "", replacement_dac
        );
    }
    
    
    
    bool runChuckFileWithArgsWithReplacementDac(
        unsigned int chuckID, const char * filename, const char * args,
        const char * replacement_dac )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        // replace dac
        chuck_instances[chuckID]->compiler()->setReplaceDac( TRUE,
            std::string( replacement_dac ) );
        
        // compile it!
        bool ret = chuck_instances[chuckID]->compileFile(
            std::string( filename ), std::string( args )
        );
        
        // don't replace dac for future compilations
        chuck_instances[chuckID]->compiler()->setReplaceDac( FALSE, "" );
        
        return ret;
    }
    
    
    
    bool setChuckInt( unsigned int chuckID, const char * name, t_CKINT val )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->setGlobalInt( name, val );
    }
    
    
    
    bool getChuckInt( unsigned int chuckID, const char * name, void (* callback)(t_CKINT) )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->getGlobalInt( name, callback );
    }
    
    
    
    bool setChuckFloat( unsigned int chuckID, const char * name, t_CKFLOAT val )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->setGlobalFloat( name, val );
    }
    
    
    
    bool getChuckFloat( unsigned int chuckID, const char * name, void (* callback)(t_CKFLOAT) )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->getGlobalFloat( name, callback );
    }
    
    
    
    bool setChuckString( unsigned int chuckID, const char * name, const char * val )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->setGlobalString( name, val );
    }
    
    
    
    bool getChuckString( unsigned int chuckID, const char * name, void (* callback)(const char *) )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->getGlobalString( name, callback );
    }
    
    
    
    bool signalChuckEvent( unsigned int chuckID, const char * name )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->signalGlobalEvent( name );
    }
    
    
    
    bool broadcastChuckEvent( unsigned int chuckID, const char * name )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }

        return chuck_instances[chuckID]->broadcastGlobalEvent( name );
    }
    
    
    
    bool listenForChuckEventOnce( unsigned int chuckID, const char * name, void (* callback)(void) )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->listenForGlobalEvent(
            name, callback, FALSE );
    }
    
    
    
    bool startListeningForChuckEvent( unsigned int chuckID, const char * name, void (* callback)(void) )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->listenForGlobalEvent(
            name, callback, TRUE );
    }
    
    
    
    bool stopListeningForChuckEvent( unsigned int chuckID, const char * name, void (* callback)(void) )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->stopListeningForGlobalEvent(
            name, callback );
    }
    
    
    bool getGlobalUGenSamples( unsigned int chuckID,
        const char * name, SAMPLE * buffer, int numSamples )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        if( !chuck_instances[chuckID]->getGlobalUGenSamples(
            name, buffer, numSamples ) )
        {
            // failed. fill with zeroes.
            memset( buffer, 0, sizeof( SAMPLE ) * numSamples );
            return false;
        }
        
        return true;
    }
    
    
    
    // int array methods
    bool setGlobalIntArray( unsigned int chuckID,
        const char * name, t_CKINT arrayValues[], unsigned int numValues )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->setGlobalIntArray(
            name, arrayValues, numValues );
    }
    
    
    
    bool getGlobalIntArray( unsigned int chuckID,
        const char * name, void (* callback)(t_CKINT[], t_CKUINT))
    {
       if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->getGlobalIntArray(
            name, callback );
    }
    
    
    
    bool setGlobalIntArrayValue( unsigned int chuckID,
        const char * name, unsigned int index, t_CKINT value )
    {
       if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->setGlobalIntArrayValue(
            name, index, value );
    }
    
    
    
    bool getGlobalIntArrayValue( unsigned int chuckID,
        const char * name, unsigned int index, void (* callback)(t_CKINT) )
    {
       if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->getGlobalIntArrayValue(
            name, index, callback );
    }
    
    
    
    bool setGlobalAssociativeIntArrayValue(
        unsigned int chuckID, const char * name, char * key, t_CKINT value )
    {
       if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->setGlobalAssociativeIntArrayValue(
            name, key, value );
    }
    
    
    
    bool getGlobalAssociativeIntArrayValue(
        unsigned int chuckID, const char * name, char * key,
        void (* callback)(t_CKINT) )
    {
       if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->getGlobalAssociativeIntArrayValue(
            name, key, callback );
    }
    
    
    
    // float array methods
    bool setGlobalFloatArray( unsigned int chuckID,
        const char * name, t_CKFLOAT arrayValues[], unsigned int numValues )
    {
        if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->setGlobalFloatArray(
            name, arrayValues, numValues );
    }
    
    
    
    bool getGlobalFloatArray( unsigned int chuckID,
        const char * name, void (* callback)(t_CKFLOAT[], t_CKUINT))
    {
       if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->getGlobalFloatArray(
            name, callback );
    }
    
    
    
    bool setGlobalFloatArrayValue( unsigned int chuckID,
        const char * name, unsigned int index, t_CKFLOAT value )
    {
       if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->setGlobalFloatArrayValue(
            name, index, value );
    }
    
    
    
    bool getGlobalFloatArrayValue( unsigned int chuckID,
        const char * name, unsigned int index, void (* callback)(t_CKFLOAT) )
    {
       if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->getGlobalFloatArrayValue(
            name, index, callback );
    }
    
    
    
    bool setGlobalAssociativeFloatArrayValue(
        unsigned int chuckID, const char * name, char * key, t_CKFLOAT value )
    {
       if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->setGlobalAssociativeFloatArrayValue(
            name, key, value );
    }
    
    
    
    bool getGlobalAssociativeFloatArrayValue(
        unsigned int chuckID, const char * name, char * key,
        void (* callback)(t_CKFLOAT) )
    {
       if( chuck_instances.count( chuckID ) == 0 ) { return false; }
        
        return chuck_instances[chuckID]->getGlobalAssociativeFloatArrayValue(
            name, key, callback );
    }
    
    
    
    bool setChoutCallback( unsigned int chuckID, void (* callback)(const char *) )
    {
        return chuck_instances[chuckID]->setChoutCallback( callback );
    }



    bool setCherrCallback( unsigned int chuckID, void (* callback)(const char *) )
    {
        return chuck_instances[chuckID]->setCherrCallback( callback );
    }
    
    
    
    bool setStdoutCallback( void (* callback)(const char *) )
    {
        return ChucK::setStdoutCallback( callback );
    }



    bool setStderrCallback( void (* callback)(const char *) )
    {
        return ChucK::setStderrCallback( callback );
    }
    
    
    
    bool setDataDir( const char * dir )
    {
        chuck_global_data_dir = std::string( dir );
        return true;
    }
    
    
    
    bool setLogLevel( unsigned int level )
    {
        EM_setlog( level );
        return true;
    }
    
    
    
    bool EMSCRIPTEN_KEEPALIVE initChuckInstance( unsigned int chuckID, unsigned int sampleRate )
    {
        if( chuck_instances.count( chuckID ) == 0 )
        {
            // if we aren't tracking a chuck vm on this ID, create a new one
            ChucK * chuck = new ChucK();
            
            // set params: sample rate, 2 in channels, 2 out channels,
            // don't halt the vm, and use our data directory
            chuck->setParam( CHUCK_PARAM_SAMPLE_RATE, (t_CKINT) sampleRate );
            chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, (t_CKINT) 2 );
            chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, (t_CKINT) 2 );
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



    bool clearChuckInstance( unsigned int chuckID )
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
            chuck->vm()->execute_chuck_msg_with_globals( msg );
            
            return true;
        }
        
        return false;
    }



    bool clearGlobals( unsigned int chuckID )
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
            chuck->vm()->execute_chuck_msg_with_globals( msg );
            
            return true;
        }
        
        return false;
    }


    bool cleanupChuckInstance( unsigned int chuckID )
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
    void cleanRegisteredChucks() {
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
