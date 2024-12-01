// example: defining basic class constructors + destructor

// a class
class Foo
{
    // a member variable
    1 => int num;

    // constructor "default"
    fun @construct() { 2 => num; }

    // another constructor
    // (using class name OR @construct is interchaneable)
    fun Foo( int x ) { x => num; }

    // yet another constructor
    fun @construct( int x, int y ) { x*y => num; }

    // yet another constructor 
    fun Foo( int x, int y, int z ) { x*y*z => num; }

    // destructor
    fun @destruct() { <<< "destructor:", this.num >>>; }
}

// constructor "default"
Foo f0;
// constructor "default"
Foo f1();
// another constructor
Foo f2(15);
// yet another constructor
new Foo(8,9) @=> Foo @ f3;
// yet another constructor
new Foo(10,11,12) @=> Foo @ f4;
// print
<<< f0.num, f1.num, f2.num, f3.num, f4.num >>>;
