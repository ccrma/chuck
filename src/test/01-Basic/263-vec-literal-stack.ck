// testing stack alignment

// how many iterations
250000 => int N;

// some set up
@(1,2,3) => vec3 v1;
@(1,0,0) => vec3 v2a;
@(0,1,0) => vec3 v2b;
v2a + v2b => vec3 v2sum;

// calling function from literal value
repeat(N) @(1,2,3).magnitude();
repeat(N) (v2a+v2b).magnitude();
repeat(N) (@(1,0,0)+@(0,1,0)).magnitude();
repeat(N) @(1,2,3).dot(@(0,1,0));
repeat(N) @(1,2,3).dot(v2b);
repeat(N) v1.dot(@(0,1,0));
repeat(N) v1.dot(v2b);
repeat(N) @(1,0).dot(@(3,0));
repeat(N) @(1,0,2,0).dot(@(3,0,1,0));

// if we got here then good
<<< "success" >>>;
