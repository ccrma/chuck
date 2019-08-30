#pragma once

#include "chuck.h"
#include "emscripten.h"
#define UNITY_INTERFACE EXPORT


extern "C" {

    bool EMSCRIPTEN_KEEPALIVE runChuckCode( unsigned int chuckID, const char * code );
    bool runChuckCodeWithReplacementDac( unsigned int chuckID, const char * code, const char * replacement_dac );
    bool runChuckFile( unsigned int chuckID, const char * filename );
    bool runChuckFileWithReplacementDac( unsigned int chuckID, const char * filename, const char * replacement_dac );
    bool runChuckFileWithArgs( unsigned int chuckID, const char * filename, const char * args );
    bool runChuckFileWithArgsWithReplacementDac( unsigned int chuckID, const char * filename, const char * args, const char * replacement_dac );
    
    bool setChuckInt( unsigned int chuckID, const char * name, t_CKINT val );
    bool getChuckInt( unsigned int chuckID, const char * name, void (* callback)(t_CKINT) );
    
    bool setChuckFloat( unsigned int chuckID, const char * name, t_CKFLOAT val );
    bool getChuckFloat( unsigned int chuckID, const char * name, void (* callback)(t_CKFLOAT) );
    
    bool setChuckString( unsigned int chuckID, const char * name, const char * val );
    bool getChuckString( unsigned int chuckID, const char * name, void (* callback)(const char *) );
    
    bool signalChuckEvent( unsigned int chuckID, const char * name );
    bool broadcastChuckEvent( unsigned int chuckID, const char * name );
    bool listenForChuckEventOnce( unsigned int chuckID, const char * name, void (* callback)(void) );
    bool startListeningForChuckEvent( unsigned int chuckID, const char * name, void (* callback)(void) );
    bool stopListeningForChuckEvent( unsigned int chuckID, const char * name, void (* callback)(void) );
    
    bool getGlobalUGenSamples( unsigned int chuckID, const char * name, SAMPLE * buffer, int numSamples );
    
    // int array methods
    bool setGlobalIntArray( unsigned int chuckID, const char * name, t_CKINT arrayValues[], unsigned int numValues );
    bool getGlobalIntArray( unsigned int chuckID, const char * name, void (* callback)(t_CKINT[], t_CKUINT));
    bool setGlobalIntArrayValue( unsigned int chuckID, const char * name, unsigned int index, t_CKINT value );
    bool getGlobalIntArrayValue( unsigned int chuckID, const char * name, unsigned int index, void (* callback)(t_CKINT) );
    bool setGlobalAssociativeIntArrayValue( unsigned int chuckID, const char * name, char * key, t_CKINT value );
    bool getGlobalAssociativeIntArrayValue( unsigned int chuckID, const char * name, char * key, void (* callback)(t_CKINT) );
    // TODO: set entire dict, add to dict in batch; get entire dict
    
    // float array methods
    bool setGlobalFloatArray( unsigned int chuckID, const char * name, t_CKFLOAT arrayValues[], unsigned int numValues );
    bool getGlobalFloatArray( unsigned int chuckID, const char * name, void (* callback)(t_CKFLOAT[], t_CKUINT));
    bool setGlobalFloatArrayValue( unsigned int chuckID, const char * name, unsigned int index, t_CKFLOAT value );
    bool getGlobalFloatArrayValue( unsigned int chuckID, const char * name, unsigned int index, void (* callback)(t_CKFLOAT) );
    bool setGlobalAssociativeFloatArrayValue( unsigned int chuckID, const char * name, char * key, t_CKFLOAT value );
    bool getGlobalAssociativeFloatArrayValue( unsigned int chuckID, const char * name, char * key, void (* callback)(t_CKFLOAT) );
    
    
    bool EMSCRIPTEN_KEEPALIVE initChuckInstance( unsigned int chuckID, unsigned int sampleRate, unsigned int inChannels, unsigned int outChannels );
    bool clearChuckInstance( unsigned int chuckID );
    bool clearGlobals( unsigned int chuckID );
    bool cleanupChuckInstance( unsigned int chuckID );
    bool EMSCRIPTEN_KEEPALIVE chuckManualAudioCallback( unsigned int chuckID, float * inBuffer, float * outBuffer, unsigned int numFrames, unsigned int inChannels, unsigned int outChannels );
    void cleanRegisteredChucks();
    
    bool setChoutCallback( unsigned int chuckID, void (* callback)(const char *) );
    bool setCherrCallback( unsigned int chuckID, void (* callback)(const char *) );
    bool setStdoutCallback( void (* callback)(const char *) );
    bool setStderrCallback( void (* callback)(const char *) );
    
    bool setDataDir( const char * dir );
    
    bool setLogLevel( unsigned int level );


};
