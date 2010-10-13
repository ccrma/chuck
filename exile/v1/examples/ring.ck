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
adc => gain g => JCRev r => dac;
sinosc s => g;

// multiply inputs at g
3 => g.op;

// presets
.2 => r.mix;
400.0 => s.freq;

// time loop
while( true )
    1::second => now;
