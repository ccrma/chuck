//-----------------------------------------------------------------------------
// name: readme-GenX.ck
// desc: old and new lookup table utilities for ChucK
//
// author: Dan Trueman, 2007
//
// These utilities bring the familiar lookup table capabilities of the MusicX
// languages to ChucK. They all create an internal lookup table given various
// parameters set by the user. This table can then be read at the audio rate
// (with Phasor, for example, or any other source) or simply read with .value()
// calls. 
//
// A simple example for gen10 is included here. gen10 creates a table of 
// harmonically related sinusoids; the user sets the relative weights of the
// harmonics with an array of floats. The phasor UG creates a continuous 
// lookup value between 0 and 1 (values less than 0 are multiplied by -1, so 
// SinOsc and other [-1,1] sources can be used as well).
//
// All of these other than WarpTable were ported from RTcmix (thanks rtcmix folks!).
//-----------------------------------------------------------------------------

// patch
Phasor drive => Gen10 g10 => dac;
// can also lookup table values directly:
//     g10.lookup(index);
//     with index:[0,1]

// load up the coeffs; amplitudes for successive partials
g10.coefs( [1., 0.5, 0.3, 0.2] );

// set frequency for reading through table
drive.freq( 440. );

// go
while( true ) { 1::day => now; }

//-----------------------------------------------------------------------------

/*
the GenX library includes:

gen5: exponential line segment table generator (probably superceded by CurveTable)
gen7: line segment table generator (probably superceded by CurveTable)
gen9: inharmonic sinusoidal lookup table with ratio and phase control
gen10: sinusoidal lookup table *without* ratio and phase control (only partial amp control)
gen17: chebyshev polynomial lookup table
CurveTable: flexible Curve/Line segment table generator
WarpTable: end-constrained mapping table, useful for control signal conditioning
*/
