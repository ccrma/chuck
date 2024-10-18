//-----------------------------------------------------------------------------
// name: chuck_emscripten.h
// desc: emscripten binding agent for web assembly compilation
//       e.g., used for WebChucK and Chunity web target
//
// author: Jack Atherton
// date: created 4/19/17
//-----------------------------------------------------------------------------
#pragma once

#include "chuck.h"
#include "emscripten.h"

// for Chunity
#define UNITY_INTERFACE EXPORT

// C linkage
extern "C"
{
    // these functions return the new shred id, or 0 on failure
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckCode( unsigned int chuckID, const char * code );
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckCodeWithReplacementDac( unsigned int chuckID, const char * code, const char * replacement_dac );
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFile( unsigned int chuckID, const char * filename );
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFileWithReplacementDac( unsigned int chuckID, const char * filename, const char * replacement_dac );
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFileWithArgs( unsigned int chuckID, const char * filename, const char * args );
    t_CKUINT EMSCRIPTEN_KEEPALIVE runChuckFileWithArgsWithReplacementDac( unsigned int chuckID, const char * filename, const char * args, const char * replacement_dac );
    
    // various replace
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckCode( unsigned int chuckID, unsigned int shredID, const char * code );
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckCodeWithReplacementDac( unsigned int chuckID, unsigned int shredID, const char * code, const char * replacement_dac );
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFile( unsigned int chuckID, unsigned int shredID, const char * filename );
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFileWithReplacementDac( unsigned int chuckID, unsigned int shredID, const char * filename, const char * replacement_dac );
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFileWithArgs( unsigned int chuckID, unsigned int shredID, const char * filename, const char * args );
    t_CKUINT EMSCRIPTEN_KEEPALIVE replaceChuckFileWithArgsWithReplacementDac( unsigned int chuckID, unsigned int shredID, const char * filename, const char * args, const char * replacement_dac );
    
    // shred operations
    bool EMSCRIPTEN_KEEPALIVE isShredActive( unsigned int chuckID, unsigned int shredID );
    bool EMSCRIPTEN_KEEPALIVE removeShred( unsigned int chuckID, unsigned int shredID );
    
    // chuck globals: int
    bool EMSCRIPTEN_KEEPALIVE setChuckInt( unsigned int chuckID, const char * name, t_CKINT val );
    // NOTE: we don't use callbacks in JavaScript compilation because JS is non-preemptive,
    // so we don't have to worry about our code getting interrupted
    t_CKINT EMSCRIPTEN_KEEPALIVE getChuckInt( unsigned int chuckID, const char * name );
    
    // chuck globals: float
    bool EMSCRIPTEN_KEEPALIVE setChuckFloat( unsigned int chuckID, const char * name, t_CKFLOAT val );
    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getChuckFloat( unsigned int chuckID, const char * name );
    
    // chuck globals: string
    bool EMSCRIPTEN_KEEPALIVE setChuckString( unsigned int chuckID, const char * name, const char * val );
    // TODO: still necessary to use callback for string, so we don't have to worry about garbage collection?
    bool EMSCRIPTEN_KEEPALIVE getChuckString( unsigned int chuckID, const char * name, void (* callback)(const char *) );

    // chuck globals: event
    bool EMSCRIPTEN_KEEPALIVE signalChuckEvent( unsigned int chuckID, const char * name );
    bool EMSCRIPTEN_KEEPALIVE broadcastChuckEvent( unsigned int chuckID, const char * name );
    bool EMSCRIPTEN_KEEPALIVE listenForChuckEventOnce( unsigned int chuckID, const char * name, void (* callback)(void) );
    bool EMSCRIPTEN_KEEPALIVE startListeningForChuckEvent( unsigned int chuckID, const char * name, void (* callback)(void) );
    bool EMSCRIPTEN_KEEPALIVE stopListeningForChuckEvent( unsigned int chuckID, const char * name, void (* callback)(void) );

    // get ugen samples
    bool EMSCRIPTEN_KEEPALIVE getGlobalUGenSamples( unsigned int chuckID, const char * name, SAMPLE * buffer, int numSamples );
    
    // int array methods
    bool EMSCRIPTEN_KEEPALIVE setGlobalIntArray( unsigned int chuckID, const char * name, t_CKINT arrayValues[], unsigned int numValues );
    // TODO: still necessary to use callback for arrays, so we don't have to worry about garbage collection?
    bool EMSCRIPTEN_KEEPALIVE getGlobalIntArray( unsigned int chuckID, const char * name, void (* callback)(t_CKINT[], t_CKUINT));
    bool EMSCRIPTEN_KEEPALIVE setGlobalIntArrayValue( unsigned int chuckID, const char * name, unsigned int index, t_CKINT value );
    t_CKINT EMSCRIPTEN_KEEPALIVE getGlobalIntArrayValue( unsigned int chuckID, const char * name, unsigned int index );
    bool EMSCRIPTEN_KEEPALIVE setGlobalAssociativeIntArrayValue( unsigned int chuckID, const char * name, char * key, t_CKINT value );
    t_CKINT EMSCRIPTEN_KEEPALIVE getGlobalAssociativeIntArrayValue( unsigned int chuckID, const char * name, char * key );
    // TODO: set entire dict, add to dict in batch; get entire dict
    
    // float array methods
    bool EMSCRIPTEN_KEEPALIVE setGlobalFloatArray( unsigned int chuckID, const char * name, t_CKFLOAT arrayValues[], unsigned int numValues );
    // TODO: still necessary to use callback for arrays, so we don't have to worry about garbage collection?
    bool EMSCRIPTEN_KEEPALIVE getGlobalFloatArray( unsigned int chuckID, const char * name, void (* callback)(t_CKFLOAT[], t_CKUINT));
    bool EMSCRIPTEN_KEEPALIVE setGlobalFloatArrayValue( unsigned int chuckID, const char * name, unsigned int index, t_CKFLOAT value );
    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getGlobalFloatArrayValue( unsigned int chuckID, const char * name, unsigned int index );
    bool EMSCRIPTEN_KEEPALIVE setGlobalAssociativeFloatArrayValue( unsigned int chuckID, const char * name, char * key, t_CKFLOAT value );
    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getGlobalAssociativeFloatArrayValue( unsigned int chuckID, const char * name, char * key );

    // chuck instances and instancing
    bool EMSCRIPTEN_KEEPALIVE initChuckInstance( unsigned int chuckID, unsigned int sampleRate, unsigned int inChannels, unsigned int outChannels );
    bool EMSCRIPTEN_KEEPALIVE clearChuckInstance( unsigned int chuckID );
    bool EMSCRIPTEN_KEEPALIVE clearGlobals( unsigned int chuckID );
    bool EMSCRIPTEN_KEEPALIVE cleanupChuckInstance( unsigned int chuckID );
    bool EMSCRIPTEN_KEEPALIVE chuckManualAudioCallback( unsigned int chuckID, float * inBuffer, float * outBuffer, unsigned int numFrames, unsigned int inChannels, unsigned int outChannels );
    void EMSCRIPTEN_KEEPALIVE cleanRegisteredChucks();

    // I/O
    bool EMSCRIPTEN_KEEPALIVE setChoutCallback( unsigned int chuckID, void (* callback)(const char *) );
    bool EMSCRIPTEN_KEEPALIVE setCherrCallback( unsigned int chuckID, void (* callback)(const char *) );
    bool EMSCRIPTEN_KEEPALIVE setStdoutCallback( void (* callback)(const char *) );
    bool EMSCRIPTEN_KEEPALIVE setStderrCallback( void (* callback)(const char *) );

    // data
    bool EMSCRIPTEN_KEEPALIVE setDataDir( const char * dir );

    // log
    bool EMSCRIPTEN_KEEPALIVE setLogLevel( unsigned int level );

    // set param
    bool EMSCRIPTEN_KEEPALIVE setParamInt( unsigned int chuckID, const char * key, t_CKINT val );
    bool EMSCRIPTEN_KEEPALIVE setParamFloat( unsigned int chuckID, const char * key, t_CKFLOAT val );
    bool EMSCRIPTEN_KEEPALIVE setParamString( unsigned int chuckID, const char * key, const char * val );
    // get param
    t_CKINT EMSCRIPTEN_KEEPALIVE getParamInt( unsigned int chuckID, const char * key );
    t_CKFLOAT EMSCRIPTEN_KEEPALIVE getParamFloat( unsigned int chuckID, const char * key );
    const char * EMSCRIPTEN_KEEPALIVE getParamString( unsigned int chuckID, const char * key );

    // get chuck time
    t_CKTIME EMSCRIPTEN_KEEPALIVE getChuckNow( unsigned int chuckID );

    // runtime import API (TDB) | 1.5.3.5 (ge)
    t_CKBOOL EMSCRIPTEN_KEEPALIVE importModule( unsigned int chuckID, const char * path );

} // extern "C"
