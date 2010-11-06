//----------------------------|
// on-the-fly synchronization
// adapted from Perry's ChucK Drummin' + Ge's sine poops
//
// authors: Perry Cook (prc@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
// --------------------|          
// add one by one into VM (in pretty much any order):
// 
// terminal-1%> chuck --loop
// ---
// terminal-2%> chuck + otf_01.ck
// (anytime later)
// terminal-2%> chuck + otf_02.ck
// (etc...)
//--------------------------------------|

// synchronize to period
.5::second => dur T;
T - (now % T) => now;

sinosc s => gain g => JCRev r => dac;
.05 => g.gain;
.25 => r.mix;

while( true )
{
    2 * std.rand2(0,4) => int freq;
    if( freq == 6 ) 7 => freq;
    if( freq == 8 ) 9 => freq;

    std.mtof( 69.0 + (float)(std.rand2(0,2)*12 + freq) ) => s.freq;

    if( std.randf() > -.5 )
        .25::T => now;
    else
        .5::T => now;
}
