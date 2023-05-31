//---------------------------------------------------------------------
// name: features-combined.ck
// desc: extracting multiple features; collect into one vector
//       e.g.,  Centroid, Flux, RMS, MFCC
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Spring 2023
//---------------------------------------------------------------------

// audio input into a FFT
adc => FFT fft;
// a thing for collecting multiple features into one vector
FeatureCollector combo => blackhole;
// add spectral feature: Centroid
fft =^ Centroid centroid =^ combo;
// add spectral feature: Flux
fft =^ Flux flux =^ combo;
// add spectral feature: RMS
fft =^ RMS rms =^ combo;
// add spectral feature: MFCC
fft =^ MFCC mfcc =^ combo;

// set number of coefficients in MFCC (how many we get out)
// 13 is a commonly used value; using less here for printing
13 => mfcc.numCoeffs;
// set number of mel filters in MFCC
10 => mfcc.numFilters;

// set FFT size
2048 => fft.size;
// set window type and size
Windowing.hann(fft.size()) => fft.window;
// our hop size (how often to perform analysis)
fft.size()::samp => dur HOP;

// let one FFT-size of time pass (to buffer)
fft.size()::samp => now;
// control loop
while( true )
{
    //-----------------------------------------------------------------
    // a single upchuck() will trigger analysis on everything
    // connected upstream from combo via the upchuck operator (=^)
    // the total number of output dimensions is the sum of
    // dimensions of all the connected unit analyzers
    //-----------------------------------------------------------------
    combo.upchuck();

    // print
    cherr <= "features (" <= combo.fvals().size() <= " total dims): ";
    // print the MFCC results
    for( int i; i < combo.fvals().size(); i++ )
    { cherr <= combo.fval(i) <= " "; }
    // endline
    cherr <= IO.newline();
 
    // advance time
    HOP => now;
}
