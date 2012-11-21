// break in nested block
 
0 => int j;
0 => int i;

while ( j < 5 ) {
	while ( i < 10 ) { 
		1 +=> i;  
		if ( i > 5 ) break;  //arbitrary break here, but shouldn't bust of out the outer loop
	}	
	1 +=> j;
}

if ( j == 5 ) <<<"success">>>;