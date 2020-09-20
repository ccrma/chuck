// Listing 7.6 STK Mandolin UGen includes a body model and performance controls

Mandolin mand => dac; // (1) Mandolin physical instrument model

[79, 78, 74, 78, 76, 72] @=> int solomio[];

for (0 => int i; i < solomio.cap(); i++) {
    Std.mtof(solomio[i]) => mand.freq;
    Math.random2f(0.2,0.8) => mand.pluckPos;       // (2) Random plucking position
    Math.random2f(0.05,0.11) => mand.stringDetune; // (3) Random detune between strings in a pair
    
    // (4) Tremolo strumming gesture
    for (0 => int trem; trem < 12; trem++) {
        1 => mand.noteOn;
        Math.random2f(0.06,0.09) :: second => now;
    }
}
