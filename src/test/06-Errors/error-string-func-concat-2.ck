// error case: check for [function] + string
// https://github.com/ccrma/chuck/issues/162

class Foo
{
    // a function
    fun static void bar()
    { }
}

// function in LHS position
Foo.bar + " ";

