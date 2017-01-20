// nested sporked code blocks more than 2 deep
TriOsc t;
spork ~ {
    TriOsc t2 => dac;
    spork ~ { 
        TriOsc t3 => dac;
        spork ~ {
            1::samp => now;
            800 => t3.freq;
            while( true )
            {
                maybe => t.gain => t2.gain => t3.gain;
                2::samp =>now;
            }
        };
        while( true ) {
            t.freq() * Math.random2f(0.9, 1.1) => t.freq;
            t2.freq() * Math.random2f(0.9, 1.1) => t2.freq;
            4::samp => now;
        }
    };
    20::samp => now;
    t2 =< dac;
};

4::samp => now;
t => dac;
16::samp => now;
t =< dac;
2::samp => now;
<<< "success" >>>;
