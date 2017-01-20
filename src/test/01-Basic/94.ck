// current model is that inner shred gets a *copy* of 
// outer shred's primitives
spork ~ {
    2 => float f;
    spork ~ { 
        f * 1.1 => f;
        if( f != 2.2 ) { <<< "failure" >>>; }
    };
    1::samp => now;
    if( f != 2.0 ) { <<< "failure" >>>; }
};

2::samp => now;
<<< "success" >>>;
