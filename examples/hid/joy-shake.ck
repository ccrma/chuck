// STK Shakers and joystick!

// make HidIn and HidMsg
Hid hi;
HidMsg msg;

// which joystick
0 => int device;
// get from command line
if( me.args() ) me.arg(0) => Std.atoi => device;

// open joystick 0, exit on fail
if( !hi.openJoystick( device ) ) me.exit();

// patch
Shakers s => JCRev r => dac;
.02 => r.mix;

int which;

// infinite event loop
while( true )
{
    // wait on HidIn as event
    hi => now;

    // messages received
    while( hi.recv( msg ) )
    {
        // axis
        if( msg.isAxisMotion() )
        {
            if( msg.which == 0 ) (msg.axisPosition+1) / 2.0 => s.energy;
            if( msg.which == 1 ) (msg.axisPosition+1) * 64 => s.objects;
            if( msg.which == 2 ) 5 + (msg.axisPosition+1) * 300 => s.freq;
            if( msg.which == 3 ) (msg.axisPosition+1) / 2.0 => s.decay;
            //<<< "freq:", s.freq(), "objects:", s.objects(), "energy:", s.energy(), "decay:", s.decay(), "preset:", s.preset() >>>;
        }

        // button down
        if( msg.isButtonDown() )
        {
            if( msg.which == 0 ) s.noteOn( .8 );
            if( msg.which == 2 ) { which++; (which % 22) => s.preset; }
            if( msg.which == 3 ) { which--; (which % 22) => s.preset; }
            //<<< "freq:", s.freq(), "objects:", s.objects(), "energy:", s.energy(), "decay:", s.decay(), "preset:", s.preset() >>>;
        }
    }
}
