//-----------------------------------------------------------------------------
// name: kb.ck
// desc: open a keyboard and listen for key-down and key-up events
//
// note: select between keyboards by specifying the device number;
//       to see a list of devices and their numbers, either...
//       1) view the Device Browser window in miniAudicle (select
//          "Human Interface Devices" in the drop-down menu)
//       OR 2) from the command line:
//          > chuck --probe
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
//-----------------------------------------------------------------------------
Hid hi;
HidMsg msg;

// which keyboard
0 => int device;
// get from command line
if( me.args() ) me.arg(0) => Std.atoi => device;

// open keyboard (get device number from command line)
if( !hi.openKeyboard( device ) ) me.exit();
<<< "keyboard '" + hi.name() + "' ready", "" >>>;

// infinite event loop
while( true )
{
    // wait on event
    hi => now;

    // get one or more messages
    while( hi.recv( msg ) )
    {
        // check for action type
        if( msg.isButtonDown() )
        {
            <<< "down:", msg.which, "(code)", msg.key, "(usb key)", msg.ascii, "(ascii)" >>>;
        }
        
        else
        {
            //<<< "up:", msg.which, "(code)", msg.key, "(usb key)", msg.ascii, "(ascii)" >>>;
        }
    }
}
