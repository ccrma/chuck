// check that inner spork doesn't overwrite memory of outer spork
spork ~ {
    TriOsc t => dac;
    spork ~ { 
        100 => t.freq;
        1::samp => now;
        TriOsc t224 => dac;
        224 => t224.freq;
        <<< t224.freq() >>>;
        2::samp => now;
        <<< t224.freq() >>>;
    };
    TriOsc t223 => dac;
    223 => t223.freq;
    <<< t223.freq() >>>;
    2::samp => now;
    <<< t223.freq() >>>;
    t =< dac;
    1::samp => now;
};

6::samp => now;
