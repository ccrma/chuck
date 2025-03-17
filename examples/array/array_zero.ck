// .zero() zeros out the contents AND keeps size unchanged
// version: need chuck-1.5.0.0 or higher
//
// NOTE: using .zero() with multidimensional arrays
//       requires chuck-1.5.5.1 or higher

// an array
[1,3,4] @=> int array[];

// should print 3 3
<<< array[1], array.size() >>>;

// zero out the array, leaving size as is
array.zero();

// now should print 0 3
<<< array[1], array.size() >>>;

// a multi-dimensional array
[ [1,2], [3,4] ] @=> int array2[][];

// NOTE: using .zero() with multidimensional arrays
//       requires chuck-1.5.5.1 or higher
array2.zero();

// iterate over array2
for( int inner[] : array2 )
{
    // iterate over each inner array
    for( int x: inner )
    {
        // print each element within
        cherr <= x <= " ";
    }
}
// print newline
cherr <= IO.nl();
