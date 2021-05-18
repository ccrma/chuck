// 14.ck : array instantiation

// declare int array 'a'
global int a[2];
// element 1
4 => a[1];

// array initialization
[ 1, 2, 3, 4, 5 ] @=> global int b[];

// loop over and print
for( 0 => int i; i < b.size(); i++ ) 
    //<<<b[i]>>>;
    b[i];

// test
if( !a[0] && a[1] == 4 && b[2] == 3 ) <<<"success">>>;
