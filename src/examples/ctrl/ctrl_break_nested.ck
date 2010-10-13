// break in nested block
 
0 => int j;
0 => int i;

// loop
while ( j < 5 )
{
    while ( i < 10 )
    { 
        1 +=> i;  
        // arbitrary break here, shouldn't bust of out the outer loop
        if ( i > 5 ) break;  
    }	

    1 +=> j;
}

// test
if ( j == 5 ) <<<"success">>>;
