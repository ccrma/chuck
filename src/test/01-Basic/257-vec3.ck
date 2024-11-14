// test vec3
// including .cross() and .dot(), which were added in 1.5.4.2
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
// another way to do cross product (function)
a.cross(b) => vec3 cross;
// dot product (function)
a.dot(b) => float dot;

// print sum!
<<< "sum:", sum >>>;
// print difference
<<< "diff:", diff >>>;
// print cross product
<<< "cross product:", c >>>;
<<< "cross product (function):", cross >>>;
// print dot product
<<< "dot product:", dot >>>;

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


