//--------------------------------------------------------
// name: paulstretch.ck
// desc: Paul's Extreme Sound Stretch (Paulstretch) 
//       (requires: chuck-1.5.5.6 or higher)
//
// author: Celeste Betancur Gutierrez
//         adapted from Paul Nasca's Python implementation
// date: Fall 2025
//--------------------------------------------------------

//--------------------------------------------------------
// this is the stretch factor
// e.g., .05 is 20x time-stretch
// 1 is normal/no stretch
// 0 is freeze
.05 => float resolution;
//--------------------------------------------------------
// FFT window size
// larger == better quality but more bursty CPU load
4096*4 => int windowSize;

// ensure even window size
(windowSize / 2) * 2 => windowSize;
windowSize / 2 => int halfWindowSize;

// preview
SndBuf input => dac;
// feel free to use your own sample here
"special:dope" => input.read;
// print
cherr <= "DOH" <= IO.nl();
// play it without stretching
input.length() => now;
// disconnect preview patch
input =< dac;
// wait a bit
.5::second => now;

// audio analysis
input => FFT fft => blackhole;
// playhead
0 => input.pos;
// loop it
1 => input.loop;


// audio synthesis
IFFT ifft1 => Gain output => dac;
IFFT ifft2 => output => dac;

// set window size
windowSize => fft.size => ifft1.size => ifft2.size;

// Hann window to have a nice Overlap-add :)
(halfWindowSize/2) => int overlapAdd;
Windowing.hann(windowSize) => fft.window => ifft1.window => ifft2.window;
// complex frequency bins
complex freqBuffer[halfWindowSize];
// compute playback hop size
(overlapAdd * (input.sampleRate()) / (second/samp)$float)$int => int playbackHop;

// shred for randomize phase
fun void randomizePhase()
{
    while( true )
    {
        // trigger ananlysis up the UAna chain from fft
        fft.upchuck();
        
        // iterate over frequency bins
        for (int i; i < halfWindowSize; i++)
        {
            // compute magnitude
            (fft.cval(i)$polar).mag => float mag;
            // generate random phase
            Math.random2f(-Math.PI, Math.PI) => float randomPhase;
            // reconstruct with random phase
            #(mag * Math.cos(randomPhase), mag * Math.sin(randomPhase)) => freqBuffer[i];
        }
        // advance time by overlap-add duration
        overlapAdd::samp => now;

        // return the playhead by resolution factor
        (input.pos()) - ((playbackHop - playbackHop*(resolution))$int) => input.pos;
    }
}

// shred for reconstructing the audio
fun void reconstruct()
{
    while( true )
    {
        // overlap-add
        ifft1.transform( freqBuffer );
        overlapAdd::samp => now;
        ifft2.transform( freqBuffer );
        overlapAdd::samp => now;
    }
}

fun void transcribe()
{
    100::ms => dur T;
    while( true )
    {
        // the beginning
        if( input.pos() < 5 ) cherr <= "D";
        // this does not work as intended but works in effect
        else if( input.samples() - input.pos() < T/samp ) cherr <= "H";
        else cherr <= "O";
        T => now;
    }
}

// go
spork~randomizePhase();
spork~reconstruct();
spork~transcribe();

// advance time
while( true ) 1::second => now;
