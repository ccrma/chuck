// literal complex
#(1,2) => complex foo;

// declare a complex number
complex bar;
// set the real and imaginary parts
-1 => bar.re;
-2 => bar.im;

// declare array
complex array[2];
// set
3 => array[0].re;
4 => array[0].im;
5 => array[1].re;
6 => array[1].im;

// sum
complex sum;
for( auto x : array )
{
    sum + x => sum;
}

// re and im
<<< foo.re, foo.im >>>;
// print as complex
<<< foo >>>;

// mag and phase
<<< (foo$polar).mag, (foo$polar).phase/pi >>>;
// print as polar
<<< foo$polar >>>;

// print sum
<<< sum >>>;

