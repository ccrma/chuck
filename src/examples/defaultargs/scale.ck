//--------------------------------------------------------------------
// name: scale.ck
// desc: function with an array in the default args position
//
// author: Jack Atherton (http://www.jack-atherton.com/)
//   date: fall 2016
//--------------------------------------------------------------------

fun void playScale( int base; [0, 2, 4, 5, 7, 9, 11, 12] => int scale[] )
{
    SinOsc s => dac;
    for( int i; i < scale.size(); i++ )
    {
        base + scale[i] => Math.mtof => s.freq;
        250::ms => now;
    }
    s =< dac;
}

// default to major scale. if someone wants to play a 
// major scale, they only have to provide the first note.
playScale( 60 );

// optionally provide a different scale
playScale( 65, [0, 2, 3, 5, 7, 8, 10, 12] );
