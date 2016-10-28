// 94.ck : object creation in default arguments

fun void foo(SinOsc osc => UGen u) {
    0 => u.gain;
}

foo();
foo(TriOsc osc);

<<<"success">>>;