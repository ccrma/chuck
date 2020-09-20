// Listing 6.6 Simple FM test program

// FM Unit Generator Instrument Test Program
// by FM Dude, March 4, 1976
// make an FM instrument and connect to dac
Wurley instr => dac;    // (1) FM electric piano

// play it forever with random frequency and duration
while (true) {
    Math.random2f(100.0,300.0) => instr.freq;

    // turn note on (trigger internal ADSR)
    1 => instr.noteOn;                 // (2) Turns on note, waits a (random) bit
    Math.random2f(0.2,0.5) :: second => now;

    // turn note off (ramp down internal ADSR)
    1 => instr.noteOff;                // (3) Turns off note, waits a (random) bit
    Math.random2f(0.05,0.1) :: second => now;
}
