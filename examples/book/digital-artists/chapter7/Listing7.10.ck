// Listing 7.10 Spectral modeling using the ModalBar UGen: a struck glass sound

// Synthesize a modal glass sound from spectral model
ModalBar glass => dac;

[1206.0,2240.0,3919.0,5965.0] @=> float modeFreqs[];
[100.0,93.0,97.0,92.0] @=> float modeGainsdB[];
[0.9999,0.9997,0.9998,0.9996] @=> float modeRadii[];

for (0 => int i; i < 4; i++) {
    i => glass.mode;
        // (1) Sets frequency of each mode as ratio with zero-eth mode.
    modeFreqs[i]/modeFreqs[0] => glass.modeRatio;
        // (2) Uses dbtorms() to get a gain from a dB magnitude.
    Std.dbtorms(modeGainsdB[i]) => glass.modeGain;
        // (3) Radius sets decay time.
    modeRadii[i] => glass.modeRadius;
}

// set to first mode frequency, set gain too
1206 => glass.freq;
0.2 => glass.gain;

// whack it!!!
1 => glass.noteOn;
3.0 :: second => now;
