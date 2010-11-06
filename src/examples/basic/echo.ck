// feedforward
adc => Gain g => dac;
// feedback
g => Gain feedback => DelayL delay => g;

// set delay parameters
.75::second => delay.max => delay.delay;
// set feedback
.5 => feedback.gain;
// set effects mix
.75 => delay.gain;

// infinite time loop
while( true ) 1::second => now;

