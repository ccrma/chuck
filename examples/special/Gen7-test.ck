// gen7: line segment table generator
// probably superceded by CurveTable
// ported to ChucK from rtcmix by dan trueman
// see luke's page: http://www.music.columbia.edu/cmix/makegens.html#03

// make one, no sound
Gen7 g7;

// set up the values [0,1] and distances in the table
// first arg is initial value 
// followed by pairs indicating distance (total distance = 1)
// and destination value for linear segments

// set coefficients
[0., 0.5, 1., 0.5, 0.] => g7.coefs;

// create an envelope to scan through the table values
Envelope e => blackhole;
e.duration(10000::ms);
0. => e.value;
e.keyOn();

// loop
while (true)
{
    // print
    <<< e.value(), g7.lookup(e.value()) >>>;

    // end
    if(e.value() == 1.) break;

    // advance time
    10::ms => now;
}
