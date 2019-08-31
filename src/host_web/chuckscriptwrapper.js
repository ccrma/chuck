// EXAMPLE:
// ChucKReady.then( function() 
// {
//     var myChucKID = 1;
//     init_chuck_instance( myChucKID, 44100, 512, 2, 2 );
//     chucks[myChucKID].compile_code( "SinOsc foo => dac; 200 => foo.freq; 1::second => now;" );
//     chucks[myChucKID].run_audio_callback();
//     console.log( chucks[myChucKID].output[0], chucks[myChucKID].output[2], chucks[myChucKID].output[4], chucks[myChucKID].output[6] ); 
// });



var init_chuck_instance = function( chuckID, sampleRate, framesPerBuffer, numChannels ){};
var chucks = {}
// TODO:
// - redirect prints to the on-screen console
// - enable global variables (and other tasks)

const ChucKReady = ChucK().then( function( Module )
{
    init_chuck = Module.cwrap( 'initChuckInstance', 'number', ['number', 'number', 'number', 'number'] );
    initChuckInstance = Module.cwrap( 'initChuckInstance', 'number', ['number', 'number', 'number', 'number'] );
    clearChuckInstance = Module.cwrap( 'clearChuckInstance', 'number', ['number'] );
    clearGlobals = Module.cwrap( 'clearGlobals', 'number', ['number'] );
    cleanupChuckInstance = Module.cwrap( 'cleanupChuckInstance', 'number', ['number'] );
    cleanRegisteredChucks = Module.cwrap( 'cleanRegisteredChucks', null );

    runChuckCode = Module.cwrap( 'runChuckCode', 'number', ['number', 'string'] );
    runChuckCodeWithReplacementDac = Module.cwrap( 'runChuckCodeWithReplacementDac', 'number', ['number', 'string', 'string'] );
    runChuckFile = Module.cwrap( 'runChuckFile', 'number', ['number', 'string'] );
    runChuckFileWithReplacementDac = Module.cwrap( 'runChuckFileWithReplacementDac', 'number', ['number', 'string', 'string'] );
    runChuckFileWithArgs = Module.cwrap( 'runChuckFileWithArgs', 'number', ['number', 'string', 'string'] );
    runChuckFileWithArgsWithReplacementDac = Module.cwrap( 'runChuckFileWithArgsWithReplacementDac', 'number', ['number', 'string', 'string', 'string'] );

    // setChoutCallback = 
    // setCherrCallback = 
    // setStdoutCallback = 
    // setStderrCallback = 

    setDataDir = Module.cwrap( 'setDataDir', 'number', ['string'] );
    setLogLevel = Module.cwrap( 'setLogLevel', 'number', ['number'] );

    setChuckInt = Module.cwrap( 'setChuckInt', 'number', ['number', 'string', 'number'] );
    // getChuckInt = 
    setChuckFloat = Module.cwrap( 'setChuckFloat', 'number', ['number', 'string', 'number'] );
    // getChuckFloat = 
    setChuckString = Module.cwrap( 'setChuckFloat', 'number', ['number', 'string', 'string'] );
    // getChuckString = 

    signalChuckEvent = Module.cwrap( 'signalChuckEvent', 'number', ['number', 'string'] );
    broadcastChuckEvent = Module.cwrap( 'broadcastChuckEvent', 'number', ['number', 'string'] );
    // listenForChuckEventOnce = 
    // startListeningForChuckEvent = 
    // stopListeningForChuckEvent = 

    // note: array is SAMPLE == Float32
    // NOTE ALSO anything using arrays cannot use cwrap
    getGlobalUGenSamples = Module.cwrap( 'getGlobalUGenSamples', 'number', ['number', 'string', 'array', 'number'] );

    // note: array is t_CKINT == Int64
    // NOTE ALSO anything using arrays cannot use cwrap
    setGlobalIntArray = Module.cwrap( 'setGlobalIntArray', 'number', ['number', 'string', 'array', 'number'] );
    // getGlobalIntArray = 
    setGlobalIntArrayValue = Module.cwrap( 'setGlobalIntArrayValue', 'number', ['number', 'string', 'number', 'number'] );
    // getGlobalIntArrayValue = 
    setGlobalAssociativeIntArrayValue = Module.cwrap( 'setGlobalAssociativeIntArrayValue', 'number', ['number', 'string', 'string', 'number'] );
    // getGlobalAssociativeIntArrayValue = 

    // note: array is t_CKFLOAT == Float64
    // NOTE ALSO anything using arrays cannot use cwrap
    setGlobalFloatArray = Module.cwrap( 'setGlobalFloatArray', 'number', ['number', 'string', 'array', 'number'] );
    // getGlobalFloatArray = 
    setGlobalFloatArrayValue = Module.cwrap( 'setGlobalFloatArrayValue', 'number', ['number', 'string', 'number', 'number'] );
    // getGlobalFloatArrayValue = 
    setGlobalAssociativeFloatArrayValue = Module.cwrap( 'setGlobalAssociativeFloatArrayValue', 'number', ['number', 'string', 'string', 'number'] );
    // getGlobalAssociativeFloatArrayValue = 


    

    init_chuck_instance = function( chuckID, sampleRate, framesPerBuffer, inChannels, outChannels )
    {
        chucks[chuckID] = {};
        chucks[chuckID].framesPerBuffer = framesPerBuffer;
        chucks[chuckID].inChannels = inChannels;
        chucks[chuckID].outChannels = outChannels;
        chucks[chuckID].sampleRate = sampleRate;
        init_chuck( chuckID, sampleRate, inChannels, outChannels );
        chucks[chuckID].input = new Float32Array( framesPerBuffer * inChannels );
        chucks[chuckID].output = new Float32Array( framesPerBuffer * outChannels );
        chucks[chuckID].inputPtr = Module._malloc( chucks[chuckID].input.length * chucks[chuckID].input.BYTES_PER_ELEMENT );
        chucks[chuckID].outputPtr = Module._malloc( chucks[chuckID].output.length * chucks[chuckID].output.BYTES_PER_ELEMENT );
        
        chucks[chuckID].compile_code = function( codeString )
        {
            return runChuckCode( chuckID, codeString );
        }
        
        chucks[chuckID].run_audio_callback = function() 
        {
            var to_ret = Module._chuckManualAudioCallback( 
                chuckID, 
                chucks[chuckID].inputPtr, 
                chucks[chuckID].outputPtr, 
                chucks[chuckID].framesPerBuffer, 
                chucks[chuckID].inChannels, 
                chucks[chuckID].outChannels 
            );
            
            chucks[chuckID].output = new Float32Array( 
                Module.HEAPU8.buffer, 
                chucks[chuckID].outputPtr, 
                chucks[chuckID].framesPerBuffer * chucks[chuckID].outChannels 
            );
            
            return to_ret;
        }
    }    
});