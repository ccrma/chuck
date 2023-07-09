var AudioContext = window.AudioContext || window.webkitAudioContext;
var audioContext = undefined;
var theChuck = undefined;
var theChuckAlmostReady = defer();
var theChuckReady = defer();
var filesToPreload = [];
var whereIsChuck = whereIsChuck || "./js";
var currentChuckID = 1;

var chuckPrint = function( text )
{
    // override me!
}


// START taken from emscripten source
var readAsync = function( url, onload, onerror )
{
    var xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.responseType = 'arraybuffer';
    xhr.onload = function xhr_onload() 
    {
        if (xhr.status == 200 || (xhr.status == 0 && xhr.response)) // file URLs can return 0
        { 
            onload(xhr.response);
            return;
        }
        onerror();
    };
    xhr.onerror = onerror;
    xhr.send(null);
};

var asyncLoadFile = function( url, onload, onerror ) 
{
    readAsync(url, function(arrayBuffer) {
        onload(new Uint8Array(arrayBuffer));
    }, function(event) {
        if (onerror) {
            onerror();
        } else {
            throw 'Loading data file "' + url + '" failed.';
        }
    });
}
// END taken from emscripten source

var loadWasm = new Promise( function( resolve, reject )
{
    asyncLoadFile( whereIsChuck + '/webchuck.wasm', resolve, reject ); 
});

var preloadFilenames = async function( filenamesToPreload )
{
    var promises = [];
    for( var i = 0; i < filenamesToPreload.length; i++ )
    {
        (function( filenameToPreload )
        {
            promises.push( new Promise( function( resolve, reject )
            {
                asyncLoadFile( filenameToPreload.serverFilename, function( byteArray )
                {
                    filesToPreload.push({
                        filename: filenameToPreload.virtualFilename,
                        data: byteArray
                    });
                    resolve();
                }, function()
                {
                    console.error( "Error fetching file:", filenameToPreload.serverFilename );
                    //reject( e );
                    resolve();
                });
            }) );
        })( filenamesToPreload[i] );
        
    }
    await Promise.all( promises );
}

var theWasm;
var startAudioContext = async function()
{
    theWasm = await loadWasm;
    audioContext = new AudioContext({
        //sampleRate: 48000
    });
    await audioContext.audioWorklet.addModule( whereIsChuck + '/webchuck.js');
}

var startChuck = async function() 
{
    if( audioContext === undefined )
    {
        await startAudioContext();
        
        var newID = currentChuckID;
        currentChuckID++;
        
        theChuck = await createAChuck( newID, theChuckReady );
        theChuck.connect( audioContext.destination );
        theChuckAlmostReady.resolve();
    }
};

