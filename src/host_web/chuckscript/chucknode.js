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
    
    var replaceChuckCode = Module.cwrap( 'replaceChuckCode', 'number', ['number', 'number', 'string'] );
    var replaceChuckCodeWithReplacementDac = Module.cwrap( 'replaceChuckCodeWithReplacementDac', 'number', ['number', 'number', 'string', 'string'] );
    var replaceChuckFile = Module.cwrap( 'replaceChuckFile', 'number', ['number', 'number', 'string'] );
    var replaceChuckFileWithReplacementDac = Module.cwrap( 'replaceChuckFileWithReplacementDac', 'number', ['number', 'number', 'string', 'string'] );
    var replaceChuckFileWithArgs = Module.cwrap( 'replaceChuckFileWithArgs', 'number', ['number', 'number', 'string', 'string'] );
    var replaceChuckFileWithArgsWithReplacementDac = Module.cwrap( 'replaceChuckFileWithArgsWithReplacementDac', 'number', ['number', 'number', 'string', 'string', 'string'] );
    
    
    var isShredActive = Module.cwrap( 'isShredActive', 'number', ['number', 'number'] );
    var removeShred = Module.cwrap( 'removeShred', 'number', ['number', 'number'] );

    // don't care about these because emscripten has another mechanism
    // for rerouting print statements
    // setChoutCallback = 
    // setCherrCallback = 
    // setStdoutCallback = 
    // setStderrCallback = 

    // set/get global int, float, string
    var setChuckInt = Module.cwrap( 'setChuckInt', 'number', ['number', 'string', 'number'] );
    var getChuckInt = Module.cwrap( 'getChuckInt', 'number', ['number', 'string'] );
    var setChuckFloat = Module.cwrap( 'setChuckFloat', 'number', ['number', 'string', 'number'] );
    var getChuckFloat = Module.cwrap( 'getChuckFloat', 'number', ['number', 'string'] );
    var setChuckString = Module.cwrap( 'setChuckString', 'number', ['number', 'string', 'string'] );
    var getChuckString = Module.cwrap( 'getChuckString', 'number', ['number', 'string', 'number'] );

    // global Events
    var signalChuckEvent = Module.cwrap( 'signalChuckEvent', 'number', ['number', 'string'] );
    var broadcastChuckEvent = Module.cwrap( 'broadcastChuckEvent', 'number', ['number', 'string'] );
    var listenForChuckEventOnce = Module.cwrap( 'listenForChuckEventOnce', 'number', ['number', 'string', 'number'] );
    var startListeningForChuckEvent = Module.cwrap( 'startListeningForChuckEvent', 'number', ['number', 'string', 'number']);
    var stopListeningForChuckEvent = Module.cwrap( 'stopListeningForChuckEvent', 'number', ['number', 'string', 'number']);

    // note: array is SAMPLE == Float32
    // NOTE ALSO anything using arrays cannot use cwrap; use in similar manner to Module._chuckManualAudioCallback
    // TODO
    // getGlobalUGenSamples = Module.cwrap( 'getGlobalUGenSamples', 'number', ['number', 'string', 'array', 'number'] );

    // set/get global int arrays
    // note: array is t_CKINT == Int32
    // NOTE ALSO anything using arrays cannot use cwrap
    // TODO
    // setGlobalIntArray = Module.cwrap( 'setGlobalIntArray', 'number', ['number', 'string', 'array', 'number'] );
    // getGlobalIntArray = 
    var setGlobalIntArrayValue = Module.cwrap( 'setGlobalIntArrayValue', 'number', ['number', 'string', 'number', 'number'] );
    var getGlobalIntArrayValue = Module.cwrap( 'getGlobalIntArrayValue', 'number', ['number', 'string', 'number'] );
    var setGlobalAssociativeIntArrayValue = Module.cwrap( 'setGlobalAssociativeIntArrayValue', 'number', ['number', 'string', 'string', 'number'] );
    var getGlobalAssociativeIntArrayValue = Module.cwrap( 'getGlobalAssociativeIntArrayValue', 'number', ['number', 'string', 'string'] );

    // set/get global float arrays
    // note: array is t_CKFLOAT == Float32
    // NOTE ALSO anything using arrays cannot use cwrap
    // TODO
    // setGlobalFloatArray = Module.cwrap( 'setGlobalFloatArray', 'number', ['number', 'string', 'array', 'number'] );
    // getGlobalFloatArray = 
    var setGlobalFloatArrayValue = Module.cwrap( 'setGlobalFloatArrayValue', 'number', ['number', 'string', 'number', 'number'] );
    var getGlobalFloatArrayValue = Module.cwrap( 'getGlobalFloatArrayValue', 'number', ['number', 'string', 'number'] );
    var setGlobalAssociativeFloatArrayValue = Module.cwrap( 'setGlobalAssociativeFloatArrayValue', 'number', ['number', 'string', 'string', 'number'] );
    var getGlobalAssociativeFloatArrayValue = Module.cwrap( 'getGlobalAssociativeFloatArrayValue', 'number', ['number', 'string', 'string'] );

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
            
            this.myPointers = {}
            
            this.myActiveShreds = [];
        }
        
        handleNewShredID( newShredID, shredCallback )
        {
            if( newShredID > 0 )
            {
                // keep track for myself
                this.myActiveShreds.push( newShredID );
            }
            else
            {
                // compilation failed
            }
            // tell the host
            this.port.postMessage( { 
                type: "newShredCallback", 
                callback: shredCallback,
                shred: newShredID
            } );
        }
        
        handleReplacedShredID( oldShredID, newShredID, shredCallback )
        {
            if( newShredID > 0 )
            {
                // keep track for myself
                this.myActiveShreds.push( newShredID );
            }
            else
            {
                // compilation failed --> we did not actually remove oldShredID
                this.myActiveShreds.push( oldShredID );
            }
            // tell the host
            this.port.postMessage( { 
                type: "newShredCallback", 
                callback: shredCallback,
                shred: newShredID
            } );
        }
        
        findMostRecentActiveShred()
        {
            // find the most recent shred that is still active,
            // and forget about all the more recently shredded ones
            // that are no longer active
            var shredID = this.myActiveShreds.pop();
            while( shredID && !isShredActive( this.myID, shredID ) )
            {
                shredID = this.myActiveShreds.pop();
            }
            return shredID;
        }
        
        findShredToReplace()
        {
            var shredToReplace = this.findMostRecentActiveShred();
            if( !shredToReplace )
            {
                this.port.postMessage( { 
                    type: "console print", 
                    message: "[chuck](VM): no shreds to replace..." 
                } );
            }
            return shredToReplace;
        }
        
        handle_message( event )
        {
            switch( event.data.type )
            {
            // ================== Run / Compile ================== //
                case 'runChuckCode':
                    var shredID = runChuckCode( this.myID, event.data.code );
                    this.handleNewShredID( shredID, event.data.callback );
                    break;
                case 'runChuckCodeWithReplacementDac':
                    var shredID = runChuckCodeWithReplacementDac( this.myID, event.data.code, event.data.dac_name );
                    this.handleNewShredID( shredID, event.data.callback );
                    break;
                case 'runChuckFile':
                    var shredID = runChuckFile( this.myID, event.data.filename );
                    this.handleNewShredID( shredID, event.data.callback );
                    break;
                case 'runChuckFileWithReplacementDac':
                    var shredID = runChuckFileWithReplacementDac( this.myID, event.data.filename, event.data.dac_name );
                    this.handleNewShredID( shredID, event.data.callback );
                    break;
                case 'runChuckFileWithArgs':
                    var shredID = runChuckFileWithArgs( this.myID, event.data.filename, event.data.colon_separated_args );
                    this.handleNewShredID( shredID, event.data.callback );
                    break;
                case 'runChuckFileWithArgsWithReplacementDac':
                    var shredID = runChuckFileWithArgsWithReplacementDac( this.myID, event.data.filename, event.data.colon_separated_args, event.data.dac_name );
                    this.handleNewShredID( shredID, event.data.callback );
                    break;
                case 'replaceChuckCode':
                    var shredToReplace = this.findShredToReplace();
                    if( shredToReplace )
                    {
                        var shredID = replaceChuckCode( this.myID, shredToReplace, event.data.code );
                        this.handleReplacedShredID( shredToReplace, shredID, event.data.callback );
                    }
                    break;
                case 'replaceChuckCodeWithReplacementDac':
                    var shredToReplace = this.findShredToReplace();
                    if( shredToReplace )
                    {
                        var shredID = replaceChuckCodeWithReplacementDac( this.myID, shredToReplace, event.data.code, event.data.dac_name );
                        this.handleReplacedShredID( shredToReplace, shredID, event.data.callback );
                    }                    
                    break;
                case 'replaceChuckFile':
                    var shredToReplace = this.findShredToReplace();
                    if( shredToReplace )
                    {
                        var shredID = replaceChuckFile( this.myID, shredToReplace, event.data.filename );
                        this.handleReplacedShredID( shredToReplace, shredID, event.data.callback );
                    }
                    break;
                case 'replaceChuckFileWithReplacementDac':
                    var shredToReplace = this.findShredToReplace();
                    if( shredToReplace )
                    {
                        var shredID = replaceChuckFileWithReplacementDac( this.myID, shredToReplace, event.data.filename, event.data.dac_name );
                        this.handleReplacedShredID( shredToReplace, shredID, event.data.callback );
                    }
                    break;
                case 'replaceChuckFileWithArgs':
                    var shredToReplace = this.findShredToReplace();
                    if( shredToReplace )
                    {
                        var shredID = replaceChuckFileWithArgs( this.myID, shredToReplace, event.data.filename, event.data.colon_separated_args );
                        this.handleReplacedShredID( shredToReplace, shredID, event.data.callback );
                    }   
                    break;
                case 'replaceChuckFileWithArgsWithReplacementDac':
                    var shredToReplace = this.findShredToReplace();
                    if( shredToReplace )
                    {
                        var shredID = replaceChuckFileWithArgsWithReplacementDac( this.myID, shredToReplace, event.data.filename, event.data.colon_separated_args, event.data.dac_name );
                        this.handleReplacedShredID( shredToReplace, shredID, event.data.callback );
                    }                    
                    break;
                case 'removeLastCode':
                    var shredID = this.findMostRecentActiveShred();
                    // if we found a shred, remove it, otherwise,
                    // there are no shreds left to remove
                    if( shredID )
                    {
                        removeShred( this.myID, shredID );
                    }
                    else
                    {
                        this.port.postMessage( { 
                            type: "console print", 
                            message: "[chuck](VM): no shreds to remove..." 
                        } );
                    }
                    break;
                case 'removeShred':
                    removeShred( this.myID, event.data.shred );
                    break;
            // ================== Int, Float, String ============= //
                case 'setChuckInt':
                    setChuckInt( this.myID, event.data.variable, event.data.value );
                    break;
                case 'getChuckInt':
                    var result = getChuckInt( this.myID, event.data.variable );
                    this.port.postMessage( { type: "intCallback", callback: event.data.callback, result: result } );
                    break;
                case 'setChuckFloat':
                    setChuckFloat( this.myID, event.data.variable, event.data.value );
                    break;
                case 'getChuckFloat':
                    var result = getChuckFloat( this.myID, event.data.variable );
                    this.port.postMessage( { type: "floatCallback", callback: event.data.callback, result: result } );
                    break;
                case 'setChuckString':
                    setChuckString( this.myID, event.data.variable, event.data.value );
                    break;
                case 'getChuckString':
                    (function( thePort, theCallback, theVariable, theID ) 
                    {
                        var pointer = Module.addFunction( (function(thePort, theCallback)
                        {   
                            return function( string_ptr )
                            {
                                thePort.postMessage( { 
                                    type: "stringCallback", 
                                    callback: theCallback, 
                                    result: Module.UTF8ToString( string_ptr )
                                } );
                                Module.removeFunction( pointer );
                            }
                        })(thePort, theCallback) );
                        getChuckString( theID, theVariable, pointer );
                    })(this.port, event.data.callback, event.data.variable, this.myID);
                    break;
            // ================== Event =================== //
                case 'signalChuckEvent':
                    signalChuckEvent( this.myID, event.data.variable );
                    break;
                case 'broadcastChuckEvent':
                    broadcastChuckEvent( this.myID, event.data.variable );
                    break;
                case 'listenForChuckEventOnce':
                    (function( thePort, theCallback, theVariable, theID ) 
                    {
                        var pointer = Module.addFunction( (function(thePort, theCallback)
                        {   
                            return function()
                            {
                                thePort.postMessage( { type: "eventCallback", callback: theCallback } );
                                Module.removeFunction( pointer );
                            }
                        })(thePort, theCallback) );
                        listenForChuckEventOnce( theID, theVariable, pointer );
                    })(this.port, event.data.callback, event.data.variable, this.myID);
                    break;
                case 'startListeningForChuckEvent':
                    this.myPointers[event.data.callback] = Module.addFunction( (function(thePort, theCallback)
                    {
                        return function() 
                        {
                            thePort.postMessage( { type: "eventCallback", callback: theCallback } );
                        }
                    })(this.port, event.data.callback) );
                    startListeningForChuckEvent( this.myID, event.data.variable, this.myPointers[event.data.callback] );
                    break;
                case 'stopListeningForChuckEvent':
                    stopListeningForChuckEvent( this.myID, event.data.variable, this.myPointers[event.data.callback] );
                    Module.removeFunction( this.myPointers[event.data.callback] );
                    break;
            // ================== Int[] =================== //
                case 'setGlobalIntArray':
                    //TODO (requires heap ptr manipulation)
                    break;
                case 'getGlobalIntArray':
                    //TODO (requires heap ptr manipulation)
                    break;
                case 'setGlobalIntArrayValue':
                    setGlobalIntArrayValue( this.myID, event.data.variable, event.data.index, event.data.value );
                    break;
                case 'getGlobalIntArrayValue':
                    var result = getGlobalIntArrayValue( this.myID, event.data.variable, event.data.index );
                    this.port.postMessage( { type: "intCallback", callback: event.data.callback, result: result } );
                    break;
                case 'setGlobalAssociativeIntArrayValue':
                    setGlobalAssociativeIntArrayValue( this.myID, event.data.variable, event.data.key, event.data.value );
                    break;
                case 'getGlobalAssociativeIntArrayValue':
                    var result = getGlobalAssociativeIntArrayValue( this.myID, event.data.variable, event.data.key );
                    this.port.postMessage( { type: "intCallback", callback: event.data.callback, result: result } );
                    break;
            // ================== Float[] =================== //
                case 'setGlobalFloatArray':
                    //TODO (requires heap ptr manipulation)
                    break;
                case 'getGlobalFloatArray':
                    //TODO (requires heap ptr manipulation)
                    break;
                case 'setGlobalFloatArrayValue':
                    setGlobalFloatArrayValue( this.myID, event.data.variable, event.data.index, event.data.value );
                    break;
                case 'getGlobalFloatArrayValue':
                    var result = getGlobalFloatArrayValue( this.myID, event.data.variable, event.data.index );
                    this.port.postMessage( { type: "floatCallback", callback: event.data.callback, result: result } );
                    break;
                case 'setGlobalAssociativeFloatArrayValue':
                    setGlobalAssociativeFloatArrayValue( this.myID, event.data.variable, event.data.key, event.data.value );
                    break;
                case 'getGlobalAssociativeFloatArrayValue':
                    var result = getGlobalAssociativeFloatArrayValue( this.myID, event.data.variable, event.data.key );
                    this.port.postMessage( { type: "floatCallback", callback: event.data.callback, result: result } );
                    break;
            // ==================== VM Functions ====================== //
                case 'clearChuckInstance':
                    clearChuckInstance( this.myID );
                    break;
                case 'clearGlobals':
                    clearGlobals( this.myID );
                    this.port.postMessage( { 
                        type: "console print", 
                        message: "[chuck](VM): resetting all global variables" 
                    } );
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
