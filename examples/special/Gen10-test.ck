// gen10 sinusoidal lookup table
// ported to ChucK from rtcmix by dan trueman
// see luke's page: http://www.music.columbia.edu/cmix/makegens.html#05

Phasor drive => Gen10 g10 => dac;

// can also lookup table values directly:
//     g10.lookup(index);
//     with index:[0,1]

// load up the coeffs; amplitudes for successive partials
g10.coefs( [1., 0.5, 0.3, 0.2] );

// set frequency for reading through table
drive.freq(440.);

// infinite time loop
while (true)
{
    // advance time
    500::ms => now;
    // nothing needed to do, so lets look
    // up some random parts of the table!
    <<< g10.lookup( Math.random2f(0,1) ) >>>;
}
