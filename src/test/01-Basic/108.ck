// calling a function that sporks a code block
// and sporking a function that sporks a code block
// (no segfaults please)
fun void sporkFun(float f) {
    spork ~ {
        TriOsc t => dac;
        f => t.freq;
        2::samp => now;
        t =< dac;
    };
    2::samp => now;
}

spork ~ sporkFun(200);
1::samp => now;
sporkFun(800);
4::samp => now;
<<< "success" >>>;
