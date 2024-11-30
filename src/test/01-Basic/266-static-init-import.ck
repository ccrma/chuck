// test basic static initialization across @import #2024-static-init

// import
@import "265-static-init.ck"

// print static variables
<<< Foo.S_INT >>>;
<<< Foo.S_FLOAT >>>;
<<< Foo.S_DUR / second >>>;
<<< (Foo.S_TIME-now) / second >>>;
<<< Foo.S_VEC3 >>>;
for( auto i : Foo.S_INT_ARRAY ) <<< i >>>;
<<< Foo.S_STRING >>>;
<<< Foo.S_SINOSC.freq() >>>;