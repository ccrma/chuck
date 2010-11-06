// connect a sine oscillator to audio output
sinosc s => dac;
// set the oscillator frequency (hz)
220.0 => s.freq;
// let time advance for 2 seconds
2::second => now;
