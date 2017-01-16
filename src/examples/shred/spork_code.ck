// spork a code block, no function definition necessary
spork ~ {
    TriOsc t => dac;
    while( true )
    {
        Math.random2f(100, 200) => t.freq;
        100::ms => now;
    }
} @=> Shred randomFreq; // remember the sporked shred

// wait a little
0.5::second => now;

// stop the sporked child shred
randomFreq.exit();

// wait some more
0.5::second => now;
