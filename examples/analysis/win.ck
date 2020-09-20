// our patch
SinOsc g => FFT fft => blackhole;
// synthesis
IFFT ifft => dac;
// set srate
second / samp => float srate;

// set parameters
1024 => fft.size;
// window
Windowing.hamming(512) => fft.window;
Windowing.hamming(512) => ifft.window;

// use this to hold contents
complex s[fft.size()/2];
// divide
int div;

// control loop
while( true )
{
    // set freq
    srate / fft.size() * div++ => g.freq;
    fft.size()/2 %=> div;
    
    // take fft
    fft.upchuck();
    // get contents
    fft.spectrum( s );
    // take ifft
    ifft.transform( s );

    // advance time
    256::samp => now;
}
