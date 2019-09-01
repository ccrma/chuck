var AudioContext = window.AudioContext || window.webkitAudioContext;
var audioContext = undefined;
var chuckNode = undefined;

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
        
        
        await audioContext.audioWorklet.addModule('chucknode.js')
        chuckNode = new AudioWorkletNode( audioContext, 'chuck-node', { 
            numberOfInputs: inChannels,
            numberOfOutputs: outChannels,
            outputChannelCount: outputChannelCount,
            processorOptions: {
                srate: audioContext.sampleRate
            }
        } );
        chuckNode.connect( audioContext.destination );
        
        chuckNode.callbacks = {};
        
        // respond to messages
        chuckNode.port.onmessage = function( event )
        {
            switch( event.data.type ) 
            {
                case "console print":
                    printToConsole( event.data.message );
                    break;
                case "eventCallback":
                    if( event.data.callback in chuckNode.callbacks )
                    {
                        chuckNode.callbacks[event.data.callback]();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // send message to compile code 
    chuckNode.port.postMessage( { type: "runChuckCode", code: chuckEditor.getValue() } );
    
    
    if( window.Event ) { document.captureEvents( Event.MOUSEMOVE ); }
    document.onmousemove = function( e )
    {
        var x = (window.Event) ? e.pageX : event.clientX + (document.documentElement.scrollLeft ? document.documentElement.scrollLeft : document.body.scrollLeft);
	    var y = (window.Event) ? e.pageY : event.clientY + (document.documentElement.scrollTop ? document.documentElement.scrollTop : document.body.scrollTop);
	    chuckNode.port.postMessage( { type: "setChuckInt", variable: "mouseX", value: x } );
	    chuckNode.port.postMessage( { type: "setChuckInt", variable: "mouseY", value: y } );    
    }
    
    document.onclick = function( e )
    {
        chuckNode.port.postMessage( { type: "broadcastChuckEvent", variable: "mouseClicked" } );
    }
    
    var secretx = 0;
    var incrementAndPrint = function() 
    {
        secretx++;
        console.log( "secret x is ", secretx );
        if( secretx > 5 )
        {
//             chuckNode.port.postMessage( { type: "stopListeningForChuckEvent", callback: "incrementAndPrint", variable: "metro" } );
        }
    }
    
    chuckNode.callbacks["incrementAndPrint"] = incrementAndPrint;
//     chuckNode.port.postMessage( { type: "startListeningForChuckEvent", callback: "incrementAndPrint", variable: "metro" } ); 
    chuckNode.port.postMessage( { type: "listenForChuckEventOnce", callback: "incrementAndPrint", variable: "metro" } );
    
    
});
