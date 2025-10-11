// Paul's Extreme Sound Stretch (Paulstretch) 
// Converted from Python implementation by Nasca Octavian PAUL
// ChucK version by Celeste Betancur Gutierrez


0. => float resolution;
// This could be changed to change fft window size
4096*8 => int windowSize;

// Ensure even window size
(windowSize / 2) * 2 => windowSize;
windowSize / 2 => int halfWindowSize;

SndBuf input => FFT fft => blackhole;  

// Use your own sample here
"special:dope" => input.read;
0 => input.pos;

IFFT ifft1 => Gain output => dac;
IFFT ifft2 => output => dac;

windowSize => fft.size => ifft1.size => ifft2.size;

// Hann window to have a nice Overlap-add :)
(halfWindowSize/2) => int overlapAdd;
Windowing.hann(windowSize) => fft.window => ifft1.window => ifft2.window;
complex freqBuffer[halfWindowSize];

(overlapAdd * (input.sampleRate()) / (second/samp)$float)$int => int playbackHop;

fun void randomizePhase() {

    while (true) {
        
        fft.upchuck();

        for (int i; i < halfWindowSize; i++) {

            (fft.cval(i)$polar).mag => float mag;
            // Generate random phase
            Math.random2f(0.0, 2.0 * Math.PI) => float randomPhase;
            // Reconstruct with random phase
            #(mag * Math.cos(randomPhase), mag * Math.sin(randomPhase)) => freqBuffer[i];
        }
        overlapAdd::samp => now;

        // return the playhead by resolution factor
        (input.pos()) - ((playbackHop - playbackHop*(resolution))$int) => input.pos;
    }
}

fun void reconstruct(){
    while(true){
        // Overlap-add
        ifft1.transform(freqBuffer);
        overlapAdd::samp => now;
        ifft2.transform(freqBuffer);
        overlapAdd::samp => now;
    }
}

spork~randomizePhase();
spork~reconstruct();

eon => now;