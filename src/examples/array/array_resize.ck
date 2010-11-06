// instantiate a float array
float argh[0];

// print
<<< "array size:", argh.size() >>>;

// resize
4 => argh.size;

// print
<<< "array size:", argh.size() >>>;

// set elements
3.0 => argh[0];
4 => argh[1];
5.0 => argh[2];
6 => argh[3];

// print
<<< "contents:", "">>>;
for( int i; i < argh.size(); i++ )
    <<< "argh[", i, "]:", argh[i] >>>;

// recap
2 => argh.size;

// print
<<< "array size:", argh.size() >>>;

// clear the contents
argh.clear();

// print
<<< "array size:", argh.size() >>>;

