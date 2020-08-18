// fft1.ck: using uana blobs!
// authors: Rebecca Fiebrink and Ge Wang

// our patch
SinOsc g => FFT fft => blackhole;

// set srate
second / samp => float srate;
// set parameters
8 => fft.size;

// divide
int div;
// the blob
UAnaBlob blob;

// control loop
while( true )
{
    // set freq
    srate / fft.size() * div++ => g.freq;
    fft.size()/2 %=> div;
    
    // take fft
    fft.upchuck() @=> blob;
    // examine contents
    <<< blob.cval(0)$polar, blob.cval(1)$polar,
        blob.cval(2)$polar, blob.cval(3)$polar >>>;

    // advance time
    100::ms => now;
}
