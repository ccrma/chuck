// really really bad cross synthesizer...

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
Windowing.hann(512) => X.window;
Windowing.hann(512) => Y.window;
Windowing.hann(512) => ifft.window;
// use this to hold contents
complex Z[FFT_SIZE/2];

// control loop
while( true )
{
    // take ffts
    X.upchuck();
    Y.upchuck();
    
    // multiply in frequency domain
    for( int i; i < X.size()/2; i++ )
        Math.sqrt((Y.cval(i)$polar).mag) * X.cval(i) => Z[i];
        // 2 * Y.cval(i) * X.cval(i) => Z[i];
    
    // take ifft
    ifft.transform( Z );
    
    // advance time
    HOP_SIZE::samp => now;
}
