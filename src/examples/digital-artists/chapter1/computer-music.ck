// our patch: sine oscillator to dac
SinOsc s => dac;

// infinite time loop
while( true )
{
    // randomly choose frequency from 30 to 1000
    Math.random2f( 30, 1000 ) => s.freq;

    // advance time by 100 millisecond
    100::ms => now;
}

