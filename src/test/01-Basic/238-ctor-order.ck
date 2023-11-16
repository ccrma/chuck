// verify pre-ctor is invoked even when object is declared before
// class definition

// declare before class def
Foo foo;
// check values
if( foo.x == 5 && foo.y == 2 ) <<< "success" >>>;

// class def
class Foo
{
    5 => int x;
    0 => int y;
    
    // constructor
    fun @construct() { 2 => y; }
}