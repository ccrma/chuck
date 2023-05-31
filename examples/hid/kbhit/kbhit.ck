// name: kbhit.ck
// desc: KBHit (terminal only) is a simple mechanism for capturing
//       keyboard input (for a more flexible mechanism, see HID)

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

