// current model is that inner shred gets a *copy* of 
// outer shred's primitives
spork ~ {
    2 => float f;
    spork ~ { 
        f * 1.1 => f;
        <<< f >>>;
    };
    1::samp => now;
    <<< f >>>;
};

2::samp => now;