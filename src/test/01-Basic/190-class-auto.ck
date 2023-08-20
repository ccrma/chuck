// auto can be used inside class defs EXCEPT
// top-level class members cannot be declared as auto
// introduced in 1.5.1.1 as class vars could be accessed
// from anywhere but currently there is no mechanism to
// resolve the 'auto' until type checking stage...

// class def
class Foo
{    
    // ok
    for( auto x : [1,2,3] ) { <<< x >>>; }
    
    // ok
    fun void bar()
    {
        4 => auto y;
        <<< y >>>;
    }

    // ok
    if( true ) { 5 => auto x; }
   
    // not ok
    // 3 => auto m_var;
}

// instantiate foo
Foo foo;
// call bar()
foo.bar();
