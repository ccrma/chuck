// error case: should not be able to overload reserved overloading
// e.g., int + int

// what could possibly go wrong
fun int @operator +( int lhs, int rhs )
{
    // we live in a chaotic world
    return lhs - rhs;
}

// should get here
<<< 1 + 2 >>>;
