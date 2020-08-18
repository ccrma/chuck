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
me.dir() + "data/snare.wav" => buf.read;
.5 => g.gain;
.05 => r.mix;

// where we actually want to start (in # of sample frames)
25 => int where;

// time loop
while( true )
{
    // randomize the gain a bit
    Math.random2f(.8,.9) => buf.gain;

    // note: Math.randomf() returns value between 0 and 1
    if( Math.randomf() > .75 )
    {
        0 => int i;
        while( i < 8 )
        {
            // gradually increase gain
            i / 8.0 => buf.gain;
            // set play position
            where => buf.pos;

            // advance time
            (1.0/8.0)::T => now;
            // increment loop counter
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
