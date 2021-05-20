// name: i.ck
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
int i;

// make osc bank
Impulse s[N];
// connect to individual channels
for( int i; i < N; i++ )
{
    s[i] => dac.chan(i);
    // .5 => s[i].gain;
}

// go
while( true )
{
    1 => s[i].next;
    100::ms => now;
    (i+1)%N => i;
}
