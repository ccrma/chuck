// float div-by-zero
<<< Math.isinf(1./0) >>>;
<<< Math.isinf(1/0.) >>>;
<<< Math.isnan(0./0) >>>;
<<< Math.isnan(0/0.) >>>;
// vec div-by-zero
@(1,1,1)/0 => vec3 v;
<<< Math.isinf(v.x), Math.isinf(v.y), Math.isinf(v.z) >>>;
@(1,0,1,0)/0 => vec4 v4;
<<< Math.isinf(v4.x), Math.isnan(v4.y), Math.isinf(v4.z), Math.isnan(v4.w) >>>;
// complex div-by-zero
#(1,1) / 0 => complex c;
<<< Math.isnan(c.re), Math.isnan(c.im) >>>;
#(1,1) / #(0,0) => c;
<<< Math.isnan(c.re), Math.isnan(c.im) >>>;
// polar div-by-zero
0 => polar p;
p /=> p;
<<< Math.isnan(p.mag), p.phase >>>;

// integer div-by-zero
<<< 1/
0 >>>;
