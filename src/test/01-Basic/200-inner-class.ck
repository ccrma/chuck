// experimental feature: nested class definitions

class Outer
{
    1 => int x;

    // this works?!
    class Inner
    {
        2 => int y;
        
        fun void foo()
        {
            <<< "in inner class, y =", y >>>;
        }
    }
    
    // this works?!
    Inner inner;

    fun void bar()
    {
        3 => inner.y;
        inner.foo();

        Inner barin;
        5 => barin.y;
        barin.foo();
    }
}

Outer outer;
<<< outer.x >>>;
// this works?
<<< outer.inner.y >>>;
// this works?
outer.inner.foo();
// call bar, which operates with Outer and Inner
outer.bar();


// this doesn't work, but should it?
// Outer.Inner x;
