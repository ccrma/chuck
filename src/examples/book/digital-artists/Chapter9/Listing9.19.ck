// Listing 9.19 Polymorphism with an array of StkInstruments

// Example of polymorphism and base class use
// make an array of generic instruments
StkInstrument inst[4];

// make each instrument a different type
Sitar inst0 @=> inst[0] => dac;
Mandolin inst1 @=> inst[1] => dac;
Clarinet inst2 @=> inst[2] => dac;
BlowBotl inst3 @=> inst[3] => dac;

// take advantage of polymorphism to play them all
for (0 => int i; i < 4; i++) {
    500.0 - (i*100.0) => inst[i].freq;
    1 => inst[i].noteOn;
    second => now;
    1 => inst[i].noteOff;
}

