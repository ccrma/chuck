// error case: same as error-depend-class-func.ck but this
// specifically calling bar() on the same line...

public class Foo
{
    bar() => vec2 foo;
    
    fun vec2 bar()
    {
        return foo;
    }
}

Foo foo;
<<< foo.foo >>>;
