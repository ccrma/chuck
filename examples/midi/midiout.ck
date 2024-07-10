//-----------------------------------------------------------------------------
// name: midiout.ck
// desc: example of sending MIDI messages
// note: for a good explanation of how MIDI messages work, see after the code
//       also see midiout2.ck for sending by common channel voice message
//-----------------------------------------------------------------------------

// instantiate a MIDI out object
MidiOut mout;
// a message to work with
MidiMsg msg;
// open a MIDI device for output
if( !mout.open(0) ) me.exit();

<<< "MIDI output device opened...", "" >>>;

// loop
while( true )
{
    // MIDI note on message
    // 0x90 + channel (0 in this case)
    0x90 => msg.data1;
    // pitch
    60 => msg.data2;
    // velocity
    127 => msg.data3;
    // print
    <<< "sending NOTE ON message...", "" >>>;
    // send MIDI message
    mout.send( msg );

    // let time pass
    1::second => now;
    
    // MIDI note off message
    // 0x80 + channel (0 in this case)
    0x80 => msg.data1;
    // pitch
    60 => msg.data2;
    // velocity
    0 => msg.data3;
    // print
    <<< "sending NOTE OFF message...", "" >>>;
    // send MIDI message
    mout.send( msg );

    // let time pass
    1::second => now;
}


//-----------------------------------------------------------------------------
// MIDI Messages (how they work)
// thanks to Dave Marshall for this excellent and clear explanation
// https://users.cs.cf.ac.uk/dave/Multimedia/node158.html
//-----------------------------------------------------------------------------
// MIDI messages are used by MIDI devices to communicate with each other.
//
// Structure of MIDI messages:
// MIDI message includes a status byte and up to two data bytes.
//
// Status byte
//   The most significant bit of status byte is set to 1.
//   The 4 low-order bits identify which channel it belongs to (four bits produce 16 possible channels).
//   The 3 remaining bits identify the message.
//   The most significant bit of data byte is set to 0.
//
// Classification of MIDI messages:
//                                                ----- voice messages
//                    ---- channel messages -----|
//                   |                            ----- mode messages
//                   |
// MIDI messages ----|
//                   |                            ---- common messages
//                    ----- system messages -----|---- real-time messages
//                                                ---- exclusive messages
// A. Channel messages:
// - messages that are transmitted on individual channels rather that globally to all devices in the MIDI network.
//
// A.1. Channel voice messages:
// Instruct the receiving instrument to assign particular sounds to its voice
// Turn notes on and off
// Alter the sound of the currently active note or notes
//
// Voice Message           Status Byte      Data Byte1          Data Byte2
// -------------           -----------   -----------------   -----------------
// Note off                      8x      Key number          Note Off velocity
// Note on                       9x      Key number          Note on velocity
// Polyphonic Key Pressure       Ax      Key number          Amount of pressure
// Control Change                Bx      Controller number   Controller value
// Program Change                Cx      Program number      None
// Channel Pressure              Dx      Pressure value      None
// Pitch Bend                    Ex      MSB                 LSB
// Notes: `x' in status byte hex value stands for a channel number.
//
// Example: a Note On message is followed by two bytes, one to identify the note,  and on to specify the velocity.
// To play note number 80 with maximum velocity on channel 13, the MIDI device would send these three hexadecimal byte values: 9C 50 7F
//
// A.2. Channel mode messages: - Channel mode messages are a special case of the Control Change message ( Bx or 1011nnnn).
// The difference between a Control message and a Channel Mode message, which share the same status byte value, is in the first data byte.
// Data byte values 121 through 127 have been reserved in the Control Change message for the channel mode messages.
//
// Channel mode messages determine how an instrument will process MIDI voice messages.
//
// 1st Data Byte      Description                Meaning of 2nd Data Byte
// -------------   ----------------------        ------------------------
//      79        Reset all  controllers            None; set to 0
//      7A        Local control                     0 = off; 127  = on
//      7B        All notes off                     None; set to 0
//      7C        Omni mode off                     None; set to 0
//      7D        Omni mode on                      None; set to 0
//      7E        Mono mode on (Poly mode off)      **
//      7F        Poly mode on (Mono mode off)      None; set to 0
// ** if value = 0 then the number of channels used is determined by the receiver; all other values set a specific number of channels, beginning with the current basic channel.
//
// B. System Messages:
// System messages carry information that is not channel specific, such as timing signal for synchronization, positioning information in pre-recorded MIDI sequences, and detailed setup information for the destination device.
//
// B.1. System real-time messages:
// messages related to synchronization
//
// System Real-Time Message         Status Byte
// ------------------------         -----------
// Timing Clock                         F8
// Start Sequence                       FA
// Continue Sequence                    FB
// Stop Sequence                        FC
// Active Sensing                       FE
// System Reset                         FF
//
// B.2. System common messages:
// contain the following unrelated messages
//
// System Common Message   Status Byte      Number of Data Bytes
// ---------------------   -----------      --------------------
// MIDI Timing Code            F1                   1
// Song Position Pointer       F2                   2
// Song Select                 F3                   1
// Tune Request                F6                  None
//
// B.3. System exclusive message:
// (a) Messages related to things that cannot be standardized, (b) addition to the original MIDI specification.
//
// It is just a stream of bytes, all with their high bits set to 0, bracketed by a pair of system exclusive start and end messages (F0 and F7).
//-----------------------------------------------------------------------------
