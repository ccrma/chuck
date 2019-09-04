import { RENDER_QUANTUM_FRAMES, MAX_CHANNEL_COUNT, HeapAudioBuffer } from './wasm-audio-helper.js';
var currentChuckID = 1;
// metaproperties (e.g. setDataDir must be called before init_chuck
var setDataDir;
var setLogLevel;
// initialize a VM
var initChuckInstance;
// cleanup 
var clearChuckInstance;
var clearGlobals;
var cleanupChuckInstance;
var cleanRegisteredChucks;

// running code 
var runChuckCode;
var runChuckCodeWithReplacementDac;
var runChuckFile;
var runChuckFileWithReplacementDac;
var runChuckFileWithArgs;
var runChuckFileWithArgsWithReplacementDac;

var replaceChuckCode;
var replaceChuckCodeWithReplacementDac;
var replaceChuckFile;
var replaceChuckFileWithReplacementDac;
var replaceChuckFileWithArgs;
var replaceChuckFileWithArgsWithReplacementDac;


var isShredActive;
var removeShred;

// don't care about these because emscripten has another mechanism
// for rerouting print statements
// setChoutCallback = 
// setCherrCallback = 
// setStdoutCallback = 
// setStderrCallback = 

// set/get global int, float, string
var setChuckInt;
var getChuckInt;
var setChuckFloat;
var getChuckFloat;
var setChuckString;
var getChuckString;

// global Events
var signalChuckEvent;
var broadcastChuckEvent;
var listenForChuckEventOnce;
var startListeningForChuckEvent;
var stopListeningForChuckEvent;

// note: array is SAMPLE == Float32
// NOTE ALSO anything using arrays cannot use cwrap; use in similar manner to Module._chuckManualAudioCallback
// TODO
// getGlobalUGenSamples = Module.cwrap( 'getGlobalUGenSamples', 'number', ['number', 'string', 'array', 'number'] );

// set/get global int arrays
// note: anything using arrays cannot use cwrap (since 'array' is an Int64Array)
//  called manually with heap manipulation below
//  setGlobalIntArray = 
var getGlobalIntArray;
var setGlobalIntArrayValue;
var getGlobalIntArrayValue;
var setGlobalAssociativeIntArrayValue;
var getGlobalAssociativeIntArrayValue;

// set/get global float arrays
// note: anything using arrays cannot use cwrap. called manually with heap manipulation below
//  setGlobalFloatArray = 
var getGlobalFloatArray;
var setGlobalFloatArrayValue;
var getGlobalFloatArrayValue;
var setGlobalAssociativeFloatArrayValue;
var getGlobalAssociativeFloatArrayValue;


