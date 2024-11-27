// #2024-func-call-update verification
// test spork static function from within a class
class Foo
{
    static int N;
    fun static void foo() { N++; spork ~ bar(); me.yield(); }
    fun static void bar() { if( N >= 10000 ) <<< "success" >>>; }
}

0 => Foo.N;
// test for stack overflow
repeat(10000) Foo.foo();
