// error case: destructors cannot access values outside the class

int y;

class Foo
{
    5 => int x;
    
    fun @destruct()
    {
        <<< "destructor called...", y >>>;
    }
}

Foo foo;