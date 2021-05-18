// xcorr.ck: calculting how much delay is applied to a signal
// author: Curtis Ullerich

SndBuf signal => dac;
signal => DelayL delay => dac;
"special:dope" => signal.read;
Math.random2f(100, 400)$int => int delayMs;
delayMs::ms => delay.max => delay.delay;
0.8 => signal.gain;
signal => Flip f1 =^ XCorr xcorr => blackhole;
delay => Flip f2 =^ xcorr;
(second/samp)$int => int srate;

// will get bumped to the next power of two for the actual Fourier transform
signal.samples() + delayMs*srate/1000 => float size;
size$int => f1.size => f2.size;
signal.samples()::samp + delayMs::ms => now;
xcorr.upchuck();
0 => int maxI;
for (0 => int i; i < size; i++) {
  if (xcorr.fval(i) >= xcorr.fval(maxI)) {
    i => maxI;
  }
}
<<< "    actual delay was", delayMs, "milliseconds" >>>;
<<< "calculated delay was", 1000*maxI/srate$float, "milliseconds" >>>;
1::second => now;
