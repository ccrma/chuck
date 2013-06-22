// Sound Network
SinOsc s => dac;

// Global Variables
[60,62,63,65,63,64,65,58,57] @=> int A[];

// Function 
fun void swell(float begin, float end, float grain)
{
    // This Function is a volume sweller
    
    // Swell UP
    for (begin => float j; j < end; j+grain => j)
    {
        j => s.gain; // sets volume
        .01::second => now; // move time
    } 
    
    // Swell Down
    for (end => float j; j > begin; j - grain => j)
    {
        j => s.gain; // sets volume
        .01::second => now;
    }
}

// Main Program
for( 0 => int i; i < A.cap(); i++)
{
    Std.mtof(A[i]) => s.freq; // Sets Freq using Array A
    swell(.2, 1.0, .01); // Calls Function to control volume
    .1::second => now; // Advance Time
}


