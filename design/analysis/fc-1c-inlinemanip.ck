Noise n => FFT fft =^ IFFT ifft => dac;

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
    fft.upchuck();

    // somehow get the infoblob from fft?
    // operate on it, probably in place

    // take IFFT
    ifft.upchuck();

    // advance time (implicit hop size)
    128::samp => now;
}
