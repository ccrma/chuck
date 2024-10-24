// import
@import "Foo.ck"

// instantiate class defined in imported file
Foo a(1), b(2);

// use operator overload defined in imported file
<<< (a + b).num >>>;