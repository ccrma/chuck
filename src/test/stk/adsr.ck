// run white noise through ADSR envelop
noise n => ADSR e => dac;

// infinite time-loop
while( true )
{
    // key on - start attack
    1 => e.keyOn;
    // advance time by 800 ms
    800::ms => now;
    // key off - start release
    1 => e.keyOff;
    // advance time by 800 ms
    800::ms => now;
}
