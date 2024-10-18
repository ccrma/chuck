// error case: test using an operator local to Foo.ck
@import ".deps/Foo.ck"

// make a Foo
Foo a, b;

// this should throw an error
a =^ b;