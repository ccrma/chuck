// Loading a stereo file, accessing its samples (both channels)
// and setting the playback channel.
// ---
// This example uses SndBuf which is a mono UGen,
// thus only 1 channel at the time can be played back.
// SndBuf2 is its stereo alter ego!



SndBuf buffer => dac;


// Below, uncomment the row with the filename that matches
// your current sample rate settings (44100Hz, 48000Hz, etc.).
// To check your settings launch your terminal and type the following:
//    chuck --probe
// or check the audio settings in miniAudicle


// load a stereo file
//me.dir() + "../data/audio/stereo_fx_01_44_1_kHz.wav" => string filename;
me.dir() + "../data/stereo_fx_01_48_kHz.wav" => string filename;
buffer.read(filename);

// Check the sample has been loaded
if (buffer.samples() == 0)
{
    chout <= "\nIssue loading the file - check file name and path" <= IO.nl();
    me.exit();
}
buffer.rate(0);  // stop playback



// The method .samples() returns the total number of sample frames.
// That means that the actual number of samples contained in the audio file is:
// samples() * number_of_channels
chout <= "Number of samples per channel:\t";
chout <= buffer.samples() <= IO.nl();
chout <= "Total number of samples:\t";
chout <= buffer.samples() * buffer.channels() <= IO.nl();
chout <= "Number of channels:\t\t" <= buffer.channels() <= IO.nl();
2::second => now;



// samples in stereo files are accessed the following way:
// buffer[0] = left_channel[0]
// buffer[1] = right_channel[0]
// buffer[2] = left_channel[1]
// buffer[3] = right_channel[1]
// buffer[4] = left_channel[2]
// ...

// print to the console the first 25 samples - both left and right channels
chout <= "\nFirst 25 samples in the audio file\n---" <= IO.nl();
for (0 => int c; c < 25; c++)
{
    buffer.valueAt(c*2)     => float lx_sample;      // left channel
    buffer.valueAt(c*2 + 1) => float rx_sample ;     // right channel
    chout <= "[" <= c <= "]\tLEFT: " <= lx_sample <= "\t| RIGHT: " <= rx_sample <= IO.nl();
}
second => now;



// set channel to playback
chout <= "\nPlaying back left channel" <= IO.nl();
buffer.pos(0);
buffer.channel(0);  // LEFT
buffer.rate(1);
buffer.length() + second => now;

chout <= "Playing back right channel" <= IO.nl();
buffer.pos(0);
buffer.channel(1);  // RIGHT
buffer.rate(1);
buffer.length() => now;

