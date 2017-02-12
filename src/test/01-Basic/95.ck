// check that function calls don't corrupt stack variables
// (no segfaults please)
spork ~ {
    2 => float f;
    spork ~ { 
        f * Math.random2f(1.1,1.2) => f;
    };
    1::samp => now;
    if( f > 2 ) <<< "success" >>>;
};

2::samp => now;
