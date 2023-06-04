// name: wvout-24bit.ck
// desc: example of writing 24-bit audio files using WvOut
//
// requires: chuck-1.5.0.1 or higher
// date: Summer 2023

// output
SndBuf buffy => WvOut wout => blackhole;
// monitor the audio
buffy => dac;

// load sound
"special:dope" => buffy.read;

// open WAV file to write to, with particular bit depth
// possible bit depths include:
// IO.INT16, IO.INT24, IO.INT32, IO.FLOAT32, IO.FLOAT64
("wvout-24bit.wav", IO.INT24) => wout.wavFilename;

// print message
cherr <= "outputting '" <= wout.filename() <= "'..." <= IO.nl();

// repeat 3 times
repeat( 3 )
{
    // seek to beginning
    0 => buffy.pos;
    // advance time
    buffy.samples()::samp => now;
}
