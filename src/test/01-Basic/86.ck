// big number
1000000000000000000000000000.0 => float n;
// overflow it
repeat( 20 ) n *=> n;
// test it
<<< Math.isinf( n ) >>>;
// bye sanity
0 *=> n;
// test it
// *** disabled due to different isnan on different architecture ***
// *** for example, the following returns 1 on Mac intel but 0 on Mac M1 ***	
// <<< Math.isnan( n ) >>>;

// more
<<< 1.0 / 0.0 => Math.isinf >>>;
// *** disabled due to above reason ***
// <<< 0.0 / 0.0 => Math.isnan >>>;
<<< Math.INFINITY, -Math.INFINITY >>>;
<<< Math.INFINITY => Math.isinf >>>;
