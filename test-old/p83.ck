// do-until ( test initial )

0 => int j;
5 => int i;

do { 
	1 +=> j;
	1 +=> i;
} until ( i >= 5 );

if ( j == 1 ) <<<"success">>>;