// name: array_erase2.ck
// desc: shows array.erase(int, int) to remove a range
//
// requires: chuck-1.5.0.8 or higher

// create zero-length array
int a[0];

// print size
<<< "size (before append):", a.size() >>>;

// append
a << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8;

// print size
<<< "size (after append):", a.size() >>>;

// erase 3rd to 5th element
// NOTE erase(begin, end) remove [begin,end):
// i.e., starting with begin and up to but NOT including end
a.erase( 2, 5 );

// print size
<<< "size (after erase and popFront):", a.size() >>>;

// print contents
for( int i; i < a.size(); i++ )
{
    <<< i, ":", a[i] >>>;
}
