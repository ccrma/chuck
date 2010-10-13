// pulse width modulation (gewang / philipd)

// drive sinosc with blackhole
sinosc modw => blackhole; 
0.125 => modw.freq;

// patch
phasor x => triosc t => ADSR te => dac;
40.0 => x.freq;
2 => t.sync;  // sync to input

// modulate pulse width
fun void pwm() {
    while( true ) {
        modw.last * .95 => t.width;
        1::ms => now;
    }
}

// go forth...
spork ~ pwm();

// go
while( true ) {
    1 => te.keyOn; 125::ms => now;
    1 => te.keyOff; 75::ms => now;
}
