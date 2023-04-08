// our UGen/UAna patch
adc => FFT fft =^ Flux flux => blackhole;

// set fft size
1024 => fft.size;
// set window (optional here)
Windowing.hann( 1024 ) => fft.window;

// infinite time loop
while( true )
{
    // propogate the analysis computations
    flux.upchuck();
    // print it (should always be 0)
    <<< flux.fval(0) >>>;

    // hop along
    512::samp => now;
}
