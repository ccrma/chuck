// verify that dependency analysis properly is not producing
// compiler error across files | added 1.5.1.1
//
// FYI incorrect error reporting in 1.5.1.0
// 191-c-User.ck:4:16: error: calling 'make()' at this point skips initialization of a needed variable:
// [4]     FooFactory.make() @=> Foo foo;
//                    ^
//
// (this is followed by sometimes gibberish file locations
// when it is using a within-file "where" position that was
// intended to operate beyond files)
//
// 191-c-User.ck:6:10: error: ...(note: this skipped variable initialization is needed by 'fun FooChild FooFactory.make()')
// 191-c-User.ck:6:103: error: ...(note: this is where the variable is used within 'fun FooChild FooFactory.make()' or its subsidiaries)
// 191-c-User.ck: ...(hint: try calling 'make()' after the variable initialization)

// construct path from POV of src/test/
me.dir(-1)+"/01-Basic/" => string base;

Machine.add( base+"191-a-Foo.ck.disabled" );
Machine.add( base+"191-b-Factory.ck.disabled" );
Machine.add( base+"191-c-User.ck.disabled" );