// testing Math.euclidean, which has three overloaded variants
// added 1.5.0.0

@(0,0,0) => vec3 a;
@(1,1,1) => vec3 b;

@(0,0,0,0) => vec4 aa;
@(1,1,1,1) => vec4 bb;

[0.0, 0, 0, 0, 0] @=> float aaa[];
[1.0, 1, 1, 1, 1] @=> float bbb[];

Math.euclidean( a, b ) => float y;
Math.euclidean( aa, bb ) => float yy;
Math.euclidean( aaa, bbb ) => float yyy;

// the answer
Math.sqrt(3) => float answer1;
Math.sqrt(4) => float answer2;
Math.sqrt(5) => float answer3;

// use Math.equal() to test for equality
if( Math.equal( y, answer1 ) &&
    Math.equal( yy, answer2 ) &&
    Math.equal( yyy, answer3 ) )
{
    <<< "success" >>>;
}

