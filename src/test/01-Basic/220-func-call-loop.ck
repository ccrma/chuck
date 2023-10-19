// verify func-call returning objects is properly refcounted
// as part or the stmt's object release mechanism | 1.5.1.7

// a class
class Foo
{
    5 => int x;
}

// a file-global instance
Foo foo;
// a function that returns foo
fun Foo bar() { return foo; } 

// since bar() returns an object every iteration, verify the
// stmt reference count is correct
while( bar().x )
{
    // do this a few times
    bar().x--;
}

// check
if( Machine.refcount(foo) == 1 ) <<< "success" >>>;
