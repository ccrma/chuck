// 48.ck : midi read (fail-correct)

MidiOut mout;
MidiMsg mg;
MidiMsgIn mrw;

// open MIDI output - see 'chuck --probe'
if( !mout.open( 0 ) )
    me.exit();

Mandolin m => JCRev r => dac;
.2 => r.mix;

// open file for read
mrw.open( "z.txt" );
//"foo.wav" => s.read;

// time scale
0.5 => float scale;

// get current time
time t;
now => t;

int count;

// while there is more to read
while( mrw.read( mg ) != 0 )
{
    // advance time
    scale * (mg.when - t) => now;
    mg.when => t;

    mout.send( mg );
    std.rand2f( .8, .9 ) => m.pluckPos;
    std.mtof( mg.data2 ) => m.freq;
    mg.data3 / 128.0 => m.pluck;

    <<< "when:", mg.when, mg.data1, mg.data2, mg.data3 >>>;
    // <<<count + 1 => count>>>;
}

mrw.close();

//<<<mrw.read( mg, t )>>>;
