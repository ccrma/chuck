// testing continue and break

// create empty array
int array[0];

// append
array << 1 << 2 << 3;

// for each element 'x' in array
for( int x : array )
{
    // print the element
    <<< x >>>;
    // should be after last element printed
    if( x == 3 ) continue;
}

// for each element 'x' in array
for( int x : array )
{
    // print the element
    <<< x >>>;
    if( x == 2 ) break;
}
