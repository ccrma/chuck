// suck samples through Gain UGen into blackhole
adc => Gain g => blackhole; // (1) blackhole sucks samples from adc through Gain

while (true)
{
    if (g.last() > 0.9) // if it's loud enough
    {                       // (2) .last() gets last sample from any UGen
        <<< "LOUD!!!", g.last() >>>; // print it out
    }
    samp => now; // do this for every sample
}
