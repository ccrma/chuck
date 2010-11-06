adc => FFT fft => Centroid c => Flux f => MFCC m => Clusterer custard => blackhole;

// set window
( hamming, 512 ) => fft.window;
// zero padding
512 => fft.padding;
// name for enqueuing onto info-stream
"joanne" => fft.signature;

while( true )
{
    // manual
    fft.go();
    c.go();
    f.go();
    m.go();
    
    custard.eat();
    
    // hop size
    256::samp => now;
}