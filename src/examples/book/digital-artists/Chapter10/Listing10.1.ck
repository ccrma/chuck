// Listing 10.1 Standard code to create a Hid event
// Declare a new Hid object
Hid myHid;                  // (1) Makes a new Hid.

// message to convey data from Hid device
HidMsg msg;                 // (2) Makes a Hid message holder.

// device number: which keyboard to open
0 => int device;            // (3) Opens Hid on keyboard device.

// open keyboard; or exit if fail to open
if( !myHid.openKeyboard( device ) )
{                           // (4) Error if it can't be opened.
    <<< "Can't open this device!! ", "Sorry." >>>;  
    me.exit();              // (5) Exit, because nothing more can be done.
}

// print status of open keyboard Hid
                            // (6) If success, print happy message.
<<< "keyboard '" + myHid.name() + "' ready", "" >>>;

// Testing the keyboard Hid
// Impulse keyboard "clicker"
Impulse imp => dac;         // (7) "Clicker" to hear key strokes.

// infinite event loop
while( true )               // (8) Infinite loop. 
{
    // wait for event
    myHid => now;           // (9) Wait here for a Hid event.

    // get message(s)
    while( myHid.recv( msg ) )
    {                             // (10) Loop over all messages.
        // Process only key (button) down events
        if( msg.isButtonDown() )
        {                         // (11) If keydown...
            // print ascii value and make a click
            <<< "key DOWN:", msg.ascii >>>; // (12) ...then print which key...
            5 => imp.next;        // (13) ...and make click.
        }
        else // key (button) up
        {
            // do nothing for now // (14) Do nothing on keyUp
                                  //      (you could add something here).
        }
    }
}
