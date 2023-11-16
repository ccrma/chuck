// verify that default constructor is invoked with or without ()

// a class
class Foo
{
    // a member var
    int x;
    
    // default constructor
    fun void Foo()
    {
        5 => x;
    }
}

// instantiate with and without ()
new Foo @=> Foo @ f1;
new Foo() @=> Foo @ f2;
Foo f3;
Foo f4();

// test
if( f1.x == 5 && f2.x == 5 && f3.x == 5 && f4.x == 5 )
    <<< "success" >>>;
