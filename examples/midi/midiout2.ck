//-----------------------------------------------------------------------------
// name: midiout2.ck
// desc: example of sending MIDI messages using MidiOut only
// requires: chuck-1.5.2.5 or higher | author: cviejo
// note: for a good explanation of how MIDI messages work and sending raw midi
//       messages, see midiout.ck
//-----------------------------------------------------------------------------

// instantiate a MIDI out object
MidiOut mout;
// open a MIDI device for output
if( !mout.open(0) ) me.exit();

<<< "MIDI output device opened...", "" >>>;

// loop
while( true )
{
    <<< "sending note on message...", "" >>>;
    mout.noteOn( 0, 60, 127 );
    1::second => now;

    <<< "sending note off message...", "" >>>;
    mout.noteOff( 0, 60, 0 );
    1::second => now;

    <<< "sending control change message...", "" >>>;
    mout.controlChange( 0, 14, 127 );
    1::second => now;

    <<< "sending program change message...", "" >>>;
    mout.programChange( 0, 6 );
    1::second => now;

    <<< "sending polyphonic key pressure (aftertouch)...", "" >>>;
    mout.polyPressure( 0, 60, 127 );
    1::second => now;

    <<< "sending channel pressure (aftertouch)...", "" >>>;
    mout.channelPressure( 0, 127 );
    1::second => now;

    <<< "sending pitch bend message...", "" >>>;
    mout.pitchBend( 0, 96 ); // +50% or 1.00 semitone up
    1::second => now;

    <<< "sending fine resolution pitch bend message...", "" >>>;
    mout.pitchBend( 0, 50, 96 ); // +51% or 1.01 semitone up
    1::second => now;
}
