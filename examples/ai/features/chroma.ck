//---------------------------------------------------------------------
// name: chroma.ck
// desc: chroma unit anlyzer, used for feature extraction of pitch data
//       e.g., a set of frequencies
//
// uncomment for Chroma API:
// Chroma.help();
//
// author: Yikai Li
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Winter 2023
//---------------------------------------------------------------------

// synthesis / analysis network
SinOsc s => FFT fft =^ Chroma chroma => blackhole;

// set FFT size
2048 => fft.size;
// set window type and size
Windowing.hann(fft.size()) => fft.window;
// our hop size (how often to perform analysis)
fft.size()::samp => dur HOP;

// pitch names
["C4", "C#", "D4", "D#", "E4", "F4", "F#", "G4", "G#", "A4", "A#", "B4"] @=> string note_names[];
// test frequencies
[262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494] @=> int freqs[];

// print header
cherr <= "freq ";
for (0 => int i; i < 12; i++) {
    cherr <= note_names[i] + " ";
}
cherr <= IO.newline();

// let one FFT-size of time pass (to buffer)
fft.size()::samp => now;

// extract chroma features
for( 0 => int i; i < freqs.size(); i++ )
{
    // set frequency for test
    freqs[i] => s.freq;

    //----------------------------------------------------------------
    // upchuck() computes our Chroma, automatically computing upstream
    // dependencies connected to it using =^ (e.g., FFT)
    //----------------------------------------------------------------
    chroma.upchuck();

    // print features
    cherr <= freqs[i] <= ": ";
    for( 0 => int j; j < chroma.fvals().size(); j++ )
    {
        Math.round(chroma.fval(j) * 100) => float tmp;
        cherr <= (tmp < 10 ? " ":"") <= tmp <= " ";
    }
    cherr <= IO.newline();

    // advance time
    HOP => now;
}
