// audio synthesis network
// a sine wave oscillator (foo) connected to the audio output (dac)
SinOsc foo => dac;

// an infinite time loop
while( true )
{
    // generate a random number between 30 and 1000;
    // set this number to be foo's frequency
    Math.random2f(30,1000) => foo.freq;
    
    // advance chuck time by precisely 100 milliseconds
    // this mechanism implies the control rate
    100::ms => now;
}