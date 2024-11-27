// #2024-func-call-update verification

// test constructor instruction stack alignment
true => int OK;

// class definition
class Foo
{
    int N;
    // constructor
    fun Foo(int n) { n => N; }
}

// test stack overflow
repeat( 100000 )
{ Foo foo(5); if( foo.N != 5 ) { false => OK; break; } }

// print
if( OK ) <<< "success" >>>;
else <<< "noooooo" >>>;