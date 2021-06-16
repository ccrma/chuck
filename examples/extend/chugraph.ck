// name: chugraph.ck
// desc: pronounced "chuh-graph" -- rhymes with "subgraph"
//       create new UGens by compositing existing UGens!
// note: (chuck-1.4.1.0 and up) "Chubgraph" deprecated; use "Chugraph"

// make a plucked string
class PluckedString extends Chubgraph // Chugraph
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
        // delay length
        1/f => L;
        // set delay length
        L::second => delay.delay;
        // set gain
        Math.pow( R, L ) => delay.gain;
        // return frequency through
        return f;
    }
    
    fun void pluck( dur ringDur )
    {
        // turn on the noise...
        1 => imp.gain;
        // for one delay length
        L::second => now;
        // turn off the noise
        0 => imp.gain;
        // let it ring
        ringDur => now;
    }
}

// instantiate three chugraphs
PluckedString ps[3];
// connect as any other UGen
for( int i; i < ps.size(); i++ ) ps[i] => dac;
// ring duration
2::second => dur ringDur;

// infinite time loop
while( true )
{
    // iterate over plucked strings
    for( int i; i < ps.size(); i++ )
    {
        // randomize pitch
        Math.random2( 36, 72 ) => Std.mtof => ps[i].freq;
        // spork the pluck
        spork ~ ps[i].pluck( 2*ringDur );
        // wait a bit
        0.25::second => now;
    }
    
    // let ring...
    ringDur => now;
}
