// check that function calls don't overwrite local vars
// (no segfaults please)
TriOsc tGlobal;
spork ~ {
    TriOsc t => dac;
    spork ~ { 
        TriOsc t2;
        t.freq() + Math.random2f ( 100, 200 ) => t.freq;
    };
    1::samp => now;
    t =< dac;
};

2::samp => now;
<<< "success" >>>;
