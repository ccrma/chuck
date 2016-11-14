//--------------------------------------------------------------------
// name: method.ck
// desc: function using default args to specify function input
//       via a static method, but also allowing this input to be
//       provided manually
//
// author: Jack Atherton (http://www.jack-atherton.com/)
//   date: fall 2016
//--------------------------------------------------------------------

// function that prints the most recently pressed key,
// though it can also print other things that are manually passed in
fun void printAThing( mostRecentKey.was() => string thing )
{
    chout <= thing <= IO.newline();
}

// implementation for detecting the most recent key
// (see SMELT: http://smelt.cs.princeton.edu/)
class mostRecentKey
{
    static int mostRecent;
    
    fun static string was()
    {
        return "ascii: " + mostRecent;
    }
    
    fun static void monitor()
    {
        KBHit kb;
        
        while( true )
        {
            // wait on event
            kb => now;
            
            // loop through 1 or more keys
            while( kb.more() )
            {
                // store key
                kb.getchar() => mostRecentKey.mostRecent;
            }
        }
    }
}
spork ~ mostRecentKey.monitor();

// implementation for detecting the highest ascii value so far
class highestKey
{
    static int highest;
    
    fun static string is()
    {
        return "highest ascii: " + highest;
    }
    
    fun static void monitor()
    {
        KBHit kb;
        
        while( true )
        {
            // wait on event
            kb => now;
            
            // loop through 1 or more keys
            while( kb.more() )
            {
                // store key if higher
                Math.max( kb.getchar(), highestKey.highest ) $ int
                    => highestKey.highest;
            }
        }
    }
}
spork ~ highestKey.monitor();

// do something with the default-arg functions
while( true )
{
    // use default functionality
    printAThing();
    0.5::second => now;
    // use another method for generating interesting things to print
    printAThing( highestKey.is() );
    0.5::second => now;
    // just pass something in manually
    printAThing( "get ready again!" );
    0.5::second => now;
}
