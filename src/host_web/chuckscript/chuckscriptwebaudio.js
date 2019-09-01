var AudioContext = window.AudioContext || window.webkitAudioContext;
var audioContext = undefined;
var theChuck = undefined;
var theChuckReady = defer();

var printToConsole = (function() {
    var element = document.getElementById('output');
    if (element) element.value = ''; // clear browser cache
    return function(text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
        // These replacements are necessary if you render to raw HTML
        //text = text.replace(/&/g, "&amp;");
        //text = text.replace(/</g, "&lt;");
        //text = text.replace(/>/g, "&gt;");
        //text = text.replace('\n', '<br>', 'g');
        if (element) {
            element.value += text + "\n";
            element.scrollTop = element.scrollHeight; // focus on bottom
        }
    };
})();


var compileButton = document.getElementById( "compileButton" );
compileButton.disabled = false;
compileButton.addEventListener( "click", async function() 
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
        await audioContext.audioWorklet.addModule('./chucknode.js');
        theChuck = new AudioWorkletNode( audioContext, 'chuck-node', { 
            numberOfInputs: inChannels,
            numberOfOutputs: outChannels,
            outputChannelCount: outputChannelCount,
            processorOptions: {
                srate: audioContext.sampleRate
            }
        } );
        
        
        (function( self )
        {
            self.eventCallbacks = {};
            self.intCallbacks = {};
            self.floatCallbacks = {};
            self.stringCallbacks = {};
            self.eventCallbackCounter = 0;
            self.intCallbackCounter = 0;
            self.floatCallbackCounter = 0;
            self.stringCallbackCounter = 0;
            // respond to messages
            self.port.onmessage = function( event )
            {
                switch( event.data.type ) 
                {
                    case "console print":
                        printToConsole( event.data.message );
                        break;
                    case "eventCallback":
                        if( event.data.callback in self.eventCallbacks )
                        {
                            self.eventCallbacks[event.data.callback]();
                        }
                        break;
                    case "intCallback":
                        if( event.data.callback in self.intCallbacks )
                        {
                            self.intCallbacks[event.data.callback](event.data.result);
                        }
                        break;
                    case "floatCallback":
                        if( event.data.callback in self.floatCallbacks )
                        {
                            self.floatCallbacks[event.data.callback](event.data.result);
                        }
                        break;
                    case "stringCallback":
                        if( event.data.callback in self.stringCallbacks )
                        {
                            self.stringCallbacks[event.data.callback](event.data.result);
                        }
                        break;
                    default:
                        break;
                }
            }
            // ================== Run / Compile ================== //
            self.runChuckCode = function( code )
            {
                self.port.postMessage( { type: "runChuckCode", code: code } ); 
            }
            self.runChuckCodeWithReplacementDac = function( code, dac_name )
            {
                self.port.postMessage( { 
                    type: "runChuckCodeWithReplacementDac", 
                    code: code,
                    dac_name: dac_name
                } ); 
            }
            self.runChuckFile = function( filename )
            {
                self.port.postMessage( { type: "runChuckFile", filename: filename } ); 
            }
            self.runChuckFileWithReplacementDac = function( filename, dac_name )
            {
                self.port.postMessage( { 
                    type: "runChuckFileWithReplacementDac", 
                    filename: filename,
                    dac_name: dac_name
                } ); 
            }
            self.runChuckFileWithArgs = function( filename, colon_separated_args )
            {
                self.port.postMessage( { 
                    type: "runChuckFileWithArgs", 
                    filename: filename,
                    colon_separated_args: colon_separated_args
                } );
            }
            self.runChuckFileWithArgsWithReplacementDac = function( filename, colon_separated_args, dac_name )
            {
                self.port.postMessage( { 
                    type: "runChuckFileWithArgs", 
                    filename: filename,
                    colon_separated_args: colon_separated_args,
                    dac_name: dac_name
                } );
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
            self.getInt = function( variable, callback )
            {
                var callbackID = self.intCallbackCounter++;
                self.intCallbacks[callbackID] = callback;
                self.port.postMessage( {
                    type: 'getChuckInt',
                    variable: variable,
                    callback: callbackID
                } );
            }
            self.setFloat = function( variable, value )
            {
                self.port.postMessage( { 
                    type: 'setChuckFloat', 
                    variable: variable,
                    value: value
                } );
            }
            self.getFloat = function( variable, callback )
            {
                var callbackID = self.floatCallbackCounter++;
                self.floatCallbacks[callbackID] = callback;
                self.port.postMessage( {
                    type: 'getChuckFloat',
                    variable: variable,
                    callback: callbackID
                } );
            }
            self.setString = function( variable, value )
            {
                self.port.postMessage( { 
                    type: 'setChuckString', 
                    variable: variable,
                    value: value
                } );
            }
            self.getString = function( variable, callback )
            {
                var callbackID = self.stringCallbackCounter++;
                self.stringCallbacks[callbackID] = callback;
                self.port.postMessage( {
                    type: 'getChuckString',
                    variable: variable,
                    callback: callbackID
                } );
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
            }
            self.stopListeningForEvent = function( variable, callback )
            {
                var callbackID = self.eventCallbackCounter++;
                self.eventCallbacks[callbackID] = callback;
                self.port.postMessage( { 
                    type: 'stopListeningForChuckEvent',
                    variable: variable,
                    callback: callbackID
                } );
            }
            // ================== Int[] =================== //
            self.setIntArray = undefined;
            self.getIntArray = undefined;
            self.setIntArrayValue = function( variable, index, value )
            {
                self.port.postMessage( {
                    type: 'setGlobalIntArrayValue',
                    variable: variable,
                    index: index,
                    value: value
                } );
            }
            self.getIntArrayValue = function( variable, index, callback )
            {
                var callbackID = self.intCallbackCounter++;
                self.intCallbacks[callbackID] = callback;
                self.port.postMessage( {
                    type: 'getGlobalIntArrayValue',
                    variable: variable,
                    index: index,
                    callback: callbackID
                } );
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
            self.getAssociativeIntArrayValue = function( variable, key, callback )
            {
                var callbackID = self.intCallbackCounter++;
                self.intCallbacks[callbackID] = callback;
                self.port.postMessage( {
                    type: 'getGlobalAssociativeIntArrayValue',
                    variable: variable,
                    key: key,
                    callback: callbackID
                } );
            }
            // ================== Float[] =================== //
            self.setFloatArray = undefined;
            self.getFloatArray = undefined;
            self.setFloatArrayValue = function( variable, index, value )
            {
                self.port.postMessage( {
                    type: 'setGlobalFloatArrayValue',
                    variable: variable,
                    index: index,
                    value: value
                } );
            }
            self.getFloatArrayValue = function( variable, index, callback )
            {
                var callbackID = self.floatCallbackCounter++;
                self.floatCallbacks[callbackID] = callback;
                self.port.postMessage( {
                    type: 'getGlobalFloatArrayValue',
                    variable: variable,
                    index: index,
                    callback: callbackID
                } );
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
            self.getAssociativeFloatArrayValue = function( variable, key, callback )
            {
                var callbackID = self.floatCallbackCounter++;
                self.floatCallbacks[callbackID] = callback;
                self.port.postMessage( {
                    type: 'getGlobalAssociativeFloatArrayValue',
                    variable: variable,
                    key: key,
                    callback: callbackID
                } );
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
    }

    // send message to compile code 
    theChuck.runChuckCode( chuckEditor.getValue() );
    
    theChuckReady.resolve();
});
