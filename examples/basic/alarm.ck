// how long
2::hour => dur T;
// frequency
880 => float f;

// remember
now => time start;
now + T => time later;

// wait
while( now < later )
{
    <<< (T - (now - start)) / second, "left..." >>>;
    1::second => now;
}

// patch
SinOsc s => JCRev r => dac;
.025 => r.mix;
f => s.freq;

// infinite while loop
while( true )
{
    // go
    1.0 => s.gain;
    300::ms => now;
    // stop
    0.0 => s.gain;
    300::ms => now;
}
