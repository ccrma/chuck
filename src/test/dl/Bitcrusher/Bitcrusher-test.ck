
adc => Delay d => Bitcrusher bc => dac;

0.5::second => d.max => d.delay;

0.5 => bc.gain;
5 => bc.bits;
12 => bc.downsampleFactor;

<<< "bits:", bc.bits(), "downsampling:", bc.downsampleFactor() >>>;

while(true)
{
    1::second => now;
}
