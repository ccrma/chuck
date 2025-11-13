//-----------------------------------------------------------------------------
// name: LiSa-load.ck
// desc: loading an audio file into a LiSa
// requires: chuck-1.5.5.6 (see LiSa-load-classic.ck for older method)
//
// author: Nick Shaheed (2025): at last, a read function
//         this drastically shortens the code needed (compared to the "classic" method)
//         this also increases performance for audio file loading by around 100x (!)
//         ~~~~~~~~~~~~~~~
//         Ge Wang (2021): rolled function, added twilight
//                  sound (see twilight-granular-kb-interp.ck for more info)
//                  and bi-directional loop
//         Dan Trueman (2007): original example; was LiSa-SndBuf.ck
//-----------------------------------------------------------------------------

// instantiate a LiSa audio file name; connect to dac
LiSa lisa( "twilight/twilight-granular.aiff" ) => dac;

// alternately can use the .read() function
// lisa.read( "twilight/twilight-granular.aiff" );

// party on...
2 => lisa.rate; // rate!
1 => lisa.loop; // loop it!
1 => lisa.bi; // bi-directional loop!
1 => lisa.play; // play!

// commence party
while( true ) 1::second => now;
