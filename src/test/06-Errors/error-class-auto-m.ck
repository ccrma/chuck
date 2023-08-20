// error case: top-level class vars cannot be declared as auto
// member (instance) edition
//
// introduced in 1.5.1.1 as class vars could be accessed
// from anywhere but currently there is no mechanism to
// resolve the 'auto' until type checking stage...

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

    // ok
    { 6 => auto x; }
   
    // not ok *** compiler should report error
    7 => auto m_var;

    // not ok *** compiler should report error
    // 8 => static auto m_var;
}
