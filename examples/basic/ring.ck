//--------------------------------------------
// simple ring modulation
//
// any ugen has .op:
// ---
// -1 pass through the input
// 0 stop input
// 1 add inputs (default)
// 2 subtract from first input
// 3 multiply inputs
// 4 divide from first input
//--------------------------------------------

// the patch
adc => Gain g => dac;
SinOsc s => g;

// multiply inputs at g
3 => g.op;

// presets
400.0 => s.freq;

// time loop
while( true )
    1::second => now;
