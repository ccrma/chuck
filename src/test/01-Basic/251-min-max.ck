// different overloading

// should use min(int,int)
<<< Math.min( 1, 2 ) >>>;
// should use min(float, float) if at least one is float
<<< Math.min( 3.0, 4.0 ) >>>;
<<< Math.min( 5.0, 6 ) >>>;
<<< Math.min( 7, 8.0 ) >>>;

// should use max(int,int)
<<< Math.max( 1, 2 ) >>>;
// should use max(float, float) if at least one is float
<<< Math.max( 3.0, 4.0 ) >>>;
<<< Math.max( 5.0, 6 ) >>>;
<<< Math.max( 7, 8.0 ) >>>;
