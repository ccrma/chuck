// check that function calls don't corrupt stack variables
// (no segfaults please)
spork ~ {
    2 => float i;
    spork ~ { 
        i * Math.random2f(1.1,1.2) => i;
    };
    1::samp => now;
};

2::samp => now;
<<< "success" >>>;
