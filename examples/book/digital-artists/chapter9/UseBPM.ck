// Listing 9.6 Make a BPM; setting tempo here affects all others
// This lives in file UseBPM.ck
SinOsc s => dac;
BPM t; // Define t Object of Class BPM
t.tempo(300); // set tempo in BPM
0.3 => s.gain;

400 => int freq;
while (freq < 800)
{
    freq => s.freq;
    t.quarterNote => now;
    50 +=> freq;
}

