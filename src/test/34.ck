// 34.ck : function overloading

// declare class
class X
{
    // delcare foo
    fun void foo( int a )
    { <<<a>>>; }

    // delcare foo with different arguments
    fun void foo( int a, int b )
    { <<<"success">>>; }
}

// extend X
class Y extends X
{
    fun void foo( int a )
    { <<<a+1>>>; }
}

// instantiate a Y
Y y;

// call the right one, hopefully
y.foo( 3, 5 );
