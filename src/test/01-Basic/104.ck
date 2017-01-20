// nested sporked code blocks (no segfaults please)
TriOsc tGlobal;
200 => tGlobal.freq;
spork ~ {
    TriOsc t => dac;
    400 => t.freq;
    
    spork ~ { 
        if( t.freq() != 400 ) { <<< "failure" >>>; }
    };
    1::samp => now;
    t =< dac;
};

2::samp => now;
if( tGlobal.freq() != 200 ) { <<< "failure" >>>; }
<<< "success" >>>;
