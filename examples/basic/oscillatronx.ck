// oscillator demo
// - philipd

[ 0, 2, 4, 7, 9 ] @=> int f[];

SinOsc s => dac;
SawOsc saw => dac;
TriOsc tri => dac;
PulseOsc pul => dac;
SqrOsc sqr => dac;

TriOsc trictrl => SinOsc sintri => dac;

[ s, saw, tri, pul, sqr, trictrl ] @=> Osc oscillators[];

0.2 => s.gain;
0.1 => saw.gain;
0.1 => tri.gain;
0.1 => pul.gain;
0.1 => sqr.gain;
0.1 => sintri.gain;
2 => sintri.sync;

while ( true ) { 

	Math.random2(0,7) => int select;
	if ( select > 5 ) 5 => select;
	Std.mtof( f[Math.random2( 0, 4 )] + 60 ) => float newnote;
	newnote => oscillators[select].freq;
	0.25::second => now;
	for ( 0 => int i; i < 10; i+1 => i ) { 
		Math.random2f (0.2, .8 ) => trictrl.width; 
		0.05 ::second => now;
	}

}
