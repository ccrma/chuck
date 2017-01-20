// set object variable in sporked code block
TriOsc t;

spork ~ {
    t => dac;
    while(true) {
        Math.random2f(100,400) => t.freq;
        1::samp => now;
    }
    
};

5::samp => now;
t =< dac;
5::samp => now;
<<< "success" >>>;
