// import public class from another directory relative to this one
@import "../deep/ks-chord.ck"

// sound to chord to dac
SndBuf buffy => KSChord object => dac;
// load a sound
"special:dope" => buffy.read;
// set feedback
object.feedback( .96 );

// offset
-12 => int x;
// tune
object.tune( 60+x, 64+x, 72+x, 79+x );
// loop
while( true )
{
    // set playhead to beginning
    0 => buffy.pos;
    // set rate
    1 => buffy.rate;
    // advance time
    550::ms / buffy.rate() => now;
}
