// input and three echoes
adc => Echo e1 => Echo e2 => Echo e3 => dac;
500::ms => e1.max => e2.max => e3.max;
500::ms => e1.delay => e2.delay => e3.delay;

// time loop
while( true ) 1::second => now;
