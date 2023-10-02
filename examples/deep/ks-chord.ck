//-----------------------------------------------------------------------------
// name: ks-chord.ck
// desc: karplus strong comb filter bank
//
// authors: Madeline Huberth (mhuberth@ccrma.stanford.edu)
//          Ge Wang (ge@ccrma.stanford.edu)
// date: summer 2014
//       Stanford Center @ Peking University
//-----------------------------------------------------------------------------

// single voice Karplus Strong chubgraph
class KS extends Chugraph
{
    // sample rate
    second / samp => float SRATE;
    
    // ugens!
    DelayA delay;
    OneZero lowpass;
    
    // noise, only for internal use
    Noise n => delay;
    // silence so it doesn't play
    0 => n.gain;
    
    // the feedback
    inlet => delay => lowpass => delay => outlet;
    // max delay
    1::second => delay.max;
    // set lowpass
    -1 => lowpass.zero;
    // set feedback attenuation
    .9 => lowpass.gain;

    // mostly for testing
    fun void play( float pitch, dur T )
    {
        tune( pitch ) => float length;
        // turn on noise
        1 => n.gain;
        // fill delay with length samples
        length::samp => now;
        // silence
        0 => n.gain;
        // let it play
        T-length::samp => now;
    }
    
    // tune the fundamental resonance
    fun float tune( float pitch )
    {
        // computes further pitch tuning for higher pitches
        pitch - 43 => float diff;
        0 => float adjust;
        if( diff > 0 ) diff * .0125 => adjust;
        // compute length
        computeDelay( Std.mtof(pitch+adjust) ) => float length;
        // set the delay
        length::samp => delay.delay;
        //return
        return length;
    }
    
    // set feedback attenuation
    fun float feedback( float att )
    {
        // sanity check
        if( att >= 1 || att < 0 )
        {
            <<< "set feedback value between 0 and 1 (non-inclusive)" >>>;
            return lowpass.gain();
        }

        // set it        
        att => lowpass.gain;
        // return
        return att;
    }
    
    // compute delay from frequency
    fun float computeDelay( float freq )
    {
        // compute delay length from srate and desired freq
        return SRATE / freq;
    }
}

// chord class for KS
class KSChord extends Chugraph
{
    // array of KS objects    
    KS chordArray[4];
    
    // connect to inlet and outlet of chubgraph
    for( int i; i < chordArray.size(); i++ ) {
        inlet => chordArray[i] => outlet;
    }

    // set feedback    
    fun float feedback( float att )
    {
        // sanith check
        if( att >= 1 || att < 0 )
        {
            <<< "set feedback value between 0 and 1 (non-inclusive)" >>>;
            return att;
        }
        
        // set feedback on each element
        for( int i; i < chordArray.size(); i++ )
        {
            att => chordArray[i].feedback;
        }

        return att;
    }
    
    // tune 4 objects
    fun void tune( float pitch1, float pitch2, float pitch3, float pitch4 )
    {
        pitch1 => chordArray[0].tune;
        pitch2 => chordArray[1].tune;
        pitch3 => chordArray[2].tune;
        pitch4 => chordArray[3].tune;
    }
}


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
