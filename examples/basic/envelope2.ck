// name: envelope2.ck
// desc: real-time segmented envelope generation
// requires: 1.5.2.5 or higher

// no sound in this example; only printing
Step s(1) => Envelope e => blackhole;

// spork printing shred
spork ~ print();

// infinite time-loop
while( true )
{
    // next duration to target
    Math.random2f(500,1000)::ms => dur duration;
    // next target
    Math.random2f(.1, 10) => float target;
    // print duration and target
    <<< "duration:", duration/second, "target:", target >>>;

    // over a duration, ramp to target
    e.ramp( duration, target ) => now;
}

// print function
fun void print()
{
    // infinite time loop
    while( true )
    {
        // print value of e
        // NOTE: this is only a rough print-out and
        // will not necessary print the target values
        // but values approaching towards the targets
        <<< e.last(), "" >>>;
        50::ms => now;
    }
}
