// sound chain
SinOsc s => dac;
SqrOsc t => dac;

// only play s
0.5 => s.gain;
0 => t.gain;
for( 0 => int i; i < 500; i++ )
{
    i => s.freq;
    .001::second => now;
}

// only play t
0 => s.gain;
0.2 => t.gain;
for( 0 => int i; i < 500; i++ )
{
    i => t.freq;
    .001::second => now;
}

// play s and t at the same time
0.5 => s.gain;
0.2 => t.gain;
for( 0 => int i; i < 500; i++ )
{
    i => t.freq;
    i*2 => s.freq;
    .001::second => now;
}