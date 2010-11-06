sinosc osc => dac;
220.0 => osc.freq;
.25 => osc.gain;

while( true )
{
    // print out last
    stdout => "at " => now => " last == ";
    osc.last => stdout;
    std.rand2(100,1000)::samp => now;
}
