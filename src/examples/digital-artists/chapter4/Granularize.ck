// Sound Network
SndBuf click => dac;

// SndBuf Preparation
"sounds/snare_01.wav" => click.read; // Open File
click.samples() => click.pos; // Set playhead to end of file so no initial sound
.25 => click.gain;  // set gain of sample

// Function 
fun void granularize(int steps)
{
    click.samples()/steps => int grain; // Samples/STEPS == Grain Size
    Std.rand2(0, click.samples() - grain) + grain => click.pos; // Randomly set Grain Position
    grain::samp => now;
}

// Main Program
while(true)
{
    // Loop Forever calling Granularize
    granularize(20);
}