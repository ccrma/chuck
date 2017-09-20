// dither.ck
// demo of dithering
//
// can use any UGen as source in play_with_dither()
// qbits : number of bits to quantize to
// do_dither : whether to dither
//
// gewang

// patch
Impulse imp => dac;

// sine wave generator
SinOsc s => blackhole;
220 => s.freq;

// go
play_with_dither( s, 2::second, 6, false );
play_with_dither( s, 2::second, 6, true );
.5::second => now;

play_with_dither( s, 2::second, 5, false );
play_with_dither( s, 2::second, 5, true );
.5::second => now;

play_with_dither( s, 2::second, 4, false );
play_with_dither( s, 2::second, 4, true );
.5::second => now;

// dither
fun void play_with_dither( UGen src, dur T, int qbits, int do_dither )
{
    // sanity check
    if( qbits <= 0 || qbits > 24 )
    {
        <<< "quantization bits out of range (1-24)", "" >>>;
        return;
    }

    // loop
    float sample;
    int quantized;
    (1 << 24) => int max;
    while( T > 0::second )
    {
        // get the last sample
        src.last() => sample;
        // quantize it
        if( do_dither ) // dither
            ((sample + Math.random2f(0,Math.pow(2,-qbits))) * max) $ int => quantized;
        else // no dither
            (sample * max) $ int => quantized;
        
        // throw away extra resolution
        quantized >> (24-qbits) << (24-qbits) => quantized;
        // cast it back for playback
        (quantized $ float) / max => imp.next;
        // advance time
        1::samp => now;
        // decrement
        1::samp -=> T;
    }
}

