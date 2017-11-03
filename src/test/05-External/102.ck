external SinOsc s => dac;

fun void foo()
{
    external SinOsc s;
    1::second => now;
    s =< dac;
}

foo();

<<< "success" >>>;
