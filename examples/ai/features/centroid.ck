//---------------------------------------------------------------------
// name: centroid.ck
// desc: fantastic audio features and where to extract them:
//       extracting spectral centroid; also demonstrates using
//       unit analyzers (UAna), connecting them using the
//       upchuck operator (=^), and getting results
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Spring 2023
//---------------------------------------------------------------------

// synthesis / analysis network
SinOsc foo => FFT fft =^ Centroid centroid => blackhole;
// set sine frequency
440 => foo.freq;

// set FFT size
1024 => fft.size;
// set window type and size
Windowing.hann(fft.size()) => fft.window;
// our hop size (how often to perform analysis)
fft.size()::samp => dur HOP;
// compute srate
second / samp => float srate;

// let one FFT-size of time pass (to buffer)
fft.size()::samp => now;
// control loop
while( true )
{
    //----------------------------------------------------------------
    // upchuck() computes our centroid, automatically computing upstream
    // dependencies connected to it using =^ (e.g., FFT)
    //----------------------------------------------------------------
    centroid.upchuck();

    // get and print the output
    <<< "centroid (hz):", centroid.fval(0) * srate / 2 >>>;

    // advance time
    HOP => now;
}
