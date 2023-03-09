// an array
[1,3,4] @=> int array[];

// should be 3
array[1] => int three;

// zero out the array, leaving size as is
array.zero();

// check: size should be 3 but array[1] should now be zero
if( three == 3 && array.size() == 3 && array[1] == 0 )
    <<< "success" >>>;
