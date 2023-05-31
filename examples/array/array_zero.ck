// .zero() zeros out the contents AND keeps size unchanged
// version: need chuck-1.5.0.0 or higher

// an array
[1,3,4] @=> int array[];

// should print 3 3
<<< array[1], array.size() >>>;

// zero out the array, leaving size as is
array.zero();

// now should print 0 3
<<< array[1], array.size() >>>;
