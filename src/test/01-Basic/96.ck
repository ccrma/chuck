// check that inner spork doesn't overwrite memory of outer spork
spork ~ {
    TriOsc t => dac;
    spork ~ { 
        100 => t.freq;
        1::samp => now;
        TriOsc t224 => dac;
        224 => t224.freq;
        if( t224.freq() != 224 ) { <<< "failure" >>>; }
        2::samp => now;
        if( t224.freq() != 224 ) { <<< "failure" >>>; }
    };
    TriOsc t223 => dac;
    223 => t223.freq;
    if( t223.freq() != 223 ) { <<< "failure" >>>; }
    2::samp => now;
    if( t223.freq() != 223 ) { <<< "failure" >>>; }
    t =< dac;
    1::samp => now;
};

6::samp => now;
<<< "success" >>>;
