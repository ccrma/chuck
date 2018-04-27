// big number
1000000000000000000000000000.0 => global float n;
// overflow it
repeat( 20 ) n *=> n;
// test it
<<< Math.isinf( n ) >>>;
// bye sanity
0 *=> n;
// test it
<<< Math.isnan( n ) >>>;

// more
<<< 1.0 / 0.0 => Math.isinf >>>;
<<< 0.0 / 0.0 => Math.isnan >>>;
<<< Math.INFINITY, -Math.INFINITY >>>;
<<< Math.INFINITY => Math.isinf >>>;
