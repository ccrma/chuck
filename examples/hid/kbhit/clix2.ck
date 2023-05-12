// name: clix2.ck
// desc: typing-based instrument, quantized, multi-channel
// author: Ge Wang and Perry Cook

// computer keyboard input via terminal
KBHit kb;

// time
4096::samp => dur T;

// patch
Impulse i => BiQuad f => Envelope e => JCRev r;

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
// period duration
N * T => dur period;

// last unen
UGen @ last;
// total number of channels
dac.channels() => int C;
// keep track of which
int which;

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
        // figure out period
        (now % period / period * N) $ int => int index;
        // generate impulse
        mygains[index] => i.next;
        // set filtre freq
        kb.getchar() => int c => Std.mtof => f.pfreq;
        // print int value
        <<< "ascii:", c, "velocity:", mygains[index], "channel:", which >>>;

        // disconnect from previous
        if( last != NULL ) r =< last;
        // the dac channel to connect
        dac.chan(which) @=> last;
        // the next channel
        (which + 1) % C => which;
        // connect revert to dac channel
        r => last;

        // open
        e.keyOn();
        // advance time
        T-2::ms => now;
        // close
        e.keyOff();
        2::ms => now;
    }
}
