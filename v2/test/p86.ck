// continue ( skipping secondary increment )
 
0 => int j;
0 => int i;

while ( j < 10 ) {
	1 +=> j;
	if ( j < 5 ) { 
		continue;
	}
	1 +=> i;
}

<<<i>>>;
if ( i == 6 ) <<<"success">>>;
