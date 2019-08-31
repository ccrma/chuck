import ChucK from './chuckscript.js';
import { RENDER_QUANTUM_FRAMES, MAX_CHANNEL_COUNT, HeapAudioBuffer } from './wasm-audio-helper.js';
var currentChuckID = 1;

ChucK().then( function( Module ) {
    var init_chuck = Module.cwrap( 'initChuckInstance', 'number', ['number', 'number', 'number', 'number'] );
    var run_chuck_code = Module.cwrap( 'runChuckCode', 'number', ['number', 'string'] );
    var setLogLevel = Module.cwrap( 'setLogLevel', 'number', ['number'] );

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
            init_chuck( this.myID, this.srate, this.inChannels, this.outChannels );
            // setLogLevel( 10 );
            
            // do this in response to an incoming message
            this.port.onmessage = this.handle_message.bind(this);
        
        }
        
        handle_message( event )
        {
            console.log( event.data.code );
            run_chuck_code( this.myID, event.data.code );

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
