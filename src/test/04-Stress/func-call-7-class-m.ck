// #2024-func-call-update verification

0 => int N;

// test calling member function from within class definition
class Foo
{
    fun void foo() { N++; bar(); }
    fun void bar() { if( N >= 100000 ) <<< "success" >>>; }
}

// test for stack overflow
repeat(100000) { Foo foo; foo.foo(); }
