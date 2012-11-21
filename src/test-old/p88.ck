// continue in nested block
 
0 => int j;
0 => int i;
0 => int check;

while ( j < 5 ) {
        0 => i;
	while ( i < 5 ) { 
		1 +=> i;  
		if ( j < 4 ) continue;  
		2 +=> check; //because of continue, this only happens in last ( j==0 )
	}	
	1 +=> j;  //if continue malfunctions, this is skipped, and we get INFINITE LOOOOOP
}

//this should make sure that continue does the right thing in the inner loop;
if ( check == 10 ) <<<"success">>>;


