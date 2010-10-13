// our patch
adc => FFT fft =^ RMS rms => blackhole;

// set parameters
1024 => fft.size;
// set hann window
Windowing.hann(1024) => fft.window;

// control loop
while( true )
{
    // upchuck: take fft then rms
    rms.upchuck() @=> UAnaBlob blob;
    // print out RMS
    <<< blob.fval(0) >>>;

    // advance time
    fft.size()::samp => now;
}
