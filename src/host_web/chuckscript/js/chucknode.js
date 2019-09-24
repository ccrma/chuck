import ChucK from './chuckscript.js';
import { RENDER_QUANTUM_FRAMES, MAX_CHANNEL_COUNT, HeapAudioBuffer } from './wasm-audio-helper.js';
var globalInit = false;
var globalPromise = undefined;
var chucks = {};

var setDataDir, setLogLevel, initChuckInstance,

    clearChuckInstance, clearGlobals, cleanupChuckInstance,
    cleanRegisteredChucks, 
    
    runChuckCode, runChuckCodeWithReplacementDac, runChuckFile,
    runChuckFileWithReplacementDac, runChuckFileWithArgs,
    runChuckFileWithArgsWithReplacementDac,
    
    replaceChuckCode, replaceChuckCodeWithReplacementDac,
    replaceChuckFile, replaceChuckFileWithReplacementDac,
    replaceChuckFileWithArgs, replaceChuckFileWithArgsWithReplacementDac,
    
    isShredActive,
    removeShred,

    // don't care about these because emscripten has another mechanism
    // for rerouting print statements
    // setChoutCallback = 
    // setCherrCallback = 
    // setStdoutCallback = 
    // setStderrCallback = 

    // set/get global int, float, string
    setChuckInt, getChuckInt, setChuckFloat, getChuckFloat,
    setChuckString, getChuckString,

    // global Events
    signalChuckEvent, broadcastChuckEvent, listenForChuckEventOnce,
    startListeningForChuckEvent, stopListeningForChuckEvent,

    // note: array is SAMPLE == Float32
    // NOTE ALSO anything using arrays cannot use cwrap; use in similar manner to Module._chuckManualAudioCallback
    // TODO
    // getGlobalUGenSamples = Module.cwrap( 'getGlobalUGenSamples', 'number', ['number', 'string', 'array', 'number'] );

    // set/get global int arrays
    // note: anything using arrays cannot use cwrap (since 'array' is an Int64Array)
    //  called manually with heap manipulation below
    //  setGlobalIntArray = Module.cwrap( 'setGlobalIntArray', 'number', ['number', 'string', 'array', 'number'] );
    getGlobalIntArray, setGlobalIntArrayValue, getGlobalIntArrayValue,
    setGlobalAssociativeIntArrayValue, getGlobalAssociativeIntArrayValue,

    // set/get global float arrays
    // note: anything using arrays cannot use cwrap. called manually with heap manipulation below
    //  setGlobalFloatArray = Module.cwrap( 'setGlobalFloatArray', 'number', ['number', 'string', 'array', 'number'] );
    getGlobalFloatArray, setGlobalFloatArrayValue, getGlobalFloatArrayValue,
    setGlobalAssociativeFloatArrayValue, getGlobalAssociativeFloatArrayValue;

