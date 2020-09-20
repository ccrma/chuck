// make HidIn and HidMsg
Hid hi;
HidMsg msg;

// which joystick
0 => int device;
// get from command line
if( me.args() ) me.arg(0) => Std.atoi => device;

// open joystick 0, exit on fail
if( !hi.openJoystick( device ) ) me.exit();

<<< "joystick '" + hi.name() + "' ready", "" >>>;

// infinite event loop
while( true )
{
    // wait on HidIn as event
    hi => now;

    // messages received
    while( hi.recv( msg ) )
    {
        // joystick axis motion
        if( msg.isAxisMotion() )
        {
            <<< "joystick axis", msg.which, ":", msg.axisPosition >>>;
        }
        
        // joystick button down
        else if( msg.isButtonDown() )
        {
            <<< "joystick button", msg.which, "down" >>>;
        }
        
        // joystick button up
        else if( msg.isButtonUp() )
        {
            <<< "joystick button", msg.which, "up" >>>;
        }
        
        // joystick hat/POV switch/d-pad motion
        else if( msg.isHatMotion() )
        {
            <<< "joystick hat", msg.which, ":", msg.idata >>>;
        }
    }
}
