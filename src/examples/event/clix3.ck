// name: clix.ck
// desc: typing-based instrument, quantized, multi-channel
// author: Ge Wang + Perry Cook

// computer keyboard input via terminal
KBHit kb;

// time
4096::samp => dur T;

// patch
Impulse i => BiQuad f => Envelope e => JCRev r => dac;

// set the filter's pole radius
.99 => f.prad;
// set equal gain zeros
1 => f.eqzs;
// envelope rise/fall time
1::ms => e.duration;
// reverb mix
.01 => r.mix;

// strengths
[ 1.0, 0.2, 0.3, 0.2, 0.4, 0.1, 0.2, 0.1,
  0.5, 0.1, 0.3, 0.2, 0.4, 0.1, 0.2, 0.1,
  0.8, 0.1, 0.3, 0.2, 0.5, 0.1, 0.2, 0.1,
  0.4, 0.1, 0.3, 0.2, 0.3, 0.1, 0.2, 0.1 ] @=> float mygains[];

// capacity
mygains.size() => int N;
// total period
N * T => dur period;

// time-loop
while( true )
{
    // wait on event
    kb => now;

    // synch
    T - (now%T) => now;

    // loop through 1 or more keys
    while( kb.more() )
    {
        // get char
        kb.getchar() => int c;

        // space
        if( c == 32 )
        {
            <<< "skipping...", "" >>>;
            T => now;
            continue;
        }

        // figure out period
        (now % period / period * N) $ int => int index;
        // generate impulse
        mygains[index] => i.next;
        // set filtre freq
        c => Std.mtof => f.pfreq;
        // print int value
        <<< "ascii:", c, "velocity:", mygains[index] >>>;

        // open
        e.keyOn();
        // advance time
        T-2::ms => now;
        // close
        e.keyOff();
        2::ms => now;
    }
}
