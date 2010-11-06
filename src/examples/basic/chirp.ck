// po-tweet!

// patch
SinOsc s => dac;
// gain
.4 => s.gain;

// call chirp
chirp( 127, 20, 1::second );

// call chirp (with tinc)
chirp( 20, 120, 1.5::second, 100::ms );

// chirp function
fun void chirp( float src, float target, dur duration )
{
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
}

