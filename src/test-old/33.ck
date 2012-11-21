// 33.ck : sine

// sine tones (fail-correct)
sinosc s => dac;

[ 0, 2, 4, 7, 9 ] @=> int f[];

// time loop
while( true )
{
    // choose freq
    std.mtof( f[math.rand2( 0, 4 )] + 72 ) => s.freq;
    // advance time
    100::ms => now;
}
