//-----------------------------------------------------------------------------
// name: mouse-cursor.ck
// desc: quickly access the current Mouse cursor coordinates
//       (for full access to mouse input, see the Hid class)
//-----------------------------------------------------------------------------
//       ALSO SEE mouse-abs.ck for absolute screen position
//       ALSO SEE mouse.ck for relative X Y deltas
//
// requires: chuck-1.5.4.2 or higher
//-----------------------------------------------------------------------------

// time loop
while( true )
{
    // get current XY mouse cursor absolute coordinates
    // as well as scaled coordinates (in [0,1])
    // .xy() could return negative values for multi-monitor
    // .scaled() will always return values in [0,1]
    <<< MouseCursor.xy(), MouseCursor.scaled() >>>;

    // advance time
    25::ms => now;
}
