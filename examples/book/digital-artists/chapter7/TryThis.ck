// Trying out the Moog parameters
Moog mog => dac;
200.0 => mog.freq;

while (1) {
    Math.random2f(1.0,1.0) => mog.filterQ;
    Math.random2f(0.01,1.0) => mog.filterSweepRate;
    1 => mog.noteOn;
    if (Math.random2(0,10) == 0) {
        Math.random2f(1.0,20.0) => mog.vibratoFreq;
        0.5 => mog.vibratoGain;
        second => now;
    }
    else {
        0.01 => mog.vibratoGain;
        0.125 :: second => now;
    }
    1 => mog.noteOff;
    0.125 :: second => now;
}

