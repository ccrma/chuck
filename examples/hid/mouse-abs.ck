//-----------------------------------------------------------------------------
// name: mouse-abs.ck
// desc: basic mouse input (absolute normalized screen X Y coordinates)
//       also see mouse.ck for relative X Y deltas
//
// note: select between mice/trackpads by specifying device number;
//       to see a list of devices and their numbers, either...
//       1) view the Device Browser window in miniAudicle (select
//          "Human Interface Devices" in the drop-down menu)
//       OR 2) from the command line:
//          > chuck --probe
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
//-----------------------------------------------------------------------------

// HID input and a HID message
Hid hi;
HidMsg msg;

// which mouse
0 => int device;
// get from command line
if( me.args() ) me.arg(0) => Std.atoi => device;

// open mouse 0, exit on fail
if( !hi.openMouse( device ) ) me.exit();
<<< "mouse '" + hi.name() + "' ready", "" >>>;

// infinite event loop
while( true )
{
    // wait on HidIn as event
    hi => now;

    // messages received
    while( hi.recv( msg ) )
    {
        // mouse motion
        if( msg.isMouseMotion() )
        {
            // get the normalized X-Y screen cursor pofaition
            <<< "mouse normalized position --",
               "x:", msg.scaledCursorX, "y:", msg.scaledCursorY >>>;
        }
        
        // mouse button down
        else if( msg.isButtonDown() )
        {
            <<< "mouse button", msg.which, "down" >>>;
        }
        
        // mouse button up
        else if( msg.isButtonUp() )
        {
            <<< "mouse button", msg.which, "up" >>>;
        }
        
        // mouse wheel motion
        else if( msg.isWheelMotion() )
        {
            if( msg.deltaX )
            {
                <<< "mouse wheel:", msg.deltaX, "on x-axis" >>>;
            }
            
            if( msg.deltaY )
            {
                <<< "mouse wheel:", msg.deltaY, "on y-axis" >>>;
            }
        }
    }
}
