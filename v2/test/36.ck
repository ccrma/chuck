// 36.ck : function overloading across inheritance, with overriding

// base calss
class X
{
    fun void foo( int a )
    { <<<a>>>; }

    fun void foo( int a, int b )
    { <<<a+b>>>; }
}

// class Y
class Y extends X
{
    // override X's foo( int )
    fun void foo( int a )
    { <<<a+1>>>; }

    fun void foo( int a, int b )
    { <<<a+b+1>>>; }
}

// class Z
class Z extends Y
{
    // override Y's foo( int, int )
    fun void foo( int a, int b )
    { <<<"success">>>;}
}

// instantiate a Z
Z x;

// call the foo
x.foo(3,5);
