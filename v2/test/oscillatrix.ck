// oscillator demo
// - philipd

[0, 2, 4, 7, 9] @=> int f[];

sinosc s => dac;
sawosc saw => dac;
triosc tri => dac;
pulseosc pul => dac;
sqrosc sqr => dac;

triosc trictrl => sinosc sintri => dac;

[s, saw, tri, pul, sqr] @=> osc oscillators[];

0.2 => s.gain;
0.1 => saw.gain;
0.1 => tri.gain;
0.1 => pul.gain;
0.1 => sqr.gain;
0.1 => sintri.gain;


while ( true ) { 
    math.rand2(0,4) => int select;
    std.mtof( f[math.rand2( 0, 4 )] + 60 ) => float newnote;
    newnote => oscillators[select].freq;
    0.25::second => now;
}
