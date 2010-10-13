// WarpTable: end-constrained mapping table
//
// created originally for the PLOrk Utilities by dan trueman
// ported to ChucK by dan trueman, 2007.
//
// this table is mostly useful for conditioning control signals
// it is end-constrained in that an input of 0 always yields 0
// and an output of 1 always yields 1 
//
//     WarpTable.value(0) => 0 always
//     WarpTable.value(1) => 1 always
//
// expects input [0,1], generates output [0,1]
//
// what happens in between is what is interesting....
//
// WarpTable is useful for mapping sensors and focusing
// attention on:
//     --either extreme (asymmetrical warping)
//     --both extremes (symmetrical warping)
//     --central values (also symmetrical warping)
//
// the first coefficient sets the asymmetrical warping value
//     --asym = 1. => linear
//     --asym > 1. => exponential (focusing resolution on lower output values)
//     --asym < 1. => log (focusing resolution on higher output values)
//
// the second coefficient sets the symmetrical warping value
//     --sym = 1. => linear
//     --sym < 1. => focusing resolution around central (0.5) output value
//         __________/
//        /
//
//      --sym > 1. => focusing resolution on extremes (0 and 1) output values
//                     _____
//              _____/
//
// and of course they can be combined to create related warped shapes

// make one
WarpTable w;

// some examples:
w.coefs( [4., 1.] );    // fairly strong focus on low values; 4 is useful for octave mappings
// w.coefs([0.25, 1.]);    // some focus on high values
// w.coefs([1., 0.3]);     // strong focus on central values
// w.coefs([1., 3.]);      // strong focus on hi/low values
// w.coefs([10., 3.]);     // some focus on high values, strong focus on low values; warped
// w.coefs([0.1, 0.3]);
// w.coefs([1., 1.]);      // linear

// create an envelope to scan through the table values
Envelope e => blackhole;
e.duration( 10000::ms );
0. => e.value;
e.keyOn(); // ramp 0 to 1 in 10 seconds

// on your mark
while( true )
{
    // print
    <<< e.value(), w.lookup(e.value()) >>>;

    // end
    if( e.value() == 1. ) break;

    // advance time
    10::ms => now;
}
