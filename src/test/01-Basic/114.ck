// static int and static function
class Foo {
    static int i;
    fun static void countforever() {
        spork ~ {
            while( true ) { i++; 1::samp => now; }
        };
    }
}

Foo.countforever();
<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
