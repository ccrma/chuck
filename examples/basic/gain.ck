// using the Gain and GainDB unit generators
// authors: Nick, Andrew, Kunwoo, Ge
// date: Fall 2025

// Gain can be inserted anywhere in the chain
SndBuf buffy => Gain g => dac;
// load a sound
"special:doh" => buffy.read;

// play at full volume (gain.gain defaults to 1.0)
play( g );
// set amplitude directly and play
.5 => g.gain; play( g );
// set amplitude directly and play
.25 => g.gain; play( g );

// play at unity (0 dB is equivalent to gain of 1)
0 => g.db; play( g );
// every -6 dB approximately halves the signal strength
-6 => g.db; play( g );
// 12 dB down (roughly equivalent to gain of .25)
-12 => g.db; play( g );

// next, disconnect ugens
buffy =< g =< dac;

// connect to GainDB, using a constructor to specify initial dB
buffy => GainDB gdb(-18) => dac; play( gdb );
// otherwise GainDB is identical to Gain
-12 => gdb.db; play( gdb );
// 6 dB down is roughly
-6 => gdb.db; play( gdb );
// back to unity gain
0 => gdb.db; play( gdb );
// 3 dB up (roughtly gain = 1.4)
3 => gdb.db; play( gdb );
// back to unity gain
0 => gdb.db; play( gdb );

// quick play function
fun void play( Gain gg ) { 
    <<< "db:", gg.db(), "gain:", gg.gain() >>>;
    0 => buffy.pos; buffy.length() => now; }
