//------------------------------------------------------------------------------
// name: phasor.ck
// desc: basic demo using the Phasor UGen to do modulate the width of a PulseOsc
//
// The phasor linearly ramps from 0 --> 1 every cycle, and rate can be controlled
// programmatically. Note: this is *not* intended to be a sound-generating UGen!
// It is more commonly used to modulate other signals or do lookups in a wavetable.
// For example, in this demo, the phasor signal modulates the PulseOsc width.
// 
// Although the phasor signal is the shape of a Sawtooth, it performs no anti-aliasing!
// For a sawtooth shaped signal, use SawOsc instead.
//
// author: azaday
// date: Spring 2023
//------------------------------------------------------------------------------
Phasor phasor => blackhole;
PulseOsc pulseOsc => dac;

// set phasor to ramp from 0-1 at a frequency of 1hz
1.0 => phasor.freq;

// set pulseOsc frequency
440.0 => pulseOsc.freq;

while (true) {
    phasor.last() => pulseOsc.width;
    1::samp => now;
}
