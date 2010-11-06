// Flip: turns the last N samples into a vector in the UAna domain
// pilF: turns a UAna frame back into the time domain via overlap add
// author: Ge Wang (ge@ccrma.stanford.edu)
//         Rebecca Fiebrink (fiebrink@cs.princeton.edu)

// identity test
SinOsc s => Flip flip =^ pilF pilf => dac;

// set size
512 => flip.size => pilf.size;

// go
while( true )
{
    // do it
    pilf.upchuck();
    // hop
    pilf.size()::samp => now;
}
