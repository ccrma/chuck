// instantiate
Skot skot;

// infinite event loop
while( true )
{
    // prompt and wait
    skot.prompt("enter data:") => now;
    // get the results
    while( skot.more() )
    {
        // get and print
        <<< skot.getLine() >>>;
    }
}