var createAChuck = function( chuckID, initPromise )
{
    // important: "number of inputs / outputs" is like an aggregate source
    // most of the time, you only want one input source and one output
    // source, but each one has multiple channels
    var numInOutChannels = 2;
    
    var aChuck = new AudioWorkletNode( audioContext, 'chuck-node', { 
        numberOfInputs: 1,
        numberOfOutputs: 1,
        outputChannelCount: [numInOutChannels],
        processorOptions: {
            chuckID: chuckID,
            srate: audioContext.sampleRate,
            preloadedFiles: filesToPreload,
            wasm: theWasm
        }
    } );
    
    aChuck.chuckID = chuckID;
    
    (function( self )
    {
        self.eventCallbacks = {};
        self.deferredPromises = {};
        self.eventCallbackCounter = 0;
        self.deferredPromiseCounter = 0;
        self.amReady = initPromise;
        
        self.onprocessorerror = function( e )
        {
            console.log( e );
        }
        
        // respond to messages
        self.port.onmessage = function( event )
        {
            switch( event.data.type ) 
            {
                case "initCallback":
                    if( self.amReady )
                    {
                        self.amReady.resolve();
                    }
                    break;
                case "console print":
                    chuckPrint( event.data.message );
                    break;
                case "eventCallback":
                    if( event.data.callback in self.eventCallbacks )
                    {
                        self.eventCallbacks[event.data.callback]();
                    }
                    break;
                case "intCallback":
                case "floatCallback":
                case "stringCallback":
                case "intArrayCallback":
                case "floatArrayCallback":
                    if( event.data.callback in self.deferredPromises )
                    {
                        self.deferredPromises[event.data.callback].resolve(event.data.result);
                        delete self.deferredPromises[event.data.callback];
                    }
                    break;
                case "newShredCallback":
                    if( event.data.callback in self.deferredPromises )
                    {
                        if( event.data.shred > 0 )
                        {
                            self.deferredPromises[event.data.callback].resolve(event.data.shred);
                        }
                        else
                        {
                            self.deferredPromises[event.data.callback].reject("Running code failed");
                        }
                    }
                    break;
                case "replacedShredCallback":
                    if( event.data.callback in self.deferredPromises )
                    {
                        if( event.data.newShred > 0 )
                        {
                            self.deferredPromises[event.data.callback].resolve({
                                newShred: event.data.newShred,
                                oldShred: event.data.oldShred
                            });
                        }
                        else
                        {
                            self.deferredPromises[event.data.callback].reject("Replacing code failed");
                        }
                    }
                    break;
                case "removedShredCallback":
                    if( event.data.callback in self.deferredPromises )
                    {
                        if( event.data.shred > 0 )
                        {
                            self.deferredPromises[event.data.callback].resolve(event.data.shred);
                        }
                        else
                        {
                            self.deferredPromises[event.data.callback].reject("Removing code failed");
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        self.nextDeferID = function()
        {
            var callbackID = self.deferredPromiseCounter++;
            self.deferredPromises[callbackID] = defer();
            return callbackID;
        }
        // ================== Filesystem ===================== //
        self.createFile = function( directory, filename, data )
        {
            self.port.postMessage( {
                type: 'loadFile',
                directory: directory,
                filename: filename,
                data: data
            } );
        }
        // ================== Run / Compile ================== //
        self.runCode = function( code )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( { 
                type: "runChuckCode", 
                code: code,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.runCodeWithReplacementDac = function( code, dac_name )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( { 
                type: "runChuckCodeWithReplacementDac", 
                code: code,
                dac_name: dac_name,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.runFile = function( filename )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: "runChuckFile",
                filename: filename,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.runFileWithReplacementDac = function( filename, dac_name )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( { 
                type: "runChuckFileWithReplacementDac", 
                filename: filename,
                dac_name: dac_name,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.runFileWithArgs = function( filename, colon_separated_args )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( { 
                type: "runChuckFileWithArgs", 
                filename: filename,
                colon_separated_args: colon_separated_args,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.runFileWithArgsWithReplacementDac = function( filename, colon_separated_args, dac_name )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( { 
                type: "runChuckFileWithArgs", 
                filename: filename,
                colon_separated_args: colon_separated_args,
                dac_name: dac_name,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.replaceCode = function( code )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: "replaceChuckCode",
                code: code,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.replaceCodeWithReplacementDac = function( code, dac_name )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( { 
                type: "replaceChuckCodeWithReplacementDac", 
                code: code,
                dac_name: dac_name,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.replaceFile = function( filename )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: "replaceChuckFile",
                filename: filename,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.replaceFileWithReplacementDac = function( filename, dac_name )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( { 
                type: "replaceChuckFileWithReplacementDac", 
                filename: filename,
                dac_name: dac_name,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.replaceFileWithArgs = function( filename, colon_separated_args )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( { 
                type: "replaceChuckFileWithArgs", 
                filename: filename,
                colon_separated_args: colon_separated_args,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.replaceFileWithArgsWithReplacementDac = function( filename, colon_separated_args, dac_name )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( { 
                type: "replaceChuckFileWithArgs", 
                filename: filename,
                colon_separated_args: colon_separated_args,
                dac_name: dac_name,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.removeLastCode = function()
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: "removeLastCode",
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.removeShred = function( shred )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: "removeShred",
                shred: shred,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.isShredActive = function( shred )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: "isShredActive",
                shred: shred,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        // ================== Int, Float, String ============= //
        self.setInt = function( variable, value )
        {
            self.port.postMessage( { 
                type: 'setChuckInt', 
                variable: variable,
                value: value
            } );
        }
        self.getInt = function( variable )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: 'getChuckInt',
                variable: variable,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.setFloat = function( variable, value )
        {
            self.port.postMessage( { 
                type: 'setChuckFloat', 
                variable: variable,
                value: value
            } );
        }
        self.getFloat = function( variable )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: 'getChuckFloat',
                variable: variable,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.setString = function( variable, value )
        {
            self.port.postMessage( { 
                type: 'setChuckString', 
                variable: variable,
                value: value
            } );
        }
        self.getString = function( variable )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: 'getChuckString',
                variable: variable,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        // ================== Event =================== //
        self.signalEvent = function( variable )
        {
            self.port.postMessage( { type: 'signalChuckEvent', variable: variable } );
        }
        self.broadcastEvent = function( variable )
        {
            self.port.postMessage( { type: 'broadcastChuckEvent', variable: variable } );
        }
        self.listenForEventOnce = function( variable, callback )
        {
            var callbackID = self.eventCallbackCounter++;
            self.eventCallbacks[callbackID] = callback;
            self.port.postMessage( { 
                type: 'listenForChuckEventOnce',
                variable: variable,
                callback: callbackID
            } );
        }
        self.startListeningForEvent = function( variable, callback )
        {
            var callbackID = self.eventCallbackCounter++;
            self.eventCallbacks[callbackID] = callback;
            self.port.postMessage( { 
                type: 'startListeningForChuckEvent',
                variable: variable,
                callback: callbackID
            } );
            return callbackID;
        }
        self.stopListeningForEvent = function( variable, callbackID )
        {
            self.port.postMessage( { 
                type: 'stopListeningForChuckEvent',
                variable: variable,
                callback: callbackID
            } );
        }
        // ================== Int[] =================== //
        self.setIntArray = function( variable, values )
        {
            self.port.postMessage( {
                type: 'setGlobalIntArray',
                variable: variable,
                values: values
            } );
        }
        self.getIntArray = function( variable )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: 'getGlobalIntArray',
                variable: variable,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.setIntArrayValue = function( variable, index, value )
        {
            self.port.postMessage( {
                type: 'setGlobalIntArrayValue',
                variable: variable,
                index: index,
                value: value
            } );
        }
        self.getIntArrayValue = function( variable, index )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: 'getGlobalIntArrayValue',
                variable: variable,
                index: index,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.setAssociativeIntArrayValue = function( variable, key, value )
        {
            self.port.postMessage( {
                type: 'setGlobalAssociativeIntArrayValue',
                variable: variable,
                key: key,
                value: value
            } );
        }
        self.getAssociativeIntArrayValue = function( variable, key )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: 'getGlobalAssociativeIntArrayValue',
                variable: variable,
                key: key,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        // ================== Float[] =================== //
        self.setFloatArray = function( variable, values )
        {
            self.port.postMessage( {
                type: 'setGlobalFloatArray',
                variable: variable,
                values: values
            } );
        }
        self.getFloatArray = function( variable )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: 'getGlobalFloatArray',
                variable: variable,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        };
        self.setFloatArrayValue = function( variable, index, value )
        {
            self.port.postMessage( {
                type: 'setGlobalFloatArrayValue',
                variable: variable,
                index: index,
                value: value
            } );
        }
        self.getFloatArrayValue = function( variable, index )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: 'getGlobalFloatArrayValue',
                variable: variable,
                index: index,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        self.setAssociativeFloatArrayValue = function( variable, key, value )
        {
            self.port.postMessage( {
                type: 'setGlobalAssociativeFloatArrayValue',
                variable: variable,
                key: key,
                value: value
            } );
        }
        self.getAssociativeFloatArrayValue = function( variable, key )
        {
            var callbackID = self.nextDeferID();
            self.port.postMessage( {
                type: 'getGlobalAssociativeFloatArrayValue',
                variable: variable,
                key: key,
                callback: callbackID
            } );
            return self.deferredPromises[callbackID];
        }
        // ================= Clear ====================== //
        self.clearChuckInstance = function()
        {
            self.port.postMessage( { type: 'clearChuckInstance' } );
        }
        self.clearGlobals = function()
        {
            self.port.postMessage( { type: 'clearGlobals' } );
        }
        
    })( aChuck );
    
    return aChuck;
}

var createASubChuck = function( chuck, dacName, initPromise )
{
    var aSubChuck = new AudioWorkletNode( audioContext, 'chuck-sub-node',
    { 
        numberOfInputs: 1,
        numberOfOutputs: 1,
        outputChannelCount: [1],
        processorOptions: {
            chuckID: chuck.chuckID,
            dac: dacName
        }
    } );
    
    (function( self )
    {
        self.myChuck = chuck;
        self.dac = dacName;
        self.amReady = initPromise;
        
        // respond to messages
        self.port.onmessage = function( event )
        {
            switch( event.data.type ) 
            {
                case "initCallback":
                    if( self.amReady )
                    {
                        self.amReady.resolve();
                    }
                    break;
                default:
                    break;
            }
        }
        // ================== Filesystem ===================== //
        self.createFile = function( directory, filename, data )
        {
            return self.myChuck.createFile( directory, filename, data );
        }
        // ================== Run / Compile ================== //
        self.runCode = function( code )
        {
            // N.B. self.dac
            return self.myChuck.runCodeWithReplacementDac( code, self.dac );
        }
        self.runCodeWithReplacementDac = function( code, dac_name )
        {
            return self.myChuck.runCodeWithReplacementDac( code, dac_name );
        }
        self.runFile = function( filename )
        {
            // N.B. self.dac
            return self.myChuck.runFileWithReplacementDac( filename, self.dac );
        }
        self.runFileWithReplacementDac = function( filename, dac_name )
        {
            return self.myChuck.runFileWithReplacementDac( filename, dac_name );
        }
        self.runFileWithArgs = function( filename, colon_separated_args )
        {
            // N.B. self.dac
            return self.myChuck.runFileWithArgsWithReplacementDac( filename, colon_separated_args, self.dac );
        }
        self.runFileWithArgsWithReplacementDac = function( filename, colon_separated_args, dac_name )
        {
            return self.myChuck.runFileWithArgsWithReplacementDac( filename, colon_separated_args, dac_name );
        }
        self.replaceCode = function( code )
        {
            // N.B. self.dac
            return self.myChuck.replaceCodeWithReplacementDac( code, self.dac );
        }
        self.replaceCodeWithReplacementDac = function( code, dac_name )
        {
            return self.myChuck.replaceCodeWithReplacementDac( code, dac_name );
        }
        self.replaceFile = function( filename )
        {
            // N.B. self.dac
            return self.myChuck.replaceFileWithReplacementDac( filename, self.dac );
        }
        self.replaceFileWithReplacementDac = function( filename, dac_name )
        {
            return self.myChuck.replaceFileWithReplacementDac( filename, dac_name );
        }
        self.replaceFileWithArgs = function( filename, colon_separated_args )
        {
            // N.B. self.dac
            return self.myChuck.replaceFileWithArgsWithReplacementDac( filename, colon_separated_args, self.dac );
        }
        self.replaceFileWithArgsWithReplacementDac = function( filename, colon_separated_args, dac_name )
        {
            return self.myChuck.replaceFileWithArgsWithReplacementDac( filename, colon_separated_args, dac_name );
        }
        self.removeLastCode = function()
        {
            return self.myChuck.removeLastCode();
        }
        self.removeShred = function( shred )
        {
            return self.myChuck.removeShred( shred );
        }
        self.isShredActive = function( shred )
        {
            return self.myChuck.isShredActive( shred );
        }
        // ================== Int, Float, String ============= //
        self.setInt = function( variable, value )
        {
            return self.myChuck.setInt( variable, value );
        }
        self.getInt = function( variable )
        {
            return self.myChuck.getInt( variable );
        }
        self.setFloat = function( variable, value )
        {
            return self.myChuck.setFloat( variable, value );
        }
        self.getFloat = function( variable )
        {
            return self.myChuck.getFloat( variable );
        }
        self.setString = function( variable, value )
        {
            return self.myChuck.setString( variable, value );
        }
        self.getString = function( variable )
        {
            return self.myChuck.getString( variable );
        }
        // ================== Event =================== //
        self.signalEvent = function( variable )
        {
            return self.myChuck.signalEvent( variable );
        }
        self.broadcastEvent = function( variable )
        {
            return self.myChuck.broadcastEvent( variable );
        }
        self.listenForEventOnce = function( variable, callback )
        {
            return self.myChuck.listenForEventOnce( variable, callback );
        }
        self.startListeningForEvent = function( variable, callback )
        {
            return self.myChuck.startListeningForEvent( variable, callback );
        }
        self.stopListeningForEvent = function( variable, callbackID )
        {
            return self.myChuck.stopListeningForEvent( variable, callbackID );
        }
        // ================== Int[] =================== //
        self.setIntArray = function( variable, values )
        {
            return self.myChuck.setIntArray( variable, values );
        }
        self.getIntArray = function( variable )
        {
            return self.myChuck.getIntArray( variable );
        }
        self.setIntArrayValue = function( variable, index, value )
        {
            return self.myChuck.setIntArrayValue( variable, index, value );
        }
        self.getIntArrayValue = function( variable, index )
        {
            return self.myChuck.getIntArrayValue( variable, index );
        }
        self.setAssociativeIntArrayValue = function( variable, key, value )
        {
            return self.myChuck.setAssociativeIntArrayValue( variable, key, value );
        }
        self.getAssociativeIntArrayValue = function( variable, key )
        {
            return self.myChuck.getAssociativeIntArrayValue( variable, key );
        }
        // ================== Float[] =================== //
        self.setFloatArray = function( variable, values )
        {
            return self.myChuck.setFloatArray( variable, values );
        }
        self.getFloatArray = function( variable )
        {
            return self.myChuck.getFloatArray( variable );
        };
        self.setFloatArrayValue = function( variable, index, value )
        {
            return self.myChuck.setFloatArrayValue( variable, index, value );
        }
        self.getFloatArrayValue = function( variable, index )
        {
            return self.myChuck.getFloatArrayValue( variable, index );
        }
        self.setAssociativeFloatArrayValue = function( variable, key, value )
        {
            return self.myChuck.setAssociativeFloatArrayValue( variable, key, value );
        }
        self.getAssociativeFloatArrayValue = function( variable, key )
        {
            return self.myChuck.getAssociativeFloatArrayValue( variable, key );
        }
        // ================= Clear ====================== //
        self.clearChuckInstance = function()
        {
            return self.myChuck.clearChuckInstance();
        }
        self.clearGlobals = function()
        {
            return self.myChuck.clearGlobals();
        }
        
        
    })( aSubChuck );
    
    return aSubChuck;
}


