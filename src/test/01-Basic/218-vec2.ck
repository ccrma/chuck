// verifying vec2

// print as vec2
<<< "print vec2 literal:", @(1,2) >>>;

// variable
vec2 a;
// assignment
@(3,4) => a;
// one line
@(5,6) => vec2 b;

// print
<<< "print variables:", a, b >>>;
<<< "print fields (x,y):", a.x, a.y >>>;
<<< "print fields (u,v):", a.u, a.v >>>;
<<< "print fields (s,t):", a.s, a.t >>>;

// equivalence
<<< "a == a :", a == a ? "true" : "false" >>>;
<<< "a != 2*a :", a != 2*a ? "true" : "false" >>>;
<<< "a == b :", a == b ? "true" : "false" >>>;

// casting vec2 to vec3
@(7,8) $ vec3 => vec3 v3;
<<< "casting vec2 to vec3:", v3 >>>;
<<< "casting vec2 to vec4:", @(9,10) $ vec4 >>>;
<<< "casting vec3 to vec2:", @(9,10,11) $ vec2 >>>;
<<< "casting vec4 to vec2:", @(9,10,11,12) $ vec2 >>>;

// add and minus
a + b => vec2 c;
b - a => auto d;
.5 * a => vec2 e;
b * 2 => vec2 f;
b / 2 => vec2 g;
<<< "plus minus:", c, d >>>;
<<< "scalar multiply:", e, f, g >>>;
<<< "adding vec2 literals:", @(11,12) + @(13,14) >>>;
<<< "vec2 + vec3:", @(15,16) + @(17,18,19) >>>;
<<< "vec4 + vec2:", @(20,21,22,23) + @(1,1) >>>;
// add assign
a +=> f;
// times assign
2 *=> g;
<<< "add/times assign:", f, g >>>;
// minus assign
a -=> f;
// divide assign
2 /=> g;
<<< "minus/divide assign:", f, g >>>;

// arrays
vec2 vs[0];
// append
vs << a << b << v3$vec2 << @(24,25) << d;
// foreach
for( auto v : vs ) <<< "foreach 1:", v >>>;

// assign to reference
[@(1,2),a,b] @=> vec2 vsB[];
// foreach
for( auto v : vsB ) <<< "foreach 2:", v >>>;

// map
vec2 map[0]; @(100,101) => map["foo"];
<<< "map:", map["foo"] >>>;

fun void foo( vec2 v ) { <<< "foo():", v >>>; }
fun void bar( vec2 vs[] ) { for( auto v : vs ) <<< "bar():", v >>>; }

// call function
foo( @(1,2) );
// call with array
bar( [@(3,4),@(5,6)] );

// print with IO
cherr <= "cherr: " <= a <= " " <= b <= IO.nl();
