// synthesis/analysis path
SinOsc foo => FFT fft =^ Centroid centroid => blackhole;
// set sine frequency
440 => s.freq;

// set FFT size
1024 => fft.size;
// set window type and size
Windowing.hann(fft.size()) => fft.window;
// our hop size (how often to perform analysis)
fft.size()::samp => dur HOP;
// compute srate
second / samp => float srate;

// control loop
while( true )
{
    // upchuck() computes centroid (and dependencies, e.g., FFT)
    centroid.upchuck();
    // get and print out result
    <<< "centroid (hz):", centroid.fval(0) * srate / 2 >>>;
    // advance time
    HOP => now;
}
