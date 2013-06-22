// Sound Network
SqrOsc s => dac;
SqrOsc t => dac;
SqrOsc u => dac;

// Functions
fun float octave(float originalFreq)
{
    // Calculate octave of input frequency
    return(originalFreq*2); // Octave is 2X original frequency
}

fun float fifth(float originalFreq)
{
    // Calculate fifth of the input frequency
    return(originalFreq*1.5); // Fifth is 1.5X original Freq
}

// Main Function
for (20 => float i; i < 1000; i+.5 => i)
{
    i => s.freq; // Set original frequency for SqrOsc s
    octave(i) => t.freq; // Set octave frequency to SqrOsc t
    fifth(i) => u.freq; // Set fifth to SqrOsc u
    <<< s.freq(), t.freq(), u.freq() >>>; // Print Frequency Values
    10::ms => now; // advance time
}