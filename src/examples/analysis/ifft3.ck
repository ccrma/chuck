// our patch
SinOsc g => FFT fft =^ IFFT ifft => dac;

// set srate
second / samp => float srate;
// set parameters
1024 => fft.size;

// divide
int div;

// control loop
while( true )
{
    // set freq
    srate / fft.size() * div++ => g.freq;
    fft.size()/2 %=> div;
    
    // pull from ifft
    ifft.upchuck();

    // advance time
    fft.size()::samp => now;
}
