// verify cascading destructors; in order for this to happen
// Objects need to release member variables upon clean-up

class Foo
{
    fun @destruct()
    {
        // if it prints that means destructor invoked
        <<< "success" >>>;
    }
}

class Bar
{
    // an object
    Foo foo;
}

// make a Bar
Bar bar;

// when bar goes out of scope, its content should be released as well
