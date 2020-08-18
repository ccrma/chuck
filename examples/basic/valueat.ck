// (see sndbuf.ck or otf_01.ck for non-insane usage of sndbuf)
SndBuf buf;
me.dir() + "/../data/kick.wav" => buf.read;

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
