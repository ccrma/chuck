// take us to your leader (talk into the mic)
// gewang, prc

// n channels
dac.channels() => int N;
// print
<<< "dac has", N, "channels..." >>>;

// delays
DelayL d[N];
// gains
Gain g3[N];

// our patch - feedforward part
adc => Gain g;
adc => Gain g2 => dac;

// set delays
for( int i; i < N; i++ )
{
    // feedfoward
    g => d[i] => dac.chan(i);
    // feedback
    d[i] => g3[i] => d[i];
    // detune
    30::ms + Math.random2f(-15,15)::ms => d[i].max => d[i].delay;
    .95 => g3[i].gain;
}

// set parameters
0.25 => g.gain;
0.05 => g2.gain;

// time loop
while( true ) 100::ms => now;
