// create zero-length array
int a[0];

// print size
<<< "size (before append):", a.size() >>>;

// append
a << 1 << 2 << 3 << 5 << 8;

// print size
<<< "size (after append):", a.size() >>>;

// print contents
for( int i; i < a.size(); i++ )
{
    <<< i, ":", a[i] >>>;
}
