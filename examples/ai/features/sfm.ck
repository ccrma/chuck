//---------------------------------------------------------------------
// name: sfm.ck
// desc: Spectral Flatness Measure
//
// uncomment for SFM API:
// SFM.help();
//
// author: Yikai Li
//         Ge Wang
// date: Winter 2023
//---------------------------------------------------------------------

// synthesis / analysis network
SinOsc s => FFT fft =^ SFM sfm => blackhole;

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
    //----------------------------------------------------------------
    // upchuck() computes SFM, automatically computing upstream
    // dependencies connected to it using =^ (e.g., FFT)
    //----------------------------------------------------------------
    sfm.upchuck();
    // print
    cherr <= "SFM: ";
    // print the SFM results
    for( int i; i < sfm.fvals().size(); i++ )
    {
        Math.round(sfm.fval(i) * 10) / 10 => float tmp;
        cherr <= tmp <= (tmp == 1 || tmp == 0?".0":"") <= " ";
    }
    // endline
    cherr <= IO.newline();
    // advance time
    HOP => now;
    // randomize frequency
    Math.random2f(20, 1000) => s.freq;
}
