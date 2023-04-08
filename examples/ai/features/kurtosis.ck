//---------------------------------------------------------------------
// name: kurtosis.ck
// desc: chroma unit anlyzer, used for feature extraction
//
// uncomment for Chroma API:
// Kurtosis.help();
//
// author: Yikai Li
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Winter 2023
//---------------------------------------------------------------------'

// our patch
adc => FFT fft =^ Kurtosis kurtosis => blackhole;

// set parameters
1024 => fft.size;
// set hann window
Windowing.hann(1024) => fft.window;
// our hop size (how often to perform analysis)
fft.size()::samp => dur HOP;

// control loop
while( true )
{
    //----------------------------------------------------------------
    // upchuck() computes our kurtosis, automatically computing upstream
    // dependencies connected to it using =^ (e.g., FFT)
    //----------------------------------------------------------------
    kurtosis.upchuck() @=> UAnaBlob blob;
    // print out Kurtosis
    <<< blob.fval(0) >>>;

    // advance time
    HOP => now;
}
