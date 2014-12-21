// Listing 4.8 Making a drum machine with SndBufs in ChucK

// Drum Machine, version 1.0
// by block-rockin programmer, Dec 31, 1999
// SndBufs for kick (bass) and snare drums
SndBuf kick => Gain master => dac; // (1) SndBuf into a master mixer Gain into dac
SndBuf snare => master;            // (2) Another SndBuf into master mixer

// load up some samples of those
me.dir()+"/audio/kick_01.wav" => kick.read;   // (3) Loads your sound files
me.dir()+"/audio/snare_01.wav" => snare.read;

while (true)
{
    // Beat 1, play kick drum alone // (4) Every even beat, play kick only
    0 => kick.pos;
    0.6 :: second => now;

    // play both drums on Beat 2  // (5) All beats, play both kick and snare
    0 => kick.pos;
    0 => snare.pos;
    0.6 :: second => now;
}
