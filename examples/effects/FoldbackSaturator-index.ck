SinOsc s => FoldbackSaturator foldy => dac;

0.3 => foldy.threshold;

0.001 => float delta;

while(1::ms => now)
{
    if((foldy.index() < 1.0) || (foldy.index() > 4.0))
    {
        -1.0 *=> delta;
    }
    foldy.index(foldy.index() - delta);
}
