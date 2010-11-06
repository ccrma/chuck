// our patch
adc => FFT fft =^ RollOff roff50 => blackhole;
// a second rolloff
fft =^ RollOff roff85 => blackhole;

// set parameters
1024 => fft.size;
// set hann window
Windowing.hann(1024) => fft.window;
// compute srate
second / samp => float srate;
// set rolloff percents
.5 => roff50.percent;
.85 => roff85.percent;

// control loop
while( true )
{
    // upchuck: take fft then the rolloffs
    roff50.upchuck();
    roff85.upchuck();
    // print out
    <<< "50% rolloff:", roff50.fval(0) * srate / 2,
        "85% rolloff:", roff85.fval(0) * srate / 2 >>>;

    // advance time
    fft.size()::samp => now;
}
