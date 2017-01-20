// basic object access in sporked code block
TriOsc t;

spork ~ {
    t => dac;
    1::samp => now;
};

2::samp => now;
<<< "success" >>>;
