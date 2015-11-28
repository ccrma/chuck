// vec4 is a primitive type
@(1,2,3,4) => vec4 a;
// declare another
@(5,6,7,8) => vec4 b;

// add them
a + b => vec4 sum;
// difference
a - b => vec4 diff;
// cross product (ignores w component)
a * b => vec4 c;


// print sum!
<<< "sum:", sum >>>;
// print difference
<<< "diff:", diff >>>;
// print cross product
<<< "cross product:", c >>>;


// array
[ a, b, c ] @=> vec4 group[];

// print them
<<< group[0], group[1], group[2] >>>;
