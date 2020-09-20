// run white noise through envelope
Noise n => Envelope e => dac;

// infinite time-loop
while( true )
{
    // random choose rise/fall time
    Math.random2f(10,500)::ms => dur t => e.duration;
    // print
    <<< "rise/fall:", t/ms, "ms" >>>;

    // key on - start attack
    e.keyOn();
    // advance time by 800 ms
    800::ms => now;
    // key off - start release
    e.keyOff();
    // advance time by 800 ms
    800::ms => now;
}
