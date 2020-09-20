// Listing 9.10 UseBPM3.ck randomly sets BPM tempo and sweeps another sine

// This lives in another file, UseBPM3.ck
SinOsc s => dac;

BPM t2; // Define another BPM called t2
0.3 => s.gain;

// and set tempo to a random number
Math.random2f(200.0,1000.0) => t2.tempo;  // (1) Sets random tempo
1000 => int freq;

while (freq > 400)           // (2) Sweeps sine frequency downward
{
    freq => s.freq;
    t2.quarterNote => now;
    50 -=> freq;
}
