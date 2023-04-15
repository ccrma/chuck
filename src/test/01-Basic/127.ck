// 1.5.0.0 | (ge)
// testing random seed with shuffle

[1,2,3,4,5] @=> int a[];
Math.srandom(11);
a.shuffle();

[1,2,3,4,5] @=> int b[];
Math.srandom(11);
b.shuffle();

// compare
for( int i; i < a.size(); i++ )
{
    // if different
    if( a[i] != b[i] ) me.exit();
}

// got here, good
<<< "success" >>>;