var initGlobals = function( Module )
{
    // metaproperties (e.g. setDataDir must be called before init_chuck)
    setDataDir = Module.cwrap( 'setDataDir', 'number', ['string'] );
    setLogLevel = Module.cwrap( 'setLogLevel', 'number', ['number'] );

    // initialize a VM
    initChuckInstance = Module.cwrap( 'initChuckInstance', 'number', ['number', 'number', 'number', 'number'] );



    // cleanup 
    clearChuckInstance = Module.cwrap( 'clearChuckInstance', 'number', ['number'] );
    clearGlobals = Module.cwrap( 'clearGlobals', 'number', ['number'] );
    cleanupChuckInstance = Module.cwrap( 'cleanupChuckInstance', 'number', ['number'] );
    cleanRegisteredChucks = Module.cwrap( 'cleanRegisteredChucks', null );

    // running code 
    runChuckCode = Module.cwrap( 'runChuckCode', 'number', ['number', 'string'] );
    runChuckCodeWithReplacementDac = Module.cwrap( 'runChuckCodeWithReplacementDac', 'number', ['number', 'string', 'string'] );
    runChuckFile = Module.cwrap( 'runChuckFile', 'number', ['number', 'string'] );
    runChuckFileWithReplacementDac = Module.cwrap( 'runChuckFileWithReplacementDac', 'number', ['number', 'string', 'string'] );
    runChuckFileWithArgs = Module.cwrap( 'runChuckFileWithArgs', 'number', ['number', 'string', 'string'] );
    runChuckFileWithArgsWithReplacementDac = Module.cwrap( 'runChuckFileWithArgsWithReplacementDac', 'number', ['number', 'string', 'string', 'string'] );

    replaceChuckCode = Module.cwrap( 'replaceChuckCode', 'number', ['number', 'number', 'string'] );
    replaceChuckCodeWithReplacementDac = Module.cwrap( 'replaceChuckCodeWithReplacementDac', 'number', ['number', 'number', 'string', 'string'] );
    replaceChuckFile = Module.cwrap( 'replaceChuckFile', 'number', ['number', 'number', 'string'] );
    replaceChuckFileWithReplacementDac = Module.cwrap( 'replaceChuckFileWithReplacementDac', 'number', ['number', 'number', 'string', 'string'] );
    replaceChuckFileWithArgs = Module.cwrap( 'replaceChuckFileWithArgs', 'number', ['number', 'number', 'string', 'string'] );
    replaceChuckFileWithArgsWithReplacementDac = Module.cwrap( 'replaceChuckFileWithArgsWithReplacementDac', 'number', ['number', 'number', 'string', 'string', 'string'] );


    isShredActive = Module.cwrap( 'isShredActive', 'number', ['number', 'number'] );
    removeShred = Module.cwrap( 'removeShred', 'number', ['number', 'number'] );

    // don't care about these because emscripten has another mechanism
    // for rerouting print statements
    // setChoutCallback = 
    // setCherrCallback = 
    // setStdoutCallback = 
    // setStderrCallback = 

    // set/get global int, float, string
    setChuckInt = Module.cwrap( 'setChuckInt', 'number', ['number', 'string', 'number'] );
    getChuckInt = Module.cwrap( 'getChuckInt', 'number', ['number', 'string'] );
    setChuckFloat = Module.cwrap( 'setChuckFloat', 'number', ['number', 'string', 'number'] );
    getChuckFloat = Module.cwrap( 'getChuckFloat', 'number', ['number', 'string'] );
    setChuckString = Module.cwrap( 'setChuckString', 'number', ['number', 'string', 'string'] );
    getChuckString = Module.cwrap( 'getChuckString', 'number', ['number', 'string', 'number'] );

    // global Events
    signalChuckEvent = Module.cwrap( 'signalChuckEvent', 'number', ['number', 'string'] );
    broadcastChuckEvent = Module.cwrap( 'broadcastChuckEvent', 'number', ['number', 'string'] );
    listenForChuckEventOnce = Module.cwrap( 'listenForChuckEventOnce', 'number', ['number', 'string', 'number'] );
    startListeningForChuckEvent = Module.cwrap( 'startListeningForChuckEvent', 'number', ['number', 'string', 'number']);
    stopListeningForChuckEvent = Module.cwrap( 'stopListeningForChuckEvent', 'number', ['number', 'string', 'number']);

    // note: array is SAMPLE == Float32
    // NOTE ALSO anything using arrays cannot use cwrap; use in similar manner to Module._chuckManualAudioCallback
    // TODO
    // getGlobalUGenSamples = Module.cwrap( 'getGlobalUGenSamples', 'number', ['number', 'string', 'array', 'number'] );

    // set/get global int arrays
    // note: anything using arrays cannot use cwrap (since 'array' is an Int64Array)
    //  called manually with heap manipulation below
    //  setGlobalIntArray = Module.cwrap( 'setGlobalIntArray', 'number', ['number', 'string', 'array', 'number'] );
    getGlobalIntArray = Module.cwrap( 'getGlobalIntArray', 'number', ['number', 'string', 'number'] );
    setGlobalIntArrayValue = Module.cwrap( 'setGlobalIntArrayValue', 'number', ['number', 'string', 'number', 'number'] );
    getGlobalIntArrayValue = Module.cwrap( 'getGlobalIntArrayValue', 'number', ['number', 'string', 'number'] );
    setGlobalAssociativeIntArrayValue = Module.cwrap( 'setGlobalAssociativeIntArrayValue', 'number', ['number', 'string', 'string', 'number'] );
    getGlobalAssociativeIntArrayValue = Module.cwrap( 'getGlobalAssociativeIntArrayValue', 'number', ['number', 'string', 'string'] );

    // set/get global float arrays
    // note: anything using arrays cannot use cwrap. called manually with heap manipulation below
    //  setGlobalFloatArray = Module.cwrap( 'setGlobalFloatArray', 'number', ['number', 'string', 'array', 'number'] );
    getGlobalFloatArray = Module.cwrap( 'getGlobalFloatArray', 'number', ['number', 'string', 'number'] );
    setGlobalFloatArrayValue = Module.cwrap( 'setGlobalFloatArrayValue', 'number', ['number', 'string', 'number', 'number'] );
    getGlobalFloatArrayValue = Module.cwrap( 'getGlobalFloatArrayValue', 'number', ['number', 'string', 'number'] );
    setGlobalAssociativeFloatArrayValue = Module.cwrap( 'setGlobalAssociativeFloatArrayValue', 'number', ['number', 'string', 'string', 'number'] );
    getGlobalAssociativeFloatArrayValue = Module.cwrap( 'getGlobalAssociativeFloatArrayValue', 'number', ['number', 'string', 'string'] );



    // set data dir to "/" for embedded files
    setDataDir( "/" );
}



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
        
        var Module = options.processorOptions.Module;
        initGlobals( Module );

        
        // override which port the Module uses for print statements
        // (if there's multiple chuck nodes, the latest one will just 
        //  redefine it. hacky but fine?)
        Module['any_port'] = this.port;
        
        this.myPointers = {}
        
        this.myActiveShreds = [];
        this.Module = Module;
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
            type: "replacedShredCallback", 
            callback: shredCallback,
            newShred: newShredID,
            oldShred: oldShredID
        } );
    }
    
    handleRemoveShred( shredID, callback )
    {
        if( removeShred( this.myID, shredID ) )
        {
            this.handleRemovedShredID( shredID, callback );
        }
        else
        {
            this.handleRemovedShredID( 0, callback );
        }
    }
    
    handleRemovedShredID( shredID, shredCallback )
    {
        this.port.postMessage( { 
            type: "removedShredCallback", 
            callback: shredCallback,
            shred: shredID
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
        // ================== Filesystem ===================== //
            case 'loadFile':
                this.Module.FS_createDataFile( '/' + event.data.directory, 
                    event.data.filename, event.data.data, true, true, true );
                break;
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
                    this.handleRemoveShred( shredID, event.data.callback );
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
                this.handleRemoveShred( event.data.shred, event.data.callback );
                break;
            case 'isShredActive':
                this.port.postMessage({
                    type: "intCallback",
                    callback: event.data.callback,
                    result: isShredActive( this.myID, event.data.shred )
                });
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
                (function( thePort, theCallback, theVariable, theID, Module ) 
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
                    })(thePort, theCallback), 'vi' );
                    getChuckString( theID, theVariable, pointer );
                })(this.port, event.data.callback, event.data.variable, this.myID, this.Module);
                break;
        // ================== Event =================== //
            case 'signalChuckEvent':
                signalChuckEvent( this.myID, event.data.variable );
                break;
            case 'broadcastChuckEvent':
                broadcastChuckEvent( this.myID, event.data.variable );
                break;
            case 'listenForChuckEventOnce':
                (function( thePort, theCallback, theVariable, theID, Module ) 
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
                })(this.port, event.data.callback, event.data.variable, this.myID, this.Module);
                break;
            case 'startListeningForChuckEvent':
                this.myPointers[event.data.callback] = this.Module.addFunction( (function(thePort, theCallback)
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
                this.Module.removeFunction( this.myPointers[event.data.callback] );
                break;
        // ================== Int[] =================== //
            case 'setGlobalIntArray':
                // convert to Int32Array
                var values = new Int32Array( event.data.values );
                // put onto heap
                var valuesPtr = this.Module._malloc( values.length * values.BYTES_PER_ELEMENT );
                var heapview = this.Module.HEAP32.subarray( (valuesPtr >> 2), (valuesPtr >> 2) + values.length );
                heapview.set( values );
                
                // put variable name on heap as well
                var stringBytes = event.data.variable.length << 2 + 1;
                var stringPtr = this.Module._malloc( stringBytes );
                this.Module.stringToUTF8( event.data.variable, stringPtr, stringBytes );
                
                // call
                this.Module._setGlobalIntArray( this.myID, stringPtr, valuesPtr, values.length );
                
                // free
                this.Module._free( valuesPtr );
                this.Module._free( stringPtr );
                break;
            case 'getGlobalIntArray':
                (function( thePort, theCallback, theVariable, theID, Module ) 
                {
                    var pointer = Module.addFunction( (function(thePort, theCallback)
                    {   
                        return function( int32_ptr, len )
                        {
                            var result = new Int32Array(
                                Module.HEAPU8.buffer,
                                int32_ptr,
                                len
                            );
                            thePort.postMessage( { 
                                type: "intArrayCallback", 
                                callback: theCallback, 
                                result: result
                            } );
                            Module.removeFunction( pointer );
                        }
                    })(thePort, theCallback), 'vii' );
                    getGlobalIntArray( theID, theVariable, pointer );
                })(this.port, event.data.callback, event.data.variable, this.myID, this.Module);
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
                // convert to Float32Array
                var values = new Float32Array( event.data.values );
                // put onto heap
                var valuesPtr = this.Module._malloc( values.length * values.BYTES_PER_ELEMENT );
                var heapview = this.Module.HEAPF32.subarray( (valuesPtr >> 2), (valuesPtr >> 2) + values.length );
                heapview.set( values );
                
                // put variable name on heap as well
                var stringBytes = event.data.variable.length << 2 + 1;
                var stringPtr = this.Module._malloc( stringBytes );
                this.Module.stringToUTF8( event.data.variable, stringPtr, stringBytes );
                
                // call
                this.Module._setGlobalFloatArray( this.myID, stringPtr, valuesPtr, values.length );
                
                // free
                this.Module._free( valuesPtr );
                this.Module._free( stringPtr );
                break;
            case 'getGlobalFloatArray':
                (function( thePort, theCallback, theVariable, theID, Module ) 
                {
                    var pointer = Module.addFunction( (function(thePort, theCallback)
                    {   
                        return function( float32_ptr, len )
                        {
                            var result = new Float32Array(
                                Module.HEAPU8.buffer,
                                float32_ptr,
                                len
                            );
                            thePort.postMessage( { 
                                type: "floatArrayCallback", 
                                callback: theCallback, 
                                result: result
                            } );
                            Module.removeFunction( pointer );
                        }
                    })(thePort, theCallback), 'vii' );
                    getGlobalFloatArray( theID, theVariable, pointer );
                })(this.port, event.data.callback, event.data.variable, this.myID, this.Module);
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
        this.Module._chuckManualAudioCallback( 
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

