sinosc s => dac;

while( true )
{
    std.rand2f( 30.0, 1000.0 ) => s.freq;
    100::ms => now;
}
