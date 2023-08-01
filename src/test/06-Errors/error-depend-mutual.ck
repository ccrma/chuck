// mutual recursion + dependency tracking

// this should not work
foo();

// shared variable init
5 => int n;

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
