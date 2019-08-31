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
        var inChannels = 2; // TODO: 1? 0?
        var outChannels = 2; // TODO: 2?
        await audioContext.audioWorklet.addModule('chucknode.js')
        // TODO: what is "number of outputs" and why is it different than "outputChannelCount"? 
        // TODO: how to get stereo audio? Even if I only fill the first channel of audio,
        // it still fills both speakers equally.
        // (I can tell by printing out the outputs object that ChucK is filling
        //  the different channels correctly.)
        chuckNode = new AudioWorkletNode( audioContext, 'chuck-node', { 
            numberOfInputs: inChannels,
            numberOfOutputs: outChannels,
            processorOptions: {
                srate: audioContext.sampleRate
            }
        } );
        chuckNode.connect( audioContext.destination );
    }

    // send message to compile code 
    chuckNode.port.postMessage( { type: "compile code", code: programField.value } );
    
});
