// verifying accessing member declared below where it's used
// from inside a class def

// class def
class Foo
{
    Bar bar;

    fun void go()
    {
        // try to access bar.val, below
        <<< bar.val >>>;
    }
}

// another class
class Bar
{
    int val;
}

// success if no errors
<<< "success" >>>;
