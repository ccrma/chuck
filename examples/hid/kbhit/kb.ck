// the event
KBHit kb;

// time-loop
while( true )
{
    // wait on kbhit event
    kb => now;

    // potentially more than 1 key at a time
    while( kb.more() )
    {
        // print key value
        <<< "ascii: ", kb.getchar() >>>;
    }
}

