// func_overload_multi.ck : function overloading

fun void bar()
{
    <<<1>>>;
}

fun void bar( int i )
{
    <<<2>>>;
}

fun void bar( string s )
{
    <<<3>>>;
}

class X
{
    fun void foo()
    { <<<"X.foo()">>>; }

    fun void foo( int i )
    { <<<"X.foo(int)">>>; }

    fun void foo( int a, int b )
    { <<<"X.foo(int, int)">>>; }
}

class Y extends X
{
    fun void foo()
    { <<<"Y.foo()">>>; }

    fun void foo( int a, int b )
    { <<<"Y.foo(int, int)">>>; }
}

Y y;
y.foo();
y.foo( 1 );
y.foo( 1, 2 );

X x;
x.foo( 1, 2 );

bar();
bar(1);
bar("a");
