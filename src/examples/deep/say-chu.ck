// Fairly silly program to synthesize the
// word "chuck" in ChucK.  Perry Cook, Nov. 2007

TwoPole r[3];	// Resonators
Noise n => Envelope ne => r[0] => TwoZero z => Gain gain => dac;
n => r[1] => z; n => r[2] => z; 
Impulse i => Envelope ie => OnePole o => r[0]; o => r[1]; o => r[2];
0.99 => o.pole; 10.0 => o.gain; 1.0 => z.b0; 0.0 => z.b1; -1.0 => z.b2;

// louder
4.0 => gain.gain;

// say
doCh();
1 => int notDone;
doUhh();
doKay();

fun void doUhh()  {
    0.1=>ie.time;
    0.0 => n.gain;
    600.0 => r[0].freq; 0.995 => r[0].radius; 1.0 => r[0].gain;
    1500.0 => r[1].freq; 0.995 => r[1].radius; 0.5 => r[1].gain;
    3900.0 => r[2].freq; 0.99 => r[2].radius; 0.2 => r[2].gain;
    spork ~ doimpulse();
    0.4 => i.gain;
    1.0 => i.gain;
    1 => ie.keyOn;
    0.1 :: second => now;
    1 => ie.keyOff;
    0.1 :: second => now;
    0 => notDone;
}

fun void doCh()  {
    <<< "Ch" >>>;
    0.03=>ne.time;
    1900.0 => r[0].freq; 0.99 => r[0].radius; 1.0 => r[0].gain;
    2700.0 => r[1].freq; 0.99 => r[1].radius; 0.7 => r[1].gain;
    3200.0 => r[2].freq; 0.99 => r[2].radius; 0.8 => r[2].gain;
    0.0 => i.gain;
    0.02 => n.gain;
    1=>ne.keyOn;0.03 :: second => now;
    1=>ne.keyOff;0.03 :: second => now;
}

fun void doKay()  {
    <<< "KKK" >>>;
    0.0 => i.gain;
    0.05 :: second => now;
    0.005=>ne.time;
    0.007 => n.gain;
    380.0 => r[0].freq; 0.99 => r[0].radius; 0.7 => r[0].gain;
    1700.0 => r[1].freq; 0.99 => r[1].radius; 1.0 => r[1].gain;
    4500.0 => r[2].freq; 0.99 => r[2].radius; 0.7 => r[2].gain;
    1 => ne.keyOn; 0.005 :: second => now;
    1 => ne.keyOff;0.01=>ne.time; 0.01 :: second => now;
}

fun void doimpulse()  {
    150.0 => float freq;
    <<< "uhh" >>>;
    while (notDone)  {
        1.0 => i.next;
        (1.0 / freq) :: second => now;    
        freq * 0.98 => freq;
    }
}
