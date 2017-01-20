// call the same sporked code block many times
// with shred cleanup between each one
fun void sporkFun(float f) {
    spork ~ {
        TriOsc t => dac;
        f => t.freq;
        1::samp => now;
        t =< dac;
    };
    1::samp => now;
}

sporkFun(400);
1::samp => now;

sporkFun(500);
1::samp => now;

sporkFun(600);
1::samp => now;

sporkFun(800);
2::samp => now;
<<< "success" >>>;
