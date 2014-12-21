// Listing 5.16 Computing factorial by using recursion

fun int factorial( int x)
{
    if ( x <= 1 )
    {
        // when we reach here, function ends
        return 1;
    }
    else
    {
        // recursive function calls itself
        return (x*factorial(x-1));
    }
}

// Main Program, call factorial
<<< factorial(4) >>>;

