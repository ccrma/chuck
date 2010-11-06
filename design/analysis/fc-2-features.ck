Clusterer custard => blackhole;
UAna one => blackhole;

adc => FFT fft;
fft =^ Centroid c =^ one;
fft =^ Flux f =^ one;
fft =^ MFCC m =^ custard;
fft =^ Tilt t =^ custard;

// set window and size
Window.hamming( 512 ) => fft.window;
// set FFT size (implicit zero padding)
2048 => fft.size;

// spork centroid thing
spork ~ centroid_ctrl();

// basic pipeline driver
while( true )
{
    // compute upstream UAna synchronously, such that
    // values can be used immediately, i.e., without
    // advancing time
    one.process(); // TODO: consider alternative funky name
    one.upchuck(); // TODO: consider alternative funky name
    
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
        // this should be the most recent value computed
        c.getFloat() => synthesis_param;
        // get when value was computed
        c.getWhenComputed() => time when;
        // trigger something here!        
    }
}
