// fft3.ck: yet another way to get FFT spectrum - without blobs
// authors: Rebecca Fiebrink and Ge Wang

// our patch
SinOsc g => FFT fft => blackhole;
// set srate
second / samp => float srate;

// set parameters
8 => fft.size;

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
    // examine contents
    <<< s[0]$polar, s[1]$polar, s[2]$polar, s[3]$polar >>>;

    // advance time
    100::ms => now;
}
