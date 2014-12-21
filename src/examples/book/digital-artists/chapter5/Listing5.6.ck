// Listing 5.6 Using functions to set oscillator frequencies

// three oscillators in stereo
SqrOsc s => dac.left;   // (1) Three square waves, panned left,
SqrOsc t => dac;        //     center,
SqrOsc u => dac.right;  //     and right

// set gains so we don't overload the dac
0.4 => s.gain => t.gain => u.gain;        // (2) Sets gains of all three

// functions for octave and fifth
fun float octave( float originalFreq )
{
    return 2.0*originalFreq;
}

fun float fifth( float originalFreq )
{
    return 1.5*originalFreq;
}

// Main program
for (100 => float freq; freq < 500; 0.5 +=> freq )  // (3) Sweeps frequency 
{                                                // from 100 to 500 by 1/2 Hz each step
    freq => s.freq;                // (4) Sets left square wave to freq
    octave(freq) => t.freq;        // (5) Sets center square wave to octave above
    fifth(freq) => u.freq;         // (6) Sets right square wave to fifth above
    <<< s.freq(), t.freq(), u.freq() >>>;
    10::ms => now;
}
