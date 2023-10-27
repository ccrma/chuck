// verify ref counting for `new` unary operator
// NOTE as of 1.5.1.8 `new` always add-ref then release at
// end of statement, so if the ref is retained, it will be 
// properly disposed by statement's end

// a class
class Foo
{
    // member
    5 => int x;

    // print stuff
    fun Foo get()
    {
        // <<< Machine.refcount( this ) >>>;
        return this;
    }
}

// should print 5
<<< (new Foo).x, "" >>>;

// should be 2 ref counts: `new` and get()
<<< Machine.refcount((new Foo).get()), "" >>>;
