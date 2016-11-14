//--------------------------------------------------------------------
// name: oscillator.ck
// desc: function with an oscillator in the default args position
//
// author: Jack Atherton (http://www.jack-atherton.com/)
//   date: fall 2016
//--------------------------------------------------------------------

fun void playMySound( SinOsc s => UGen u, 500::ms => dur d )
{
    u => dac;
    d => now;
    u =< dac;
}

// SinOsc plays for 500 ms
playMySound();

// TriOsc plays for 500 ms
TriOsc t;
0.6 => t.gain;
playMySound( t );

// SqrOsc plays for 1 second
SqrOsc s;
0.2 => s.gain;
playMySound( s, 1::second );
