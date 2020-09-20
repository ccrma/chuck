adc => FFT fft => Centroid c => Flux f => MFCC m => Clusterer custard => blackhole;
adc => FFT fft2 => Tilt t => m;

// something that takes more than one audio input
// option A
adc.left => StereoCorr sc => suck;
adc.right => sc => suck;
// option B
StereoCorr sc => suck;
adc.left => sc.left;
adc.right => sc.right;

// set window
( hamming, 512 ) => fft.window;
// zero padding
512 => fft.padding;

spork ~ centroid_ctrl();

// basic pipeline driver
while( true )
{
    // take fft
    fft.go();
    
    // get flux, down the line, should be computed
    // for most recent fft frame
    f.output() => result;
    
    // hop size
    256::samp => now;
}

// mapping
fun void centroid_ctrl()
{
    while( true )
    {
        // wait for c to do someting
        c.event() => now;
        // when it does, get result (we know as float)
        c.getFloat() => synthesis_param;
        // trigger something here!        
    }
}
