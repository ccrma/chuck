// more chirping...

// patch with envelope and pan
SinOsc s => Envelope e => Pan2 p => dac;
// gain
.5 => s.gain;

// pan hard left
-1 => p.pan;
// call chirp
chirp( 127, 20, 1::second );

// wait for envelope to go down
10::ms => now;
// pan hard right
1 => p.pan;
// call chirp
chirp( 20, 120, 1::second, 100::ms );

// wait a second
1::second => now;

// randomize pan
Math.random2f( -1, 1 ) => p.pan;
// call chirp
chirp( 30, 110, .5::second );

// wait for envelope to go down
10::ms => now;
// pan
Math.random2f( -1, 1 ) => p.pan;
// call chirp
chirp( 110, 30, 1::second, 100::ms );

// wait a second
1::second => now;


// chirp function
fun void chirp( float src, float target, dur duration )
{
    // just call the other one with default tinc
    chirp( src, target, duration, 1::ms );
}


// chirp function (with tinc)
fun void chirp( float src, float target, dur duration, dur tinc )
{
    // initialize freq
    src => float freq;
    // find the number of steps
    duration / tinc => float steps;
    // find the inc
    ( target - src ) / steps => float inc;
    // counter
    float count;
    // set env
    .01 * duration / second => e.time;
    // open env
    1 => e.keyOn;

    // do the actual work over time
    while( count < steps )
    {
        // increment the freq
        freq + inc => freq;
        // count
        1 +=> count;

        // set the freq
        Std.mtof( freq ) => s.freq;

        // advance time
        tinc => now;
    }

    // close env
    1 => e.keyOff;
}

