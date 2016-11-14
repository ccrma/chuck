//--------------------------------------------------------------------
// name: debug_message.ck
// desc: function with a boolean flag in the default args position
//
// author: Jack Atherton (http://www.jack-atherton.com/)
//   date: fall 2016
//--------------------------------------------------------------------

fun void playUGen( UGen u, dur d; false => int should_log )
{
    if( should_log )
    {
        <<< u >>>; // logs static type, not dynamic type
        chout <= "playing for " <= (d / 1::second) <= " seconds" <= IO.newline();
    }
    
    u => dac;
    d => now;
    u =< dac;
}

TriOsc t;
SinOsc s;
// default: do not log
playUGen( t, 1::second );
// provide flag to log
playUGen( s, 2::second, true );
