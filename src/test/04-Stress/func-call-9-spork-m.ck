// #2024-func-call-update verification

0 => int N;

// test spork member function from within class definition
class Foo
{
    fun void foo() { N++; spork ~ bar(); me.yield(); }
    fun void bar() { if( N >= 10000 ) <<< "success" >>>; }
}

// test for stack overflow
repeat(10000) { Foo foo; foo.foo(); }
