// power up!
// - philipd, stereo gewang

0.0 => float t;
Noise n;

// timer shred
fun void timer() { 
    while ( true ) { 
        t + 0.01 => t;
        10::ms => now;
    }
}

// spork the timer
spork ~ timer();

// sweep shred
fun void sweep( float st, float inc, float end, int c )
{
    n => TwoPole z => Pan2 p => dac; 
    1  => z.norm;
    0.1 => z.gain;
    t => float dt;
    st => float frq;
    Math.random2f( -1, 1 ) => float s;
    Math.random2f( -1, 1 ) => float e;
    ( e - s ) / ( end - t ) * .01 => float i;
    s => p.pan;
    Math.random2f( 0.94, 0.99 ) => z.radius;
    while( t < end ) {
        Math.max( ( t - dt ) * 4.0, 1.0 ) * 0.1 => z.gain; 
        frq + inc * -0.02  => frq; 
        frq => z.freq;
        p.pan() + i => p.pan;
        10::ms => now;
    }

    n =< z;
    z =< p;
    p =< dac;
}

0 => int c;
// time loop
while( true ) { 
    500::ms => dur d;
    if( Math.random2( 0, 10 ) > 3 ) d * 2.0 => d;
    if( Math.random2( 0, 10 ) > 6 ) d * 3.0 => d;
    spork ~ sweep( 220.0 * Math.random2( 1, 8 ), 
                   880.0 + Math.random2f( 100.0, 880.0 ),
                   t + Math.random2f( 1.0, 3.0 ) , c);
    1 + c => c; 
    d => now;
}
