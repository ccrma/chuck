// Chubgraph
// Create new UGens by compositing existing UGens

class MyString extends Chubgraph
{
    // karplus + strong plucked string filter
    // Ge Wang (gewang@cs.princeton.edu)
    
    Noise imp => OneZero lowpass => outlet;
    lowpass => DelayA delay => lowpass;
    
    .99999 => float R;
    1/220 => float L;
    -1 => lowpass.zero;
    220 => freq;
    0 => imp.gain;
    
    fun float freq( float f )
    {
        1/f => L;
        L::second => delay.delay;
        Math.pow( R, L ) => delay.gain;
        return f;
    }
    
    fun void pluck()
    {
        1 => imp.gain;
        L::second => now;
        0 => imp.gain;
        (Math.log(.0001) / Math.log(R))::samp => now;
    }
}

MyString s[3];
for(int i; i < s.cap(); i++) s[i] => dac;

while( true )
{
    for( int i; i < s.cap(); i++ )
    {
        Math.random2( 60,72 ) => Std.mtof => s[i].freq;
        spork ~ s[i].pluck();
        0.25::second => now;
    }
    
    2::second => now;
}
