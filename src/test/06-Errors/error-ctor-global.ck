// error case: global Object cannot invoke non-default constructors
global Foo foo(6);

// public class def
public class Foo
{
    fun void Foo( int x )
    {
        <<< "constructor:", x >>>;
    }
}