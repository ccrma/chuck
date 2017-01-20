// allocate and unchuck object in sporked code block
spork ~ {
    TriOsc t => dac;
    1::samp => now;
    t =< dac;
};

2::samp => now;
<<< "success" >>>;
