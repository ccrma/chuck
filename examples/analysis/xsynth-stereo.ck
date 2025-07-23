// name: xsynth-stereo.ck
// desc: a FFT-based cross-synthesizer (stereo delay edition)
//       => input X modulates a second input Y,
//          carried out in the frequency domain
//
// authors: Ge Wang
//          Celeste Betancur
//          Andrew Zhu Aday
//
// date: 2007 initial (really bad) version
//       2025 made less bad (and added stereo version)

// input x (mic)
adc => FFT X => blackhole;
// source two (to be connected below)
FFT Y => blackhole;
// synthesis
IFFT ifft => Delay dL => dac.left;
dL => Gain dLg => dL;
300::ms => dL.max => dL.delay;
.5 => dLg.gain;

ifft => Delay dR => dac.right;
dR => Gain dRg => dR;
450::ms => dR.max => dR.delay;
.5 => dRg.gain;

// input y
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
    
    // multiply in frequency domain
    // NOTE: using input X's magnitude to modulate input Y's signal
    //       X and Y are NOT commutative
    for( int i; i < Z.size(); i++ )
        (X.cval(i)$polar).mag * Y.cval(i) => Z[i];
    
    // take ifft
    ifft.transform( Z );
    
    // advance time
    HOP_SIZE::samp => now;
}
