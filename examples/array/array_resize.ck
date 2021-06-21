// name: array_resize.ck
// desc: this demonstrates how array resize and clear;
//       also see: array_capacity.ck
//       modified: 1.4.1.0 (ge)
//
//       (NOTE: .capacity() is not the same as .cap(); the latter is
//       maintained for historical / compatibility reasons; .cap()
//       actually is the same as .size() -- which is rather confusing!
//
//       It is strongly recommended that .cap() be avoided; instead,
//       explicitly use .size() or .capacity() as appropriate.)

// instantiate a float array
float argh[0];

// print
<<< "array size:", argh.size(), "capacity:", argh.capacity() >>>;

// resize
4 => argh.size;

// print
<<< "array size:", argh.size(), "capacity:", argh.capacity() >>>;

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
<<< "array size:", argh.size(), "capacity:", argh.capacity() >>>;

// clear the contents
argh.clear();

// print
<<< "array size:", argh.size(), "capacity:", argh.capacity() >>>;
