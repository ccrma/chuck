// name: array_shuffle.ck
// desc: shuffling the contents of an array; useful for tasks like random without replacement
// author: kunwoo, nshaheed, andrew, ge | 1.5.0.0

// an array
[1,2,3,4,5] @=> int a[];

// uncomment to print array API
// a.help();
// uncomment to explicitly set a random seed
// Math.srandom(11);

// shuffle a few times
repeat( 10 )
{
    // print
    for( int i; i < a.size(); i++ )
    { cherr <= a[i] <= " "; } cherr <= IO.nl();
    // shuffle!
    a.shuffle();
}

// one way to do random without replacement
repeat( 3 )
{
    cherr <= "--- random without replacment --- " <= IO.nl();
    // shuffle!
    a.shuffle();
    // just go through array in order after shuffle
    for( int i; i < a.size(); i++ )
    {
        // print the next "random" value
        cherr <= "next value: " <= a[i] <= IO.nl();
    }
}
