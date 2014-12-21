// Listing 10.2 Keyboard organ controlled by Hid event
// Hid object
Hid hi;                  // (1) Makes a new Hid object...

// message to convey data from Hid device
HidMsg msg;              // (2) ...and Hid message holder.

// device number: which keyboard to open
0 => int device;         // (3) Keyboard device number.

// open keyboard; or exit if fail to open
if( !hi.openKeyboard( device ) ) me.exit();  // (4) Opens it, exits if failed.

// print a message!           // (5) Prints message if success.
<<< "keyboard '" + hi.name() + "' ready", "" >>>;

// sound chain for Hid keyboard controlled organ
BeeThree organ => JCRev r => dac; // (6) Organ UGen through reverb to dac.

// infinite event loop
while( true )
{
    // wait for event
    hi => now;                    // (7) Waits for keyboard event.
    // get message
    while( hi.recv( msg ) )       // (8) loops over all messages (keys pressed).
    {
        // button (key) down, play a Note
        if( msg.isButtonDown() )
        {                         // (9) If keyDown, set frequency from keycode...
            // take ascii value of button, convert to freq
            Std.mtof( msg.ascii ) => organ.freq;
            // sound the note
            1 => organ.noteOn;    // (10) ... and play a note.
        }
        else // button up, noteOff
        {
            // deactivate the note
            0 => organ.noteOff;   // (11) End the note on keyUp.
        }
    }
}
