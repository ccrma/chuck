// kill a sporked code shred

// this code modifies the frequency of a triosc some,
// then stops modifying it and lets it sound for a bit

// note: as of this writing, UGens are not disconnected
// when their Shred is exited. Thus, these first two lines 
// could be interchanged and the output would sound identical.
spork ~ {
    TriOsc t => dac;
    while( true )
    {
        Math.random2f(100, 200) => t.freq;
        2::samp => now;
    }
} @=> Shred randomFreq;

10::samp => now;

// stop the child shred
randomFreq.exit();

10::samp => now;
<<< "success" >>>;
