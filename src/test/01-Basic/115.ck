// nested spork doesn't keep going forever
class Foo {
    static int i;
    fun static void countforever() {
        spork ~ {
            while( true ) { i++; 1::samp => now; }
        };
    }
}

// counterintuitive behavior: when this spork ends,
//   it ends all its child shreds too.
// need to spork a shred from global or 
//   from a parent that won't ever exit
//   for the new shred to not exit
spork ~ Foo.countforever();
<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
