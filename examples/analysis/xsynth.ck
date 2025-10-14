
// source one (mic)
adc => FFT X => blackhole;
// source two (to be connected below)
FFT Y => blackhole;
// synthesis
IFFT ifft => dac;

// what to cross
BlitSquare blt[6];
[ 40, 46, 52, 60, 64, 87] @=> int pitches[];
for( int i; i < blt.size(); i++ )
{
    blt[i] => Y;
    20 => blt[i].harmonics;
    pitches[i] => Std.mtof => blt[i].freq;
}

// set FFT size

1024 => X.size => Y.size => int FFT_SIZE;
// desired hop size
FFT_SIZE / 4 => int HOP_SIZE;
// set window and window size
Windowing.hann(FFT_SIZE) => X.window;
Windowing.hann(FFT_SIZE) => Y.window;
Windowing.hann(FFT_SIZE) => ifft.window;
// use this to hold contents
complex Z[FFT_SIZE/2];

// control loop
while( true )
{
    // take ffts
    X.upchuck();
    Y.upchuck();
    
    
    for( int i; i < Z.size(); i++ ){
        X.cval(i).re => float realX;
        X.cval(i).im => float imagX;
        Math.sqrt(realX*realX + imagX*imagX) => float magX;
        Y.cval(i).re => float realY;
        Y.cval(i).im => float imagY;
        Math.sqrt(realY*realY + imagY*imagY) => float magY;
        
        // multiply in frequency domain 
        #(magX*magY, magX*magY) => Z[i];
    }
        
    // take ifft
    ifft.transform( Z );
    
    // advance time
    HOP_SIZE::samp => now;
}
