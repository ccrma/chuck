// function return stress test (run with --silent)

// recursion max depth
25 => int MAX_DEPTH;

// f1
fun int f1( int depth )
{
    if( depth >= MAX_DEPTH ) return 1;
    if(maybe) return f2( depth+1 );
    return 1;
}

// f2
fun int f2( int depth )
{
    if( depth >= MAX_DEPTH ) return 2;
    if(maybe) return f1( depth+1 );
    return 2;
}

// test duration
now + 5::second => time later;
while(now < later)
{
    f1(1); f2(1);
    1::samp => now;
}

// if we got here then yay
<<< "success" >>>;
