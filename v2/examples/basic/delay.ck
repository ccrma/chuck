// patch
adc => DelayL delay => dac;

// set delay parameters
.75::second => delay.max => delay.delay;

// infinite time loop
while( true ) 1::second => now;

