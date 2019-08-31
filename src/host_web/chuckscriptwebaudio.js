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
var programField = document.getElementById( "program" );
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
        
        // respond to messages
        chuckNode.port.onmessage = function( event )
        {
            switch( event.data.type ) 
            {
                case "console print":
                    printToConsole( event.data.message );
                    break;
                default:
                    break;
            }
        }
    }

    // send message to compile code 
    chuckNode.port.postMessage( { type: "compile code", code: programField.value } );
    
});
