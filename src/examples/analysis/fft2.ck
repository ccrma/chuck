// fft2.ck: yet another way to FFT, by getting array out of blob
// author: Rebecca Fiebrink and Ge Wang

// our patch
SinOsc g => FFT fft => blackhole;

// set srate
second / samp => float srate;
// set parameters
8 => fft.size;

// divide
int div;
// array ref
complex c[];

// control loop
while( true )
{
    // set freq
    srate / fft.size() * div++ => g.freq;
    fft.size()/2 %=> div;
    
    // take fft
    fft.upchuck().cvals() @=> c;
    // examine contents
    <<< c[0]$polar, c[1]$polar, c[2]$polar, c[3]$polar >>>;

    // advance time
    100::ms => now;
}
