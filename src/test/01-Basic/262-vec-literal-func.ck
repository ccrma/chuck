// (arcane!) unit test for special primitives calling "member" functions from a literal value
// created 1.5.4.2 | #special-primitive-member-func-from-literal

fun void assert( float A, float B, int which )
{
    if( !Math.equal( A, B ) )
    {
        <<< "TEST #", which, "FAILED: expecting", A, "got", B >>>;
        me.exit();
    }
}

// some set up
@(1,2,3) => vec3 v1;
@(1,0,0) => vec3 v2a;
@(0,1,0) => vec3 v2b;
v2a + v2b => vec3 v2sum;

// calling function from literal value
assert( @(1,2,3).magnitude(), v1.magnitude(), 1 );
// calling from intermediate result (which is considered a literal value)
assert( (v2a+v2b).magnitude(), v2sum.magnitude(), 2 );
// calling from intermediate results with literals
assert( (@(1,0,0)+@(0,1,0)).magnitude(), v2sum.magnitude(), 3 );
// literals calling function with literal args
assert( @(1,2,3).dot(@(0,1,0)), 2, 4 );
// various permuatations
assert( @(1,2,3).dot(v2b), 2, 5 );
assert( v1.dot(@(0,1,0)), 2, 6 );
assert( v1.dot(v2b), 2, 7 );
// vec2
assert( @(1,0).dot(@(3,0)), 3, 8 );
// vec4
assert( @(1,0,2,0).dot(@(3,0,1,0)), 5, 8 );

// if we got here then good
<<< "success" >>>;
