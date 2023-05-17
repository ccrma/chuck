SinOsc s => FoldbackSaturator foldy => dac;

0.001 => float delta;

while(10::ms => now)
{
    if((foldy.threshold() < 0.03) || (foldy.threshold() > 0.8))
    {
        -1.0 *=> delta;
    }
    foldy.threshold(foldy.threshold() - delta);
}
