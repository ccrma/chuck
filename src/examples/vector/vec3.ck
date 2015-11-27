// vec3 is a primitive type
@(1,0,0) => vec3 a;
// declare another
@(0,1,0) => vec3 b;

// add them
a + b => vec3 sum;
// difference
a - b => vec3 diff;
// cross product
a * b => vec3 c;

// print sum!
<<< "sum:", sum >>>;
// print difference
<<< "diff:", diff >>>;
// print cross product
<<< "cross product:", c >>>;

// array
[ a, b, @(-1,1,0) ] @=> vec3 group[];
// print them
<<< group[0], group[1], group[2] >>>;

// another v
vec3 v;
// set value
v.set( 2,2,2 );
// print magnitude
<<< "magnitude:", v.magnitude() >>>;
// normalize
v.normalize();
// print vector
<<< "normalize:", v >>>;

// multiply
5 * v => vec3 v2;
// print result
<<< "scalar multiply:", v2 >>>;
