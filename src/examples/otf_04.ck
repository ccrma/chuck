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

// this synchronizes to period
.5::second => dur T;
T - (now % T) => now;

// one period offset
T => now;

// construct the patch
SndBuf buf => Gain g => dac;
"data/snare-hop.wav" => buf.read;
.6 => g.gain;

// where we actually want to start
100 => int where;

// time loop
while( true )
{
    Std.rand2f(.8,.9) => buf.gain;

    if( Std.randf() > -.5 )
    {
        where => buf.pos;
        2::T => now;
    }
    else
    {
        where => buf.pos;
        .75::T => now;
        .8 => buf.gain;
        where => buf.pos;
        1.25::T => now;
    }
}
