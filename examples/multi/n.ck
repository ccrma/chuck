// n.ck : n-channel demo

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
