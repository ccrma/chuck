// allocate object in sporked code block
spork ~ {
    TriOsc t => dac;
    1::samp => now;
};

2::samp => now;
<<< "success" >>>;
