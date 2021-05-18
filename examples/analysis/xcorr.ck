// name: xcorr.ck
// desc: calculting how much delay is applied to a signal
// author: Curtis Ullerich

// signal source
SndBuf signal => dac;
// delayed signal
signal => DelayL delay => dac;
// load sound
"special:dope" => signal.read;
// randomize delay
Math.random2f(100, 400)$int => int delayMs;
// set delay
delayMs::ms => delay.max => delay.delay;
// set gain
0.8 => signal.gain;

// set analysis
signal => Flip f1 =^ XCorr xcorr => blackhole;
delay => Flip f2 =^ xcorr;
(second/samp)$int => int srate;

// will get bumped to the next power of two for the actual
// Fourier transform
signal.samples() + delayMs*srate/1000 => float size;
// FFT sizes
size$int => f1.size => f2.size;
// wait
signal.samples()::samp + delayMs::ms => now;
// analyze!
xcorr.upchuck();

// iterate
0 => int maxI;
for (0 => int i; i < size; i++)
{
    // look for max
    if (xcorr.fval(i) >= xcorr.fval(maxI)) {
        i => maxI;
    }
}

// print
<<< "    actual delay was", delayMs, "milliseconds" >>>;
<<< "calculated delay was", 1000*maxI/srate$float, "milliseconds" >>>;
// wait
1::second => now;
