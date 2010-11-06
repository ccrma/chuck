//------------------------------------------------
// name: polyfony.ck
// desc: polyfonic midi
//
// by: Ananya Misra and Ge Wang
// send all complaints to prc@cs.princeton.edu
//--------------------------------------------

MidiIn min;
MidiMsg msg;

if( !min.open( 1 ) ) me.exit();

class NoteEvent extends Event
{
    int note;
    int velocity;
}

// the event
NoteEvent on;
// array of ugen's handling each note
Event us[128];


gain g => JCRev r => dac;
.1 => g.gain;
.2 => r.mix;

fun void handler()
{
    // don't connect to dac until we need it
    Mandolin m;
    Event off;
    int note;

    while( true )
    {
        on => now;
        on.note => note;
        // dynamically repatch
        m => g;
        std.mtof( note ) => m.freq;
        std.rand2f( .6, .8 ) => m.pluckPos;
        on.velocity / 128.0 => m.pluck;
        off @=> us[note];

        off => now;
        null @=> us[note];
        m =< g;
    }
}

// spork handlers
for( 0 => int i; i < 20; i++ ) spork ~ handler();

while( true )
{
    // wait on midi event
    min => now;

    // get the midimsg
    while( min.recv( msg ) )
    {
        if( msg.data1 != 144 )  // !noteon
            continue;

        if( msg.data3 > 0 )
        {
            // store midi note number
            msg.data2 => on.note;
            // store velocity
            msg.data3 => on.velocity;
            // signal the event
            on.signal();
            // yield without advancing time to allow shred to run
            me.yield();
        }
        else
        {
            if( us[msg.data2] != null ) us[msg.data2].signal();
        }
    }
}
