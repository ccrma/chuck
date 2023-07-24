// name: array_erase.ck
// desc: shows array.erase()
//
// requires: chuck-1.5.0.8 or higher

// create zero-length array
int a[0];

// print size
<<< "size (before append):", a.size() >>>;

// append
a << 1 << 2 << 3 << 5 << 8;

// print size
<<< "size (after append):", a.size() >>>;

// erase 5th element
a.erase( 4 );
// erase front
a.popFront();

// print size
<<< "size (after erase and popFront):", a.size() >>>;

// print contents
for( int i; i < a.size(); i++ )
{
    <<< i, ":", a[i] >>>;
}
