// mutual recursion + dependency tracking

// shared variable init
5 => int n;

// this should work
foo();

// foo(), calls bar()
fun void foo()
{
    // condition
    if( n > 0 )
    {
        n--;
        bar();
    }
}

// bar(), calls foo()
fun void bar()
{
    // condition
    if( n > 0 )
    {
        n--;
        foo();
    }
}

if( n == 0 ) <<< "success" >>>;
