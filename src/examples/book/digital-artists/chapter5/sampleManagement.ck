// sample management

// sound chain
SndBuf snare => dac;

// array of strings (paths)
string snare_samples[3]; 

// load array with file paths of snare samples
me.dir()  + "/audio/snare_01.wav" => snare_samples[0];
me.dir()  + "/audio/snare_02.wav" => snare_samples[1];
me.dir()  + "/audio/snare_03.wav" => snare_samples[2];

// how many samples?
<<< snare_samples.cap() >>>;
 
// infinite loop 
while ( true )
{
    // select a random sample to play
    Math.random2(0, snare_samples.cap() -1) => int which; // 0,1 or 2
    snare_samples[which] => snare.read; // read in the sample
    250::ms => now; // advance time and play sample
}

