var AudioContext = window.AudioContext || window.webkitAudioContext;
var audioContext = undefined;
var chuckNode = undefined;


var compileButton = document.getElementById( "compileButton" );
var programField = document.getElementById( "program" );
compileButton.disabled = false;
compileButton.addEventListener( "click", async function() 
{
    if( audioContext === undefined )
    {
        audioContext = new AudioContext();
        var inChannels = 1; // TODO: 1? 0?
        var outChannels = 1; // TODO: 2?
        await audioContext.audioWorklet.addModule('chucknode.js')
        // TODO: what is "number of outputs" and why is it different than "outputChannelCount"? 
        // TODO: how to pass the chuck object?
        chuckNode = new AudioWorkletNode( audioContext, 'chuck-node', { 
            numberOfInputs: inChannels,
            numberOfOutputs: outChannels,
            processorOptions: {
                srate: audioContext.sampleRate
            }
        } );
        chuckNode.connect( audioContext.destination );
    }

    // TODO: send message
    chuckNode.port.postMessage( { code: programField.value } );
    
});
