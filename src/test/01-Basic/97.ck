// check classic sporks still working
spork ~ f();

fun void f () {
    TriOsc t => dac;
    1::samp => now;
    t =< dac;
};

2::samp => now;
<<< "success" >>>;
