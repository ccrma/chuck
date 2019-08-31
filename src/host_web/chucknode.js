import ChucK from './chuckscript.js';
import { RENDER_QUANTUM_FRAMES, MAX_CHANNEL_COUNT, HeapAudioBuffer } from './wasm-audio-helper.js';
var currentChuckID = 1;

ChucK().then( function( Module ) 
{
    // metaproperties (e.g. setDataDir must be called before init_chuck
    var setDataDir = Module.cwrap( 'setDataDir', 'number', ['string'] );
    var setLogLevel = Module.cwrap( 'setLogLevel', 'number', ['number'] );

    // initialize a VM
    var initChuckInstance = Module.cwrap( 'initChuckInstance', 'number', ['number', 'number', 'number', 'number'] );
    
    
    
    // cleanup 
    var clearChuckInstance = Module.cwrap( 'clearChuckInstance', 'number', ['number'] );
    var clearGlobals = Module.cwrap( 'clearGlobals', 'number', ['number'] );
    var cleanupChuckInstance = Module.cwrap( 'cleanupChuckInstance', 'number', ['number'] );
    var cleanRegisteredChucks = Module.cwrap( 'cleanRegisteredChucks', null );

    // running code 
    var runChuckCode = Module.cwrap( 'runChuckCode', 'number', ['number', 'string'] );
    var runChuckCodeWithReplacementDac = Module.cwrap( 'runChuckCodeWithReplacementDac', 'number', ['number', 'string', 'string'] );
    var runChuckFile = Module.cwrap( 'runChuckFile', 'number', ['number', 'string'] );
    var runChuckFileWithReplacementDac = Module.cwrap( 'runChuckFileWithReplacementDac', 'number', ['number', 'string', 'string'] );
    var runChuckFileWithArgs = Module.cwrap( 'runChuckFileWithArgs', 'number', ['number', 'string', 'string'] );
    var runChuckFileWithArgsWithReplacementDac = Module.cwrap( 'runChuckFileWithArgsWithReplacementDac', 'number', ['number', 'string', 'string', 'string'] );

    // don't care about these because emscripten has another mechanism
    // for rerouting print statements
    // setChoutCallback = 
    // setCherrCallback = 
    // setStdoutCallback = 
    // setStderrCallback = 

    // set/get global int, float, string
    var setChuckInt = Module.cwrap( 'setChuckInt', 'number', ['number', 'string', 'number'] );
    // getChuckInt = 
    var setChuckFloat = Module.cwrap( 'setChuckFloat', 'number', ['number', 'string', 'number'] );
    // getChuckFloat = 
    var setChuckString = Module.cwrap( 'setChuckFloat', 'number', ['number', 'string', 'string'] );
    // getChuckString = 

    // global Events
    var signalChuckEvent = Module.cwrap( 'signalChuckEvent', 'number', ['number', 'string'] );
    var broadcastChuckEvent = Module.cwrap( 'broadcastChuckEvent', 'number', ['number', 'string'] );
    // listenForChuckEventOnce = 
    // startListeningForChuckEvent = 
    // stopListeningForChuckEvent = 

    // note: array is SAMPLE == Float32
    // NOTE ALSO anything using arrays cannot use cwrap; use in similar manner to Module._chuckManualAudioCallback
    // getGlobalUGenSamples = Module.cwrap( 'getGlobalUGenSamples', 'number', ['number', 'string', 'array', 'number'] );

    // set/get global int arrays
    // note: array is t_CKINT == Int32
    // NOTE ALSO anything using arrays cannot use cwrap
    var setGlobalIntArray = Module.cwrap( 'setGlobalIntArray', 'number', ['number', 'string', 'array', 'number'] );
    // getGlobalIntArray = 
    var setGlobalIntArrayValue = Module.cwrap( 'setGlobalIntArrayValue', 'number', ['number', 'string', 'number', 'number'] );
    // getGlobalIntArrayValue = 
    var setGlobalAssociativeIntArrayValue = Module.cwrap( 'setGlobalAssociativeIntArrayValue', 'number', ['number', 'string', 'string', 'number'] );
    // getGlobalAssociativeIntArrayValue = 

    // set/get global float arrays
    // note: array is t_CKFLOAT == Float32
    // NOTE ALSO anything using arrays cannot use cwrap
    var setGlobalFloatArray = Module.cwrap( 'setGlobalFloatArray', 'number', ['number', 'string', 'array', 'number'] );
    // getGlobalFloatArray = 
    var setGlobalFloatArrayValue = Module.cwrap( 'setGlobalFloatArrayValue', 'number', ['number', 'string', 'number', 'number'] );
    // getGlobalFloatArrayValue = 
    var setGlobalAssociativeFloatArrayValue = Module.cwrap( 'setGlobalAssociativeFloatArrayValue', 'number', ['number', 'string', 'string', 'number'] );
    // getGlobalAssociativeFloatArrayValue = 

    class ChuckNode extends AudioWorkletProcessor
    {
        constructor( options ) {
            super();
            
            this.myID = currentChuckID;
            currentChuckID++;
        
            // Allocate the buffer for the heap access. Start with stereo, but it can
            // be expanded up to 32 channels.
            this._heapInputBuffer = new HeapAudioBuffer(Module, RENDER_QUANTUM_FRAMES,
                                                        options.numberOfInputs, MAX_CHANNEL_COUNT);
            this._heapOutputBuffer = new HeapAudioBuffer(Module, RENDER_QUANTUM_FRAMES,
                                                         options.numberOfOutputs, MAX_CHANNEL_COUNT);
            
            this.srate = options.processorOptions.srate;
            this.inChannels = options.numberOfInputs;
            this.outChannels = options.numberOfOutputs;
            initChuckInstance( this.myID, this.srate, this.inChannels, this.outChannels );
            // setLogLevel( 10 );
            
            // do this in response to an incoming message
            this.port.onmessage = this.handle_message.bind(this);
            
            // override which port the Module uses for print statements
            // (if there's multiple chuck nodes, the latest one will just 
            //  redefine it. hacky but fine?)
            Module['any_port'] = this.port;
        
        }
        
        handle_message( event )
        {
            switch( event.data.type )
            {
                case 'runChuckCode':
                    runChuckCode( this.myID, event.data.code );
                    break;
                default:
                    break;
            }
            
        }

        process(inputs, outputs, parameters) {
            // structure for two-channel audio appears to be [[Float32Array],[Float32Array]]
            // i.e. each channel is surrounded by an extraneous array            
            
            // Prepare HeapAudioBuffer for the channel count change in the current
            // render quantum.
            this._heapInputBuffer.adaptChannel( inputs.length );
            this._heapOutputBuffer.adaptChannel( outputs.length );
    
            // Copy-in, process and copy-out.
            for (let channel = 0; channel < inputs.length; channel++) 
            {
                // ignore empty arrays (which signals we have no input)
                // (arguably if there is no input, the input array should be
                //  an array of 0's but who am I to judge)
                if( inputs[channel][0].length > 0 )
                {
                    this._heapInputBuffer.getChannelData(channel).set(inputs[channel][0]);
                }
            }
        
            // for multichannel, WebAudio uses planar buffers.
            // this version of ChucK has been specially compiled to do the same
            // (ordinarily, ChucK uses interleaved buffers since it processes
            //  sample by sample)
            Module._chuckManualAudioCallback( 
                this.myID,  // chuck ID
                this._heapInputBuffer.getHeapAddress(),
                this._heapOutputBuffer.getHeapAddress(),
                outputs[0][0].length, // frame size (probably 128)
                this.inChannels,  // in channels
                this.outChannels  // out channels
            );
                                     
            for (let channel = 0; channel < outputs.length; channel++) {
                outputs[channel][0].set(this._heapOutputBuffer.getChannelData(channel));
            }
            
            return true;
        }

    }



    registerProcessor( 'chuck-node', ChuckNode );
});
