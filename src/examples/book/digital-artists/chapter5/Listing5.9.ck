// Listing 5.9 Creating and using a cool granularize() function to chop up a sound file

SndBuf2 click => dac;  // (1) Makes and connects a stereo SndBuf2

// read soundfile,             (2) Loads stereo SndBuf2 with a stereo sound file
me.dir() + "/audio/stereo_fx_01.wav" => click.read;

// function to hack up any sound file
fun void granularize(SndBuf myWav, int steps ) // (3) Defines granularize function
{
    myWav.samples()/steps => int grain;  // (4) Calculates grain size
           // (5) Sets play pointer to random grain location in buffer
    Math.random2(0,myWav.samples() - grain) + grain => myWav.pos;
    grain :: samp => now;
}

// Main Program
while (true)
{
    // call function, time passes there
    granularize(click, 70);
}
