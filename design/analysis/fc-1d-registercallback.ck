Noise n => FFT fft =^ @(handleStuff) =^ IFFT ifft => dac;

// set window and size
Window.hamming(512) => fft.window;
// set FFT size (implicit zero padding)
2048 => fft.size;

// set window and size
Window.hamming(512) => ifft.window;

// go
while( true )
{
    // take FFT
    fft.transform();
    
    // advance time (implicit hop size)
    128::samp => now;
}

fun void handleStuff( Stream stream )
{
    
}