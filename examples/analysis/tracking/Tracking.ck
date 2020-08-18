//--
// name: Tracking.ck
// desc: class and process for tracking a peak and amplitutde via FFT
//       the Tracking class can be accessed from external shreds
//
// author: Ge Wang + Rebecca Fiebrink
// date: 2007
//--


// public class
public class Tracking
{
    static float the_freq;
    static float the_gain;
    static Event @ the_event;
}

// initialize separately (due to a bug)
new Event @=> Tracking.the_event;

// analysis
adc => PoleZero dcblock => FFT fft => blackhole;

// set to block DC
.99 => dcblock.blockZero;
// set FFT params
2048 => fft.size;
// window
Windowing.hamming( fft.size() ) => fft.window;

// go for it
while( true )
{
    // take fft
    fft.upchuck() @=> UAnaBlob blob;
    
    // find peak
    0 => float max; float where;
    for( int i; i < blob.fvals().size()/8; i++ )
    {
        // compare
        if( blob.fvals()[i] > max )
        {
            // save
            blob.fvals()[i] => max;
            i => where;
        }
    }
    
    // set freq
    (where / fft.size() * (second / samp)) => Tracking.the_freq;
    // set gain
    (max / .5) => Tracking.the_gain;
    // clamp
    if( Tracking.the_gain > 1 )
        1 => Tracking.the_gain;
    // fire!
    Tracking.the_event.broadcast();

    // hop
    (fft.size()/4)::samp => now;
}
