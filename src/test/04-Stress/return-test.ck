
fun int f1()
{
    if(maybe) return f2();
}

fun int f2()
{
    if(maybe) return f1();
}

now + 5::second => time later;
while(now < later)
{
    f1(); f2();
    1::samp => now;
}

<<< "success" >>>;