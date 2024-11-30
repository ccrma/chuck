// static variables & initialization example
// requires: chuck-1.5.4.3 or higher
//
//-----------------------------------------
// STATIC VARAIBLE INITIALIZATION SEMANTICS
//-----------------------------------------
// static variable can only be DECLARED:
// a) within a class definition
// b) outside of any class function defintion
// c) at the outer-most class scope (cannot be in nested { })
//-----------------------------------------
// COMPILE-TIME checks:
// 1) a statement containing a static variable declaration
//    CANNOT access member data/functions
// 2) otherwise, a statement can access both static and member data.
// 3) a statement containing a static variable declaration
//    CANNOT access local (i.e., outside of class def) vars or funcs
//    even in non-public classes
//-----------------------------------------
// RUNTIME checks:
// 4) static initialization statements are run in immediate mode
//    meaning no time advances; this is enforced at runtime; any
//    time advances, including waiting on events, will result in
//    a runtime exception
//-----------------------------------------

// a class
public class Foo
{
    // int
    1 => static int S_INT;
    // float
    2 => static float S_FLOAT;
    // dur
    3::second => static dur S_DUR;
    // time
    now + 4::second => static time S_TIME;
    // vec3
    @(5,6,7) => static vec3 S_VEC3;
    // array
    [8,9,10,11] @=> static int S_INT_ARRAY[];
    // string
    static string S_STRING("12");
    // ugen
    static SinOsc S_SINOSC(440);
}

// print static variables
<<< Foo.S_INT >>>;
<<< Foo.S_FLOAT >>>;
<<< Foo.S_DUR / second >>>;
<<< (Foo.S_TIME-now) / second >>>;
<<< Foo.S_VEC3 >>>;
for( auto i : Foo.S_INT_ARRAY ) <<< i >>>;
<<< Foo.S_STRING >>>;
<<< Foo.S_SINOSC.freq() >>>;
