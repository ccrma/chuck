// an array
[1,3,4] @=> int array[];

// should be 3
array[1] => int three;

// zero out the array, leaving size as is
array.zero();

// print (should be three 3s)
<<< three, array.size(), array[1] >>>;

// using .zero() with multidimensional arrays
// NOTE: requires chuck-1.5.5.1 or higher
[ [1,2], [3,4] ] @=> int array2[][];

// zero out the multidimensional array, leaving size as is
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


