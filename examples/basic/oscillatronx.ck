// oscillator demo
// - philipd

// scale degrees in semi-tones
[ 0, 2, 4, 7, 9 ] @=> int f[];

// various oscialltors
SinOsc s => dac;
SawOsc saw => dac;
TriOsc tri => dac;
PulseOsc pul => dac;
SqrOsc sqr => dac;
// modulator and carrier
TriOsc trictrl => SinOsc sintri => dac;
// interpret input as frequency modulation
2 => sintri.sync;
100  => trictrl.gain;

// array of Oscs
[ s, saw, tri, pul, sqr, trictrl ] @=> Osc oscillators[];

// set gains
0.2 => s.gain;
0.1 => saw.gain;
0.1 => tri.gain;
0.1 => pul.gain;
0.1 => sqr.gain;
0.1 => sintri.gain;

// infinite time-loop
while( true )
{ 
    // randomize
	Math.random2(0,7) => int select;
    // clamp (giving more weight to 5)
	if( select > 5 ) 5 => select;
    // generate new frequenc value
	Std.mtof( f[Math.random2( 0, 4 )] + 60 ) => float newnote;
    // set frequency
	newnote => oscillators[select].freq;
    // wait a bit
	0.25::second => now;
    // 10 times
	repeat(10)
    {
        Math.random2f( 0.2, 0.8 ) => trictrl.width;
        // <<< "trictrl width:", trictrl.width() >>>;
        0.05::second => now;
    }
}
