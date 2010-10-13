// CurveTable: flexible Curve/Line Segment table generator

// make one
CurveTable c;

// coeffs are all FLOATS, specifying proportional times, values, and curvature:
// [time0, value0, curvature to next, time1, value1, curvature,..
//      timeN-1, valueN-1, curvaturen-1, timeN, valueN]

// times => ascending unitless values (NOT Chuck Time values)
// values => whatever
// curvature(0) => linear
// curvature(<0) => convex
// curvature(>0) => concave

// see:
//     http://www.music.columbia.edu/cmc/Rtcmix/docs/scorefile/maketable.html#curve
//     for more documentation

// set
[0., 0., -0.5, 1., 1., 0.5, 2., -1.] => c.coefs;

// create an envelope to scan through the table values
Envelope e => blackhole;
e.duration(10000::ms);
0. => e.value;
e.keyOn(); //ramp to 1 in 10 seconds

// patch
Phasor p => c => dac;
// set
440. => p.freq;

// ready
while (true)
{
    // print
    <<< e.value(), c.lookup(e.value()) >>>;
    
    // end
    if(e.value() == 1.) break;

    // advance time
    10::ms => now; 
}
