// Listing 7.8 Fiddling around with the STK Bowed UGen

// Test some Bowed parameters
Bowed viol => dac;

for (1 => int i; i <= 5; i++) {
    for (1 => int j; j <= 5; j++) {
        i*0.2 => viol.bowPressure; // (1) Gradually increases bowPressure...
        j*0.2 => viol.bowPosition; // (2) ...and changes bowPosition.

        <<< "pres=", i*0.2, "pos=", j*0.2 >>>;

        1 => viol.noteOn;
        0.3 :: second => now;

        1 => viol.noteOff;
        0.1 :: second => now;
    }
}
