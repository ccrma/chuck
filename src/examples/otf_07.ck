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

// construct the patch
SndBuf buf => Gain g => JCRev r => dac;
"data/snare.wav" => buf.read;
.5 => g.gain;
.05 => r.mix;

// where we actually want to start
25 => int where;

// time loop
while( true )
{
    Std.rand2f(.8,.9) => buf.gain;

    if( Std.randf() > .5 )
    {
        0 => int i;
        while( i < 8 )
        {
            i / 8.0 => buf.gain;
            where => buf.pos;
            (1.0/8.0)::T => now;
            i++;
        }

        while( i > 0 )
        {
            i / 8.0 => buf.gain;
            where => buf.pos;
            (1.0/8.0)::T => now;
            i--;
        }
    }
    else
    {
        .9 => buf.gain;
        where => buf.pos;
        .25::T => now;
        .3 => buf.gain;
        where => buf.pos;
        .25::T => now;
        .3 => buf.gain;
        where => buf.pos;
        .25::T => now;

        .9 => buf.gain;
        where => buf.pos;
        .25::T => now;
        .3 => buf.gain;
        where => buf.pos;
        .25::T => now;
        .3 => buf.gain;
        where => buf.pos;
        .25::T => now;

        .9 => buf.gain;
        where => buf.pos;
        .25::T => now;
        .3 => buf.gain;
        where => buf.pos;
        .25::T => now;
    }
}
