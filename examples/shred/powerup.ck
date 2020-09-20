// power up!
// - philipd

// global noise source 
Noise n;

// sweep shred
fun void sweep( float st, float inc, time end)
{
    //set up the audio chain
    n => TwoPole z => dac; 
    1  => z.norm;
    0.1 => z.gain;
    st => z.freq;

    //store the time we entered the thread
    now => time my_start;
    0.0 => float my_seconds;

    Math.random2f( 0.94, 0.99 ) => z.radius;

    // keep going until we've passed the end time sent in by the control thread.
    while( now < end )
    {
        ( now - my_start ) / 1.0::second => my_seconds; 
        Math.max( my_seconds * 4.0, 1.0 ) * 0.1  => z.gain; 
        z.freq() + inc * -0.02  => z.freq;
        10::ms => now;
    }

    n =< z;
    z =< dac;
}

// time loop
while( true )
{ 
    500::ms => dur d;
    if( Math.random2( 0, 10 ) > 3 ) d * 2.0 => d;
    if( Math.random2( 0, 10 ) > 6 ) d * 3.0 => d;
    spork ~ sweep( 220.0 * Math.random2(1,8), 
        	       880.0 + Math.random2f(100.0, 880.0), 
		           now + Math.random2f(1.0, 3.0)::second );
    d => now;
}
