// our UGen/UAna patch
SinOsc s => FFT fft =^ Flux flux => blackhole;

// compute sample rate
second / samp => float srate;
// set a nice frequency to minimize flux across frames
srate / 8 => s.freq;
// set fft size
1024 => fft.size;
// set window (optional here)
Windowing.hann( 1024 ) => fft.window;

// infinite time loop
while( true )
{
    // propogate the analysis computations (example: optional usage of blob)
    flux.upchuck() @=> UAnaBlob blob;
    // print it (should always be 0)
    <<< blob.fval(0) >>>;

    // hop along
    512::samp => now;
}
