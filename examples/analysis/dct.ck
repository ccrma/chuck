// dct.ck: taking DCT via upchcuk, printing the spectrum
// authors: Rebecca Fiebrink and Ge Wang

// our patch
SinOsc g => DCT dct => blackhole;

// set srate
second / samp => float srate;
// set parameters
8 => dct.size;
// divide
int div;

// control loop
while( true )
{
    // set freq
    srate / dct.size() * div++ => g.freq;
    dct.size()/2 %=> div;
    
    // take dct
    dct.upchuck();
    // examine contents
    <<< dct.fval(0), dct.fval(1), dct.fval(2), dct.fval(3) >>>;

    // advance time
    100::ms => now;
}
