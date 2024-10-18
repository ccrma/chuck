@import ".deps/Foo.ck"

Foo a, b;
1 => a.num;
2 => b.num;

if( (a + b).num == 3 ) <<< "success" >>>;
