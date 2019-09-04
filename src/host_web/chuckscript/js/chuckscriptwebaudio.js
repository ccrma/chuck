var AudioContext = window.AudioContext || window.webkitAudioContext;
var audioContext = undefined;
var theChuck = undefined;
var theChuckReady = defer();
var theChuckModule = undefined;

var chuckPrint = function( text )
{
    // override me!
}

var startChuck = async function( whereIsChuck ) 
{
    if( audioContext === undefined )
    {
        audioContext = new AudioContext();
        var inChannels = 2; // TODO: 1? 0?
        var outChannels = 2; // TODO: 2?
        // TODO: what is "outputChannelCount"? 
        // TODO: how to get stereo audio? Even if I only fill the first channel of audio,
        // it still fills both speakers equally.
        // (I can tell by printing out the outputs object that ChucK is filling
        //  the different channels correctly.)
        // one channel used per output
        // note: if this is [2,2] then it mixes down both channels to fill the
        // left channel. so, I think [1,1] is correct. but I am still left
        // wondering how to avoid this node getting mixed to mono before
        // being played.
        var outputChannelCount = (new Array( outChannels )).fill( 1 );
        
        
        // TODO: for some reason, this fails if the HTML file is not in the same directory
        // as this file.
        await audioContext.audioWorklet.addModule( whereIsChuck + '/chucknode.js');
        theChuck = new AudioWorkletNode( audioContext, 'chuck-node', { 
            numberOfInputs: inChannels,
            numberOfOutputs: outChannels,
            outputChannelCount: outputChannelCount,
            processorOptions: {
                srate: audioContext.sampleRate,
                Module: theChuckModule
            }
        } );
        
        
        (function( self )
        {
            self.eventCallbacks = {};
            self.deferredPromises = {};
            self.eventCallbackCounter = 0;
            self.deferredPromiseCounter = 0;
            // respond to messages
            self.port.onmessage = function( event )
            {
                switch( event.data.type ) 
                {
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
                return callbackID
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
            
        })( theChuck );
        
        theChuck.connect( audioContext.destination );
        theChuckReady.resolve();
    }
};
