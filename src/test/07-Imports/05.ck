// Both A and B depend on C
@import ".deps/05-A";
@import ".deps/05-B";

A a;
B b;

if (a.a() == "c!" && b.b() == "c!") <<< "success" >>>;