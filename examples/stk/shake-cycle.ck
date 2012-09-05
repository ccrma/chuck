// shake-cycle.ck : effects abuse
// author: Adam Tindale

// the patch
// Shakers s => Chorus c1 => JCRev rev => Chorus c2 => Chorus c3 =>dac;
Shakers s => dac;

0 => s.which;
1 => s.gain;
100 => float theTime;

while( true )
{
    1.0 => s.noteOn;
    theTime::ms => now;

    1.0 => s.noteOff;
    theTime::ms => now;   

    ( s.which() + 1 ) % 20 => s.which;
    Math.random2f( 20, 140 ) => theTime;
}
