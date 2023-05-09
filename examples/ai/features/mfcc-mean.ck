//---------------------------------------------------------------------
// name: mfcc-mean.ck
// desc: mel-frequency cepstral coefficients
//       this example aggregates multiple MFCC frames; compute mean
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// to see API, uncomment:
// MFCC.help();
//
// author: Yikai Li and Ge Wang
// date: Spring 2023
//---------------------------------------------------------------------

// analysis patch
adc => FFT fft =^ MFCC mfcc => blackhole;

// FFT size
2048 => fft.size;
// FFT hop size
512 => int hopSize;
// number of filters in MEL space
10 => mfcc.numFilters;
// number of MFCC coefficients to compute per frame
13 => mfcc.numCoeffs => int nCoeffs;
// used to compute the frame size
.1::second => dur aggregateTime;
// set FFT window
Windowing.hamming(fft.size()) => fft.window;
// current sample sample
second / samp => float sampleRate;
// this should already be set to the current chuck srate
// but could be overridden, if desired
// sampleRate => mfcc.sampleRate;

// calculate the number of frames
(aggregateTime/second * sampleRate / hopSize + .5) $ int => int nFrames;

// print out
chout <= IO.newline();
<<< "--------------------------------------------", "" >>>;
<<< "num filters:", mfcc.numFilters() >>>;
<<< "num MFCC coefficients:", mfcc.numCoeffs() >>>;
<<< "agregrateTime:", aggregateTime/second, "seconds" >>>;
<<< "num frames:", nFrames >>>;
<<< "--------------------------------------------", "" >>>;
chout <= IO.newline();

// per-frame MFCC coefficients
float mfccs[nFrames][nCoeffs];
// average of MFCC coefficients across all frames
float mfccMean [nCoeffs];

// let one FFTsize of time pass
fft.size()::samp => now;

// analysis loop
while( true )
{
    // aggregate mfcc features
    for( 0 => int fi; fi < nFrames; fi++ )
    {
        // triggers MFCC and its dependencies (e.g., FFT)
        mfcc.upchuck();
        // get contents
        for( 0 => int i; i < nCoeffs; i++ )
        {
            // copy the resulting coeffs
            mfcc.fval(i) => mfccs[fi][i];
        }
        // advance time by one hop
        hopSize::samp => now;
    }

    // compute mean
    for( 0 => int i; i < nCoeffs; i++ )
    {
        // zero out
        0.0 => mfccMean[i];
        // sum
        for (0 => int j; j < nFrames; j++)
        {
            mfccs[j][i] +=> mfccMean[i];
        }
        // divide by frames
        nFrames /=> mfccMean[i];
        // print
        chout <= mfccMean[i] <= " ";
    }

    // new line
    chout <= IO.newline();
}
