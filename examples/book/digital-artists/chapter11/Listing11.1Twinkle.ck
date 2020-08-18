// Listing 11.1 MIDI output note on/off boilerplate

// MIDI out setup, make a MidiOut object, open it on a device
MidiOut mout;

// MIDI Port (Window > Device Browser > MIDI > Output)
0 => int port;

// try to open that port, fail gracefully
if( !mout.open(port) )
{
    <<< "Error: MIDI port did not open on port: ", port >>>;
    me.exit();
}

// Make a MIDI msg holder for sending
MidiMsg msg;

// utility function to send MIDI out notes
fun void MIDInote(int onoff, int note, int velocity)
{
    if (onoff == 0) 128 => msg.data1;
    else 144 => msg.data1;
    note => msg.data2;
    velocity => msg.data3;
    mout.send(msg);
}

// declare and initialize array of MIDI notes
[57,57,64,64,66,66,64,62,62,61,61,59,59,57] @=> int myNotes[];

// quarter note and half note durations
0.3 :: second => dur q;
0.8 :: second => dur h;
[ q, q, q, q, q, q, h, q, q, q, q, q, q, h] @=> dur myDurs[];

// loop for length of array
for (0 => int i; i < myNotes.cap(); i++)
{
    MIDInote(1, myNotes[i], 100);
    myDurs[i] => now;
    MIDInote(0, myNotes[i], 100);
    0.2 :: second => now;
}
