// error case: check for string + [function]
// https://github.com/ccrma/chuck/issues/162

class Foo
{
    // a function
    fun string getFoo()
    {
        return "foo";
    }
}

// make a foo
Foo f;

// add string to function reference (instead of calling function)
<<< " " + f.getFoo >>>;
