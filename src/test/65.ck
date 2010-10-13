// sporking

public class X
{
    int i;

    fun void foo() { <<< "foo:", i >>>; }
    fun void bar( float a ) { <<< "bar:", a >>>; }
    fun void go() { spork ~ bar( 1 ); }
    fun static void y() { <<< "success" >>>; }
}

// make x
X x;
2 => x.i;

spork ~ x.foo();
spork ~ x.bar( 3 );
x.go();
spork ~ X.y();

// wait
me.yield();
