// name: we-robot.ck
// desc: take us to your leader (talk into the mic), multi-channel
// author: gewang, prc
// note: ChucK can be started with as much channels as is
//       supported on the underlyding audio device; this can be
//       done on command line (see chuck --help for more info)
//       or in the miniAudicle Preferences (note changes will
//       not take effect until the ChucK VM is (re)started

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
