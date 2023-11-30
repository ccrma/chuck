// test passing objects to constructors defined in chuck

// class
class Foo
{
    fun Foo( Bar g )
    {
        <<< "ref:", Machine.refcount(g), "num:", g.num >>>;
    }
}

class Bar
{
    int num;
    
    fun Bar(int n)
    { 
        n => num; 
        <<< "@construct:", n >>>; 
    }
    
    fun @destruct()
    {
        <<< "@destruct:", num >>>;
    }
}

Bar s(1);

<<< "ref:", Machine.refcount(s) >>>;

// invoke from variable
Foo foo1(s);

// invoke with new
Foo foo2( new Bar(2) );

<<< "ref:", Machine.refcount(s) >>>;
