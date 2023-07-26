// name: type_auto.ck
// desc: using the 'auto' type which infers type from context
//
// requires: chuck-1.5.0.8 or higher

// int
1 => auto a;
<<< a, "" >>>;

// array
[1,2,3] @=> auto arr[];
<<< arr[1], "" >>>;

// multi-dim array
[ [1,2,3], [4,5,6] ] @=> auto arr2[][];
<<< arr2[0][2], "" >>>;

// objects
new SinOsc @=> auto z;
440 => z.freq;
<<< z.freq()$int, "" >>>;

// for-each
for( auto x : arr )
{
    // print
    <<< x, "" >>>;
}
