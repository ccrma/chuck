// 46.ck : midi read (fail-correct)

MidiOut mout;
MidiMsg mg;
MidiRW mrw;

// open - see 'chuck --probe'
if( !mout.open( 0 ) )
    me.exit();

Mandolin m => JCRev r => dac;
.2 => r.mix;

mrw.open( "z.txt" );
//"foo.wav" => s.read;
time t;
now => t;

int count;

// play back from file
while( mrw.read( mg ) != 0 )
{
    mg.when - t => now;
    mg.when => t;
    mout.send( mg );
    std.rand2f( .8, .9 ) => m.pluckPos;
    std.mtof( mg.data2 ) => m.freq;
    mg.data3 / 128.0 => m.pluck;
    <<< "when:", mg.when, " msg:", mg.data1, mg.data2, mg.data3 >>>; 
}

mrw.close();

//<<<mrw.read( mg, t )>>>;
