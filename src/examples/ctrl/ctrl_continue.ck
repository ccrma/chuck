// continue in nested block
 
0 => int j;
0 => int i;
0 => int check;

// loop
while ( j < 5 )
{
    0 => i;
    while ( i < 5 )
    {
        1 +=> i;  
        if ( j < 4 ) continue;  

        // because of continue, this only happens in last ( j==0 )
        2 +=> check; 
    }

    // if continue malfunctions, this is skipped, and we get INFINITE LOOP 
    1 +=> j;
}

// make sure that continue does the right thing in the inner loop?
if ( check == 10 ) <<<"success">>>;


