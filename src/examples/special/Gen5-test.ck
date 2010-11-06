// gen5: exponential line segment table generator
// probably superceded by CurveTable
// ported to ChucK from rtcmix by dan trueman
// see luke's page: http://www.music.columbia.edu/cmix/makegens.html#02

// make one, no sound here
Gen5 g5;

// set up the values [0,1] and distances in the table
// first arg is initial value 
// followed by pairs indicating distance (total distance = 1)
// and destination value for exponential segments
// NOTE: real minimum value is 0.000001

// the following will create a triangle, with a peak at 1 halfway
// through the table
[0., 0.5, 1., 0.5, 0.] => g5.coefs;

// create an envelope to scan through the table values
// creates a continuous input from 0 -> 1 over 10 seconds
Envelope e => blackhole;
e.duration( 10000::ms );
0. => e.value;
e.keyOn();

// loop
while (true)
{
    // print
    <<<e.value(), g5.lookup(e.value())>>>;

    // end
    if( e.value() == 1. ) break;

    // advance time
    10::ms => now;
}
