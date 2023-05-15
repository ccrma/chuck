// Flip: turns the last N samples into a vector in the UAna domain
// UnFlip: turns a UAna frame back into the time domain via overlap add
// author: Ge Wang (ge@ccrma.stanford.edu)
//         Rebecca Fiebrink (fiebrink@cs.princeton.edu)

// identity test
SinOsc s => Flip flip =^ UnFlip unflip => dac;

// set size
512 => flip.size => unflip.size;

// go
while( true )
{
    // do it
    unflip.upchuck();
    // hop
    unflip.size()::samp => now;
}
