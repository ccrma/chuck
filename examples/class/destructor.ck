// example of class destructor

// a class
class Foo
{
    // class destructor
    fun @destruct()
    {
        <<< "destructor called...", "" >>>;
    }
}

Foo foo;
// when `foo` goes out of scope, the destructor should be called...
