// big number
1000000000000000000000000000.0 => float n;
// overflow it
repeat( 20 ) n *=> n;
// test it
<<< Math.isinf( n ) >>>;
// bye sanity
0 *=> n;
// test it
<<< Math.isnan( n ) >>>;

// *** RE-ENABLED in 1.4.2.1; differences due the fast-math compiler flag
// for example, if -ffast-math enabled, the above returns 1 on Mac 
// intel but 0 on Mac arm64; fast-math is now disabled by default ***

// more
<<< 1.0 / 0.0 => Math.isinf >>>;
// *** the following alos differs if fast-math is enabled
<<< 0.0 / 0.0 => Math.isnan >>>;
<<< Math.INFINITY, -Math.INFINITY >>>;
<<< Math.INFINITY => Math.isinf >>>;
