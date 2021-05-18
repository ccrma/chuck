// name: autocorr.ck
// desc: finding the pitch of a note
// author: Curtis Ullerich

// UGen/UAna patch
SinOsc s => Flip flip =^ AutoCorr corr => blackhole;
// analysis zie
1024 => flip.size;
// output in [-1,1]
true => corr.normalize;
// calculate sample rate
second/samp => float srate;

// pitch estimate
fun float estimatePitch()
{
    // perform analysis at corr (propagating backwards)
    corr.upchuck();
    // for simplicity, ignore bins for notes that are "too high"
    // to care about; stop at the mid-point because it's symmetrical.
    (srate/Std.mtof(90)) $ int => int maxBin;
    // iterate over result of analysis
    for( maxBin => int bin; bin < corr.fvals().size()/2; bin++ )
    {
        // look for max
        if( corr.fval(bin) >= corr.fval(maxBin) ) {
            bin => maxBin;
        }
    }
    // return frequency
    return srate/maxBin;
}

// loop
for (int i; i < 10; i++)
{
    // randomize freq
    Math.random2f(Std.mtof(40), Std.mtof(80)) => s.freq;
    // move forward in time (by analysis frame size)
    flip.size()::samp => now;
    // print
    <<< "pitch", s.freq(), "estimated to be", estimatePitch(), "hz" >>>;
}
