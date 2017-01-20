// check that function calls don't overwrite local variables
// (no segfaults please)
TriOsc t2;
spork ~ {
    TriOsc t => dac;
    spork ~ { 
        while( true ) {
            t.freq() * Math.random2f(0.9, 1.1) => t.freq;
            t2.freq() * Math.random2f(0.9, 1.1) => t2.freq;
            1::samp => now;
        }
    };
    5::samp => now;
    t =< dac;
};

2::samp => now;
t2 => dac;
3::samp => now;
t2 =< dac;
2::samp => now;
<<< "success" >>>;
