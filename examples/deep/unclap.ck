// name: unclap.ck
// desc: configurable "clapping music" (Steve Reich)
// author: Jesus Gollonet (original)
//         Ge Wang (shreds and glottal pops)
// date: Summer 2006

// our patch
SndBuf clapper1 => dac.left;
SndBuf clapper2 => dac.right;

// load built-in sounds
"special:glot_ahh" => clapper1.read; 3.0 => clapper1.gain;
"special:glot_ahh" => clapper2.read; 3.0 => clapper2.gain;

// the full "clapping music" figure
[.5, .5, 1, .5, 1, 1, .5, 1 ] @=> float seq[];

// length of quarter note
.4::second => dur quarter;
// how many measures per shift
3 => int shift_period;
// how much to shift by (in quarter notes)
.5 => float shift_factor;

// one clapper
fun void clap( SndBuf buffy, int max, float factor )
{
    1 => int shifts;

    // infinite time loop
    for( ; true; shifts++ )
    {
        // one measure
        for( 0 => int count; count < seq.size(); count++ )
        {
            // set gain
            seq[count] * 2 => buffy.gain;
            // clap!
            0 => buffy.pos;
            // let time go by
            if( !max || shifts < max || count != (seq.size() - 1) )
                seq[count]::quarter => now;
            else
            {
                <<< "shift!!!", "" >>>;
                seq[count]*factor*quarter => now;
                0 => shifts;
            }
        }
    }
}

// spork one clapper, shift every shift_period measures
spork ~ clap( clapper1, shift_period, shift_factor );
// spork, no shift
spork ~ clap( clapper2, 0, 0 );

// infinite time loop
while( true ) 1::day => now;
