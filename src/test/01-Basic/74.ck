// re: https://lists.cs.princeton.edu/pipermail/chuck-users/2015-May/007919.html
{
    Noise n[1] => Pan2 p => dac;
    p.gain(0.1);
    second => now;
}

<<< "success" >>>;
