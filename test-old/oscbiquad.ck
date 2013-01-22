// oscillator demo
// - philipd

[0, 2, 4, 7, 9] @=> int f[];

biquad bf => dac;
twozero oz => bf;
sinosc s => bf;
triosc tri => bf;
pulseosc pul => oz;

[s, tri, pul] @=> osc oscillators[];

1110 => oz.freq;
0.94 => oz.rad;
.92 => bf.prad;
1 => bf.eqzs;
0.10 => bf.gain;
0.05 => s.gain;
0.05 => tri.gain;
0.10 => pul.gain;

0.0 => float v;

while ( true ) { 
    Math.rand2(0,2) => int select;
    Std.mtof( f[Math.rand2( 0, 4 )] + 60 ) => float newnote;
    newnote => oscillators[select].freq;

    for ( 0 => int i ; i < 100 ; 1 +=> i ) { 
	50.0 + Std.abs( Math.sin(v)) * 200.0 => bf.pfreq;
	0.75 + 0.05 * Std.abs ( Math.sin(v * 0.333) ) => oz.rad;
	200.0 + 200.0 *  Math.sin(v * 10) => oz.freq;
	v + .03 => v;
	5::ms => now;
    }
    0.25::second => now;   
}
