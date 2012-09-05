//--------------------------------------|
// Dyno demo 2: "Compressor? I didn't even touch 'er!"
// Takes a silly little plucked string demo and runs it through a 
// compressor to enhance sustain. (Ostensibly.) Note that there 
// are many other (better) reasons to use compressors.
//
// authors: Matt Hoffman (mdhoffma at cs.princeton.edu)
//--------------------------------------|

// plucked string, dynamics processor, reverb, dac.
StifKarp m => Dyno d => JCRev r => dac;

// set reverb params
0.75 => r.gain;
0.01 => r.mix;

// set dyno to default compress mode
d.compress();
// set release time to be quite long to enhance sustain
2000::ms => d.releaseTime;
// set threshold to be a bit lower for more pronounced effect.
0.2 => d.thresh;
// set slope (in dB) above the threshold to 1/3 of what it would naturally be
0.33 => d.slopeAbove;
// compensate for the gain reduction
2 => d.gain;

// set plucked string params
0.1 => m.pickupPosition;
0.0 => m.sustain;
0.0 => m.stretch;

// go
while(true)
{
	// play a random note
	Std.mtof( Math.random2(60, 90) ) => m.freq;

    // twice
    repeat( 2 )
    {
        // say whether this pluck is compressed or not
        if(d.op() < 0)
            <<< "uncompressed" >>>;
        else
            <<< "compressed" >>>;
        
        0.5 => m.pluck;
        2000::ms => now;
        
        // make the next note compressed if this one wasn't, or vice versa
        -d.op() => d.op;
    }
}
