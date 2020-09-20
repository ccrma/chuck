// multi-input UAna

// connecting two inputs
Correlation corr => blackhole;
adc.left => corr.left;
adc.right => corr.right;

// set size (for circular correlation)
512 => corr.size;

InfoBlob @ result;

while( true )
{
    // take correlation
    corr.go();
    // get result
    corr.output() @=> result;
    
    // hop size
    256::samp => now;
}


// ---- less automatic ----

// connecting two inputs
Correlation corr => IFFT afd;

// blob
InfoBlob @ result;

while( true )
{
    // corr of a and b, size in a/b
    corr.do( a, b );
    // get result
    corr.output() @=> result;

    // manually place result blob in afd    
    result => afd;
    
    // time
    256::samp => now;
}
