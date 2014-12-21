//  Listing 9.7 Make another BPM, but static variables are shared

// Lives in another file, UseBPM2.ck
SinOsc s => dac.right;
BPM t2; // Define another BPM named t2
0.3 => s.gain;
800 => int freq;

while (freq > 400)
{
    freq => s.freq;
    t2.quarterNote => now;
    50 -=> freq;
}
