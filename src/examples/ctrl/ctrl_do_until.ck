// do-until

0 => int j;
0 => int i;

do { 
    1 +=> j;
    1 +=> i;
} until ( i >= 5 );

// test
if ( j == 5 ) <<<"success">>>;
