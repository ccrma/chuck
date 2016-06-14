// print
<<< #(1,1) >>>;
// print polar
<<< %(1,2*pi) >>>;

// cast int to complex
<<< 1 $ complex >>>;
// cast int to polar
<<< 2 $ polar >>>;
// cast float to complex
<<< 3.0 $ complex >>>;
// cast float to polar
<<< 4.0 $ polar >>>;

// cast complex to polar
<<< "cast:", #(10,10) $ polar >>>;
// cast polar to complex
<<< "cast:", %(1,pi/2) $ complex >>>;
// cast to and to
<<< "cast:", #(5,5) $ polar $ complex >>>;

// store in variable
#(1,2.0) => complex c;
// print
<<< c >>>;
// store in variable
%(3.0,4) => polar p;
// print
<<< p >>>;

// add
<<< #(1,1) + #(1,1) >>>;
<<< %(1,0) + 3*%(1,1) >>>;
// minus
<<< #(1,1) - #(.5,-1) >>>;
<<< %(1,1) - %(.5,0) >>>;
// times
<<< #(1,2) * #(3,4) >>>;
<<< %(2,.25*pi) * %(2,.35*pi) >>>;
// divide
<<< #(2,2) / #(1,1) >>>;
<<< %(5,.25*pi) / %(4,.5*pi) >>>;

// add assign
#(1,1) => complex foo;
#(2,3) +=> foo; <<< foo >>>;
#(2,3) -=> foo; <<< foo >>>;
#(0,5) *=> foo; <<< foo >>>;
#(0,5) /=> foo; <<< foo >>>;
%(2,pi) => polar bar;
%(3,pi) +=> bar; <<< bar >>>;
%(-1,0) -=> bar; <<< bar >>>;
%(2,pi) *=> bar; <<< bar >>>;
%(2,pi) /=> bar; <<< bar >>>;

// constant
<<< "Math.j:", Math.j >>>;

// math
<<< Math.re( #(1,2) ) >>>;
<<< Math.im( #(1,2) ) >>>;
<<< Math.mag( %(2,pi) ) >>>;
<<< Math.phase( %(2,pi) ) >>>;

// array
complex arr[10];
<<< arr.size() >>>;
<<< arr[0] >>>;
#(10, 11) => arr[1];
<<< arr[1] >>>;

polar brr[10];
<<< brr.size() >>>;
<<< brr[0] >>>;
%(5,pi) => brr[1];
<<< brr[1] >>>;
<<< Math.mag( brr[1] ) >>>;

// components
<<< #(1,2).re, #(1,2).im >>>;
<<< %(3,pi).mag, %(3,pi).phase >>>;
<<< foo.re, foo.im >>>;
5 => foo.re => foo.im;
<<< foo, foo.re, foo.im >>>;

// arrays
[ #(1,2), #(3,4), #(5,6) ] @=> complex cs[];
polar ps[cs.size()];
// convert
Math.rtop( cs, ps );
<<< ps[0], ps[1], ps[2] >>>;
Math.ptor( ps, cs );
<<< cs[0], cs[1], cs[2] >>>;
