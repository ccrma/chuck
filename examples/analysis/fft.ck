// fft.ck: taking FFT via upchcuk, printing the spectrum
// authors: Rebecca Fiebrink and Ge Wang

// our patch
SinOsc g => FFT fft => blackhole;

// set srate
second / samp => float srate;
// set parameters
8 => fft.size;
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
    // examine contents
    <<< fft.cval(0)$polar, fft.cval(1)$polar,
        fft.cval(2)$polar, fft.cval(3)$polar >>>;

    // advance time
    100::ms => now;
}
