// STK Moog
// by: phil

Moog mog => dac;
440.0 => mog.freq;
0.0 => float t;

fun void varmod()
{
    while ( true )
    {
        0.5 + 0.4 *  Math.sin( t * 0.1 ) => mog.modDepth;	
        0.5 + 0.4 *  Math.sin( t * 0.2 ) => mog.modSpeed;	
        0.5 + 0.4 *  Math.sin( t * 0.3 ) => mog.filterQ;	
        0.5 + 0.4 *  Math.sin( t * 0.4 ) => mog.filterSweepRate;	
        10::ms => now;
        t + 0.01 => t;
    }
}

fun void atouch( float imp)
{
    imp => float atouch;
    while ( atouch >= 0.0 )
    {
        atouch => mog.afterTouch;
        atouch - 0.05 => atouch;
        10::ms => now;
    }
}

// spork varmod shred
spork ~varmod();

0.0 => float vel;
while ( true )
{
    // Math.random2f ( 440.0, 880.0 ) => mog.freq;
    278.43 => mog.freq;
    Math.random2f(0.5, 0.8) => vel;
    vel  => mog.noteOn;
    spork ~ atouch(vel);
    // note: Math.randomf() returns value between 0 and 1
    if ( Math.randomf() > 0.65 ) { 1::second => now; }
    else { 0.5::second => now; }
}
