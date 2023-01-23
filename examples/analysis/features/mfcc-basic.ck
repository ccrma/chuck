//---------------------------------------------------------------------
// name: mfcc-basic.ck
// desc: mel-frequency cepstral coefficients
//       basic usage of the MFCC unit analyzer
//
// to see API, uncomment:
// MFCC.help();
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Spring 2023
//---------------------------------------------------------------------

// synthesis / analysis network
adc => FFT fft =^ MFCC mfcc => blackhole;

// set number of mel filters in MFCC
10 => mfcc.numFilters;
// set number of coefficients in MFCC (how many we get out)
// 13 is a commonly used value; using less here for printing
7 => mfcc.numCoeffs;

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
    // upchuck() computes MFCC (and dependencies, e.g., FFT)
    mfcc.upchuck();
    // print
    cherr <= "MFCC: ";
    // print the MFCC results
    for( int i; i < mfcc.fvals().size(); i++ )
    { cherr <= mfcc.fval(i) <= " "; }
    // endline
    cherr <= IO.newline();
    // advance time
    HOP => now;
}
