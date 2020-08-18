// i.ck : n-channel demo

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
