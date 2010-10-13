512 => int WINSIZE;
128 => int HOPSIZE;

Noise n => Accum buf => blackhole;
Accum outbuf => dac;

// set size on buf (window size)
// buf holds the most recent "size" # of samples
WINSIZE => buf.size;

// buffer
float signal[WINSIZE];
complex rspectrum[WINSIZE/2];
polar pspectrum[WINSIZE/2];

// window
float window[WINSIZE];
// fill in win up to win.cap()-1;
Window.computeHamming( window );

// go
while( true )
{
    // plan b
    buf.copyTo( signal );
    // window
    Window.apply( window, signal );
    
    // take fft, third argument optional FFT size (auto zero pad)
    FFT.transform( signal, rspectrum, 2048 );
    
    // do stuff to spectrum
    for( int i; i < rspectrum.cap(); i++ )
    {
        // operate on bin
        rspectrum[i] => complex val;
    }
    
    // take inverse FFT
    IFFT.transform( rspectrum, signal );
    
    // window
    Window.apply( window, signal );
    // overlap add 
    outbuf.add( signal );
    
    // advance time
    HOPSIZE::samp => now;
}
