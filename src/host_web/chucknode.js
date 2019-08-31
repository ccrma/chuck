import ChucK from './chuckscript.js';
import { RENDER_QUANTUM_FRAMES, MAX_CHANNEL_COUNT, HeapAudioBuffer } from './wasm-audio-helper.js';

ChucK().then( function( Module ) {
    var init_chuck = Module.cwrap( 'initChuckInstance', 'number', ['number', 'number', 'number', 'number', 'number'] );
    var run_chuck_code = Module.cwrap( 'runChuckCode', 'number', ['number', 'string'] );
    var setLogLevel = Module.cwrap( 'setLogLevel', 'number', ['number'] );

    class ChuckNode extends AudioWorkletProcessor
    {
        constructor( options ) {
            super();
        
            // Allocate the buffer for the heap access. Start with stereo, but it can
            // be expanded up to 32 channels.
            this._heapInputBuffer = new HeapAudioBuffer(Module, RENDER_QUANTUM_FRAMES,
                                                        options.numberOfInputs, MAX_CHANNEL_COUNT);
            this._heapOutputBuffer = new HeapAudioBuffer(Module, RENDER_QUANTUM_FRAMES,
                                                         options.numberOfOutputs, MAX_CHANNEL_COUNT);
            
            // TODO: is buffer size ever not 128?
            this.srate = options.processorOptions.srate;
            this.inChannels = options.numberOfInputs;
            this.outChannels = options.numberOfOutputs;
            init_chuck( 1, this.srate, 128, this.inChannels, this.outChannels );
            setLogLevel( 10 );
            
            // do this in response to an incoming message
            this.port.onmessage = this.handle_message.bind(this);
        
        }
        
        handle_message( event )
        {
            console.log( event.data.code );
            run_chuck_code( 1, event.data.code );

        }

        process(inputs, outputs, parameters) {
            // Use the 1st input and output only to make the example simpler. |input|
            // and |output| here have the similar structure with the AudioBuffer
            // interface. (i.e. An array of Float32Array)
            let input = inputs[0];
            let output = outputs[0];

            // For this given render quantum, the channel count of the node is fixed
            // and identical for the input and the output.
            let channelCount = input.length;

            // Prepare HeapAudioBuffer for the channel count change in the current
            // render quantum.
            this._heapInputBuffer.adaptChannel(channelCount);
            this._heapOutputBuffer.adaptChannel(channelCount);
    
            // Copy-in, process and copy-out.
            for (let channel = 0; channel < channelCount; ++channel) {
                this._heapInputBuffer.getChannelData(channel).set(input[channel]);
            }
        
            // TODO: for multichannel, WebAudio uses planar buffers and ChucK uses interleaved
            // (why would you ever use planar for anything? I've never heard of it being used before.)
            Module._chuckManualAudioCallback( 
                1,  // chuck ID
                this._heapInputBuffer.getHeapAddress(),
                this._heapOutputBuffer.getHeapAddress(),
                128, // frame size
                this.inChannels,  // in channels
                this.outChannels  // out channels
            );
                                     
            for (let channel = 0; channel < channelCount; ++channel) {
                output[channel].set(this._heapOutputBuffer.getChannelData(channel));
            } 

            return true;
        }

    }



    registerProcessor( 'chuck-node', ChuckNode );
});
