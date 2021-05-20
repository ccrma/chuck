// name: n.ck
// desc: n-channel demo
// note: ChucK can be started with as much channels as is
//       supported on the underlyding audio device; this can be
//       done on command line (see chuck --help for more info)
//       or in the miniAudicle Preferences (note changes will
//       not take effect until the ChucK VM is (re)started

// number of channels
dac.channels() => int N;
// print
<<< "dac has", N, "channels" >>>;

// make osc bank
SinOsc s[N];
// connect to individual channels
for( int i; i < N; i++ )
{
    s[i] => dac.chan(i);
    Math.random2f( 400, 440 ) => s[i].freq;
    .2 => s[i].gain;
}

// go
while( true ) 1::second => now;
