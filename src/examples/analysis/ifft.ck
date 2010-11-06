// our patch
SinOsc g => FFT fft =^ IFFT ifft => dac;

// set parameters
1024 => fft.size;
440 => g.freq;

// control loop
while( true )
{
    // take fft then ifft
    ifft.upchuck();

    // advance time
    fft.size()::samp => now;
}
