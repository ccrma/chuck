PRC perry;
Skot skot;

// declare this out here
string out;

// infinite event loop
while( true )
{
    // prompt for line
    skot.prompt( "enter line:" ) => now;

    // loop over lines
    while( skot.more() )
    {
        // set the line
        skot.getLine() => perry.set;
        // process line
        while( perry.more() )
        {
            // get it
            perry.next( out );
            <<< out >>>;
        }
    }   
}
