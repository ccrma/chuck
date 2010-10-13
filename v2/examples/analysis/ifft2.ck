// our patch
SinOsc g => FFT fft => blackhole;
// synthesize
IFFT ifft => dac;

// set parameters
1024 => fft.size;
440 => g.freq;

// use this to hold contents
complex s[fft.size()/2];

// control loop
while( true )
{
    // take fft
    fft.upchuck();
    // get contents
    fft.spectrum( s );
    // take ifft
    ifft.transform( s );

    // advance time
    fft.size()::samp => now;
}
