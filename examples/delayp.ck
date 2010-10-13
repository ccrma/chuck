//delayp.ck  sound check for delayph class
// delay p - delay with variable write head ( vs read )
// - philipd

Wurley wur => delayp dp => dac;
Wurley w2 => dac;
440.0 => wur.freq;
wur => gain g => dac;
0.2 => g.gain;
impulse i => dac;

fun void ticksec ( int times ) {
    while ( times > 0 ) { 
        0.3 => i.next;
        times - 1 => times;
        1::second => now;
    }
}
"listen for ticks..." => stdout;
10::second => dp.max;
0.05::second => dp.window;

6::second=>dp.delay;
0.05::second => now;
0.5 => wur.noteOn;
ticksec( 2 );
1::second => dp.delay;
ticksec( 1 );
880.0 => wur.freq;
0.5 => wur.noteOn;

ticksec ( 10 ) ;

"end" => stdout;
