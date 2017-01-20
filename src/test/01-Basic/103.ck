// spork code block inside if block
if( true )
{
    TriOsc t;
    
    spork ~ {
        t => dac;
        1::samp => now;
    };
    
    2::samp => now;
}
<<< "success" >>>;
