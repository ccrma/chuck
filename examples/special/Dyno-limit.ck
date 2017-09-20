//--------------------------------------|
// Dyno demo 1: "Limiter? I hardly know 'er!"
// Takes otf_07.ck, cranks the gain up obscenely high, then limits 
// it so it won't clip. 
//
// authors: Matt Hoffman (mdhoffma at cs.princeton.edu)
//          Ge Wang (gewang at cs.princeton.edu)
//          Perry Cook (prc at cs.princeton.edu)
//--------------------------------------|

// this synchronizes to period
.5::second => dur T;
T - (now % T) => now;

// construct the patch
SndBuf buf => Gain g => Dyno dy => JCRev r => dac;
me.sourceDir() + "/../data/kick.wav" => buf.read;
.01 => r.mix;

// make gain way too high, i.e. force clipping
100 => g.gain;
// turn on limiter functionality
dy.limit();
// compensate for the limiter's gain reduction
5 => dy.gain;
// uncomment this to turn off the limiter. note that just because clipping 
// sounds cool here doesn't mean it will in your patch.
// -1 => dy.op;

// where we actually want to start
25 => int where;

// time loop
while( true )
{
    Math.random2f(.8,.9) => buf.gain;

    // note: Math.randomf() returns value between 0 and 1
    if( Math.randomf() > .75 )
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
