// for demo'ing SndBuf.valueAt() and using Impulse to render
// audio one-sample-at-a-time; not a conventional use case
// (see sndbuf.ck or otf_01.ck for more conventional usage of sndbuf)

// use this for reading from file only
SndBuf buf;
// read a file
me.dir() + "../data/kick.wav" => buf.read;

// the patch (no sndbuf involved)
Impulse i => dac;

// infinite time-loop
while( true )
{
    // index
    int pos;
    // repeat this many times
    repeat( buf.samples() )
    {
        // set next sample
        buf.valueAt( pos ) => i.next;
        // increment index
        pos++;
        // advance time by one samp
        1::samp => now;
    }
}
