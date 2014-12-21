// Listing 2.6 Using a Pan2 object to connect a SinOsc to stereo dac output

// panning example
SinOsc s => Pan2 p => dac;   // (1) Runs the oscillator through a Pan2 object

// initialize pan position
-1.0 => float panPosition;   // (2) Sets initial pan to hard left...

// loop to vary panning
while (panPosition < 1.0) {  // (3) ...until panPosition hits hard right.
    panPosition => p.pan;    // (4) Sets new pan position...
    <<< panPosition >>>;
    panPosition + 0.01 => panPosition;  // (5) ...and increments it a little.
    10 :: ms => now;
}

