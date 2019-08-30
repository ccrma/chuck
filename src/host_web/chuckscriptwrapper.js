// EXAMPLE:
// init_chuck_instance( 1, 44100, 512, 2, 2 );
// chucks[1].compile_code( "SinOsc foo => dac; 200 => foo.freq; 1::second => now;" );
// chucks[1].run_audio_callback();
// console.log( chucks[1].output[0], chucks[1].output[2], chucks[1].output[4], chucks[1].output[6] ); 



var init_chuck_instance = function( chuckID, sampleRate, framesPerBuffer, numChannels ){};
var chucks = {}

ChucK().then( function( Module )
{
    init_chuck = Module.cwrap( 'initChuckInstance', 'number', ['number', 'number', 'number', 'number'] );
    compile_chuck_code = Module.cwrap( 'runChuckCode', 'number', ['number', 'string'] );
    init_chuck_instance = function( chuckID, sampleRate, framesPerBuffer, inChannels, outChannels )
    {
        chucks[chuckID] = {};
        chucks[chuckID].framesPerBuffer = framesPerBuffer;
        chucks[chuckID].inChannels = inChannels;
        chucks[chuckID].outChannels = outChannels;
        chucks[chuckID].sampleRate = sampleRate;
        // TODO: why doesn't chuck need to know the number of channels upon init?
        // Right now it is just forcing the number of channels to be 2 anyway.
        init_chuck( chuckID, sampleRate, inChannels, outChannels );
        chucks[chuckID].input = new Float32Array( framesPerBuffer * inChannels );
        chucks[chuckID].output = new Float32Array( framesPerBuffer * outChannels );
        chucks[chuckID].inputPtr = Module._malloc( chucks[chuckID].input.length * chucks[chuckID].input.BYTES_PER_ELEMENT );
        chucks[chuckID].outputPtr = Module._malloc( chucks[chuckID].output.length * chucks[chuckID].output.BYTES_PER_ELEMENT );
        
        chucks[chuckID].compile_code = function( codeString )
        {
            return compile_chuck_code( chuckID, codeString );
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