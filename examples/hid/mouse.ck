// make HidIn and HidMsg
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
            if( msg.deltaX )
            {
                <<< "mouse motion:", msg.deltaX, "on x-axis" >>>;
            }
            
            if( msg.deltaY )
            {
                <<< "mouse motion:", msg.deltaY, "on y-axis" >>>;
            }
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
