// test basic static initialization across @import
// see examples/class/static-init.ck for more info

// import
@import "../class/static-init.ck"

// print static variables from imported class Foo
<<< Foo.S_INT >>>;
<<< Foo.S_FLOAT >>>;
<<< Foo.S_DUR / second >>>;
<<< (Foo.S_TIME-now) / second >>>;
<<< Foo.S_VEC3 >>>;
for( auto i : Foo.S_INT_ARRAY ) <<< i >>>;
<<< Foo.S_STRING >>>;
<<< Foo.S_SINOSC.freq() >>>;
