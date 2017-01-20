// call function that sporks code block multiple times
// including sporking said function
fun void sporkFun(float f) {
    spork ~ {
        TriOsc t => dac;
        f => t.freq;
        2::samp => now;
        t =< dac;
    };
    2::samp => now;
}

sporkFun(400);
2::samp => now;

// spork a fun that sporks a code block
spork ~ sporkFun(200);
1::samp => now;

// don't spork the fun, but the fun still sporks a code block
sporkFun(800);
4::samp => now;
<<< "success" >>>;
