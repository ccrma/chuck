Noise n => FFT fft =^ UAnaOp oper =^ IFFT ifft => dac;

// do multiply
3 => oper.op;
// something to multiply
Spectrum spec => oper;

// set the spec to arbitrary complex buffer
spec.set( rspectrum );

// set window and size
Window.hamming(512) => fft.window;
// set FFT size (implicit zero padding)
2048 => fft.size;

// set window and size
Window.hamming(512) => ifft.window;

// go
while( true )
{
    // process ifft, overlap add result to buffer
    // will return samples to UGen's downstream, possibly
    // 0's if ifft not recently taken and data has been
    // consumed...
    ifft.upchuck();
    
    // advance time (implicit hop size)
    128::samp => now;
}