var initGlobals = function( Module )
{
    // metaproperties (e.g. setDataDir must be called before init_chuck
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
    constructor( options ) 
    {
        super();
        
        this.srate = options.processorOptions.srate;
        this.inChannels = options.outputChannelCount[0];
        this.outChannels = options.outputChannelCount[0];
        
        this.myID = options.processorOptions.chuckID;
        
        chucks[this.myID] = this;
        
        // do this in response to an incoming message
        this.port.onmessage = this.handle_message.bind(this);
                    
        this.myPointers = {}
        this.myActiveShreds = [];
        this.haveInit = false;

        this.init( options.processorOptions.preloadedFiles, 
                   options.processorOptions.wasm );
    }
    
    init( preloadedFiles, wasm )
    {
        if( !globalInit )
        {
            var PreModule = {
                wasmBinary: wasm,
                print: (function( self ) 
                    {
                        return function( text )
                        {
                            console.log( text );
                            self.port.postMessage( { type: "console print", message: text } );
                        }
                    })( this ),
                printErr: (function( self )
                    {
                        return function( text )
                        {
                            console.error( text );
                            self.port.postMessage( { type: "console print", message: text } );
                        }
                    })( this ),
                // don't try to decode audio files; I'm really truly trying to copy the binaries only I promise
                noAudioDecoding: true,
                // don't try to decode images either, just so it will stop printing warnings it can't do it
                noImageDecoding: true
            };
    
            PreModule[ "preRun" ] = [
                (function( filesToPreload, Module )
                {
                    return function()
                    {
                        for( var i = 0; i < filesToPreload.length; i++ )
                        {
                            Module["FS_createPreloadedFile"]( "/", 
                                filesToPreload[i].filename, 
                                filesToPreload[i].data, true, true );
                        }
                    }
                })( preloadedFiles, PreModule )
            ]
            
            globalPromise = ChucK( PreModule );
            globalInit = true;
        }
        globalPromise.then( (function( self )
        {
            return function( Module ) 
            {
                self.Module = Module;

                initGlobals( Module );

                // Allocate the buffer for the heap access. Start with stereo, but it can
                // be expanded up to 32 channels.
                self._heapInputBuffer = new HeapAudioBuffer(Module, RENDER_QUANTUM_FRAMES,
                    self.inChannels, MAX_CHANNEL_COUNT);
                self._heapOutputBuffer = new HeapAudioBuffer(Module, RENDER_QUANTUM_FRAMES,
                    self.outChannels, MAX_CHANNEL_COUNT);

                initChuckInstance( self.myID, self.srate, self.inChannels, self.outChannels );
                // setLogLevel( 10 );
                self.haveInit = true;
                
                self.port.postMessage( { type: 'initCallback' } );
            }
        })( this ) );   
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
                // convert to Float64Array
                var values = new Float64Array( event.data.values );
                // put onto heap
                var valuesPtr = this.Module._malloc( values.length * values.BYTES_PER_ELEMENT );
                var heapview = this.Module.HEAPF64.subarray( (valuesPtr >> 3), (valuesPtr >> 3) + values.length );
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
                        return function( float64_ptr, len )
                        {
                            var result = new Float64Array(
                                Module.HEAPU8.buffer,
                                float64_ptr,
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

    process(inputs, outputs, parameters) 
    {
        if( !this.haveInit ) { return true; }
        // structure for two-channel audio is [[Float32Array,Float32Array]]
        // since it's ONE output (outer array) with TWO channels (inner array of arrays)
        let input = inputs[0];
        let output = outputs[0];
        // recompute subarray views just in case memory has grown recently
        this._heapInputBuffer.adaptChannel( input.length );

        // copy input
        for (let channel = 0; channel < input.length; channel++)
        {
            // ... but only if they actually gave us something
            if( input[channel].length > 0 )
            {
                this._heapInputBuffer.getChannelData(channel).set(input[channel]);
            }
        }
        
        // process
        // for multichannel, WebAudio uses planar buffers.
        // this version of ChucK has been specially compiled to do the same
        // (ordinarily, ChucK uses interleaved buffers since it processes
        //  sample by sample)
        this.Module._chuckManualAudioCallback( 
            this.myID,        // chuck ID
            this._heapInputBuffer.getHeapAddress(),
            this._heapOutputBuffer.getHeapAddress(),
            output[0].length, // frame size (probably 128)
            this.inChannels,  // in channels
            this.outChannels  // out channels
        );
        
        // recompute subarray views just in case memory grew while we
        // were calling chuck audio callback
        this._heapOutputBuffer.adaptChannel( output.length );
        
        // copy output
        for (let channel = 0; channel < output.length; channel++) 
        {
            output[channel].set(this._heapOutputBuffer.getChannelData(channel));
        }
        
        return true;
    }

}

class ChuckSubNode extends AudioWorkletProcessor
{
    constructor( options ) 
    {
        super();
        
        this.inChannels = 0;
        // SHOULD BE 1
        this.outChannels = options.outputChannelCount[0];
        
        
        this.myID = options.processorOptions.chuckID;
        this.myDac = options.processorOptions.dac;
        
        // do this in response to an incoming message
        this.port.onmessage = this.handle_message.bind(this);
        
        console.assert( globalInit && this.myID in chucks, "ChuckSubNode can't find its ChuckNode with ID " + this.myID );
        this.myChuck = chucks[ this.myID ];

        this.haveInit = false;
        this.init();
    }
    
    init()
    {
        globalPromise.then( (function( self )
        {
            return function( Module ) 
            {
                self.Module = Module;

                // Allocate the buffer for the heap access. Start with however many channels we have
                // (should be 1)
                self._heapOutputBuffer = new HeapAudioBuffer(Module, RENDER_QUANTUM_FRAMES,
                    self.outChannels, MAX_CHANNEL_COUNT);
                
                var stringBytes = self.myDac.length << 2 + 1;
                self.myDacCString = self.Module._malloc( stringBytes );
                self.Module.stringToUTF8( self.myDac, self.myDacCString, stringBytes );

                self.haveInit = true;
                
                self.port.postMessage( { type: 'initCallback' } );
            }
        })( this ) );   
    }

    handle_message( event )
    {
        switch( event.data.type )
        {
            default:
                break;
        }
        
    }

    process( inputs, outputs, parameters ) 
    {
        if( !this.haveInit ) { return true; }

        let output = outputs[0];
        
        // fetch
        this.Module._getGlobalUGenSamples(
            this.myID,
            this.myDacCString,
            this._heapOutputBuffer.getHeapAddress(),
            output[0].length // frame size (probably 128)
        );
        
        // recompute subarray views just in case memory grew recently
        this._heapOutputBuffer.adaptChannel( output.length );
        
        // copy output
        for (let channel = 0; channel < output.length; channel++) 
        {
            output[channel].set(this._heapOutputBuffer.getChannelData(channel));
        }
        
        return true;
    }

}

registerProcessor( 'chuck-node', ChuckNode );
registerProcessor( 'chuck-sub-node', ChuckSubNode );

