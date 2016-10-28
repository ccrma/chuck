// 92.ck : function with default arguments that 
//         are evaluated exactly once for every
//         time default is relied on

int count;

fun time getnow()
{
    100::samp => now;
    1 +=> count;
    return now;
}

fun time bar(5 => int x, getnow() + 100::samp => time n)
{
    return x::samp + n;
}

if (bar(100, now - 100::samp) == now &&
    bar(-100) == now &&
    bar() == (now + 105::samp) &&
    count == 2)
{
    <<<"success">>>;    
}
