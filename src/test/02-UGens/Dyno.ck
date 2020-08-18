Dyno u => blackhole;
1::samp => now;

10::ms => u.attackTime;
100::ms => u.releaseTime;

if(!dur_eq(u.attackTime(), 10::ms))
{
    <<< "failure" >>>;

    me.exit();
}

if(!dur_eq(u.releaseTime(), 100::ms))
{
    <<< "failure" >>>;
    
    me.exit();
}

u =< blackhole;
null @=> u;

<<< "success" >>>;

fun int dur_eq(dur a, dur b)
{
    return Math.fabs((a-b)/second) < 0.001*(a/second);
}
