// Listing 5.5 Function to cut gain (or any float) in half

SinOsc s => dac;                    // (1) Oscillator to test halfGain function

// our function
fun float halfGain( float originalGain ) // (2) Defines the halfGain function
{
    return (originalGain*0.5);
}

// remember that .gain is a function built into SinOsc
<<< "Full Gain: ", s.gain() >>>;    // (3) Prints initial gain of SinOsc
second => now;

// call halfGain()
halfGain(s.gain()) => s.gain;            // (4) Prints new SinOsc gain
<<< "Gain is now half: ", s.gain() >>>;  //     after cutting it in half
second => now;                           
