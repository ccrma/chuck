// Listing 4.5 Playing different sound files (method 2) using multiple SndBufs

// Playing multiple sound files (method 2)
// by ChucK Programmer, August, 2023
SndBuf snare[3];          // (1) Declares an array of three SndBufs...

// for fun, let's pan them to left, right, center
snare[0] => dac.left;
snare[1] => dac;
snare[2] => dac.right;

// (2) pre-load three different sound files at the beginning
me.dir()+"/audio/snare_01.wav" => snare[0].read;
me.dir()+"/audio/snare_02.wav" => snare[1].read;
me.dir()+"/audio/snare_03.wav" => snare[2].read;

// infinite loop
while (true)
{
    // (2) pick a random number between 0 and #files
    Math.random2(0,snare.cap()-1) => int which;

    // play that sample
    0 => snare[which].pos;

    // let it play
    0.5 :: second => now;
}
