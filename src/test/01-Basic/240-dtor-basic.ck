// verify destructo is called

class Foo
{
    // destructor
    fun @destruct()
    {
        <<< "destructor called...", "" >>>;
    }
}

// instance
Foo foo;

// destructor should be called when foo goes out of scope
