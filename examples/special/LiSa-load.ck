//-----------------------------------------------------------------------------
// name: LiSa-load.ck
// desc: function for loading an audio file into LiSa
//
// author: Dan Trueman, original example (2007): was LiSa-SndBuf.ck
//         Ge Wang, modified example (2021): rolled function, added twilight
//                  sound (see twilight-granular-kb-interp.ck for more info)
//                  and bi-directional loop
//-----------------------------------------------------------------------------
// this example shows how to open a soundfile and use it in LiSa. someday LiSa
// may be able to open soundfiles directly, but don't hold your breath. 
//
// note that unlike SndBuf, LiSa wants a dur (not an int) to specify the index
// of the sample location
//-----------------------------------------------------------------------------

// one-stop function for creating a LiSa, loaded with the specified audio file
load( me.dir() + "twilight/twilight-granular.aiff" ) @=> LiSa @ lisa;
// connect
lisa => dac;

// party on...
2 => lisa.rate; // rate!
1 => lisa.loop; // loop it!
1 => lisa.bi; // bi-directional loop!
1 => lisa.play; // play!

// commence party
while( true ) 1::second => now;

// create a new LiSa pre-loaded with the specified file
fun LiSa load( string filename )
{
    // sound buffer
    SndBuf buffy;
    // load it
    filename => buffy.read;

    // instantiate new LiSa (will be returned)
    LiSa lisa;
    // set duration
    buffy.samples()::samp => lisa.duration;

    // transfer values from SndBuf to LiSa
    for( 0 => int i; i < buffy.samples(); i++ )
    {
        // args are sample value and sample index
        // (dur must be integral in samples)
        lisa.valueAt( buffy.valueAt(i), i::samp );        
    }

    // set default LiSa parameters; actual usage parameters intended
    // to be set to taste by the user after this function returns
    lisa.play( false );
    lisa.loop( false );

    return lisa;
}
